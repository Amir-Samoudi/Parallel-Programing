#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>
#include <pthread.h>
#include <chrono>
#include <algorithm>
#include <thread>
#include <cmath>

const int WAIT_TIME = 1;

using namespace std;
using Clock = chrono::high_resolution_clock;
using ns = chrono::nanoseconds;

pthread_mutex_t ovenLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t resLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ovenCond = PTHREAD_COND_INITIALIZER;
vector<long int> times;


double calculate_mean(const vector<long int>& times) {
    if (times.empty()) {
        return 0.0;
    }

    long long sum = 0;
    for (const auto& time : times) {
        sum += time;
    }
    double mean_ns = static_cast<double>(sum) / times.size();
    return mean_ns / 1e9;
}

double calculate_stddev(const vector<long int>& times, double mean_ms) {
    if (times.empty()) {
        return 0.0;
    }

    double sum_of_squares = 0.0;
    for (const auto& time : times) {
        double diff = static_cast<double>(time) / 1e9 - mean_ms;
        sum_of_squares += diff * diff;
    }

    double variance = sum_of_squares / times.size();
    return sqrt(variance);
}

vector<string> split_string(const string &line) {
    vector<string> result;
    stringstream ss(line);
    string word;
    while (ss >> word) {
        result.push_back(word);
    }
    return result;
}

void parse_input(istream &input, int &num_bakers, vector<vector<string>> &baker_names, vector<vector<int>> &baker_orders) {
    string line;

    if (!getline(input, line) || line.empty()) {
        cerr << "Error: Number of bakers not provided.\n";
        exit(EXIT_FAILURE);
    }
    num_bakers = stoi(line);
    if (num_bakers <= 0) {
        cerr << "Number of bakers must be a positive integer.\n";
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < num_bakers; i++) {
        if (!getline(input, line) || line.empty()) {
            cerr << "Error: Missing names line for baker " << i + 1 << ".\n";
            exit(EXIT_FAILURE);
        }
        vector<string> names = split_string(line);
        baker_names.push_back(names);

        if (!getline(input, line) || line.empty()) {
            cerr << "Error: Missing orders line for baker " << i + 1 << ".\n";
            exit(EXIT_FAILURE);
        }
        vector<string> order_strings = split_string(line);
        vector<int> orders;
        for (const string &order : order_strings) {
            orders.push_back(stoi(order));
        }
        baker_orders.push_back(orders);
    }
}

struct BakerData {
    int id;
    vector<string> names;
    vector<int> orders;
};


int oven_capacity = 0;
int oven_capacity_used = 0;


void* baker_thread(void* arg) {
    BakerData* data = (BakerData*)arg;

    for (size_t i = 0; i < data->names.size(); i++) {
        auto start = Clock::now();

        while (data->orders[i] > 0) {
            pthread_mutex_lock(&ovenLock);

            while (oven_capacity_used == oven_capacity) {
                pthread_cond_wait(&ovenCond, &ovenLock);
            }

            int breads_to_bake = min(data->orders[i], oven_capacity - oven_capacity_used);
            oven_capacity_used += breads_to_bake;
            data->orders[i] -= breads_to_bake;

            cout << "Baker " << data->id << " is baking " << breads_to_bake << " breads for " 
                 << data->names[i] << " (" << data->orders[i] << " remaining).\n";

            pthread_mutex_unlock(&ovenLock);
            this_thread::sleep_for(chrono::seconds(WAIT_TIME));

            pthread_mutex_lock(&ovenLock); 
            oven_capacity_used -= breads_to_bake;

            pthread_cond_signal(&ovenCond);
            pthread_mutex_unlock(&ovenLock);
        }

        pthread_mutex_lock(&resLock);
        times.push_back(chrono::duration_cast<ns>(Clock::now() - start).count());
        pthread_mutex_unlock(&resLock);
    }

    return NULL;
}

int main() {
    int num_bakers;
    vector<vector<string>> baker_names;
    vector<vector<int>> baker_orders;

    cout << "Enter the number of bakers and their customer data:\n";
    parse_input(cin, num_bakers, baker_names, baker_orders);

    oven_capacity = num_bakers * 10;

    cout << "Number of bakers: " << num_bakers << "\n";
    cout << "Oven capacity: " << oven_capacity << " breads\n";

    vector<pthread_t> threads(num_bakers);
    vector<BakerData> baker_data(num_bakers);

    for (int i = 0; i < num_bakers; i++) {
        baker_data[i] = {i + 1, baker_names[i], baker_orders[i]};
        pthread_create(&threads[i], NULL, baker_thread, &baker_data[i]);
    }

    for (int i = 0; i < num_bakers; i++) {
        pthread_join(threads[i], NULL);
    }

    // cout << "Baking times(ns):\n";
    // for (const auto& time : times) {
    //     cout << time << "\n";
    // }

    double mean_ms = calculate_mean(times);
    cout << "Mean time: " << mean_ms*(2/WAIT_TIME) << " min\n";

    double stddev_ms = calculate_stddev(times, mean_ms);
    cout << "Standard deviation: " << stddev_ms*(2/WAIT_TIME) << " min\n";

    return 0;
}
