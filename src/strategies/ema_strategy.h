#pragma once
#include "../data_loader.h"
#include "../backtester.h"

// Exponential moving average crossover strategy
class EMACrossoverStrategy : public Strategy {
private:
    int short_window;
    int long_window;
    double short_ema;
    double long_ema;
    double short_alpha;
    double long_alpha;
    bool initialized;
    
    double calculate_alpha(int period) const {
        return 2.0 / (period + 1.0);
    }
    
public:
    EMACrossoverStrategy(int short_w = 12, int long_w = 26) 
        : short_window(short_w), long_window(long_w), 
          short_ema(0.0), long_ema(0.0), initialized(false) {
        short_alpha = calculate_alpha(short_window);
        long_alpha = calculate_alpha(long_window);
    }
    
    ~EMACrossoverStrategy() override = default;
    
    void on_bar(const MarketData& bar, Portfolio& portfolio) override;
    std::string get_name() const override;
};
