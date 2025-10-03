#include "data_loader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <format>

// Performance-optimized string cleaning
std::string clean_number(const std::string& str) {
    std::string cleaned;
    cleaned.reserve(str.size()); // Pre-allocate to avoid reallocations
    cleaned = str;
    
    // Single-pass removal of quotes and commas (more efficient)
    cleaned.erase(std::remove_if(cleaned.begin(), cleaned.end(), 
        [](char c) { return c == '"' || c == ','; }), cleaned.end());
    
    return cleaned;
}

// Enhanced error handling version with performance optimizations
LoadResult DataLoader::loadCSV_safe(const std::string& filename) {
    LoadResult result;
    result.status = LoadStatus::Success;
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        result.status = LoadStatus::FileNotFound;
        result.error_message = std::format("Could not open file: {}", filename);
        return result;
    }
    
    // Performance: Pre-allocate vector capacity
    result.data.reserve(1000); // SPY data typically ~250 rows
    
    std::string line;
    int line_number = 0;
    
    // Skip header line
    if (!std::getline(file, line)) {
        result.status = LoadStatus::EmptyData;
        result.error_message = "File is empty or has no header";
        return result;
    }
    line_number++;
    
    while (std::getline(file, line)) {
        line_number++;
        
        try {
            std::stringstream ss(line);
            std::string item;
            MarketData row;
            
            // Parse each field with error handling
            if (!std::getline(ss, row.date, ',')) {
                result.status = LoadStatus::ParseError;
                result.error_message = std::format("Parse error at line {}", line_number);
                return result;
            }
            
            // Parse prices with validation
            auto parse_price = [&](double& price, const std::string& field_name) -> bool {
                if (!std::getline(ss, item, ',')) return false;
                try {
                    price = std::stod(clean_number(item));
                    return price > 0; // Validate positive price
                } catch (...) {
                    return false;
                }
            };
            
            if (!parse_price(row.open, "open") || 
                !parse_price(row.high, "high") || 
                !parse_price(row.low, "low") || 
                !parse_price(row.close, "close")) {
                result.status = LoadStatus::InvalidPriceData;
                result.error_message = std::format("Invalid price data at line {}", line_number);
                return result;
            }
            
            // Parse volume
            if (!std::getline(ss, item, ',')) {
                result.status = LoadStatus::ParseError;
                result.error_message = std::format("Parse error at line {}", line_number);
                return result;
            }
            try {
                row.volume = std::stol(clean_number(item));
                if (row.volume <= 0) {
                    result.status = LoadStatus::InvalidPriceData;
                    result.error_message = std::format("Invalid volume at line {}", line_number);
                    return result;
                }
            } catch (...) {
                result.status = LoadStatus::ParseError;
                result.error_message = std::format("Volume parse error at line {}", line_number);
                return result;
            }
            
            // Validate market data integrity
            if (!row.is_valid()) {
                result.status = LoadStatus::InvalidPriceData;
                result.error_message = std::format("Invalid OHLC data at line {}", line_number);
                return result;
            }
            
            // Performance: Use move semantics
            result.data.push_back(std::move(row));
            
        } catch (const std::exception& e) {
            result.status = LoadStatus::ParseError;
            result.error_message = std::format("Exception at line {}: {}", line_number, e.what());
            return result;
        }
    }
    
    if (result.data.empty()) {
        result.status = LoadStatus::EmptyData;
        result.error_message = "No valid data found in file";
        return result;
    }
    
    return result;
}

std::vector<MarketData> DataLoader::loadCSV(const std::string& filename) {
    std::vector<MarketData> data;
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << "\n";
        return data;
    }

    // Skip header line
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string item;
        MarketData row;

        // Parse each field, handling quoted values
        std::getline(ss, row.date, ',');
        
        std::getline(ss, item, ','); 
        row.open = std::stod(clean_number(item));
        
        std::getline(ss, item, ','); 
        row.high = std::stod(clean_number(item));
        
        std::getline(ss, item, ','); 
        row.low = std::stod(clean_number(item));
        
        std::getline(ss, item, ','); 
        row.close = std::stod(clean_number(item));
        
        std::getline(ss, item, ','); 
        row.volume = std::stol(clean_number(item));

        data.push_back(row);
    }

    return data;
}

void DataLoader::print_summary(const std::vector<MarketData>& data) {
    if (data.empty()) {
        std::cout << "No data to summarize\n";
        return;
    }

    std::cout << std::format("=== Market Data Summary ===\n");
    std::cout << std::format("Total records: {}\n", data.size());
    std::cout << std::format("Date range: {} to {}\n", data.back().date, data.front().date);
    
    // Calculate price statistics
    double min_price = data[0].low;
    double max_price = data[0].high;
    double total_volume = 0;
    
    for (const auto& day : data) {
        min_price = std::min(min_price, day.low);
        max_price = std::max(max_price, day.high);
        total_volume += day.volume;
    }
    
    std::cout << std::format("Price range: ${:.2f} - ${:.2f}\n", min_price, max_price);
    std::cout << std::format("Average daily volume: {:.0f}\n", total_volume / data.size());
    std::cout << std::format("Latest close: ${:.2f}\n", data.front().close);
}

std::vector<MarketData> DataLoader::filter_by_date_range(
    const std::vector<MarketData>& data, 
    const std::string& start_date, 
    const std::string& end_date
) {
    std::vector<MarketData> filtered;
    
    for (const auto& day : data) {
        if (day.date >= start_date && day.date <= end_date) {
            filtered.push_back(day);
        }
    }
    
    return filtered;
}




