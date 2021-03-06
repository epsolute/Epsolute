#pragma once

#include "definitions.h"

#include <string>

namespace DPORAM
{
	using namespace std;

	tuple<number, number, number, number> padToBuckets(pair<number, number> query, number min, number max, number buckets);

	number optimalMu(double beta, number k, number N, double epsilon, number levels, number orams);

	vector<pair<number, number>> BRC(number fanout, number from, number to, number maxLevel = ULONG_MAX);

	double sampleLaplace(double mu, double lambda);

	number gammaNodes(number m, double beta, number kZero);

	string exec(string cmd);

	wstring timeToString(long long time);

	wstring bytesToString(long long bytes);

	number salaryToNumber(string salary);

	double numberToSalary(number salary);

	string redishost(string host, int i);
}
