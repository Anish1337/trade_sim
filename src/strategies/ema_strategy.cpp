#include "ema_strategy.h"
#include <algorithm>

void EMACrossoverStrategy::on_bar(const MarketData& bar, Portfolio& portfolio) {
    if (!initialized) {
        // Initialize EMAs with first price
        short_ema = bar.close;
        long_ema = bar.close;
        initialized = true;
        return;
    }
    
    // Update EMAs using exponential smoothing
    short_ema = short_alpha * bar.close + (1.0 - short_alpha) * short_ema;
    long_ema = long_alpha * bar.close + (1.0 - long_alpha) * long_ema;
    
    // Trading logic: Buy when short EMA crosses above long EMA, sell when it crosses below
    static double prev_short_ema = 0.0;
    static double prev_long_ema = 0.0;
    
    if (prev_short_ema > 0 && prev_long_ema > 0) {
        // Buy signal: short EMA crosses above long EMA
        if (prev_short_ema <= prev_long_ema && short_ema > long_ema) {
            if (portfolio.cash > bar.close * 100) { // Buy 100 shares if we have enough cash
                portfolio.buy(bar.date, "QQQM", bar.close, 100, 1.0);
            }
        }
        // Sell signal: short EMA crosses below long EMA
        else if (prev_short_ema >= prev_long_ema && short_ema < long_ema) {
            if (portfolio.position > 0) {
                long sell_qty = std::min(portfolio.position, 100L);
                portfolio.sell(bar.date, "QQQM", bar.close, sell_qty, 1.0);
            }
        }
    }
    
    prev_short_ema = short_ema;
    prev_long_ema = long_ema;
}

std::string EMACrossoverStrategy::get_name() const {
    return "EMA Crossover";
}
