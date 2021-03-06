#include "definitions.h"
#include "path-oram/storage-adapter.hpp"
#include "utility.hpp"

#include <boost/program_options.hpp>
#include <iomanip>
#include <iostream>

using namespace std;
using namespace DPORAM;

namespace po = boost::program_options;

int main(int argc, char* argv[])
{
	po::options_description desc("Redis overhead macro benchmark", 120);
	desc.add_options()("help,h", "produce help message");
	desc.add_options()("redis", po::value<string>()->default_value("tcp://127.0.0.1:6379"), "Redis host to use");
	desc.add_options()("encrypt", po::value<bool>()->default_value(true), "enable encryption for storage provider");
	desc.add_options()("startFrom", po::value<number>()->default_value(1), "the experiment ID to start from");

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		cout << desc << "\n";
		exit(1);
	}

	try
	{
		setlocale(LC_ALL, "en_US.utf8");
		locale loc("en_US.UTF-8");
		std::wcout.imbue(loc);
	}
	catch (...)
	{
		wcerr << L"Could not set locale: en_US.UTF-8" << endl;
	}

	cout << "REDIS MACRO BENCHMARK"
		 << endl
		 << endl
		 << "`";
	for (auto i = 0; i < argc; i++)
	{
		cout << argv[i] << " ";
	}
	cout << "`"
		 << endl
		 << endl;

	if (!vm["encrypt"].as<bool>())
	{
		cout << "**Encryption disabled**" << endl;
		PathORAM::__blockCipherMode = PathORAM::BlockCipherMode::NONE;
	}

	cout << "| Experiment ID | Timestamp           | Record size | Number of buckets  | Batch size | SET time | GET time |" << endl;
	cout << "| :------------ | :------------------ | :---------: | -----------------: | ---------: | -------: | -------: |" << endl;

	auto experimentId = 0uLL;

	for (auto&& recordBytes : vector<number>{256, 1024, 16 * 1024, 512 * 1024})
	{
		for (auto&& requestsNumber : vector<number>{10, 100, 1000, 10000, 100000})
		{
			for (auto&& batchSize : vector<number>{0, 1, 100, 10000})
			{
				experimentId++;
				if (batchSize < 1000 && batchSize != 0 && requestsNumber > 1000)
				{
					// too slow with small batches
					continue;
				}
				if (requestsNumber * recordBytes > 1024 * 1024 * 1024 && (batchSize == 0 || batchSize > 1000))
				{
					// too large requests
					continue;
				}
				if (experimentId < vm["startFrom"].as<number>())
				{
					// skip until startFrom
					continue;
				}

				auto storage = make_unique<PathORAM::RedisStorageAdapter>(requestsNumber, recordBytes, bytes(), vm["redis"].as<string>(), true, 1, batchSize);

				auto payload = bytes();
				payload.resize(recordBytes, 0x13);

				vector<pair<const number, vector<pair<number, bytes>>>> setRequests;
				vector<number> getRequests;
				vector<pair<number, bytes>> getResponse;
				setRequests.reserve(requestsNumber);
				getRequests.reserve(requestsNumber);
				for (auto i = 0uLL; i < requestsNumber; i++)
				{
					setRequests.push_back({i, vector<pair<number, bytes>>{{i, payload}}});
					getRequests.push_back(i);
				}

				auto startTimestamp = chrono::steady_clock::now();

				storage->set(boost::make_iterator_range(setRequests.begin(), setRequests.end()));

				auto setTimestamp = chrono::steady_clock::now();

				storage->get(getRequests, getResponse);

				auto getTimestamp = chrono::steady_clock::now();

				auto setOverhead = chrono::duration_cast<chrono::nanoseconds>(setTimestamp - startTimestamp).count();
				auto getOverhead = chrono::duration_cast<chrono::nanoseconds>(getTimestamp - setTimestamp).count();

				auto t = time(nullptr);
				std::wcout
					<< "| "
					<< setw(13) << experimentId
					<< " | "
					<< put_time(localtime(&t), L"%d/%m/%Y %H:%M:%S")
					<< " | "
					<< setw(11) << bytesToString(recordBytes)
					<< " | "
					<< setw(18) << requestsNumber
					<< " | "
					<< setw(10) << batchSize
					<< " | "
					<< setw(8) << timeToString(setOverhead)
					<< " | "
					<< setw(8) << timeToString(getOverhead)
					<< " |"
					<< endl;
			}
		}
	}

	return 0;
}
