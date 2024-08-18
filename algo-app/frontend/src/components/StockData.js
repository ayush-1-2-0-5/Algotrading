import React, { useState, useEffect, useRef } from 'react';
import { useParams, useNavigate } from 'react-router-dom';
import axios from 'axios';

function StockData() {
  const { name } = useParams();
  const navigate = useNavigate();
  const [data, setData] = useState([]);
  const [error, setError] = useState(null);
  const [newStockName, setNewStockName] = useState('');
  const tableRef = useRef(null);

  useEffect(() => {
    const fetchStockData = async () => {
      try {
        const response = await axios.get(`${process.env.REACT_APP_API_URL}/stock/${name}`);
        setData(response.data);
      } catch (error) {
        console.error(error);
        setError('Error fetching stock data');
      }
    };
    fetchStockData();
  }, [name]);

  const handleStockChange = () => {
    if (newStockName) {
      navigate(`/stock/${newStockName}`);
    }
  };

  const goToCalculatePage = () => {
    navigate('/calculate');
  };

  if (error) {
    return (
      <div className="min-h-screen flex items-center justify-center bg-gray-100">
        <div className="bg-red-100 border border-red-400 text-red-700 px-4 py-3 rounded relative" role="alert">
          <strong className="font-bold">Error!</strong>
          <span className="block sm:inline"> {error}</span>
        </div>
      </div>
    );
  }

  return (
    <div className="min-h-screen bg-gray-100 p-6 relative">
      <div className="absolute top-4 right-4 space-x-2">
        <button
          onClick={goToCalculatePage}
          className="py-2 px-4 font-semibold text-white rounded-lg shadow-md bg-gray-600 hover:bg-gray-700 focus:outline-none focus:ring-2 focus:ring-offset-2"
        >
          Back to enter period for EMA
        </button>
        <button
          onClick={handleStockChange}
          className="py-2 px-4 font-semibold text-white rounded-lg shadow-md bg-indigo-600 hover:bg-indigo-700 focus:outline-none focus:ring-2 focus:ring-offset-2"
        >
          Calculate for other stock
        </button>
      </div>

      <h1 className="text-2xl font-bold mb-4">Stock Data for {name}</h1>

      <div className="mb-4">
        <label className="block text-gray-700 font-medium mb-1" htmlFor="newStockName">Enter Stock Name</label>
        <input
          id="newStockName"
          type="text"
          value={newStockName}
          onChange={(e) => setNewStockName(e.target.value)}
          className="block w-full text-sm text-gray-700 border border-gray-300 rounded-lg shadow-sm focus:outline-none focus:ring-indigo-500 focus:border-indigo-500"
        />
      </div>

      {data.length === 0 ? (
        <p className="text-gray-600">No data available for this stock.</p>
      ) : (
        <div className="overflow-x-auto">
          <table
            ref={tableRef}
            className="min-w-full bg-white border border-gray-200 rounded-lg shadow-md"
          >
            <thead className="bg-gray-200">
              <tr>
                <th className="py-2 px-4 text-left text-gray-600 border-b">Row</th>
                <th className="py-2 px-4 text-left text-gray-600 border-b">Date</th>
                <th className="py-2 px-4 text-left text-gray-600 border-b">EMA</th>
                <th className="py-2 px-4 text-left text-gray-600 border-b">ATH</th>
                <th className="py-2 px-4 text-left text-gray-600 border-b">Returns12M</th>
              </tr>
            </thead>
            <tbody>
              {data.map((item) => (
                <tr key={item.Row} className="hover:bg-gray-50">
                  <td className="py-2 px-4 border-b">{item.Row}</td>
                  <td className="py-2 px-4 border-b">{item.Date}</td>
                  <td className="py-2 px-4 border-b">{item.EMA}</td>
                  <td className="py-2 px-4 border-b">{item.ATH}</td>
                  <td className="py-2 px-4 border-b">{item.Returns12M}</td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      )}
    </div>
  );
}

export default StockData;
