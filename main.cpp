#include <iostream>
#include <format>
#include "src/data_loader.h"
#include "src/backtester.h"
#include "src/strategies/sma_strategy.h"
#include "src/strategies/ema_strategy.h"
#include "src/strategies/mean_reversion_strategy.h"

int main() {
    std::cout << std::format("Quantitative Trading Simulator - Backtesting Engine\n\n");
    
    try {
        // Load SPY data with 100k capital
        std::string filename = "data/spy.csv";
        Backtester backtester(filename, 100000.0);
        
        // Initialize strategies
        SMACrossoverStrategy smaStrategy(10, 30);
        EMACrossoverStrategy emaStrategy(12, 26);
        MeanReversionStrategy meanRevStrategy(20, 2.0);
        
        std::cout << std::format("Running strategy comparison...\n\n");
        
        // Run backtests for all strategies
        std::cout << std::format("=== {} Strategy ===\n", smaStrategy.get_name());
        auto smaResult = backtester.run_backtest(smaStrategy);
        smaResult.print_summary();
        
        std::cout << std::format("\n=== {} Strategy ===\n", emaStrategy.get_name());
        auto emaResult = backtester.run_backtest(emaStrategy);
        emaResult.print_summary();
        
        std::cout << std::format("\n=== {} Strategy ===\n", meanRevStrategy.get_name());
        auto meanRevResult = backtester.run_backtest(meanRevStrategy);
        meanRevResult.print_summary();
        
        // Export results for each strategy
        smaResult.export_trades_csv("output/sma_trades.csv");
        smaResult.export_performance_csv("output/sma_performance.csv");
        
        emaResult.export_trades_csv("output/ema_trades.csv");
        emaResult.export_performance_csv("output/ema_performance.csv");
        
        meanRevResult.export_trades_csv("output/mean_reversion_trades.csv");
        meanRevResult.export_performance_csv("output/mean_reversion_performance.csv");
        
        std::cout << std::format("\nResults exported to output/ directory\n");
        
        // Strategy comparison summary
        std::cout << std::format("\n=== Strategy Comparison Summary ===\n");
        std::cout << std::format("{:<20} {:<12} {:<12} {:<12} {:<12}\n", 
            "Strategy", "Total Return", "Sharpe Ratio", "Max Drawdown", "Win Rate");
        std::cout << std::format("{:<20} {:<12} {:<12} {:<12} {:<12}\n", 
            "--------", "------------", "------------", "------------", "--------");
        std::cout << std::format("{:<20} {:<12.2f} {:<12.4f} {:<12.2f} {:<12.2f}\n", 
            smaStrategy.get_name(), smaResult.total_return, smaResult.sharpe_ratio, 
            smaResult.max_drawdown, smaResult.win_rate);
        std::cout << std::format("{:<20} {:<12.2f} {:<12.4f} {:<12.2f} {:<12.2f}\n", 
            emaStrategy.get_name(), emaResult.total_return, emaResult.sharpe_ratio, 
            emaResult.max_drawdown, emaResult.win_rate);
        std::cout << std::format("{:<20} {:<12.2f} {:<12.4f} {:<12.2f} {:<12.2f}\n", 
            meanRevStrategy.get_name(), meanRevResult.total_return, meanRevResult.sharpe_ratio, 
            meanRevResult.max_drawdown, meanRevResult.win_rate);
        
        // Summarize market data
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