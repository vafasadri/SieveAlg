// SieveAlg.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "Header.hpp"
// this is a simple sieve algorithm giving us around 1 million prime numbers to start with
std::vector<unit> SimpleSieve(unit limit) {
	// turn limit into the closest power of 2
	{
		unit ts = 4;
		while (ts < limit)
		{
			ts *= 2;
		}	
		limit = ts;
	}
	std::vector<unit> primeBuff = {};
	bool* pbuff = new bool[limit];
	memset(pbuff, 1, limit);
	unit sq = ceil(sqrt(limit));

	for (unit i = 2; i <= sq; i++)
	{
		if (pbuff[i] == false) continue;
		primeBuff.push_back(i);
		bool* m = pbuff + (i * i);	
		while (m < pbuff + limit)
		{
			*m = false;
			m += i;
		}
	}
	unit i = sq;
	// make sure 'i' is odd
	if ((i & 1) == 0) i++;	
	for (; i < limit; i +=2)
	{		
		if (pbuff[i]) {
			primeBuff.push_back(i);
		}
	}
	delete[] pbuff;
	return primeBuff;
}

void SegmentedSieve(WorkerMetadata params) {
	if (SegmentLength % 8) throw std::exception("buffer size is not aligned");
	const unit* primes = params.primes;
	/// <summary>
	/// the number the zero'th index of buffer represents
	/// <para>it always follows this formula :   </para>
	/// <para> WorkerCount * SegmentLength * {CyclesPassed} + params.start</para>
	/// </summary>
	unit SegmentStart = params.start;
	auto q = params.currentResult;
	// an array of bools, each bool indicates whether BufferStart + {its index} is prime or not
	// bitset and vector<bool> take up 8 times less space but they're a lot slower
	// so forget about changing this	 
	bool* const buffer = new bool[SegmentLength];	

	while (true)
	{
		// UPDATE: I've decided not to waste any more resources on marking the even numbers
		// since i can simply ignore them when iterating through
		// UPDATE 2: this loop initialized the buffer as well, so it can't be removed
		{
			unsigned long long* longbuf = (unsigned long long*)buffer;
			unsigned long long* longend = (unsigned long long*)(buffer + SegmentLength);
			while (longbuf < longend)
			{
				*longbuf = 0x0100010001000100;
				longbuf++;
			}
		}
		
		// we'll first assume that all numbers are prime, and then multiply different numbers and mark the product as a compound number	
		unit bufferEnd = SegmentStart + SegmentLength; 	// the number end of the buffer indicates
		unit endSqrt = ceil(sqrt(bufferEnd));
		// we'll only use prime numbers as p because if it was compound number, 4 for example, every product of it would also be a product of 2
		// and already marked when iterating with p = 2		
		
		for (const unit* i = primes; *i < endSqrt; i++)
		{
			unit p = *i;
			auto div = std::lldiv(SegmentStart, p);
			// the smallest multiplier for p where m * p >= bufferStart (other words: its within buffer range)
			// the minimum for m is p because multipliers smaller than p
			// would create duplicate products and reduce performance
			// for example p = 11 , m = 2 -> p * m = 22
			// and also p = 2 , m = 11 -> p * m = 22
			// and thats a duplicate
			unit m = std::max(p,static_cast<unit>(div.quot) + (div.rem != 0));		
			if ((m % 2) == 0) m++;
			bool* j = buffer + (p * m) - SegmentStart;
			bool* bEnd = buffer + SegmentLength;

			while (j < bEnd)
			{
				*j = false;
				j += 2 * p;
			}			
		}			
		// now all the compound numbers are marked with 'false' and the primes are left 'true'	
		size_t i = SegmentLength - 1;
		while (!buffer[i])
		{
			i--;
		}
		*params.currentResult = SegmentStart + i;
		SegmentStart += WorkerCount * SegmentLength;
	}
}
// this function formats the numbers we want to print
// it works like this:
// 1000 -> 1,000
// 2100900 -> 2,100,900
// I know this is not the most efficient way possible
// but the performance doesn't really depend on this function
void format(string& str) {
	for (long long i = str.size() - 3; i > 0; i -= 3)
	{
		str.insert(str.begin() + i, ',');
	}
}
int main()
{ 
	size_t x = 0;
	auto primes = SimpleSieve(1ll << 24);	
	// starting the workers
	for (size_t i = 0; i < WorkerCount; i++)
	{
		Workers[i] = new thread(
			SegmentedSieve,
			WorkerMetadata{
				static_cast<unit>(i * SegmentLength),
				primes.data(),
				&ResultList[i]
			}
		);
		string s = "Worker " + to_string(i + 1) + ": ";
		x = std::max(s.length(), x);
		cout << s << endl;
	}
	cout.flush();
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);	
	// printing the results to console
	while (true) {
		COORD cord = { x,0 };

		for (auto& i : ResultList)
		{
			SetConsoleCursorPosition(console, cord);
			string d = to_string(i);
			format(d);
			cout << d;
			cord.Y++;
		}

	}
}