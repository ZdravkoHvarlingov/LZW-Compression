#include <iostream>
#include <cstring>
#include <string>
#include "BitsWriter.h"
#include "BitsReader.h"
#include <fstream>

#define TABLE_SIZE 1024
#define EMPTY_STRING -1
#define EMPTY_NODE -1
#define STARTING_INDEX 257
#define COMPRESSION_END 256
#define NUMBER_OF_BITS 10

#define STARTING_BITS 9

typedef unsigned char byte;

struct TableInfo
{
	int prefix;
	byte symbol;
	//the first string index that uses this string as a prefix 
	//or the root of the ordered binary tree
	int first;

	int nextLeft, nextRight;
};

//12bits
static TableInfo table[TABLE_SIZE];
static int nextTableIndex = STARTING_INDEX;

void ResetTable()
{
	for (size_t i = 0; i < 256; i++)
	{
		table[i] = { EMPTY_STRING, byte(i), EMPTY_NODE, EMPTY_NODE, EMPTY_NODE };
	}
	nextTableIndex = STARTING_INDEX;
}

//Building unbalanced binary tree.
int AddToTable(int prefixIndex, byte symbol, int &insertIndex)
{
	if (prefixIndex == -1)
	{
		return symbol;
	}

	if (insertIndex == -1)		
	{
		table[nextTableIndex] = { prefixIndex, symbol, EMPTY_NODE, EMPTY_NODE, EMPTY_NODE };
		insertIndex = nextTableIndex;
		++nextTableIndex;
		if (nextTableIndex == TABLE_SIZE)
		{
			ResetTable();
		}

		return nextTableIndex - 1;
	}

	TableInfo crrRow = table[insertIndex];
	if (crrRow.symbol == symbol)
	{
		return insertIndex;
	}

	if (crrRow.symbol > symbol)
	{
		return AddToTable(prefixIndex, symbol, crrRow.nextLeft);
	}

	return AddToTable(prefixIndex, symbol, crrRow.nextRight);
}

void Compress(const char *inputFile, const char *outputFile)
{
	BitsWriter writer(outputFile);
	for (size_t i = 0; i < 256; i++)
	{
		table[i] = { EMPTY_STRING, byte(i), EMPTY_NODE, EMPTY_NODE, EMPTY_NODE };
	}
	nextTableIndex = STARTING_INDEX;

	std::ifstream inp(inputFile);
	int prefixIndex = EMPTY_STRING;
	char fileChar;
	while (inp.get(fileChar))
	{
		byte symbol = fileChar;
		int emptyStringInsertIndex = -1;
		int previousNextTableIndex = nextTableIndex;
		int ind = AddToTable(prefixIndex, symbol, prefixIndex == EMPTY_STRING ? emptyStringInsertIndex : table[prefixIndex].first);
		if (previousNextTableIndex == nextTableIndex)
		{
			prefixIndex = ind;
		}
		else
		{
			writer.Write(BitsBuffer(prefixIndex), NUMBER_OF_BITS);
			prefixIndex = (int)symbol;
		}
	}
	writer.Write(BitsBuffer(prefixIndex), NUMBER_OF_BITS);
	writer.Write(BitsBuffer(COMPRESSION_END), NUMBER_OF_BITS);
	writer.Flush();
}

std::string GetString(int tableIndex)
{
	if (tableIndex == EMPTY_STRING)
	{
		return "";
	}

	std::string symbol;
	symbol = table[tableIndex].symbol;
	return GetString(table[tableIndex].prefix) + symbol;
}

void Decompress(const char *inputFile, const char *outputFile)
{
	BitsReader reader(inputFile);
	std::ofstream out(outputFile);
	for (size_t i = 0; i < 256; i++)
	{
		table[i] = { EMPTY_STRING, byte(i), EMPTY_NODE, EMPTY_NODE, EMPTY_NODE };
	}
	nextTableIndex = STARTING_INDEX;

	int index;
	index = reader.Read(NUMBER_OF_BITS);
	if (index == COMPRESSION_END)
	{
		return;
	}
	out << GetString(index);
	int old = index;
	while ((index = reader.Read(NUMBER_OF_BITS)) != COMPRESSION_END)
	{
		if (index < nextTableIndex)
		{
			std::string indexString = GetString(index);
			out << indexString;
			table[nextTableIndex] = { old, byte(indexString[0]), EMPTY_NODE, EMPTY_NODE, EMPTY_NODE };
		}
		else
		{
			std::string oldString = GetString(old);
			table[nextTableIndex] = { old, byte(oldString[0]), EMPTY_NODE, EMPTY_NODE, EMPTY_NODE };
			oldString += oldString[0];
			out << oldString;
		}
		++nextTableIndex;
		if (nextTableIndex == TABLE_SIZE)
		{
			ResetTable();
		}
		old = index;
	}
}

int main()
{
	Compress("text.txt", "RESULT");
	Decompress("RESULT", "Decompressed.txt");

    return 0;
}

