// SieveAlg.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "Header.hpp"
// this is a simple sieve algorithm giving us around 1 million prime numbers to start with
std::vector<unit> SimpleSieve(unit limit) {
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
	// make sure is odd
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
	if (BufferSize % 8) throw std::exception("buffer size is not aligned");
	unit bufferStart = params.start;
	unit* primes = params.primes;
	auto q = params.currentResult;
	// an array of bools, each bool indicates whether BufferStart + {its index} is prime or not
	// bitset and vector<bool> take up less space but they're a lot slower
	// so forget about changing this	 
	bool* const buffer = new bool[BufferSize];	

	while (true)
	{
		// UPDATE: I've decided not to waste any more resources on marking the even numbers
		// since i can simply ignore them when iterating through
		// UPDATE 2: this loop initialized the buffer so it doesn't contain junk values
		// as well as marking the even numbers, so it can't be removed
		{
			unsigned long long* longbuf = (unsigned long long*)buffer;
			unsigned long long* longend = (unsigned long long*)(buffer + BufferSize);
			while (longbuf < longend)
			{
				*longbuf = 0x0100010001000100;
				longbuf++;
			}
		}
		
		// we'll first assume that all numbers are prime, and then multiply different numbers and mark the product as a compound number	
		unit bufferEnd = bufferStart + BufferSize; 	// the number end of the buffer indicates
		unit endSqrt = ceil(sqrt(bufferEnd));
		// we'll only use prime numbers as p because if it was compound number, 4 for example, every product of it would also be a product of 2
		// and already marked when iterating with p = 2		
		
		for (const unit* i = primes; *i < endSqrt; i++)
		{
			unit p = *i;
			auto div =  std::lldiv(bufferStart,p);
		
			unit m = std::max(p,static_cast<unit>(div.quot) + (div.rem != 0));		
			if ((m % 2) == 0) m++;
			bool* j = buffer + (p * m) - bufferStart;
			bool* bEnd = buffer + BufferSize;

			while (j < bEnd)
			{
				*j = false;
				j += 2 * p;
			}			
		}
		
	
	
		// the smallest multiplier for p where m * p >= bufferStart (other words its within buffer range)
		// the minimum for m is p because multipliers smaller than p
		//  would create duplicate compounds and slow down the program
		// for example p = 11 , m = 2 => p * m = 22
		// and also p = 2 , m = 11 => p * m = 22
		// and thats a duplicate
		
		// now all the compound numbers are marked with 'false' and the primes are left 'true'	
		size_t i = BufferSize - 1;
		while (!buffer[i])
		{
			i--;
		}
		*q = bufferStart + i;

		bufferStart += WorkerCount * BufferSize;
	}
}
// this function formats the numbers we want to print
// it works like this:
// 1000 -> 1,000
// 2100900 -> 2,100,900
// I know this is not the most efficient way possible
// but the performance doesn't really depend on this function
void format(string& x) {
	for (long long i = x.size() - 3; i > 0; i -= 3)
	{
		x.insert(x.begin() + i, ',');
	}
}
int main()
{ 
	size_t x = 0;
	auto primes = SimpleSieve(1ll << 24);	
	for (size_t i = 0; i < WorkerCount; i++)
	{
		Workers[i] = new thread(
			SegmentedSieve,
			WorkerMetadata{
				static_cast<unit>(i * BufferSize),
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