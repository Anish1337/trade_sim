#include "backtester.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <format>
#include <ranges>
#include <chrono>
#include <cmath>

// Portfolio management implementation
void Portfolio::buy(const std::string& date, const std::string& symbol, double price, long quantity, double commission) {
    double cost = price * quantity + commission;
    if (cost <= cash) {
        cash -= cost;
        position += quantity;
        trades.emplace_back(date, symbol, "BUY", price, quantity, -commission, commission);
    }
}

void Portfolio::sell(const std::string& date, const std::string& symbol, double price, long quantity, double commission) {
    if (quantity <= position) {
        double proceeds = price * quantity - commission;
        cash += proceeds;
        position -= quantity;
        trades.emplace_back(date, symbol, "SELL", price, quantity, -commission, commission);
    }
}

void Portfolio::update_value(double current_price) {
    total_value = cash + (position * current_price);
}

double Portfolio::get_total_return() const {
    return (total_value - 100000.0) / 100000.0 * 100.0; // Assuming initial cash of 100k
}

double Portfolio::get_sharpe_ratio(const std::vector<MarketData>& data) const {
    if (data.size() < 2) return 0.0;
    
    // Calculate daily returns
    std::vector<double> returns;
    for (size_t i = 1; i < data.size(); ++i) {
        double daily_return = (data[i].close - data[i-1].close) / data[i-1].close;
        returns.push_back(daily_return);
    }
    
    if (returns.empty()) return 0.0;
    
    // Calculate mean and standard deviation
    double mean = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
    double variance = 0.0;
    for (double ret : returns) {
        variance += (ret - mean) * (ret - mean);
    }
    variance /= returns.size();
    double std_dev = std::sqrt(variance);
    
    return std_dev > 0 ? mean / std_dev : 0.0;
}

double Portfolio::get_max_drawdown() const {
    if (trades.empty()) return 0.0;
    
    double peak = 100000.0; 
    double max_dd = 0.0;
    double current_value = 100000.0;
    
    for (const auto& trade : trades) {
        if (trade.action == "BUY") {
            current_value -= trade.price * trade.quantity + trade.commission;
        } else {
            current_value += trade.price * trade.quantity - trade.commission;
        }
        
        if (current_value > peak) {
            peak = current_value;
        }
        
        double drawdown = (peak - current_value) / peak;
        if (drawdown > max_dd) {
            max_dd = drawdown;
        }
    }
    
    return max_dd * 100.0; // Return as percentage
}

double Portfolio::get_win_rate() const {
    if (trades.empty()) return 0.0;
    
    long winning_trades = 0;
    for (const auto& trade : trades) {
        if (trade.pnl > 0) {
            winning_trades++;
        }
    }
    
    return static_cast<double>(winning_trades) / trades.size() * 100.0;
}

// BacktestResult implementation
void BacktestResult::export_trades_csv(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << "\n";
        return;
    }
    
    file << "Date,Symbol,Action,Price,Quantity,PnL,Commission\n";
    for (const auto& trade : all_trades) {
        file << std::format("{},{},{},{:.2f},{},{:.2f},{:.2f}\n",
            trade.date, trade.symbol, trade.action, trade.price, 
            trade.quantity, trade.pnl, trade.commission);
    }
}

void BacktestResult::export_performance_csv(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << "\n";
        return;
    }
    
    file << "Metric,Value\n";
    file << std::format("Total Return,{:.2f}%\n", total_return);
    file << std::format("Annualized Return,{:.2f}%\n", annualized_return);
    file << std::format("Sharpe Ratio,{:.4f}\n", sharpe_ratio);
    file << std::format("Max Drawdown,{:.2f}%\n", max_drawdown);
    file << std::format("Win Rate,{:.2f}%\n", win_rate);
    file << std::format("Avg Trade PnL,{:.2f}\n", avg_trade_pnl);
    file << std::format("Execution Time,{} microseconds\n", execution_time.count());
}

void BacktestResult::print_summary() const {
    std::cout << std::format("\n=== Backtest Results ===\n");
    std::cout << std::format("Total Return: {:.2f}%\n", total_return);
    std::cout << std::format("Annualized Return: {:.2f}%\n", annualized_return);
    std::cout << std::format("Sharpe Ratio: {:.4f}\n", sharpe_ratio);
    std::cout << std::format("Max Drawdown: {:.2f}%\n", max_drawdown);
    std::cout << std::format("Win Rate: {:.2f}%\n", win_rate);
    std::cout << std::format("Avg Trade PnL: ${:.2f}\n", avg_trade_pnl);
    std::cout << std::format("Total Trades: {}\n", all_trades.size());
    std::cout << std::format("Execution Time: {} microseconds\n", execution_time.count());
}

// Backtester implementation
Backtester::Backtester(const std::string& data_file, double initial_cash) 
    : portfolio(initial_cash) {
    
    auto result = DataLoader::loadCSV_safe(data_file);
    if (!result.is_success()) {
        throw std::runtime_error(std::format("Failed to load data: {}", result.get_error()));
    }
    
    market_data = std::move(result.data);
    std::cout << std::format("Loaded {} market data points\n", market_data.size());
}



// SMACrossoverStrategy implementation
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
