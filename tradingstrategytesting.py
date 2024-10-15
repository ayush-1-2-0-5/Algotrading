import requests
import pandas as pd
from indiafactorlibrary import IndiaFactorLibrary
import warnings
from sklearn.linear_model import LinearRegression
from sklearn.metrics import r2_score
import statsmodels.api as sm

warnings.filterwarnings("ignore")

class GetNAVSeriesAndFactorData:
    def __init__(self):
        self.ifl = IndiaFactorLibrary()
        self.fund_id = None
        self.nav_series = None  
        self.nav_series_monthly = None
        self.nav_series_yearly = None
        self.dataset = None  

    def fetch_nav_data(self, fund_id):
        url = f"https://api.mfapi.in/mf/{fund_id}"
        try:
            response = requests.get(url)
            response.raise_for_status()
            data = response.json()
            dates = [entry['date'] for entry in data['data']]
            navs = [float(entry['nav']) for entry in data['data']]
            self.nav_series = pd.Series(navs, index=pd.to_datetime(dates, format='%d-%m-%Y'))
            return self.nav_series
        except requests.exceptions.RequestException as e:
            print(f"Error fetching NAV data: {e}")




    def search_fund(self):
        query = input("Enter fund name or keyword to search: ").strip()
        url = f"https://api.mfapi.in/mf/search?q={query}"
        try:
            response = requests.get(url)
            response.raise_for_status()
            funds = response.json()

            if funds:
                print(f"\nSearch Results for '{query}':")
                for fund in funds:
                    print(f"ID: {fund['schemeCode']} - Name: {fund['schemeName']}")

                self.fund_id = input("\nEnter the fund ID to fetch NAV data: ").strip()
                self.fetch_nav_data(self.fund_id)
                print("NAV Series:")
                print(self.nav_series)
            else:
                print("No matching funds found.")
        except requests.exceptions.RequestException as e:
            print(f"Error fetching data: {e}")

    def print_dataset_description(self):
        if not self.dataset or 'DESCR' not in self.dataset:
            print("No dataset description available.")
            return
      
        descr = self.dataset['DESCR']
        descr_parts = descr.split("\n\n")
        print("Available Data Options:")

       
        for part in descr_parts:
           
            if any(f"{i} :" in part for i in range(10)): 
                print(part.strip())
        

    def print_available_factors(self):
        try:
            available_datasets = self.ifl.get_available_datasets()
            if available_datasets:
                print("Available Factors names:")
                for factor in available_datasets:
                    print(f"Name: {factor}")

                user_input_name = input("Enter the factor name you want to select: ").strip()
                if user_input_name in available_datasets:
                    print(f"You selected: {user_input_name}")
                    self.display_dataset(user_input_name)
                else:
                    print("Invalid factor name selected.")
            else:
                print("No factors available or data format is incorrect.")
        except Exception as e:
            print(f"Error retrieving factors: {e}")

    def display_dataset(self, factor_id):
        try:
            self.dataset = self.ifl.read(factor_id) 
            print("Dataset loaded successfully.")
            self.print_dataset_description();  
        except Exception as e:
            print(f"Error displaying the dataset: {e}")

    def monthly_returns(self, nav_series_new=None):

        if nav_series_new:
            nav_series = nav_series_new
        else:
            nav_series = self.nav_series

        nav_series = nav_series.sort_index(ascending=True)
        ratios = pd.Series(index=nav_series.index, dtype='float64')
        ratios[nav_series.index[0]] = 0

        for i in range(1, len(nav_series)):
            ratios[nav_series.index[i]] = ((nav_series.iloc[i] - nav_series.iloc[i - 1]) / nav_series.iloc[i - 1])

        monthly_sums = ratios.groupby(ratios.index.to_period('M')).sum()
        self.nav_series_monthly = monthly_sums

    def set_monthly_returns_in_data(self,index=None):
        if index is None:
            print("Index must be provided to access the dataset.")
            return

        data = self.dataset.get(index)
        if data is None:
            print(f"No data found for index: {index}")
            return
        
        data['monthly_returns'] = data.index.to_period('M').map(self.nav_series_monthly) * 100
        data = data.dropna()
        self.dataset[index] = data
        
    def yearly_returns(self, nav_series_new=None, index=None):
        if index is None:
            print("Index must be provided to access the dataset.")
            return

        data = self.dataset.get(index)
        if data is None:
            print(f"No data found for index: {index}")
            return

        if self.nav_series_monthly is not None:
            monthly_nav_series = self.nav_series_monthly  
        else: 
            self.monthly_returns(nav_series_new=nav_series_new)
            monthly_nav_series = self.nav_series_monthly  
            
        monthly_nav_series = monthly_nav_series.sort_index(ascending=True)
        yearly_returns = pd.Series(index=monthly_nav_series.index.year.unique(), dtype='float64')
        yearly_grouped = monthly_nav_series.groupby(monthly_nav_series.index.year)

        for year, group in yearly_grouped:
            if len(group) == 12:
                yearly_return = 1
                for month_return in group:
                    yearly_return *= (1 + month_return)
                yearly_return -= 1
                yearly_returns.at[year] = yearly_return * 100

        data['year'] = data.index.year 
        data = data.join(yearly_returns.rename('yearly_return'), on='year', how='inner')
        data = data.drop(columns=['year']).dropna()
        self.dataset[index] = data

    def parse_options(self, description):
        options_dict = {}
        lines = description.strip().split('\n')
        descs = {}
        for line in lines:
            key, rest = line.split(':', 1)
            key = int(key.strip())
            parts = rest.strip().split('--')
            # Extracting the description and type
            desc = parts[0].strip()
            descs[desc] = parts[1].strip().split()[0].lower() if len(parts) > 1 else 'unknown'

            options_dict[key] = {
                "type": descs[desc],
                "description": desc
            }
        return options_dict

    def get_return_frequency(self):
        if 'DESCR' not in self.dataset:
            print("Dataset description is not available.")
            return
        descr = self.dataset['DESCR']
        descr_parts = descr.split("\n\n")
        options_dict = self.parse_options(descr_parts[-1])
        num_options = len(options_dict)
        for option in options_dict:
                if len(self.dataset[option]) < 50:
                    options_dict[option]['type'] = 'annually'
                else:
                    options_dict[option]['type'] = 'monthly'
        print(num_options)
        while True:
            print(f"Select which factor dataframe you want to choose (0 to {num_options - 1}, -1 to exit):")
            try:
                user_input = int(input(f"Enter a number between 0 and {num_options - 1}: "))

                if user_input == -1:
                    print("Exiting selection.")
                    return

                if user_input < 0 or user_input > num_options:
                    print(f"Invalid input. Please enter a number between 0 and {num_options - 1}, or -1 to exit.")
                    continue

                monthly_data_dict = {k: v for k, v in options_dict.items() if v['type'] == 'monthly'}
                yearly_data_dict = {k: v for k, v in options_dict.items() if v['type'] == 'annually'}

                self.monthly_returns()
                if options_dict[user_input]['type'] == 'monthly':
                    self.set_monthly_returns_in_data(index=user_input)
                else:
                    self.yearly_returns(index=user_input)
                print(self.dataset[user_input])
                if options_dict[user_input]['type'] == 'monthly':
                    self.perform_multiregression(dataframe=self.dataset[user_input],target_column="monthly_returns")
                else:
                    self.perform_multiregression(dataframe=self.dataset[user_input],target_column="yearly_return")
              
                print("reached")
                return
            except ValueError:
                print(f'\n\n{ValueError}\n\n')
                print(f"Invalid input. Please enter a valid number. You entered: '{user_input}'")
                return



    def perform_multiregression(self,dataframe, target_column):
        print('\n\n1\n\n')
        if target_column not in dataframe.columns:
            raise ValueError(f"'{target_column}' not found in DataFrame columns.")
        print('\n\n2\n\n')
        # Step 1: Display available columns (excluding the target column)
        available_columns = [col for col in dataframe.columns if col != target_column]
        print("Available columns for regression:")
        print(available_columns)
     
        # Step 2: User selects independent columns
        selected_columns = input("Enter the predictor columns separated by commas: ").split(',')
        selected_columns = [col.strip() for col in selected_columns]

        # Validate selected columns
        for col in selected_columns:
            if col not in available_columns:
                raise ValueError(f"Column '{col}' is not available in the DataFrame.")

        # Step 3: Prepare X (independent variables) and y (dependent variable)
        X = dataframe[selected_columns]
        y = dataframe[target_column]

        # Step 4: Perform multivariate regression using statsmodels
        X = sm.add_constant(X)  # Adds a constant term to the model (intercept)
        model = sm.OLS(y, X).fit()

        # Step 5: Output model summary
        print("\nRegression Summary:")
        print(model.summary())

        # Optionally, use scikit-learn to provide R² score
        model_sklearn = LinearRegression()
        model_sklearn.fit(X, y)
        y_pred = model_sklearn.predict(X)
        print(f"\nR² Score: {r2_score(y, y_pred)}")






if __name__ == "__main__":
    data_fetcher = GetNAVSeriesAndFactorData()
    data_fetcher.search_fund()
    print("NAV Series:")
    print(data_fetcher.nav_series)
    data_fetcher.print_available_factors() 
    return_frequency = data_fetcher.get_return_frequency()
