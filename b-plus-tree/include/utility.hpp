#pragma once

#include "definitions.h"

#include <string>

namespace BPlusTree
{
	using namespace std;

	bytes fromText(string text, number BLOCK_SIZE);
	string toText(bytes data, number BLOCK_SIZE);

	bytes concat(int count, ...);
	bytes concatNumbers(int count, ...);
	vector<bytes> deconstruct(bytes data, vector<int> stops);
	bytes bytesFromNumber(number num);
	number numberFromBytes(bytes data);
}