#pragma once
#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <string>
#include <array>
#include <Windows.h>
#undef max
#undef min
extern "C" {
	void __fastcall innerLoop(long long count, long long p, bool* m);
	long long __fastcall DivCeil(long long a, long long b);
	long long __fastcall ToUpper(long long a);
}
using namespace std;
using unit = unsigned long long;
struct WorkerMetadata {
	unit start;
	unit* primes;
	volatile unit* currentResult;
};
constexpr size_t WorkerCount = 3;
constexpr size_t BufferSize = 1 << 20;
volatile unit ResultList[WorkerCount]{};
std::array<std::thread*, WorkerCount> Workers;