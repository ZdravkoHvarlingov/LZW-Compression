#pragma once
#include <fstream>

typedef unsigned int BitsBuffer;

class BitsWriter
{
public:
	BitsWriter(const char *fileName);

	//BitsNumber is maximum 16
	void Write(BitsBuffer buff, int bitsNumber);
	void Flush();
	void Close();

	~BitsWriter();
private:

	void PerformWrite();

	//In every moment, buffer will hold maximum 7 bits(write is performed on every 8 bits)
	int bufferSize;
	BitsBuffer buffer;
	std::ofstream outFile;
};
