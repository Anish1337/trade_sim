#pragma once
#include <string>
#include <vector>
#include <concepts>
#include <ranges>


template<typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template<typename T>
concept StringLike = std::convertible_to<T, std::string_view>;

template<typename T>
concept Container = requires(T t) {
    t.begin();
    t.end();
    t.size();
};

struct MarketData {
    std::string date;
    double open;
    double high;
    double low;
    double close;
    long volume;
    
    // Helper methods 
    double get_price_change() const { return close - open; }
    double get_price_change_pct() const { return (close - open) / open * 100.0; }
    double get_high_low_spread() const { return high - low; }
    double get_high_low_spread_pct() const { return (high - low) / low * 100.0; }
    
    
    template<Numeric T>
    T get_price_change_as() const { return static_cast<T>(close - open); }
    
    template<StringLike T>
    T get_date_as() const { return T{date}; }
    
    // Data validation 
    bool is_valid() const {
        return high >= low && 
               high >= open && 
               high >= close &&
               low <= open && 
               low <= close &&
               volume > 0 &&
               open > 0 && close > 0;
    }
};

// Error handling 
enum class LoadStatus { 
    Success, 
    FileNotFound, 
    InvalidFormat, 
    EmptyData,
    InvalidPriceData,
    ParseError
};

struct LoadResult {
    std::vector<MarketData> data;
    LoadStatus status;
    std::string error_message;
    
    bool is_success() const { return status == LoadStatus::Success; }
    const std::string& get_error() const { return error_message; }
};

class DataLoader {
public:
    // Enhanced error handling 
    static LoadResult loadCSV_safe(const std::string& filename);
    
    // Original version 
    static std::vector<MarketData> loadCSV(const std::string& filename);
    
    static void print_summary(const std::vector<MarketData>& data);
    static std::vector<MarketData> filter_by_date_range(
        const std::vector<MarketData>& data, 
        const std::string& start_date, 
        const std::string& end_date
    );
    
    template<Container T>
    static auto filter_valid_data(const T& data) {
        return data | std::views::filter([](const auto& day) { return day.is_valid(); });
    }
    
    template<Container T>
    static std::pair<double, double> get_price_range(const T& data) {
        if (data.empty()) return {0.0, 0.0};
        
        double min_price = data[0].close;
        double max_price = data[0].close;
        
        for (const auto& day : data) {
            min_price = std::min(min_price, day.close);
            max_price = std::max(max_price, day.close);
        }
        
        return {min_price, max_price};
    }
    
    template<Container T>
    static std::pair<long, long> get_volume_stats(const T& data) {
        if (data.empty()) return {0, 0};
        
        long min_volume = data[0].volume;
        long max_volume = data[0].volume;
        
        for (const auto& day : data) {
            min_volume = std::min(min_volume, day.volume);
            max_volume = std::max(max_volume, day.volume);
        }
        
        return {min_volume, max_volume};
    }
};
