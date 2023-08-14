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
/// <summary>
/// This struct is passed to each thread at its starting point
/// </summary>
struct WorkerMetadata {
	/// <summary>
	///  The initial number the thread has to start its first buffer with
	/// <para>This works somehow like an index, it indicates which buffer ranges are for this thread</para>
	/// </summary>
	unit start;
	/// <summary>
	/// A pointer to the initial prime numbers
	/// </summary>
	unit* primes;
	
	/// <summary>
	/// the last prime of each segment, meant to be printed for decoration
	/// </summary>
	volatile unit* currentResult;
};
constexpr size_t WorkerCount = 3;
constexpr size_t SegmentLength = 1 << 20; /* 1MB segments */
volatile unit ResultList[WorkerCount]{};
std::array<std::thread*, WorkerCount> Workers;