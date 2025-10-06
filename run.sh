#!/bin/bash

# Build and run script for trade_sim
echo "Building trade simulator..."

g++ -std=c++20 -fconcepts -fcoroutines -g -Wall -Wextra \
    main.cpp \
    src/data_loader.cpp \
    src/backtester.cpp \
    src/strategies/sma_strategy.cpp \
    src/strategies/ema_strategy.cpp \
    src/strategies/mean_reversion_strategy.cpp \
    -o main

if [ $? -eq 0 ]; then
    echo "Build successful! Running..."
    ./main
else
    echo "Build failed!"
    exit 1
fi
