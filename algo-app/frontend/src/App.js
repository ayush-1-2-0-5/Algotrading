import React from 'react';
import { BrowserRouter as Router, Route, Routes } from 'react-router-dom';
import UploadFile from './components/UploadFile';
import CalculateIndicators from './components/CalculateIndicators';
import StockData from './components/StockData';

function App() {
  return (
    <Router>
      <div className="App">
        <Routes>
          <Route path="/upload" element={<UploadFile />} />
          <Route path="/calculate" element={<CalculateIndicators />} />
          <Route path="/stock/:name" element={<StockData />} />
        </Routes>
      </div>
    </Router>
  );
}

export default App;
