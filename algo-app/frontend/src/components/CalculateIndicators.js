import React, { useState } from 'react';
import axios from 'axios';
import { useNavigate } from 'react-router-dom';

function CalculateIndicators() {
  const [period, setPeriod] = useState('');
  const [athPeriod, setAthPeriod] = useState('');
  const [criteria, setCriteria] = useState('');
  const [stockName, setStockName] = useState('');
  const [loading, setLoading] = useState(false);
  const [calculated, setCalculated] = useState(false);
  const navigate = useNavigate();

  const onCalculate = async () => {
    setLoading(true);
    try {
      const response = await axios.post(`${process.env.REACT_APP_API_URL}/calculate`, {
        period,
        ath_period: athPeriod,
        criteria,
      });

      setCalculated(true);
      alert(response.data.message);
    } catch (error) {
      console.error(error);
      alert('Error calculating indicators');
    } finally {
      setLoading(false);
    }
  };

  const onNavigateToStock = () => {
    if (!stockName) {
      alert('Please enter a stock name.');
      return;
    }
    navigate(`/stock/${stockName}`);
  };

  return (
    <div className="flex flex-col items-center justify-center min-h-screen bg-gray-100 p-6">
      <div className="max-w-md w-full p-6 bg-white shadow-md rounded-lg">
        <h1 className="text-2xl font-semibold mb-4">Calculate Indicators</h1>

        {!calculated ? (
          <>
            <div className="mb-4">
              <label className="block text-gray-700 font-medium mb-1" htmlFor="period">Period</label>
              <input
                id="period"
                type="number"
                placeholder="Period"
                value={period}
                onChange={(e) => setPeriod(e.target.value)}
                className="block w-full text-sm text-gray-700 border border-gray-300 rounded-lg shadow-sm focus:outline-none focus:ring-indigo-500 focus:border-indigo-500"
              />
            </div>

            <div className="mb-4">
              <label className="block text-gray-700 font-medium mb-1" htmlFor="athPeriod">ATH Period</label>
              <input
                id="athPeriod"
                type="number"
                placeholder="ATH Period"
                value={athPeriod}
                onChange={(e) => setAthPeriod(e.target.value)}
                className="block w-full text-sm text-gray-700 border border-gray-300 rounded-lg shadow-sm focus:outline-none focus:ring-indigo-500 focus:border-indigo-500"
              />
            </div>

            <div className="mb-4">
              <label className="block text-gray-700 font-medium mb-1" htmlFor="criteria">Criteria</label>
              <input
                id="criteria"
                type="number"
                placeholder="Criteria"
                value={criteria}
                onChange={(e) => setCriteria(e.target.value)}
                className="block w-full text-sm text-gray-700 border border-gray-300 rounded-lg shadow-sm focus:outline-none focus:ring-indigo-500 focus:border-indigo-500"
              />
            </div>

            <button
              onClick={onCalculate}
              disabled={loading}
              className={`w-full py-2 px-4 font-semibold text-white rounded-lg shadow-md focus:outline-none focus:ring-2 focus:ring-offset-2 ${
                loading
                  ? 'bg-gray-400 cursor-not-allowed'
                  : 'bg-indigo-600 hover:bg-indigo-700'
              }`}
            >
              {loading ? 'Calculating...' : 'Calculate'}
            </button>
          </>
        ) : (
          <div className="mb-4">
            <label className="block text-gray-700 font-medium mb-1" htmlFor="stockName">Stock Name</label>
            <input
              id="stockName"
              type="text"
              placeholder="Enter stock name"
              value={stockName}
              onChange={(e) => setStockName(e.target.value)}
              className="block w-full text-sm text-gray-700 border border-gray-300 rounded-lg shadow-sm focus:outline-none focus:ring-indigo-500 focus:border-indigo-500"
            />
            <button
              onClick={onNavigateToStock}
              className="w-full py-2 px-4 mt-4 font-semibold text-white rounded-lg shadow-md bg-indigo-600 hover:bg-indigo-700 focus:outline-none focus:ring-2 focus:ring-offset-2"
            >
              Go to Stock
            </button>
          </div>
        )}
      </div>
    </div>
  );
}

export default CalculateIndicators;
