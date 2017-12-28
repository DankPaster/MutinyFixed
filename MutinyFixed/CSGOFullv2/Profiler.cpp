#include "Profiler.h"
#include "CSGO_HX.h"

std::vector<ProfStats*>ProfiledFunctions;

ProfStats* StartProfiling(char* name)
{
	ProfStats *statistic = nullptr;
	for (auto stat : ProfiledFunctions)
	{
		if (!strcmp(stat->name, name))
		{
			statistic = stat;
			break;
		}
	}

	if (!statistic)
	{
		statistic = new ProfStats{ name, 0, 0, 0, 0 };
		ProfiledFunctions.push_back(statistic);
	}

	statistic->starttime = QPCTime();
	return statistic;
}

void EndProfiling(ProfStats* statistic)
{
	double endtime = QPCTime();
	statistic->numcalls++;
	statistic->totaltime += endtime - statistic->starttime;
	if (statistic->numcalls == PROFILE_CALLS)
	{
		statistic->lastresult = statistic->totaltime;
		statistic->numcalls = 0;
		statistic->totaltime = 0;
	}
}