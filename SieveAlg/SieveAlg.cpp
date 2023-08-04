// SieveAlg.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <Windows.h>
#include<list>
#undef max
using namespace std;
struct threadData {
	long long start;
	void(*method)(long long);
};

constexpr long long BufferSize = 1 << 20;
const long long* primes;
std::vector<long long> primeBuff;
std::list<long long> q1, q2, q3, q4;
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
	auto q = *params.method;
	long long cycles = 0;
	bool* const buffer = new bool[BufferSize];
	while (true)
	{
		unsigned long long* longbuf = (unsigned long long*)buffer;
		unsigned long long* longend = (unsigned long long*)(buffer + BufferSize);
		while (longbuf < longend)
		{
			*longbuf = 0xff00ff00ff00ff00;
			longbuf++;
		}
		// we'll first assume that all numbers are prime, and then multiply different numbers and mark the product as a compound number		 // n / 8
		// the number end of the buffer indicates
		unsigned long long bufferEnd = bufferStart + BufferSize;
		unsigned long long sq = ceil(sqrt(bufferEnd));
		// we'll only use prime numbers as p because if it was compound number, 4 for example, every product of it is also a product of 2
		// and already marked
		 // sqrt(n)			
		// P ^ (0,sqrt(n)) ~= sqrt(sqrt(n))		
		const long long* i = primes;
		for (unsigned long long p = 3; p < sq; p = *(i++))
		{

			// the smallest multiplier for p where m * p >= bufferStart (within buffer range)
			// except multipliers smaller than p would create duplicate compounds and slow down the program
			// for example if p = 11 and m was smaller than p like 2 for exmaple;
			// 22 would have already been checked with p = 2, m = 11 and we don't wanna do it again
			long long m = max(p, bufferStart / p + (bufferStart % p != 0) /* this is a faster version of ceil(bufferStart / p) */);
			bool* r = buffer + (m * p - bufferStart);
			bool* bEnd = buffer + BufferSize;
			while(r < bEnd)
			{

				// mark the number as a compound
				*r = false;
				r += p * 2;
			}
		}
		// now all the compound numbers are marked with 'false' and the primes are left 'true'
		// we'll use this loop to print the first prime in the buffer
		// printing every prime we find would slow down the program so we'll just print one for every 2 million numbers going forward		
		if (cycles % 4 == 0) {
			size_t i = 0;
			while (!buffer[i])
			{
				i++;
			}
			q(bufferStart + i);
		}
		cycles++;
		bufferStart += 4 * BufferSize;
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




	std::thread worker1(sieve, threadData{ 0,[](long long data) { 
		q1.push_back(data);
		}});
	std::thread worker2(sieve, threadData{ BufferSize,[](long long data) {
		q2.push_back(data);
		} });
	std::thread worker3(sieve, threadData{ 2 * BufferSize,[](long long data) {
		q3.push_back(data);
		} });

	auto print = [](long long data) {
		q4.push_back(data);
		static int x = 0;
		if (x++ % 10 == 0)
			while (q1.size() && q2.size() && q3.size() && q4.size()) {

				long long m[4]{ q1.front(),q2.front(),q3.front(),q4.front() };
				const char* comma = ", ";
				cout <<
					m[0] << comma <<
					m[1] << comma <<
					m[2] << comma <<
					m[3] << comma;

				q1.pop_front();
				q2.pop_front();
				q3.pop_front();
				q4.pop_front();
			}
	};
	//std::thread worker4(sieve, f{ 3 * BufferSize,&q4 });
	sieve(threadData{ 3 * BufferSize,print });

}