#include "BitsReader.h"

BitsReader::BitsReader(const char * fileName)
	: bufferSize(0), buffer(0)
{
	inFile.open(fileName, std::ios::binary | std::ios::in);
}

BitsBuffer BitsReader::Read(int bitsNumber)
{
	while (bufferSize < bitsNumber)
	{
		unsigned char readBuff;
		inFile.read((char*)&readBuff, 1);

		this->buffer <<= 8;
		buffer |= readBuff;
		bufferSize += 8;
	}

	BitsBuffer res = this->buffer;
	int rest = bufferSize - bitsNumber;
	res >>= rest;

	rest = 32 - bufferSize + bitsNumber;
	if (rest == 32)
	{
		this->buffer = 0;
	}
	else
	{
		this->buffer <<= rest;
		this->buffer >>= rest;
	}
	bufferSize -= bitsNumber;

	return res;
}

BitsReader::~BitsReader()
{
	inFile.close();
}
