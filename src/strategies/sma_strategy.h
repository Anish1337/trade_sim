#pragma once
#include "../data_loader.h"
#include "../backtester.h"

// Simple moving average crossover strategy
class SMACrossoverStrategy : public Strategy {
private:
    int short_window;
    int long_window;
    std::vector<double> short_ma;
    std::vector<double> long_ma;
    
public:
    SMACrossoverStrategy(int short_w = 10, int long_w = 30) 
        : short_window(short_w), long_window(long_w) {}
    
    ~SMACrossoverStrategy() override = default;
    
    void on_bar(const MarketData& bar, Portfolio& portfolio) override;
    std::string get_name() const override;
};
