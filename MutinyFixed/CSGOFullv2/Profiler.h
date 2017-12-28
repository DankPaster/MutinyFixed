#pragma once
#include <vector>
#include "hDrawings.h"

struct ProfStats
{
	char *name;
	int numcalls;
	double starttime;
	double totaltime;
	double lastresult;
};

extern std::vector<ProfStats*>ProfiledFunctions;

ProfStats* StartProfiling(char* name);
void EndProfiling(ProfStats* statistic);
#define PROFILE_CALLS 500
#ifdef PROFILE
#define PROFILE_CALLS_STR "500"
#define START_PROFILING(name) StartProfiling(name);
#define END_PROFILING(statistic) EndProfiling(statistic);
#define DRAW_PROFILED_FUNCTIONS()\
DrawStringUnencrypted("Benchmark Results (" PROFILE_CALLS_STR " calls)", Vector2D(5, 5), ColorRGBA(255, 255, 255, 255), pFontSmall);\
int numdrawn = 0;\
char profilestr[128];\
int profiledcnt = ProfiledFunctions.size();\
for (int profileiter = 0; profileiter < profiledcnt; profileiter++)\
{\
ProfStats* stat = ProfiledFunctions.at(profileiter);\
sprintf(profilestr, "%s : Total Time: %f secs / %f msecs", stat->name, stat->lastresult, stat->lastresult * 1000.0);\
DrawStringUnencrypted(profilestr, Vector2D(5, 20 + (15 * numdrawn)), ColorRGBA(255, 255, 255, 255), pFontSmall);\
numdrawn++;\
}
#else
#define START_PROFILING(name) ( nullptr )
#define END_PROFILING(statistic)
#define DRAW_PROFILED_FUNCTIONS()
#endif