#!/bin/bash

# Build script for trade_sim
# Usage: ./build.sh [debug|release]

set -e  # Exit on any error

BUILD_TYPE=${1:-release}

if [ "$BUILD_TYPE" = "debug" ]; then
    echo "Building in DEBUG mode..."
    g++ -std=c++20 -fconcepts -fcoroutines -g -Wall -Wextra \
        main.cpp \
        src/data_loader.cpp \
        src/backtester.cpp \
        src/strategies/sma_strategy.cpp \
        src/strategies/ema_strategy.cpp \
        src/strategies/mean_reversion_strategy.cpp \
        -o main
else
    echo "Building in RELEASE mode..."
    g++ -std=c++20 -fconcepts -fcoroutines -O2 -Wall -Wextra \
        main.cpp \
        src/data_loader.cpp \
        src/backtester.cpp \
        src/strategies/sma_strategy.cpp \
        src/strategies/ema_strategy.cpp \
        src/strategies/mean_reversion_strategy.cpp \
        -o main
fi

echo "Build successful! Run with: ./main"
