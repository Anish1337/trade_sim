#include <iostream>
#include <format>
#include "src/data_loader.h"
#include "src/backtester.h"

int main() {
    std::cout << std::format("Quantitative Trading Simulator - Backtesting Engine\n\n");
    
    try {
        // Initialize backtester with SPY data and $100k initial capital
        std::string filename = "data/spy.csv";
        Backtester backtester(filename, 100000.0);
        
        // Create and configure SMA Crossover strategy
        // Short MA: 10 days, Long MA: 30 days
        SMACrossoverStrategy strategy(10, 30);
        std::cout << std::format("Running {} strategy...\n", strategy.get_name());
        
        // Execute backtest
        auto result = backtester.run_backtest(strategy);
        
        // Display comprehensive results
        result.print_summary();
        
        // Export detailed results for further analysis
        result.export_trades_csv("output/trades.csv");
        result.export_performance_csv("output/performance.csv");
        std::cout << std::format("\nResults exported to output/ directory\n");
        
        // Display market data summary
        const auto& market_data = backtester.get_market_data();
        auto valid_data = DataLoader::filter_valid_data(market_data);
        auto price_range = DataLoader::get_price_range(market_data);
        
        std::cout << std::format("\n=== Market Data Summary ===\n");
        std::cout << std::format("Valid data points: {}\n", std::ranges::distance(valid_data));
        std::cout << std::format("Price range: ${:.2f} - ${:.2f}\n", 
            price_range.first, price_range.second);
        std::cout << std::format("Data period: {} to {}\n", 
            market_data.back().date, market_data.front().date);
        
    } catch (const std::exception& e) {
        std::cout << std::format("Error: {}\n", e.what());
        return 1;
    }
    
    return 0;
}