cmake_minimum_required(VERSION 3.20)
project(trade_sim)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(trade_sim main.cpp src/data_loader.cpp src/backtester.cpp)
