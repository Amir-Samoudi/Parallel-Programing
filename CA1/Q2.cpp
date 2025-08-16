// Amir Hosein Samoudi     Id: 810100108
// Armin Ghasemi           Id: 810100198

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <intrin.h>
#include <xmmintrin.h>
#include <immintrin.h>

using namespace std;

int popcount(int x) {
    int count = 0;
    while (x) {
        count += x & 1;
        x >>= 1;
    }
    return count;
}


int main() {

    // Generate random numbers
    const int numRandomFloats = 1 << 20;
    vector<float> randomFloats(numRandomFloats);
    unsigned long long start, end;
    double s_t, p_t;
    srand(static_cast<unsigned int>(time(nullptr)));

    for (int i = 0; i < numRandomFloats; i++) {
        randomFloats[i] = static_cast<float>(rand()) / RAND_MAX;
    }


    // Serial
    start = __rdtsc();
    float s_sum = 0;
    float sum_1 = 0;
    float sum_2 = 0;
    float sum_3 = 0;
    float sum_4 = 0;

    for (int i = 0; i < numRandomFloats ; i += 4) {
        sum_1 += randomFloats[i];
    }
    for (int i = 1; i < numRandomFloats ; i += 4) {
        sum_2 += randomFloats[i];
    }
    for (int i = 2; i < numRandomFloats ;  i += 4) {
        sum_3 += randomFloats[i];
    }
    for (int i = 3; i < numRandomFloats ; i+=4) {
        sum_4 += randomFloats[i];
    }
    s_sum = sum_1 + sum_2 + sum_3 + sum_4;
    float s_m = s_sum / numRandomFloats;

    float s_sigma2 = 0;
    float sigma1 = 0;
    float sigma2 = 0;
    float sigma3 = 0;
    float sigma4 = 0;


    for (int i = 0; i < numRandomFloats; i+=4) {
        sigma1 += (randomFloats[i] - s_m) * (randomFloats[i] - s_m);
    }
    for (int i = 1; i < numRandomFloats ; i += 4) {
        sigma2 += (randomFloats[i] - s_m) * (randomFloats[i] - s_m);
    }
    for (int i = 2; i < numRandomFloats ; i += 4) {
        sigma3 += (randomFloats[i] - s_m) * (randomFloats[i] - s_m);
    }
    for (int i = 3; i < numRandomFloats ; i += 4) {
        sigma4 += (randomFloats[i] - s_m) * (randomFloats[i] - s_m);
    }

    s_sigma2 = sigma1 + sigma2;
    s_sigma2 += (sigma3 + sigma4);
    float s_v = s_sigma2 / numRandomFloats;
    float s_s = sqrtf(s_v);

    int s_o_count = 0;
    for (int i = 0; i < numRandomFloats; i++) {
        if ((randomFloats[i] - s_m) / s_s > 2.5)
        {
            s_o_count += 1;
        }
    }
    end = __rdtsc();
    s_t = end - start;

    // Parallel
    start = __rdtsc();
    __m128 sum_vec = _mm_setzero_ps();

    for (int i = 0; i < numRandomFloats; i += 4) {
        __m128 vec = _mm_loadu_ps(&randomFloats[i]);
        sum_vec = _mm_add_ps(sum_vec, vec);
    }

    sum_vec = _mm_hadd_ps(sum_vec, sum_vec);
    sum_vec = _mm_hadd_ps(sum_vec, sum_vec);
    float p_sum = _mm_cvtss_f32(sum_vec);
    float p_m = p_sum / numRandomFloats;


    __m128 variance_vec = _mm_setzero_ps();
    __m128 mean_vec = _mm_set1_ps(p_m);

    for (int i = 0; i < numRandomFloats; i += 4) {
        __m128 vec = _mm_loadu_ps(&randomFloats[i]);
        __m128 diff = _mm_sub_ps(vec, mean_vec);
        __m128 diff_sq = _mm_mul_ps(diff, diff);
        variance_vec = _mm_add_ps(variance_vec, diff_sq);
    }

    variance_vec = _mm_hadd_ps(variance_vec, variance_vec);
    variance_vec = _mm_hadd_ps(variance_vec, variance_vec);
    float p_v = _mm_cvtss_f32(variance_vec);
    p_v = p_v / numRandomFloats;
    float p_s = sqrtf(p_v);

    __m128 threshold_vec = _mm_set1_ps(2.5f);
    __m128 stddev_vec = _mm_set1_ps(p_s);

    int p_o_count = 0;

    for (int i = 0; i < numRandomFloats; i += 4) {
        __m128 vec = _mm_loadu_ps(&randomFloats[i]);
        __m128 diff = _mm_sub_ps(vec, mean_vec);
        __m128 normalized = _mm_div_ps(diff, stddev_vec);
        __m128 mask = _mm_cmpgt_ps(normalized, threshold_vec);


        int mask_int = _mm_movemask_ps(mask);
        p_o_count += popcount(mask_int);
    }
    end = __rdtsc();
    p_t = end - start;

    float SU = s_t / p_t;
    cout << "Speed-Up = " << SU << endl;
    cout << "Number of outliers (Paralell): " << p_o_count << endl;
    cout << "Number of outliers (Serial): " << s_o_count << endl;
    cout << "Clock cycles (Paralell): " << p_t << endl;
    cout << "Clock cycles (Serial): " << s_t << endl;
    cout << "mean (Paralell): " << p_m << endl;
    cout << "mean (Serial): " << s_m << endl;
    cout << "variance (Paralell): " << p_v << endl;
    cout << "variance (Serial): " << s_v << endl;
    return 0;
}
