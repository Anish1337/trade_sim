#include "sma_strategy.h"
#include <algorithm>
#include <numeric>

void SMACrossoverStrategy::on_bar(const MarketData& bar, Portfolio& portfolio) {
    // Calculate short-term moving average
    if (short_ma.size() >= static_cast<size_t>(short_window)) {
        short_ma.erase(short_ma.begin());
    }
    short_ma.push_back(bar.close);
    
    // Calculate long-term moving average
    if (long_ma.size() >= static_cast<size_t>(long_window)) {
        long_ma.erase(long_ma.begin());
    }
    long_ma.push_back(bar.close);
    
    // Only trade when we have enough data
    if (short_ma.size() < static_cast<size_t>(short_window) || 
        long_ma.size() < static_cast<size_t>(long_window)) {
        return;
    }
    
    // Calculate averages
    double short_avg = std::accumulate(short_ma.begin(), short_ma.end(), 0.0) / short_ma.size();
    double long_avg = std::accumulate(long_ma.begin(), long_ma.end(), 0.0) / long_ma.size();
    
    // Trading logic: Buy when short MA crosses above long MA, sell when it crosses below
    static double prev_short_avg = 0.0;
    static double prev_long_avg = 0.0;
    
    if (prev_short_avg > 0 && prev_long_avg > 0) {
        // Buy signal: short MA crosses above long MA
        if (prev_short_avg <= prev_long_avg && short_avg > long_avg) {
            if (portfolio.cash > bar.close * 100) { // Buy 100 shares if we have enough cash
                portfolio.buy(bar.date, "SPY", bar.close, 100, 1.0);
            }
        }
        // Sell signal: short MA crosses below long MA
        else if (prev_short_avg >= prev_long_avg && short_avg < long_avg) {
            if (portfolio.position > 0) {
                long sell_qty = std::min(portfolio.position, 100L);
                portfolio.sell(bar.date, "SPY", bar.close, sell_qty, 1.0);
            }
        }
    }
    
    prev_short_avg = short_avg;
    prev_long_avg = long_avg;
}

std::string SMACrossoverStrategy::get_name() const {
    return "SMA Crossover";
}
