// Amir Hosein Samoudi     Id: 810100108
// Armin Ghasemi           Id: 810100198

#include <iostream>
#include <random>
#include <ctime>
#include <omp.h>
#include <intrin.h>
using namespace std;
double monteCarloPiSerial(int numPoints, double radius) {
    int pointsInsideCircle = 0;

    // Random number generator for serial computation
    mt19937 generator(std::random_device{}());
    uniform_real_distribution<double> distribution(-radius, radius);

    for (int i = 0; i < numPoints; ++i) {
        double x = distribution(generator);
        double y = distribution(generator);

        if (x * x + y * y <= radius * radius) {
            ++pointsInsideCircle;
        }
    }

    return 4.0 * pointsInsideCircle / numPoints;
}

double monteCarloPiParallel(int numPoints, double radius) {
    int pointsInsideCircle = 0;

#pragma omp parallel
    {
        // Create a thread-local random number generator
        mt19937 generator(std::random_device{}() + omp_get_thread_num());
        uniform_real_distribution<double> distribution(-radius, radius);
        int localCount = 0;

#pragma omp for
        for (int i = 0; i < numPoints; ++i) {
            double x = distribution(generator);
            double y = distribution(generator);

            if (x * x + y * y <= radius * radius) {
                ++localCount;
            }
        }

#pragma omp atomic
        pointsInsideCircle += localCount;
    }

    return 4.0 * pointsInsideCircle / numPoints;
}

int main() {
    int numPoints;
    double radius;

    cout << "Enter the number of points to generate: ";
    cin >> numPoints;

    cout << "Enter the radius of the circle: ";
    cin >> radius;

    // Measure serial execution time
    double startSerial = __rdtsc();
    double estimatedPiSerial = monteCarloPiSerial(numPoints, radius);
    double endSerial = __rdtsc();

    double serialTime = endSerial - startSerial;

    // Measure parallel execution time
    double startParallel = __rdtsc();
    double estimatedPiParallel = monteCarloPiParallel(numPoints, radius);
    double endParallel = __rdtsc();

    double parallelTime = endParallel - startParallel;

    // Calculate Speedup
    double speedup = serialTime / parallelTime;

    // Print results
    cout << "Estimated value of Pi (Serial): " << estimatedPiSerial << endl;
    cout << "Estimated value of Pi (Parallel): " << estimatedPiParallel << endl;
    cout << "Time taken (Serial): " << serialTime << " cc" << endl;
    cout << "Time taken (Parallel): " << parallelTime << " cc" << endl;
    cout << "Speedup: " << speedup << endl;

    return 0;
}
