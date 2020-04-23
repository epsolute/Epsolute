#include "utility.hpp"

namespace DPORAM
{
	using namespace std;

	vector<pair<number, number>> BRC(number fanout, number from, number to)
	{
		vector<pair<number, number>> result;
		int level = 0; // leaf-level, bottom

		do
		{
			// move FROM to the right withing the closest parent, but no more than TO
			while (from % fanout != 0 && from < to)
			{
				result.push_back({level, from});
				from++;
			}

			// move TO to the left withing the closest parent, but no more than FROM
			while (to % fanout != fanout - 1 && from < to)
			{
				result.push_back({level, to});
				to--;
			}

			// after we moved FROM and TO towrds each other they may or may not meet
			if (from != to)
			{
				// if they have not met, we climb one more level
				from /= fanout;
				to /= fanout;

				level++;
			}
			else
			{
				// otherwise we added this last node to which both FROM and TO are pointing, and return
				result.push_back({level, from});
				return result;
			}
		} while (true);
	}
}