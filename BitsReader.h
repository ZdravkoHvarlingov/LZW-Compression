#pragma once
#include <fstream>

typedef unsigned int BitsBuffer;

class BitsReader
{
public:
	BitsReader(const char *fileName);

	//BitsNumber is maximum 16
	BitsBuffer Read(int bitsNumber);

	~BitsReader();
private:

	//In every moment, buffer will hold maximum 15 bits(write is performed on every 8 bits)
	int bufferSize;
	BitsBuffer buffer;
	std::ifstream inFile;
};
