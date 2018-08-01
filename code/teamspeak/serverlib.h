/*
 * This is the main header for the TeamSpeak 3 Server SDK. All the functions that are used
 * to communicate with the TeamSpeak 3 Server are here. Please view the documentation for
 * details. Note that some of these functions REQUIRE you to call ts3server_freeMemory()
 * after finishing with the result. This holds true for all the functions returning
 * strings (so the parameter is "char** result"). Again, the details are all in the
 * documentation.
 */

#ifndef SERVERLIB_H
#define SERVERLIB_H

#include "public_definitions.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ServerLibFunctions {
	void (*onVoiceDataEvent)           (anyID serverID, anyID clientID, unsigned char* voiceData, unsigned int voiceDataSize, unsigned int frequency);
	void (*onClientStartTalkingEvent)  (anyID serverID, anyID clientID);
	void (*onClientStopTalkingEvent)   (anyID serverID, anyID clientID);
	void (*onClientConnected)          (anyID serverID, anyID clientID, anyID channelID, unsigned int* removeClientError);
	void (*onClientDisconnected)       (anyID serverID, anyID clientID, anyID channelID);
	void (*onClientMoved)              (anyID serverID, anyID clientID, anyID oldChannelID, anyID newChannelID);
	void (*onChannelCreated)           (anyID serverID, anyID invokerClientID, anyID channelID);
	void (*onChannelEdited)            (anyID serverID, anyID invokerClientID, anyID channelID);
	void (*onChannelDeleted)           (anyID serverID, anyID invokerClientID, anyID channelID);
	void (*onTextMessageEvent)         (anyID serverID, enum TextMessageTargetMode textMessageMode, anyID invokerClientID, const anyID* targetIDS, const char* textMessage);
	void (*onUserLoggingMessageEvent)  (const char* logmessage, int logLevel, const char* logChannel, anyID logID, const char* logTime, const char* completeLogString);
	void (*onAccountingErrorEvent)     (anyID serverID, unsigned int errorCode);
	void (*onCustomPacketEncryptEvent) (char** dataToSend, unsigned int* sizeOfData);
	void (*onCustomPacketDecryptEvent) (char** dataReceived, unsigned int* dataReceivedSize);
}; //END OF ServerLibFunctions

/*Memory management*/
EXPORTDLL unsigned int ts3server_freeMemory(void* pointer);

/*Construction and Destruction*/
EXPORTDLL unsigned int ts3server_initServerLib(const struct ServerLibFunctions* functionPointers, int usedLogTypes, const char* logFileFolder);
EXPORTDLL unsigned int ts3server_destroyServerLib();
EXPORTDLL unsigned int ts3server_getServerLibVersion(char** result);

/*logging*/
EXPORTDLL unsigned int ts3server_setLogVerbosity(enum LogLevel logVerbosity);

/*error handling*/
EXPORTDLL unsigned int ts3server_getGlobalErrorMessage(unsigned int globalErrorCode, char** result);

/*client info*/
EXPORTDLL unsigned int ts3server_getClientVariableAsInt(anyID serverID, anyID clientID, enum ClientProperties flag, int* result);
EXPORTDLL unsigned int ts3server_getClientVariableAsString(anyID serverID, anyID clientID, enum ClientProperties flag, char** result);
EXPORTDLL unsigned int ts3server_setClientVariableAsInt(anyID serverID, anyID clientID, enum ClientProperties flag, int value);
EXPORTDLL unsigned int ts3server_setClientVariableAsString(anyID serverID, anyID clientID, enum ClientProperties flag, const char* value);
EXPORTDLL unsigned int ts3server_flushClientVariable(anyID serverID, anyID clientID);

EXPORTDLL unsigned int ts3server_setClientWhisperList(anyID serverID, anyID clID, const anyID* channelID, const anyID* clientID);

EXPORTDLL unsigned int ts3server_getClientList(anyID serverID, anyID** result);
EXPORTDLL unsigned int ts3server_getChannelOfClient(anyID serverID, anyID clientID, anyID* result);
EXPORTDLL unsigned int ts3server_clientMove(anyID serverID, anyID newChannelID, const anyID* clientIDArray); 

/*channel info*/
EXPORTDLL unsigned int ts3server_getChannelVariableAsInt(anyID serverID, anyID channelID, enum ChannelProperties flag, int* result);
EXPORTDLL unsigned int ts3server_getChannelVariableAsString(anyID serverID, anyID channelID, enum ChannelProperties flag, char** result);
EXPORTDLL unsigned int ts3server_setChannelVariableAsInt(anyID serverID, anyID channelID, enum ChannelProperties flag, int value);
EXPORTDLL unsigned int ts3server_setChannelVariableAsString(anyID serverID, anyID channelID, enum ChannelProperties flag, const char* value);
EXPORTDLL unsigned int ts3server_flushChannelVariable(anyID serverID, anyID channelID);
EXPORTDLL unsigned int ts3server_flushChannelCreation(anyID serverID, anyID channelParentID, anyID* result);

EXPORTDLL unsigned int ts3server_getChannelList(anyID serverID, anyID** result);
EXPORTDLL unsigned int ts3server_getChannelClientList(anyID serverID, anyID channelID, anyID** result);
EXPORTDLL unsigned int ts3server_getParentChannelOfChannel(anyID serverID, anyID channelID, anyID* result);

EXPORTDLL unsigned int ts3server_channelDelete(anyID serverID, anyID channelID, int force);
EXPORTDLL unsigned int ts3server_channelMove(anyID serverID, anyID channelID, anyID newChannelParentID, anyID newOrder);

/*server info*/
EXPORTDLL unsigned int ts3server_getVirtualServerVariableAsInt(anyID serverID, enum VirtualServerProperties flag, int* result);
EXPORTDLL unsigned int ts3server_getVirtualServerVariableAsString(anyID serverID, enum VirtualServerProperties flag, char** result);
EXPORTDLL unsigned int ts3server_setVirtualServerVariableAsInt(anyID serverID, enum VirtualServerProperties flag, int value);
EXPORTDLL unsigned int ts3server_setVirtualServerVariableAsString(anyID serverID, enum VirtualServerProperties flag, const char* value);
EXPORTDLL unsigned int ts3server_flushVirtualServerVariable(anyID serverID);

EXPORTDLL unsigned int ts3server_getVirtualServerConnectionVariableAsUInt64(anyID serverID, enum ConnectionProperties flag, uint64* result);
EXPORTDLL unsigned int ts3server_getVirtualServerConnectionVariableAsDouble(anyID serverID, enum ConnectionProperties flag, double* result);

EXPORTDLL unsigned int ts3server_getVirtualServerList(anyID** result);
EXPORTDLL unsigned int ts3server_stopVirtualServer(anyID serverID);
EXPORTDLL unsigned int ts3server_createVirtualServer(unsigned int serverPort, const char* serverIp, const char* serverName, const char* serverKeyPair, unsigned int serverMaxClients, anyID* result);
EXPORTDLL unsigned int ts3server_getVirtualServerKeyPair(anyID serverID, char** result);

#ifdef __cplusplus
}
#endif

#endif
