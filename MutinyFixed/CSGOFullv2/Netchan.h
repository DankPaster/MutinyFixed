#pragma once
#include "CUtlMemory.h"
#include "bfwrite.h"
#include "netadr.h"
#include "GOTV.h"
//#include "CUtlVector.h"


class Netmsgbinder;
class INetChannel;

class INetChannelHandler
{
public:
	virtual	~INetChannelHandler(void) {};

	virtual void ConnectionStart(INetChannel *chan) = 0;	// called first time network channel is established

	virtual void ConnectionClosing(const char *reason) = 0; // network channel is being closed by remote site

	virtual void ConnectionCrashed(const char *reason) = 0; // network error occured

	virtual void PacketStart(int incoming_sequence, int outgoing_acknowledged) = 0;	// called each time a new packet arrived

	virtual void PacketEnd(void) = 0; // all messages has been parsed

	virtual void FileRequested(const char *fileName, unsigned int transferID) = 0; // other side request a file for download

	virtual void FileReceived(const char *fileName, unsigned int transferID) = 0; // we received a file

	virtual void FileDenied(const char *fileName, unsigned int transferID) = 0;	// a file request was denied by other side

	virtual void FileSent(const char *fileName, unsigned int transferID) = 0;	// we sent a file
};

class INetChannel
{
public:
	__int32 vtable; //0x0000 
	Netmsgbinder* msgbinder1; //0x0004 
	Netmsgbinder* msgbinder2; //0x0008 
	Netmsgbinder* msgbinder3; //0x000C 
	Netmsgbinder* msgbinder4; //0x0010 
	unsigned char m_bProcessingMessages; //0x0014 
	unsigned char m_bShouldDelete; //0x0015 
	char pad_0x0016[0x2]; //0x0016
	__int32 m_nOutSequenceNr; //0x0018 
	__int32 m_nInSequenceNr; //0x001C 
	__int32 m_nOutSequenceNrAck; //0x0020 
	__int32 m_nOutReliableState; //0x0024 
	__int32 m_nInReliableState; //0x0028 
	__int32 m_nChokedPackets; //0x002C 

	bf_write m_StreamReliable; //0x0030 
	/*CUtlMemory*/ char m_ReliableDataBuffer[12]; //0x0048 
	bf_write m_StreamUnreliable; //0x0054 
	/*CUtlMemory*/ char m_UnreliableDataBuffer[12]; //0x006C 
	bf_write m_StreamVoice; //0x0078 
	/*CUtlMemory*/char m_VoiceDataBuffer[12]; //0x0090 
	__int32 m_Socket; //0x009C 
	__int32 m_StreamSocket; //0x00A0 
	__int32 m_MaxReliablePayloadSize; //0x00A4 
	char pad_0x00A8[0x4]; //0x00A8
	netadr_t remote_address; //0x00AC 
	char dylanpadding[84]; //padding added by dylan
	float last_received; //0x00B8  //dylan found 0x10c
	//char pad_0x00BC[0x4]; //0x00BC
	double /*float*/ connect_time; //0x00C0 //dylan found 0x110
	//char pad_0x00C4[0x4]; //0x00C4
	__int32 m_Rate; //0x00C8  //dylan found 0x118
	/*float*/double m_fClearTime; //0x00CC  //dylan found 0x120
	char pad_0x00D0[0x8]; //0x00D0
	char m_WaitingList[48];
	//CUtlVector m_WaitingList[0]; //0x00D8 
	//CUtlVector m_WaitingList[1]; //0x00EC 
	//char pad_0x0100[0x4120]; //0x0100
	char pad_0x0100[0x40F0]; //dylan changed
	__int32 m_PacketDrop; //0x4220  //dylan found 0x4250
	char m_Name[32]; //0x4224 
	__int32 m_ChallengeNr; //0x4244 
	float m_Timeout; //0x4248 //dylan found 0x4278
	INetChannelHandler* m_MessageHandler; //0x424C 
	/*CUtlVector*/char m_NetMessages[14]; //0x4250 
	__int32 dylanUnknown;
	void* m_pDemoRecorder; //0x4264 
	__int32 m_nQueuedPackets; //0x4268  //dylan found 0x4298
	float m_flInterpolationAmount; //0x426C //dylan found 0x429c
	float m_flRemoteFrameTime; //0x4270 //dylan found 0x42a0
	float m_flRemoteFrameTimeStdDeviation; //0x4274  //dylan found 0x42a4
	float m_flRemoteFrameTimeUnknown; //dylan found 0x42a8
	__int32 m_nMaxRoutablePayloadSize; //0x4278  //dylan found 0x42ac
	__int32 m_nSplitPacketSequence; //0x427C  //dylan found 0x42b0
	char pad_0x4280[0x14]; //0x4280

};//Size=0x4294