// SieveAlg.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#undef max
using namespace std;
struct threadData {
	long long start;
	long long* currentResult;
	
};
extern "C" unsigned long long __fastcall GetOffset(long long bufferStart, long long p);
constexpr unsigned long long BufferSize = 1 << 20;
const unsigned long long* primes;
std::vector<unsigned long long> primeBuff = { 3 };
std::queue<long long> q1, q2, q3, q4;
void SimpleSieve(long long limit) {
	{
		long long ts = 4;
		while (ts < limit)
		{
			ts *= 2;
		}
		limit = ts;
	}

	bool* pbuff = new bool[limit];
	memset(pbuff, -1, limit);
	long long sq = ceil(sqrt(limit));

	for (long long i = 2; i <= sq; i++)
	{
		bool* m = pbuff + (i * i);
		while (m < pbuff + limit)
		{
			*m = false;
			m += i;
		}
	}
	for (long long i = 4; i < limit; i++)
	{
		if (pbuff[i]) {
			primeBuff.push_back(i);
		}
	}
	primes = primeBuff.data();
	delete[] pbuff;
}

void sieve(threadData params) {
	long long bufferStart = params.start;
	auto q = params.currentResult;
	long long cycles = 0;
	bool* const buffer = new bool[BufferSize];
	while (true)
	{
		{
			unsigned long long* longbuf = (unsigned long long*)buffer;
			unsigned long long* longend = (unsigned long long*)(buffer + BufferSize);
			while (longbuf < longend)
			{
				*longbuf = 0xff00ff00ff00ff00;
				longbuf++;
			}
		}
		// we'll first assume that all numbers are prime, and then multiply different numbers and mark the product as a compound number		 // n / 8
		// the number end of the buffer indicates
		unsigned long long bufferEnd = bufferStart + BufferSize;
		unsigned long long endSqrt = ceil(sqrt(bufferEnd));
		// we'll only use prime numbers as p because if it was compound number, 4 for example, every product of it is also a product of 2
		// and already marked	
		for (const unsigned long long* i = primes;*i < endSqrt;i++)
		{
			unsigned long long p = *i; 
			
			// the smallest multiplier for p where m * p >= bufferStart (within buffer range)
			// except multipliers smaller than p would create duplicate compounds and slow down the program
			// for example if p = 11 and m was smaller than p like 2 for exmaple;
			// 22 would have already been checked with p = 2, m = 11 and we don't wanna do it again				
			bool* j = buffer + GetOffset(bufferStart,p);
			bool* bEnd = buffer + BufferSize;
			while (j < bEnd)
			{
				// mark the number as a compound
				*j = false;
				j += p * 2;
			}
		}
		// now all the compound numbers are marked with 'false' and the primes are left 'true'
		// we'll use this loop to print the first prime in the buffer
		// printing every prime we find would slow down the program so we'll just print one for every 8 million numbers going forward		
		if (cycles % 8 == 0) {
			size_t i = BufferSize - 1;
			while (!buffer[i])
			{
				i--;
			}
			*q = bufferStart + i;
			
		}
		cycles++;
		bufferStart += 3 * BufferSize;
	}
};
int main()
{

	cout.sync_with_stdio(false);
	// an array of bools, each bool indicates whether BufferStart + {its index} * 2 + 1 is prime or not
	// bitset and vector<bool> take up less space but they're a lot slower
	// so forget about changing this	 
	 //unsigned long long bufferStart = 0;	 
	SimpleSieve(1ll << 24);
	long long resultList[3];
	
	std::thread worker1(sieve, threadData{ 0,&resultList[0]});
	std::thread worker2(sieve, threadData{ BufferSize,&resultList[1]});
	std::thread worker3(sieve, threadData{ 2 * BufferSize,&resultList[2]});
	
	while (true) {	
		system("cls");
		long long* m = resultList;
			const char* comma = ", ";
			cout <<
				m[0] << comma <<
				m[1] << comma <<
				m[2] << endl ;			
			//Sleep(10);
	}
}