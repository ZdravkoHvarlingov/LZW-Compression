#include "BitsWriter.h"

BitsWriter::BitsWriter(const char * fileName)
	: bufferSize(0), buffer(0)
{
	outFile.open(fileName, std::ios::binary | std::ios::out);
}

void BitsWriter::Write(BitsBuffer buff, int bitsNumber)
{
	this->buffer <<= bitsNumber;

	this->buffer |= buff;
	bufferSize += bitsNumber;

	PerformWrite();
}

void BitsWriter::Flush()
{
	int padding = 8 - bufferSize;
	buffer <<= padding;
	bufferSize = 8;

	PerformWrite();
}

void BitsWriter::Close()
{
	Flush();
	outFile.close();
}

BitsWriter::~BitsWriter()
{
	Flush();
	outFile.close();
}

void BitsWriter::PerformWrite()
{
	while (bufferSize >= 8)
	{ 
		unsigned char byteToWrite = buffer >> (bufferSize - 8);
		outFile.write((char*)&byteToWrite, sizeof(unsigned char));

		int unused = 32 - bufferSize + 8;
		if (unused == 32)
		{
			buffer = 0;
		}
		else
		{
			buffer <<= unused;
			buffer >>= unused;

		}
		bufferSize -= 8;
	}
}
