/*
 * This is the main header for the TeamSpeak 3 Client SDK. All the functions that are used
 * to communicate with the Teamspeak 3 Client are here. Please view the documentation for
 * details. Note that some of these functions REQUIRE you to call ts3sclient_freeMemory()
 * after finishing with the result. This holds true for all the functions returning strings
 * (so the parameter is "char** result"). Again, the details are all in the documentation.
 */

#ifndef CLIENTLIB_H
#define CLIENTLIB_H

//system
#include <stdlib.h>

//own
#include "clientlib_publicdefinitions.h"
#include "public_definitions.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ClientUIFunctionsRare;

struct ClientUIFunctions {
	void (*onConnectStatusChangeEvent)           (anyID serverConnectionHandlerID, int newStatus, unsigned int errorNumber);
	void (*onNewChannelEvent)                    (anyID serverConnectionHandlerID, anyID channelID, anyID channelParentID);
	void (*onNewChannelCreatedEvent)             (anyID serverConnectionHandlerID, anyID channelID, anyID channelParentID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier);
	void (*onDelChannelEvent)                    (anyID serverConnectionHandlerID, anyID channelID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier);
	void (*onChannelMoveEvent)                   (anyID serverConnectionHandlerID, anyID channelID, anyID newChannelParentID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier);
	void (*onUpdateChannelEvent)                 (anyID serverConnectionHandlerID, anyID channelID);
	void (*onUpdateChannelEditedEvent)           (anyID serverConnectionHandlerID, anyID channelID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier);
	void (*onUpdateClientEvent)                  (anyID serverConnectionHandlerID, anyID clientID);
	void (*onClientMoveEvent)                    (anyID serverConnectionHandlerID, anyID clientID, anyID oldChannelID, anyID newChannelID, int visibility, const char* moveMessage);
	void (*onClientMoveSubscriptionEvent)        (anyID serverConnectionHandlerID, anyID clientID, anyID oldChannelID, anyID newChannelID, int visibility);
	void (*onClientMoveTimeoutEvent)             (anyID serverConnectionHandlerID, anyID clientID, anyID oldChannelID, anyID newChannelID, int visibility, const char* timeoutMessage);
	void (*onClientMoveMovedEvent)               (anyID serverConnectionHandlerID, anyID clientID, anyID oldChannelID, anyID newChannelID, int visibility, anyID moverID, const char* moverName, const char* moverUniqueIdentifier, const char* moveMessage);
	void (*onClientKickFromChannelEvent)         (anyID serverConnectionHandlerID, anyID clientID, anyID oldChannelID, anyID newChannelID, int visibility, anyID kickerID, const char* kickerName, const char* kickerUniqueIdentifier, const char* kickMessage);
	void (*onClientKickFromServerEvent)          (anyID serverConnectionHandlerID, anyID clientID, anyID oldChannelID, anyID newChannelID, int visibility, anyID kickerID, const char* kickerName, const char* kickerUniqueIdentifier, const char* kickMessage);
	void (*onClientIDsEvent)                     (anyID serverConnectionHandlerID, const char* uniqueClientIdentifier, anyID clientID, const char* clientName);
	void (*onClientIDsFinishedEvent)             (anyID serverConnectionHandlerID);
	void (*onServerEditedEvent)                  (anyID serverConnectionHandlerID, anyID editerID, const char* editerName, const char* editerUniqueIdentifier);
	void (*onServerUpdatedEvent)                 (anyID serverConnectionHandlerID);
	void (*onServerErrorEvent)                   (anyID serverConnectionHandlerID, const char* errorMessage, unsigned int error, const char* returnCode, const char* extraMessage);
	void (*onServerStopEvent)                    (anyID serverConnectionHandlerID, const char* shutdownMessage);
	void (*onTextMessageEvent)                   (anyID serverConnectionHandlerID, anyID targetMode, anyID fromID, const char* fromName, const char* fromUniqueIdentifier, const char* message, const anyID* targets);
	void (*onTalkStatusChangeEvent)              (anyID serverConnectionHandlerID, int status, anyID clientID);
	void (*onConnectionInfoEvent)                (anyID serverConnectionHandlerID, anyID clientID);
	void (*onServerConnectionInfoEvent)          (anyID serverConnectionHandlerID);
	void (*onChannelSubscribeEvent)              (anyID serverConnectionHandlerID, anyID channelID);
	void (*onChannelSubscribeFinishedEvent)      (anyID serverConnectionHandlerID);
	void (*onChannelUnsubscribeEvent)            (anyID serverConnectionHandlerID, anyID channelID);
	void (*onChannelUnsubscribeFinishedEvent)    (anyID serverConnectionHandlerID);
	void (*onChannelDescriptionUpdateEvent)      (anyID serverConnectionHandlerID, anyID channelID);
	void (*onChannelPasswordChangedEvent)        (anyID serverConnectionHandlerID, anyID channelID);
	void (*onCustomCaptureDeviceCloseEvent)      (anyID serverConnectionHandlerID, void* fmodSystem);
	void (*onCustomPlaybackDeviceCloseEvent)     (anyID serverConnectionHandlerID, void* fmodSystem);
	void (*onFMODChannelCreatedEvent)            (anyID serverConnectionHandlerID, anyID clientID, void* fmodChannel);
	void (*onPlaybackShutdownCompleteEvent)      (anyID serverConnectionHandlerID);
	void (*onUserLoggingMessageEvent)            (const char* logmessage, int logLevel, const char* logChannel, anyID logID, const char* logTime, const char* completeLogString);
	void (*onVoiceRecordDataEvent)               (const float* data, unsigned int dataSize);
	void (*onCustomPacketEncryptEvent)           (char** dataToSend, unsigned int* sizeOfData);
	void (*onCustomPacketDecryptEvent)           (char** dataReceived, unsigned int* dataReceivedSize);

}; //END OF ClientUIFunctions

/*Memory management*/
EXPORTDLL unsigned int ts3client_freeMemory(void* pointer);

/*Construction and Destruction*/
EXPORTDLL unsigned int ts3client_initClientLib(const struct ClientUIFunctions* functionPointers, const struct ClientUIFunctionsRare* functionRarePointers, int usedLogTypes, const char* logFileFolder);
EXPORTDLL unsigned int ts3client_destroyClientLib();
EXPORTDLL unsigned int ts3client_getClientLibVersion(char** result);

EXPORTDLL unsigned int ts3client_spawnNewServerConnectionHandler(int port, anyID* result);
EXPORTDLL unsigned int ts3client_destroyServerConnectionHandler(anyID serverConnectionHandlerID);

/*Identity management*/
EXPORTDLL unsigned int ts3client_createIdentity(char** result);

/*sound*/
EXPORTDLL unsigned int ts3client_getPlaybackDeviceList(int modeID, char**** result);
EXPORTDLL unsigned int ts3client_getPlaybackModeList(char**** result);
EXPORTDLL unsigned int ts3client_getCaptureDeviceList(int modeID, char**** result);
EXPORTDLL unsigned int ts3client_getCaptureModeList(char**** result);
EXPORTDLL unsigned int ts3client_getDefaultPlaybackDevice(int modeID, char*** result);
EXPORTDLL unsigned int ts3client_getDefaultPlayBackMode(int* result);
EXPORTDLL unsigned int ts3client_getDefaultCaptureDevice(int modeID, char*** result);
EXPORTDLL unsigned int ts3client_getDefaultCaptureMode(int* result);

EXPORTDLL unsigned int ts3client_openPlaybackDevice(anyID serverConnectionHandlerID, int modeID, const char* playbackDevice);
EXPORTDLL unsigned int ts3client_openCaptureDevice(anyID serverConnectionHandlerID, int modeID, const char* captureDevice);
EXPORTDLL unsigned int ts3client_openCustomPlaybackDevice(anyID serverConnectionHandlerID, void* fmodSystem);
EXPORTDLL unsigned int ts3client_openCustomCaptureDevice(anyID serverConnectionHandlerID, void* fmodSystem, int fmodDriverID);
EXPORTDLL unsigned int ts3client_getCurrentPlaybackDeviceName(anyID serverConnectionHandlerID, char** result);
EXPORTDLL unsigned int ts3client_getCurrentPlaybackDevice(anyID serverConnectionHandlerID, void** fmodSystemResult);
EXPORTDLL unsigned int ts3client_getCurrentPlayBackMode(anyID serverConnectionHandlerID, int* result);
EXPORTDLL unsigned int ts3client_getCurrentCaptureDeviceName(anyID serverConnectionHandlerID, char** result);
EXPORTDLL unsigned int ts3client_getCurrentCaptureDevice(anyID serverConnectionHandlerID, void** fmodSystemResult);
EXPORTDLL unsigned int ts3client_getCurrentCaptureMode(anyID serverConnectionHandlerID, int* result);
EXPORTDLL unsigned int ts3client_initiateGracefulPlaybackShutdown(anyID serverConnectionHandlerID);
EXPORTDLL unsigned int ts3client_closePlaybackDevice(anyID serverConnectionHandlerID);
EXPORTDLL unsigned int ts3client_closeCaptureDevice(anyID serverConnectionHandlerID);
EXPORTDLL unsigned int ts3client_activateCaptureDevice(anyID serverConnectionHandlerID);
EXPORTDLL unsigned int ts3client_activateCustomCaptureDevice(anyID serverConnectionHandlerID, void* fmodSystem, int fmodDriverID);

EXPORTDLL unsigned int ts3client_setLocalTestMode(anyID serverConnectionHandlerID, int status);

EXPORTDLL unsigned int ts3client_startVoiceRecording();
EXPORTDLL unsigned int ts3client_stopVoiceRecording();

/* 3d sound positioning */
EXPORTDLL unsigned int ts3client_fmod_Systemset3DListenerAttributes(anyID serverConnectionHandlerID, const TS3_FMOD_VECTOR* position, const TS3_FMOD_VECTOR* velocity,
                                                                    const TS3_FMOD_VECTOR* forward, const TS3_FMOD_VECTOR* up);
EXPORTDLL unsigned int ts3client_fmod_Systemset3DSettings(anyID serverConnectionHandlerID, float dopplerScale, float distanceFactor, float rolloffScale);
EXPORTDLL unsigned int ts3client_fmod_Channelset3DAttributes(anyID serverConnectionHandlerID, anyID clientID, const TS3_FMOD_VECTOR* position, const TS3_FMOD_VECTOR* velocity);

/*preprocessor*/
EXPORTDLL unsigned int ts3client_getPreProcessorInfoValueFloat(anyID serverConnectionHandlerID, const char* ident, float* result);
EXPORTDLL unsigned int ts3client_getPreProcessorConfigValue(anyID serverConnectionHandlerID, const char* ident, char** result);
EXPORTDLL unsigned int ts3client_setPreProcessorConfigValue(anyID serverConnectionHandlerID, const char* ident, const char* value);

/*encoder*/
EXPORTDLL unsigned int ts3client_getEncodeConfigValue(anyID serverConnectionHandlerID, const char* ident, char** result);

/*playback*/
EXPORTDLL unsigned int ts3client_getPlaybackConfigValueAsFloat(anyID serverConnectionHandlerID, const char* ident, float* result);
EXPORTDLL unsigned int ts3client_setPlaybackConfigValue(anyID serverConnectionHandlerID, const char* ident, const char* value);
EXPORTDLL unsigned int ts3client_setClientVolumeModifier(anyID serverConnectionHandlerID, anyID clientID, float value);

/*logging*/
EXPORTDLL unsigned int ts3client_logMessage(const char* logMessage, enum LogLevel severity, const char* channel, anyID logID);
EXPORTDLL unsigned int ts3client_setLogVerbosity(enum LogLevel logVerbosity);

/*error handling*/
EXPORTDLL unsigned int ts3client_getErrorMessage(unsigned int errorCode, char** error);

/*Interacting with the server*/
EXPORTDLL unsigned int ts3client_startConnection(anyID serverConnectionHandlerID, const char* identity, const char* ip, unsigned int port, const char* nickname,
                                                 const char** defaultChannelArray, const char* defaultChannelPassword, const char* serverPassword);
EXPORTDLL unsigned int ts3client_stopConnection(anyID serverConnectionHandlerID, const char* quitMessage);
EXPORTDLL unsigned int ts3client_requestClientMove(anyID serverConnectionHandlerID, anyID clientID, anyID newChannelID, const char* password, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestClientVariables(anyID serverConnectionHandlerID, anyID clientID, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestClientKickFromChannel(anyID serverConnectionHandlerID, anyID clientID, const char* kickReason, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestClientKickFromServer(anyID serverConnectionHandlerID, anyID clientID, const char* kickReason, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestChannelDelete(anyID serverConnectionHandlerID, anyID channelID, int force, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestChannelMove(anyID serverConnectionHandlerID, anyID channelID, anyID newChannelParentID, anyID newChannelOrder, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestSendTextMsg(anyID serverConnectionHandlerID, int targetMode, const char* message, const anyID* targetID, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestConnectionInfo(anyID serverConnectionHandlerID, anyID clientID, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestClientSetWhisperList(anyID serverConnectionHandlerID, anyID clientID, const anyID* targetChannelIDArray, const anyID* targetClientIDArray, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestChannelSubscribe(anyID serverConnectionHandlerID, anyID channelID, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestChannelSubscribeAll(anyID serverConnectionHandlerID, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestChannelUnsubscribe(anyID serverConnectionHandlerID, anyID channelID, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestChannelUnsubscribeAll(anyID serverConnectionHandlerID, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestChannelDescription(anyID serverConnectionHandlerID, anyID channelID, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestMuteClients(anyID serverConnectionHandlerID, const anyID* clientIDArray, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestUnmuteClients(anyID serverConnectionHandlerID, const anyID* clientIDArray, const char* returnCode);
EXPORTDLL unsigned int ts3client_requestClientIDs(anyID serverConnectionHandlerID, const char* clientUniqueIdentifier, const char* returnCode);

/*retrieve information ClientLib has stored*/

/*general info*/
EXPORTDLL unsigned int ts3client_getClientID(anyID serverConnectionHandlerID, anyID* result);

/*client connection info*/
EXPORTDLL unsigned int ts3client_getConnectionStatus(anyID serverConnectionHandlerID, int* result);
EXPORTDLL unsigned int ts3client_getConnectionVariableAsUInt64(anyID serverConnectionHandlerID, anyID clientID, size_t flag, uint64* result);
EXPORTDLL unsigned int ts3client_getConnectionVariableAsDouble(anyID serverConnectionHandlerID, anyID clientID, size_t flag, double* result);
EXPORTDLL unsigned int ts3client_getConnectionVariableAsString(anyID serverConnectionHandlerID, anyID clientID, size_t flag, char** result);
EXPORTDLL unsigned int ts3client_cleanUpConnectionInfo(anyID serverConnectionHandlerID, anyID clientID);

/*server connection info*/
EXPORTDLL unsigned int ts3client_requestServerConnectionInfo(anyID serverConnectionHandlerID, const char* returnCode);
EXPORTDLL unsigned int ts3client_getServerConnectionVariableAsUInt64(anyID serverConnectionHandlerID, size_t flag, uint64* result);

/*client info*/
EXPORTDLL unsigned int ts3client_getClientSelfVariableAsInt(anyID serverConnectionHandlerID, size_t flag, int* result);
EXPORTDLL unsigned int ts3client_getClientSelfVariableAsString(anyID serverConnectionHandlerID, size_t flag, char** result);
EXPORTDLL unsigned int ts3client_setClientSelfVariableAsInt(anyID serverConnectionHandlerID, size_t flag, int value);
EXPORTDLL unsigned int ts3client_setClientSelfVariableAsString(anyID serverConnectionHandlerID, size_t flag, const char* value);
EXPORTDLL unsigned int ts3client_flushClientSelfUpdates(anyID serverConnectionHandlerID);

EXPORTDLL unsigned int ts3client_getClientVariableAsInt(anyID serverConnectionHandlerID, anyID clientID, size_t flag, int* result);
EXPORTDLL unsigned int ts3client_getClientVariableAsString(anyID serverConnectionHandlerID, anyID clientID, size_t flag, char** result);

EXPORTDLL unsigned int ts3client_getClientList(anyID scHandlerID, anyID** result);
EXPORTDLL unsigned int ts3client_getChannelOfClient(anyID scHandlerID, anyID clientID, anyID* result);

/*channel info*/
EXPORTDLL unsigned int ts3client_getChannelVariableAsInt(anyID serverConnectionHandlerID, anyID channelID, size_t flag, int* result);
EXPORTDLL unsigned int ts3client_getChannelVariableAsString(anyID serverConnectionHandlerID, anyID channelID, size_t flag, char** result);

EXPORTDLL unsigned int ts3client_getChannelIDFromChannelNames(anyID serverConnectionHandlerID, char** channelNameArray, anyID* result);
EXPORTDLL unsigned int ts3client_setChannelVariableAsInt(anyID serverConnectionHandlerID, anyID channelID, size_t flag, int value);
EXPORTDLL unsigned int ts3client_setChannelVariableAsString(anyID serverConnectionHandlerID, anyID channelID, size_t flag, const char* value);
EXPORTDLL unsigned int ts3client_flushChannelUpdates(anyID serverConnectionHandlerID, anyID channelID);
EXPORTDLL unsigned int ts3client_flushChannelCreation(anyID serverConnectionHandlerID, anyID channelParentID);

EXPORTDLL unsigned int ts3client_getChannelList(anyID serverConnectionHandlerID, anyID** result);
EXPORTDLL unsigned int ts3client_getChannelClientList(anyID serverConnectionHandlerID, anyID channelID, anyID** result);
EXPORTDLL unsigned int ts3client_getParentChannelOfChannel(anyID scHandlerID, anyID channelID, anyID* result);

/*server info*/
EXPORTDLL unsigned int ts3client_getServerConnectionHandlerList(anyID** result);
EXPORTDLL unsigned int ts3client_getServerVariableAsInt(anyID serverConnectionHandlerID, size_t flag, int* result);
EXPORTDLL unsigned int ts3client_getServerVariableAsUInt64(anyID serverConnectionHandlerID, size_t flag, uint64* result);
EXPORTDLL unsigned int ts3client_getServerVariableAsString(anyID serverConnectionHandlerID, size_t flag, char** result);
EXPORTDLL unsigned int ts3client_requestServerVariables(anyID serverConnectionHandlerID);

#ifdef __cplusplus
}
#endif

#endif
