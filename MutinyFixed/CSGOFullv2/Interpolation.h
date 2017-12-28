#pragma once

class VarMapEntry_t
{


public:
	unsigned short  type;
	unsigned short  m_bNeedsToInterpolate; // Set to false when this var doesn't
										   // need Interpolate() called on it anymore.
	void    *data;
	void *watcher;
};

struct VarMapping_t
{
	VarMapping_t()
	{
		m_nInterpolatedEntries = 0;
	}

	//CUtlVector< VarMapEntry_t > m_Entries;
	char m_Entries[20];
	int       m_nInterpolatedEntries;
	float      m_lastInterpolationTime;
};