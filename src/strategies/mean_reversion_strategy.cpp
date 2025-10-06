#include "mean_reversion_strategy.h"
#include <algorithm>
#include <numeric>
#include <cmath>

void MeanReversionStrategy::on_bar(const MarketData& bar, Portfolio& portfolio) {
    // Add current price to price history
    prices.push_back(bar.close);
    
    // Keep only the lookback period
    if (prices.size() > static_cast<size_t>(lookback_period)) {
        prices.erase(prices.begin());
    }
    
    // Need enough data to calculate Bollinger Bands
    if (prices.size() < static_cast<size_t>(lookback_period)) {
        return;
    }
    
    // Calculate Simple Moving Average
    sma = std::accumulate(prices.begin(), prices.end(), 0.0) / prices.size();
    
    // Calculate Standard Deviation
    double variance = 0.0;
    for (double price : prices) {
        variance += (price - sma) * (price - sma);
    }
    double std_dev = std::sqrt(variance / prices.size());
    
    // Calculate Bollinger Bands
    upper_band = sma + (std_multiplier * std_dev);
    lower_band = sma - (std_multiplier * std_dev);
    
    // Mean reversion trading logic
    static bool in_position = false;
    
    // Buy signal: Price touches or goes below lower band (oversold)
    if (bar.close <= lower_band && !in_position) {
        if (portfolio.cash > bar.close * 100) {
            portfolio.buy(bar.date, "QQQM", bar.close, 100, 1.0);
            in_position = true;
        }
    }
    // Sell signal: Price touches or goes above upper band (overbought)
    else if (bar.close >= upper_band && in_position) {
        if (portfolio.position > 0) {
            long sell_qty = std::min(portfolio.position, 100L);
            portfolio.sell(bar.date, "QQQM", bar.close, sell_qty, 1.0);
            in_position = false;
        }
    }
    // Additional sell signal: Price returns to mean
    else if (bar.close >= sma && in_position) {
        if (portfolio.position > 0) {
            long sell_qty = std::min(portfolio.position, 100L);
            portfolio.sell(bar.date, "QQQM", bar.close, sell_qty, 1.0);
            in_position = false;
        }
    }
}

std::string MeanReversionStrategy::get_name() const {
    return "Mean Reversion";
}
