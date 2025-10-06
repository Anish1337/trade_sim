# Quantitative Trading Simulator v1.0

> **High-Performance C++20 Backtesting Engine** | **Modern Financial Engineering** | **Production-Ready Architecture**

A sophisticated quantitative trading simulator built with modern C++20, featuring advanced backtesting capabilities, multiple trading strategies, and comprehensive performance analytics on QQQM (NASDAQ 100 ETF) data.

## 🎯 **Key Highlights**

- **⚡ High Performance**: Microsecond execution times with optimized C++20
- **🧠 Modern Architecture**: Concepts, templates, and ranges for type-safe, maintainable code
- **📊 Multiple Strategies**: SMA, EMA, and Mean Reversion with Bollinger Bands
- **📈 Comprehensive Analytics**: Sharpe ratio, drawdown, win rate, and detailed trade logs
- **🔧 Production Ready**: Professional build system, error handling, and data validation
- **📋 Export Capabilities**: CSV outputs for further analysis and reporting

## 🏗️ **Architecture Overview**

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   Data Loader   │───▶│   Backtester     │───▶│   Strategies    │
│   (CSV/Validation)│    │   (Portfolio Mgmt)│    │   (SMA/EMA/MR)  │
└─────────────────┘    └──────────────────┘    └─────────────────┘
         │                       │                       │
         ▼                       ▼                       ▼
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│ Market Data     │    │ Performance      │    │ Trade Execution │
│ (OHLCV + Val)   │    │ Metrics          │    │ (Buy/Sell/Comm) │
└─────────────────┘    └──────────────────┘    └─────────────────┘
```

## 🚀 **Quick Start**

### Prerequisites

- **C++20 Compiler** (GCC 11+ or Clang 14+)
- **Make** (for build system)
- **Linux/macOS** (tested on Arch Linux)

### Installation & Build

```bash
# Clone the repository
git clone https://github.com/yourusername/trade_sim.git
cd trade_sim

# Build the project
make clean && make

# Run the simulator
./main
```

### Alternative Build Methods

```bash
# Debug build
make debug

# Using build script
./build.sh release
./build.sh debug

# Quick build and run
./run.sh
```

## 📊 **Trading Strategies**

### 1. **Simple Moving Average (SMA) Crossover**

- **Parameters**: 10-day short, 30-day long windows
- **Logic**: Buy when short MA crosses above long MA, sell on opposite
- **Use Case**: Trend-following strategies

### 2. **Exponential Moving Average (EMA) Crossover**

- **Parameters**: 12-day short, 26-day long windows
- **Logic**: More responsive to recent price changes than SMA
- **Use Case**: Fast trend detection and momentum trading

### 3. **Mean Reversion (Bollinger Bands)**

- **Parameters**: 20-day lookback, 2.0 standard deviation multiplier
- **Logic**: Buy when price touches lower band, sell at upper band
- **Use Case**: Range-bound markets and contrarian strategies

## 📈 **Performance Metrics**

The simulator calculates comprehensive performance metrics:

- **Total Return**: Percentage gain/loss over the period
- **Sharpe Ratio**: Risk-adjusted return measure
- **Maximum Drawdown**: Largest peak-to-trough decline
- **Win Rate**: Percentage of profitable trades
- **Average Trade PnL**: Mean profit/loss per trade
- **Execution Time**: Microsecond-level performance tracking

## 🔧 **Technical Features**

### **Modern C++20 Implementation**

```cpp
// Type-safe strategy concepts
template<typename T>
concept TradingStrategy = requires(T strategy) {
    strategy.on_bar(std::declval<const MarketData&>(), std::declval<Portfolio&>());
};

// Efficient data processing with ranges
auto valid_data = data | std::views::filter([](const auto& day) { 
    return day.is_valid(); 
});
```

### **Robust Error Handling**

- **Data Validation**: OHLCV consistency checks
- **Exception Safety**: RAII and proper resource management
- **Graceful Failures**: Detailed error messages and recovery

### **Professional Build System**

- **Incremental Builds**: Only recompiles changed files
- **Multiple Targets**: Debug, release, and clean operations
- **Cross-Platform**: Makefile with compiler detection

## 📁 **Project Structure**

```
trade_sim/
├── src/
│   ├── backtester.h/cpp          # Core backtesting engine
│   ├── data_loader.h/cpp         # Data loading & validation
│   └── strategies/               # Trading strategy implementations
│       ├── sma_strategy.h/cpp
│       ├── ema_strategy.h/cpp
│       └── mean_reversion_strategy.h/cpp
├── data/
│   └── qqqm.csv                  # QQQM (NASDAQ 100 ETF) historical data
├── output/                       # Generated results and analytics
├── scripts/                      # Utility scripts
│   └── download_qqqm.py          # Data download script (one-time use)
├── main.cpp                      # Application entry point
├── Makefile                      # Professional build system
└── README.md                     # This file
```

## 📊 **Sample Output**

```
Quantitative Trading Simulator - Backtesting Engine

Loaded 1060 market data points
Running strategy comparison...

=== SMA Crossover Strategy ===
Total Return: 5.75%
Sharpe Ratio: 0.0465
Max Drawdown: 19.09%
Win Rate: 0.00%
Total Trades: 33
Execution Time: 55 microseconds

=== Strategy Comparison Summary ===
Strategy             Total Return Sharpe Ratio Max Drawdown Win Rate    
--------             ------------ ------------ ------------ --------    
SMA Crossover        5.75         0.0465       19.09        0.00        
EMA Crossover        6.55         0.0465       19.25        0.00        
Mean Reversion       2.29         0.0465       18.52        0.00        
```

## 🎯 **Use Cases**

- **Quantitative Research**: Strategy development and backtesting
- **Risk Management**: Portfolio analysis and drawdown assessment
- **Algorithm Development**: Testing trading algorithms before live deployment
- **Educational**: Learning quantitative finance and C++20 programming
- **Performance Analysis**: Comparing different trading strategies

## 🔮 **Roadmap (v2.0+)**

- **Live Data Integration**: Real-time market data feeds via WebSocket/HTTP APIs
- **C++ Market Data Extractor**: High-performance live data collection system
- **Concurrency**: Multi-threaded strategy execution and async data processing
- **Advanced Strategies**: Pairs trading, momentum, factor models
- **Risk Management**: Position sizing, stop-losses, portfolio optimization
- **Web Interface**: Real-time dashboard and monitoring
- **Database Integration**: Historical data storage and retrieval

## 🛠️ **Development**

### Building from Source

```bash
# Debug build with symbols
make debug

# Release build with optimizations
make

# Clean build artifacts
make clean
```

### Adding New Strategies

1. Create new strategy class implementing `TradingStrategy` concept
2. Add to build system in `Makefile`
3. Include in `main.cpp` for execution
