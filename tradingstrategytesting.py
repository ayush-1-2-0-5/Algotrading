import pandas as pd
from indiafactorlibrary import IndiaFactorLibrary
import warnings
from sklearn.linear_model import LinearRegression
from sklearn.metrics import r2_score
import statsmodels.api as sm
import matplotlib.pyplot as plt
import seaborn as sns
warnings.filterwarnings("ignore")


class GetNAVSeriesAndFactorData:
    def __init__(self):
        self.ifl = IndiaFactorLibrary()
        self.fund_id = None
        self.nav_series = None  
        self.nav_series_monthly = None
        self.nav_series_yearly = None
        self.dataset = None  

    def read_nav_data_from_csv(self, file_path):
        try:
            df = pd.read_csv(file_path)
            
            if 'date' not in df.columns or 'returns' not in df.columns:
                raise ValueError("CSV file must have 'date' and 'returns' columns")
            df['date'] = pd.to_datetime(df['date'])
            df.set_index('date', inplace=True)
         
            self.nav_series = df['returns']
            
            print("NAV data loaded successfully from CSV.")
            return self.nav_series
        except Exception as e:
            print(f"Error reading NAV data from CSV: {e}")

    def search_fund(self):
        file_path = input("Enter the path to your CSV file: ").strip()
        self.read_nav_data_from_csv(file_path)
        print("NAV Series:")
        print(self.nav_series)

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

    def plot_correlation_matrix(self, df):
        corr_matrix = df.corr()  
        fig, ax = plt.subplots(figsize=(15, 10)) 
        sns.heatmap(
            corr_matrix, 
            annot=True,         
            linewidths=0.5,     
            fmt=".2f",           
            cmap="YlGnBu",       
            square=True,       
            cbar_kws={"shrink": .8} 
        )
        ax.set_title('Correlation Matrix', fontsize=16)
        plt.show()

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
            self.print_dataset_description()
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

    def set_monthly_returns_in_data(self, index=None):
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
                    self.perform_multiregression(dataframe=self.dataset[user_input], target_column="monthly_returns")
                else:
                    self.perform_multiregression(dataframe=self.dataset[user_input], target_column="yearly_return")
              
                print("reached")
                return
            except ValueError:
                print(f'\n\n{ValueError}\n\n')
                print(f"Invalid input. Please enter a valid number. You entered: '{user_input}'")
                return

    def perform_multiregression(self, dataframe, target_column):
        if target_column not in dataframe.columns:
            raise ValueError(f"'{target_column}' not found in DataFrame columns.")
        available_columns = [col for col in dataframe.columns if col != target_column]
        print("Available columns for regression:")
        print(available_columns)
        selected_columns = input("Enter the predictor columns separated by commas: ").split(',')
        selected_columns = [col.strip() for col in selected_columns]
        for col in selected_columns:
            if col not in available_columns:
                raise ValueError(f"Column '{col}' is not available in the DataFrame.")
        X = dataframe[selected_columns]
        y = dataframe[target_column]
        X = sm.add_constant(X) 
        model = sm.OLS(y, X).fit()       
        print("\nRegression Summary:")
        print(model.summary())
        model_sklearn = LinearRegression()
        model_sklearn.fit(X, y)
        y_pred = model_sklearn.predict(X)
        print(f"\nR² Score: {r2_score(y, y_pred)}")
        self.plot_correlation_matrix(dataframe)
        return


if __name__ == "__main__":
    data_fetcher = GetNAVSeriesAndFactorData()
    data_fetcher.search_fund()
    print("NAV Series:")
    print(data_fetcher.nav_series)
    data_fetcher.print_available_factors() 
    return_frequency = data_fetcher.get_return_frequency()
