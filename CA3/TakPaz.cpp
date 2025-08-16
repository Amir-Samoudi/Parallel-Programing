#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <time.h>
#include <algorithm>
#include <queue>
#include <chrono>
#include <thread>
#include <cmath>

using namespace std;

double calculateMean(const vector<long long>& times) {
    long long sum = 0;
    for (auto time : times) {
        sum += time;
    }
    return (static_cast<double>(sum) / times.size())/1e9;
}

double calculateStandardDeviation(const vector<long long>& times, double mean) {
    double sum = 0;
    for (auto time : times) {
        sum += pow(time - mean, 2);
    }
    return (sqrt(sum / times.size()))/1e9;
}

int main()
{
    string nameLines;
    string numberLines;

    getline(cin, nameLines);
    getline(cin, numberLines);

    vector<string> names;
    istringstream stream1(nameLines);
    string name_;
    while (stream1 >> name_) {
        names.push_back(name_);
    }

    vector<string> numbers_str;
    istringstream stream2(numberLines);
    string number_;
    while (stream2 >> number_) {
        numbers_str.push_back(number_);
    }

    vector<int> numbers;

    numbers.reserve(numbers_str.size());

    
    transform(numbers_str.begin(), numbers_str.end(),back_inserter(numbers),[](const std::string& str) { return std::stoi(str); });

    vector<long long> times;
    int ovenCap = 10;

    queue<pair<string, int>> orderQueue;

    for (size_t i = 0; i < names.size(); ++i) {
        orderQueue.push({ names[i], numbers[i] });
    }

    cout << "Oven capacity: " << ovenCap << endl;

    auto globalStart = chrono::high_resolution_clock::now();

    while (!orderQueue.empty()) {
        auto order = orderQueue.front();
        orderQueue.pop();
        string name = order.first;
        int orderCount = order.second;
        cout << "receive " << name << "'s order" << endl;
        auto start = chrono::high_resolution_clock::now();
        while (orderCount > 0) {
            if (orderCount > 0) {
                orderCount -= min(ovenCap, orderCount);
                this_thread::sleep_for(chrono::seconds(2));
            }
        }
        times.push_back(chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start).count());
        cout << "prepare " << name << "'s order" << endl;
    }
    double mean = calculateMean(times);
    double stddev = calculateStandardDeviation(times, mean);

    cout << "Mean is: " << mean <<" min" << endl;
    cout<< "Variance is: " << stddev <<" min" << endl;
    

    return 0;
    
}