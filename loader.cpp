#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

struct MarketData {
    std::string date;
    double open;
    double high;
    double low;
    double close;
    long volume;
};

std::vector<MarketData> loadCSV(const std::string& filename) {
    std::vector<MarketData> data;
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Error opening file\n";
        return data;
    }

    // Skip header line
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string item;
        MarketData row;  // ✅ You need this

        std::getline(ss, row.date, ',');
        std::getline(ss, item, ','); row.open = std::stod(item);
        std::getline(ss, item, ','); row.high = std::stod(item);
        std::getline(ss, item, ','); row.low = std::stod(item);
        std::getline(ss, item, ','); row.close = std::stod(item);
        std::getline(ss, item, ','); row.volume = std::stol(item);

        data.push_back(row);  // ✅ Correct usage
    }

    return data;
}




int main() {
    std::string filename = "spy.csv";
    auto marketData = loadCSV(filename);

    std::cout << "Loaded " << marketData.size() << " rows\n";

    // Example: print first 5 rows
    for (int i = 0; i < 5 && i < marketData.size(); i++) {
        std::cout << marketData[i].date << " Close: " << marketData[i].close << "\n";
    }

    // Now feed marketData into your strategy/backtester logic

    return 0;
}
