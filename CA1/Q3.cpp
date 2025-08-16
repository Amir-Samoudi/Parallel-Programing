// Amir Hosein Samoudi     Id: 810100108
// Armin Ghasemi           Id: 810100198

#include <iostream>
#include <vector>
#include <ctime>
#include <immintrin.h> 
#include <intrin.h>

using namespace std;

int main() {
	int n = 1<<13;
	vector<char> data(n);
	for (int i = 0; i < n; ++i) {
		data[i] = 'a' + (rand() % 26);
	}

	double t_start = __rdtsc();
	vector<pair<char, int>> standardCompressed;
	char currentChar = data[0];
	int count = 1;

	for (size_t i = 1; i < data.size(); ++i) {
		if (data[i] == currentChar) {
			++count;
		}
		else {
			standardCompressed.push_back({ currentChar, count });
			currentChar = data[i];
			count = 1;
		}
	}
	standardCompressed.push_back({ currentChar, count });
	double t_end = __rdtsc();
	double ser = double(t_end - t_start);

	cout << "serial time " << ser << " cc" << endl;

	t_start = __rdtsc();
	vector<pair<char, int>> simdCompressed;
	int i = 0;
	__m128i chunk;
	char prevChar;
	int cnt;
	while (i + 15 < n) {
		chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&data[i]));
		i += 16;

		prevChar = data[i - 16];
		cnt = 1;

		char curChar;
		for (int j = 1; j < 16 && i - 16 + j < data.size(); ++j) {
			curChar = reinterpret_cast<char*>(&chunk)[j];
			if (curChar == prevChar) {
				++cnt;
			}
			else {
				simdCompressed.push_back({ prevChar, cnt });
				prevChar = curChar;
				cnt = 1;
			}
		}
		simdCompressed.push_back({ prevChar, cnt });
	}
	char curChar;
	
	while (i < data.size()) {
		curChar = data[i];
		int cnt = 1;
		++i;
		while (i < data.size() && data[i] == curChar) {
			++cnt;
			++i;
		}
		simdCompressed.push_back({ curChar, cnt });
	}
	t_end = __rdtsc();
	double par = double(t_end - t_start);
	cout << "paralell time" << par << " cc" << endl;

	vector<char> decodedStandard;
	for (const auto& pair : standardCompressed) {
		decodedStandard.insert(decodedStandard.end(), pair.second, pair.first);
	}
	cout << "Verification (Standard RLE):" << (decodedStandard == data ? "Success" : "Failure") << endl;

	vector<char> decodedSIMD;
	for (const auto& pair : simdCompressed) {
		decodedSIMD.insert(decodedSIMD.end(), pair.second, pair.first);
	}
	cout << "Verification (SIMD RLE): " << (decodedSIMD == data ? "Success" : "Failure") << endl;

	cout << "speedup " << ser / par << endl;

	double standardCompressionRatio = double(standardCompressed.size()) / data.size();
	double simdCompressionRatio = double(simdCompressed.size()) / data.size();

	cout << "compression ratio serial" << standardCompressionRatio << endl;

	return 0;
}