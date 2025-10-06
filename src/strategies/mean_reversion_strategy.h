#pragma once
#include "../data_loader.h"
#include "../backtester.h"
#include <vector>
#include <cmath>

// Mean reversion strategy using Bollinger Bands
class MeanReversionStrategy : public Strategy {
private:
    int lookback_period;
    double std_multiplier;
    std::vector<double> prices;
    double sma;
    double upper_band;
    double lower_band;
    bool initialized;
    
public:
    MeanReversionStrategy(int period = 20, double multiplier = 2.0) 
        : lookback_period(period), std_multiplier(multiplier), 
          sma(0.0), upper_band(0.0), lower_band(0.0), initialized(false) {}
    
    ~MeanReversionStrategy() override = default;
    
    void on_bar(const MarketData& bar, Portfolio& portfolio) override;
    std::string get_name() const override;
};
