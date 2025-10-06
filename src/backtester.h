#pragma once
#include "data_loader.h"
#include <vector>
#include <string>
#include <functional>
#include <chrono>
#include <expected>

// Forward declarations
struct Trade;
struct Portfolio;
struct BacktestResult;

template<typename T>
concept TradingStrategy = requires(T strategy) {
    strategy.on_bar(std::declval<const MarketData&>(), std::declval<Portfolio&>());
};

// Trade execution record
struct Trade {
    std::string date;
    std::string symbol;
    std::string action;  // "BUY" or "SELL"
    double price;
    long quantity;
    double pnl;
    double commission;
    
    
    template<Numeric T>
    Trade(const std::string& d, const std::string& s, const std::string& a, T p, long q, T pnl_val = 0.0, T comm = 0.0)
        : date(d), symbol(s), action(a), price(static_cast<double>(p)), quantity(q), 
          pnl(static_cast<double>(pnl_val)), commission(static_cast<double>(comm)) {}
};

// Portfolio state
struct Portfolio {
    double cash;
    long position;  // Number of shares held
    double total_value;
    double initial_cash;
    std::vector<Trade> trades;
    
    Portfolio(double initial_cash = 100000.0) 
        : cash(initial_cash), position(0), total_value(initial_cash), initial_cash(initial_cash) {}
    
    // Portfolio management methods
    void buy(const std::string& date, const std::string& symbol, double price, long quantity, double commission = 0.0);
    void sell(const std::string& date, const std::string& symbol, double price, long quantity, double commission = 0.0);
    void update_value(double current_price);
    
    // Performance metrics
    double get_total_return() const;
    double get_sharpe_ratio(const std::vector<MarketData>& data) const;
    double get_max_drawdown() const;
    double get_win_rate() const;
};

// Backtest result with comprehensive metrics
struct BacktestResult {
    Portfolio final_portfolio;
    std::vector<Trade> all_trades;
    double total_return;
    double annualized_return;
    double sharpe_ratio;
    double max_drawdown;
    double win_rate;
    double avg_trade_pnl;
    std::chrono::microseconds execution_time;
    
    // Export methods for analysis
    void export_trades_csv(const std::string& filename) const;
    void export_performance_csv(const std::string& filename) const;
    void print_summary() const;
};

// Main backtesting engine
class Backtester {
private:
    std::vector<MarketData> market_data;
    Portfolio portfolio;
    double initial_cash;
    
public:
    // Constructor with data loading
    explicit Backtester(const std::string& data_file, double initial_cash = 100000.0);
    
    // Main backtesting method
    template<TradingStrategy T>
    BacktestResult run_backtest(T& strategy, const std::string& start_date = "", const std::string& end_date = "") {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Filter data by date range if specified
        std::vector<MarketData> filtered_data = market_data;
        if (!start_date.empty() || !end_date.empty()) {
            filtered_data = DataLoader::filter_by_date_range(market_data, start_date, end_date);
        }
        
        // Reset portfolio
        portfolio = Portfolio(initial_cash);
        
        // Run strategy on each bar
        for (const auto& bar : filtered_data) {
            strategy.on_bar(bar, portfolio);
            portfolio.update_value(bar.close);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto execution_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        
        // Calculate metrics
        BacktestResult result;
        result.final_portfolio = portfolio;
        result.all_trades = portfolio.trades;
        result.total_return = portfolio.get_total_return();
        result.annualized_return = result.total_return; // Simplified - would need actual time period
        result.sharpe_ratio = portfolio.get_sharpe_ratio(filtered_data);
        result.max_drawdown = portfolio.get_max_drawdown();
        result.win_rate = portfolio.get_win_rate();
        result.execution_time = execution_time;
        
        // Calculate average trade PnL
        if (!portfolio.trades.empty()) {
            double total_pnl = 0.0;
            for (const auto& trade : portfolio.trades) {
                total_pnl += trade.pnl;
            }
            result.avg_trade_pnl = total_pnl / portfolio.trades.size();
        }
        
        return result;
    }
    
    // Utility methods
    void set_commission(double commission_rate);
    void set_slippage(double slippage_rate);
    
    // Data access
    const std::vector<MarketData>& get_market_data() const { return market_data; }
    const Portfolio& get_portfolio() const { return portfolio; }
};

// Strategy interface
class Strategy {
public:
    virtual ~Strategy() = default;
    virtual void on_bar(const MarketData& bar, Portfolio& portfolio) = 0;
    virtual std::string get_name() const = 0;
};