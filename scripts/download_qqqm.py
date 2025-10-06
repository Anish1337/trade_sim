#!/usr/bin/env python3
"""
Download QQQM historical data using yfinance
"""

import yfinance as yf
import pandas as pd
from datetime import datetime

def download_qqqm_data():
    """Download QQQM historical data and save to CSV"""
    print("Downloading QQQM historical data...")
    
    # Download QQQM data from 2000 to present
    qqqm = yf.download('QQQM', start='2000-01-01', end='2024-12-31')
    
    if qqqm.empty:
        print("Error: No data downloaded. QQQM might not be available for this date range.")
        return False
    
    # Reset index to make Date a column
    qqqm.reset_index(inplace=True)
    
    # Rename columns to match expected format
    qqqm.columns = ['Date', 'Open', 'High', 'Low', 'Close', 'Volume']
    
    # Format date as string (MM/DD/YYYY)
    qqqm['Date'] = qqqm['Date'].dt.strftime('%m/%d/%Y')
    
    # Save to CSV
    qqqm.to_csv('data/qqqm.csv', index=False)
    
    print(f"Downloaded {len(qqqm)} data points")
    print(f"Date range: {qqqm['Date'].iloc[-1]} to {qqqm['Date'].iloc[0]}")
    print(f"Price range: ${qqqm['Close'].min():.2f} - ${qqqm['Close'].max():.2f}")
    print(f"Average volume: {qqqm['Volume'].mean():,.0f}")
    
    return True

if __name__ == "__main__":
    success = download_qqqm_data()
    if success:
        print("\nQQQM data downloaded successfully!")
        print("Saved to: data/qqqm.csv")
    else:
        print("\nFailed to download QQQM data")
