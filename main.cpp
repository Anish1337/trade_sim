#include <iostream>
#include <format>
#include "src/data_loader.h"
#include "src/backtester.h"

int main() {
    std::cout << std::format("🚀 C++20 Trading Simulator - Backtesting Engine Demo\n\n");
    
    try {
        // Initialize backtester with SPY data
        std::string filename = "data/spy.csv";
        Backtester backtester(filename, 100000.0); // $100k initial capital
        
        // Create and run SMA Crossover strategy
        SMACrossoverStrategy strategy(10, 30); // 10-day short, 30-day long MA
        
        std::cout << std::format("Running {} strategy...\n", strategy.get_name());
        
        // Run backtest
        auto result = backtester.run_backtest(strategy);
        
        // Display results
        result.print_summary();
        
        // Export results for analysis
        result.export_trades_csv("output/trades.csv");
        result.export_performance_csv("output/performance.csv");
        
        std::cout << std::format("\n✅ Results exported to output/ directory\n");
        
        
        std::cout << std::format("\n=== C++20 Features Demo ===\n");
        
        const auto& market_data = backtester.get_market_data();
        
        
        auto valid_data = DataLoader::filter_valid_data(market_data);
        std::cout << std::format("Valid data points: {}\n", std::ranges::distance(valid_data));
        
        
        auto price_range = DataLoader::get_price_range(market_data);
        std::cout << std::format("Price range: ${:.2f} - ${:.2f}\n", 
            price_range.first, price_range.second);
        
        
        const auto& latest_bar = market_data.front();
        auto price_as_int = latest_bar.get_price_change_as<int>();
        auto date_as_string = latest_bar.get_date_as<std::string>();
        
        std::cout << std::format("Latest price change as int: {}\n", price_as_int);
        std::cout << std::format("Date as string: {}\n", date_as_string);
        
    } catch (const std::exception& e) {
        std::cout << std::format("❌ Error: {}\n", e.what());
        return 1;
    }
    
    return 0;
}