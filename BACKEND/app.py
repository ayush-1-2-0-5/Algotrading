import pandas as pd
import concurrent.futures
import numpy as np
import os
import tempfile
from flask import Flask, request, jsonify
from flask_cors import CORS  

app = Flask(__name__)
CORS(app, resources={r"/*": {"origins": "*"}})  # Allow all origins, adjust as needed

# Set the maximum content length for requests to 16 MB
app.config['MAX_CONTENT_LENGTH'] = 16 * 1024 * 1024  # 16 MB

class Stock:
    def __init__(self, stock_name):
        self.name = stock_name
        self.prices = []
        self.ema = []
        self.ath = []
        self.isgood = []
        self.returns12M = []

    def add_price(self, date, price):
        self.prices.append((date, price))

    def calculate_ema_for_stock(self, period):
        close_prices = [price for _, price in self.prices]
        dates = [date for date, _ in self.prices]
        ema_values = self.calculate_ema(close_prices, period)
        self.ema = list(zip(dates, ema_values))

    def calculate_ema(self, close_prices, period):
        ema = []
        if len(close_prices) < period:
            print("Error: Not enough data to calculate EMA")
            return ema

        multiplier = 2.0 / (period + 1)
        ema_prev = np.mean(close_prices[:period])
        ema.append(ema_prev)

        for price in close_prices[period:]:
            ema_prev = (price - ema_prev) * multiplier + ema_prev
            ema.append(ema_prev)

        return [0] * (period - 1) + ema

    def calculate_ath_for_stock(self, period):
        close_prices = [price for _, price in self.prices]
        dates = [date for date, _ in self.prices]
        ath_values = self.calculate_ath(close_prices, period)
        self.ath = list(zip(dates, ath_values))

    def calculate_ath(self, close_prices, period):
        if len(close_prices) < period:
            print("Error: Not enough data to calculate ATH")
            return []

        ath = []
        dq = []
        for i, price in enumerate(close_prices):
            while dq and close_prices[dq[-1]] <= price:
                dq.pop()

            dq.append(i)

            if dq[0] <= i - period:
                dq.pop(0)

            if i >= period - 1:
                ath.append(close_prices[dq[0]])
            else:
                ath.append(0)

        return ath

    def is_ema_criteria(self, a):
        for i in range(len(self.prices)):
            if i < len(self.ema) and i < len(self.ath):
                current_price = self.prices[i][1]
                current_ema = self.ema[i][1]
                current_ath = self.ath[i][1]
                condition_met = (current_ema < current_price) and (current_price < (a / 100) * current_ath)
                self.isgood.append((self.prices[i][0], condition_met))

    def return_12_months(self):
        close_prices = [price for _, price in self.prices]
        dates = [date for date, _ in self.prices]
        result = self.calculate_12M_returns(close_prices, dates)
        result_map = {self.convert_to_dd_mm_yyyy(date): ret for date, ret in result}

        for date in dates:
            self.returns12M.append((date, result_map.get(date, -1)))

    def calculate_12M_returns(self, close_prices, dates):
        returns = []
        data_map = {}
        for i in range(len(dates)):
            year, month = self.extract_year_month(dates[i])
            if (year, month) not in data_map:
                data_map[(year, month)] = []
            data_map[(year, month)].append((int(dates[i].split('-')[2]), close_prices[i]))

        for year in range(2016, 2025):
            for month in range(1, 13):
                next_year = year if month != 12 else year + 1
                next_month = 1 if month == 12 else month + 1
                if (next_year, next_month) in data_map:
                    curr_month_data = data_map.get((year, month), [])
                    next_month_data = data_map[(next_year, next_month)]

                    if curr_month_data and next_month_data:
                        first_day_curr_price = curr_month_data[0][1]
                        last_day_next_price = next_month_data[-1][1]
                        return_rate = (last_day_next_price - first_day_curr_price) / first_day_curr_price
                        returns.append((f'{year}-{month:02d}-01', return_rate))

        return returns

    @staticmethod
    def extract_year_month(date_str):
        parts = date_str.split('-')
        return int(parts[0]), int(parts[1])

    @staticmethod
    def convert_to_dd_mm_yyyy(date):
        parts = date.split('-')
        return f'{parts[2]}-{parts[1]}-{parts[0]}'

stocks = []

def read_and_process_file(file_path):
    df = pd.read_csv(file_path)
    dates = df['Date']
    stocks.clear()

    for column in df.columns[1:]:
        stock = Stock(column)
        for date, price in zip(dates, df[column]):
            if pd.notna(price):
                stock.add_price(date, float(price))
        stocks.append(stock)

@app.route('/upload', methods=['POST'])
def upload_file():
    if 'file' not in request.files:
        return jsonify({"error": "No file part"}), 400

    file = request.files['file']
    if file.filename == '':
        return jsonify({"error": "No selected file"}), 400

    if file:
        if file.content_length > 16 * 1024 * 1024:  # 16 MB
            return jsonify({"error": "File is too large"}), 413

        with tempfile.NamedTemporaryFile(delete=False, suffix=".csv") as temp_file:
            file_path = temp_file.name
            file.save(file_path)

        try:
            with concurrent.futures.ThreadPoolExecutor() as executor:
                future = executor.submit(read_and_process_file, file_path)
                future.result()
        finally:
            os.remove(file_path)

        return jsonify({"message": f"File processed successfully. {len(stocks)} stocks detected."})

@app.route('/calculate', methods=['POST'])
def calculate_indicators():
    try:
        period = int(request.json['period'])
        ath_period = int(request.json['ath_period'])
        c = float(request.json['criteria'])
        with concurrent.futures.ThreadPoolExecutor() as executor:
            futures = []
            for stock in stocks:
                futures.append(executor.submit(stock.calculate_ema_for_stock, period))
                futures.append(executor.submit(stock.calculate_ath_for_stock, ath_period))
                futures.append(executor.submit(stock.return_12_months))
                futures.append(executor.submit(stock.is_ema_criteria, c))
            for future in futures:
                future.result()
    except KeyError as e:
        return jsonify({"error": f"Missing parameter: {str(e)}"}), 400
    except ValueError as e:
        return jsonify({"error": f"Invalid parameter value: {str(e)}"}), 400
    except Exception as e:
        return jsonify({"error": f"An unexpected error occurred: {str(e)}"}), 500

    return jsonify({"message": "Calculations completed."})

@app.route('/stock/<name>', methods=['GET'])
def get_stock_data(name):
    target_stock = next((stock for stock in stocks if stock.name.lower() == name.lower()), None)
    
    if target_stock:
        data = []
        for i, (date, ema_value) in enumerate(target_stock.ema):
            ath_value = target_stock.ath[i][1] if i < len(target_stock.ath) else 'N/A'
            is_good = target_stock.isgood[i][1] if i < len(target_stock.isgood) else 'N/A'
            # Convert boolean to string for JSON serialization
            if isinstance(is_good, bool):
                is_good = "Yes" if is_good else "No"
            returns_12m = target_stock.returns12M[i][1] if i < len(target_stock.returns12M) else 'N/A'
            data.append({
                "Row": i + 1,
                "Date": date,
                "EMA": ema_value,
                "ATH": ath_value,
                "IsGood": is_good,  # Uncomment if needed
                "Returns12M": returns_12m
            })
        return jsonify(data)
    else:
        return jsonify({"error": "Stock not found."}), 404

if __name__ == "__main__":
    app.run(debug=True, port=3001)
