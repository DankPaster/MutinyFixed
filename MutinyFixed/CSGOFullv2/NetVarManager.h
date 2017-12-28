#pragma once
#include <vector>
#include <fstream>
//#include "cx_strenc.h"

typedef enum
{
	DPT_Int = 0,
	DPT_Float,
	DPT_Vector,
	DPT_VectorXY, // Only encodes the XY of a vector, ignores Z
	DPT_String,
	DPT_Array,	// An array of the base types (can't be of datatables).
	DPT_DataTable,
#if 0 // We can't ship this since it changes the size of DTVariant to be 20 bytes instead of 16 and that breaks MODs!!!
	DPT_Quaternion,
#endif

#ifdef SUPPORTS_INT64
	DPT_Int64,
#endif

	DPT_NUMSendPropTypes

} SendPropType;

class DVariant
{
public:
	DVariant() { m_Type = DPT_Float; }
	DVariant(float val) { m_Type = DPT_Float; m_Float = val; }

	const char *ToString()
	{
		return nullptr;
#if 0
		static char text[128];
		switch (m_Type)
		{
		case DPT_Int:
			snprintf(text, sizeof(text), ("%i"), m_Int);
			break;
		case DPT_Float:
			snprintf(text, sizeof(text), ("%.3f"), m_Float);
			break;
		case DPT_Vector:
			snprintf(text, sizeof(text), ("(%.3f,%.3f,%.3f)"),
				m_Vector[0], m_Vector[1], m_Vector[2]);
			break;
		case DPT_VectorXY:
			snprintf(text, sizeof(text), ("(%.3f,%.3f)"),
				m_Vector[0], m_Vector[1]);
			break;
#if 0 // We can't ship this since it changes the size of DTVariant to be 20 bytes instead of 16 and that breaks MODs!!!
		case DPT_Quaternion:
			Q_snprintf(text, sizeof(text), ("(%.3f,%.3f,%.3f %.3f)"),
				m_Vector[0], m_Vector[1], m_Vector[2], m_Vector[3]);
			break;
#endif
		case DPT_String:
			if (m_pString)
				return m_pString;
			else
				return  ("NULL");
			break;
		case DPT_Array:
			snprintf(text, sizeof(text), ("Array"));
			break;
		case DPT_DataTable:
			snprintf(text, sizeof(text), ("DataTable"));
			break;
#ifdef SUPPORTS_INT64
		case DPT_Int64:
			snprintf(text, sizeof(text), charenc("%I64d"), m_Int64);
			break;
#endif
		default:
			snprintf(text, sizeof(text), ("type %i unknown"), m_Type);
			break;
		}

		return text;
#endif
	}
	union
	{
		float	m_Float;
		long	m_Int;
		char	*m_pString;
		void	*m_pData;
		float	m_Vector[3];
	};
	SendPropType m_Type;
};

struct RecvProp;

class CRecvProxyData
{
public:
	const RecvProp	*m_pRecvProp;
	void * Unknown;
	DVariant		m_Value;
	int				m_iElement;
	int				m_ObjectID;
};


typedef void(*RecvVarProxyFn)(const CRecvProxyData *pData, void *pStruct, void *pOut);

struct RecvTable
{
	RecvProp		*m_pProps;
	int				m_nProps;
	void			*m_pDecoder;
	char			*m_pNetTableName;
	bool			m_bInitialized;
	bool			m_bInMainList;
};

struct RecvProp
{
	char					*m_pVarName;
	int						m_RecvType;
	int						m_Flags;
	int						m_StringBufferSize;
	bool					m_bInsideArray;
	const void				*m_pExtraData;
	RecvProp				*m_pArrayProp;
	void*					m_ArrayLengthProxy;
	void*					m_ProxyFn;
	void*					m_DataTableProxyFn;
	RecvTable				*m_pDataTable;
	int						m_Offset;
	int						m_ElementStride;
	int						m_nElements;
	const char				*m_pParentArrayPropName;
};

struct ClientClass
{
	void*			m_pCreateFn;
	void*			m_pCreateEventFn;
	char			*m_pNetworkName;
	RecvTable		*m_pRecvTable;
	ClientClass		*m_pNext;
	int				m_ClassID;
};

class CNetVarManager
{
public:
	void Initialize();
	//void GrabOffsets();
	int GetOffset(const char *tableName, const char *propName);
	bool HookProp(const char *tableName, const char *propName, RecvVarProxyFn fun);
	//void DumpNetvars(std::string path);
private:
	int Get_Prop(const char *tableName, const char *propName, RecvProp **prop = 0);
	int Get_Prop(RecvTable *recvTable, const char *propName, RecvProp **prop = 0);
	RecvTable *GetTable(const char *tableName);
	std::vector<RecvTable*> m_tables;
	//void DumpTable(RecvTable *table, int depth);
	std::ofstream m_file;
};

extern CNetVarManager* NetVarManager;