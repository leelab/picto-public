
// OPXClient.h
//  
// OmniPlex online client programs should include this header,
// link with OPXClient.lib, and run with OPXClient.dll.  64 bit
// builds link with OPXClient64.lib and run with OPXClient64.dll.
//
// Copyright (c) 1994-2018 Plexon Inc., Dallas, Texas 75206
// www.plexon.com - info@plexon.com - support@plexon.com
//
// Permission is granted for users of Plexon data acquisition systems
// to use this code in their own programs.  To insure that the latest 
// version is being used, please do not post this code on the Internet 
// or include it in publicly available code, articles, or reference 
// materials.  Instead, please use links to the OmniPlex Client SDK:
//
//    http://www.plexon.com/software-downloads
//

#pragma once

//
// Source and channel types
//
#define SPIKE_TYPE (1) 
#define EVENT_TYPE (4)
#define CONTINUOUS_TYPE (5)
#define OTHER_TYPE (6)
//

//
// maximum number of sources
//
#define MAX_SOURCES (256)
// 

//
// maximum number of channels per source
//
#define MAX_CHANS_PER_SOURCE (512)
//

// maximum number of AuxAI channels
//
#define MAX_AUXAI_CHANS (64)
//

//
// Formats for client data channel numbers
//
// Channel numbers are relative to one of the four channel types
#define CHANNEL_FORMAT_LINEAR (1)
// Channel numbers are relative to the channel's source
#define CHANNEL_FORMAT_SOURCE_RELATIVE (2)
//

//
// Size of the WaveForm array (sample data array) in the OPX_DataBlock struct.
//
#define MAX_WF_LENGTH (56)
//

//
// Values for the OPXSystemType parameter returned by OPX_GetGlobalPars;
// identifies the main neural data acquisition device(s); currently, 
// these values are mutually exclusive.
//
// Indicates that no valid topology is loaded in Server
#define OPXSYSTEM_INVALID (0)
//
// OmniPlex system using a TestADC (artificial data) device
#define OPXSYSTEM_TESTADC (1)
//
// OmniPlex-A system:
#define OPXSYSTEM_AD64 (2)
//
// OmniPlex-D system, using either DigiAmp or MiniDigi
#define OPXSYSTEM_DIGIAMP (4)
//
// OmniPlex-D system, using Digital Headstage Processor (DHP)
#define OPXSYSTEM_DHSDIGIAMP (8)
//

//
// Values returned by OPX_GetOPXSystemStatus
//
// data acquisition stopped
#define OPX_DAQ_STOPPED (1)
// data acquisition started (running)
#define OPX_DAQ_STARTED (2)
// data file (pl2/plx) being recorded
#define OPX_RECORDING (3)
// data file being recorded, but paused
#define OPX_RECORDING_PAUSED (4)
//

//
// Structure returned by OPX_GetGlobalParameters
//

struct OPX_GlobalParams
{
  int32_t OPXSystemType;         // a hardware-specific OPXSYSTEM_* value (see #defines above)
  int32_t OPXSoftwareVersion;    // version of OmniPlex being used, e.g. 1181 = version 1.18.1
  int32_t OPXSDKVersion;         // version of the Native Client API being used e.g. 101 = version 1.0.1
  int32_t numSources;            // total number of sources available to client
  int32_t sourceIds[MAX_SOURCES]; // an array of source numbers, e.g. [4,1,3,6,10,11...]
  int32_t numSpikeChans;         // total number of channels of SPIKE_TYPE
  int32_t numContChans;          // total number of channels of CONTINUOUS_TYPE
  int32_t numEventChans;         // total number of channel of EVENT_TYPE
  int32_t numOtherChans;         // total number of channels of OTHER_TYPE 
  double  timestampFrequency;    // frequency of timestamp "ticks"; 
                                 // currently either 40000 or 1000000,
                                 // i.e. one tick = either 25 microseconds or 1 microsecond
  double  clientSampleRateLimit; // Server only sends continuous data to clients if the sample 
                                 // rate is this frequency or less; by default, 1000 Hz 
  int32_t bitsPerSample;         // typically either 12 or 16 bits, as set in Server options
  int32_t minimizeClientLatency; // nonzero if Server "Minimize Client Latency" option is enabled
  uint8_t reserved2[1024];       // reserved for future use
};

//
// Structure returned by OPX_GetNewData and OPX_GetNewDataEx
//

struct OPX_DataBlock
{
  uint8_t  SourceNumOrType;            // source number or source type: SPIKE_TYPE, EVENT_TYPE, or CONTINUOUS_TYPE
  uint8_t  reserved1; 
  uint8_t  reserved2; 
  uint8_t  UpperTS;                    // upper 8 bits of the raw 40-bit integer timestamp
  uint32_t Timestamp;                  // lower 32 bits of the raw 40-bit integer timestamp
  uint16_t Channel;                    // channel number within one of the four source-type ranges
  uint16_t Unit;                       // unit (0 = unsorted, 1 = a, 2 = b, etc), or strobed event value
  uint8_t  reserved3; 
  uint8_t  NumberOfBlocksPerWaveform;  // for spikes longer than one block
  uint8_t  BlockNumberForWaveform;     // for spikes longer than one block
  uint8_t  NumberOfDataWords;          // number of shorts (2-byte integers) that follow this header 
  int16_t  WaveForm[MAX_WF_LENGTH];    // the spike waveform or a series of continuous data samples
}; 

struct FilterInfo 
{ 
  bool      m_bEnabledHPF;
  int32_t   m_filterTypeHPF;     
  int32_t   m_numPolesHPF;
  float     m_freqHPF;

  bool      m_bEnabledLPF;
  int32_t   m_filterTypeLPF; 
  int32_t   m_numPolesLPF;
  float     m_freqLPF;

  bool      m_bEnabledPLIFilter;
  int32_t   m_freqPLIFilter;
  int32_t   m_numHarmonics;
  bool      m_bAdaptiveHarmonics;

  DWORD  m_reserved[16];
};

/////////////////////////////////////////////////////////////////////////////////////////////////
// 
// Functions which can be called by OmniPlex online client programs
//
/////////////////////////////////////////////////////////////////////////////////////////////////


// OPX_InitClient: Call this function once, before calling any other OPX function.
// 
// Inputs: none
// Outputs:
// - if successful, function return value is a positive integer client handle;
//   if there was an error, a negative integer error code is returned
//
extern "C" int32_t WINAPI OPX_InitClient();

// OPX_CloseClient: Call this function once, before terminating.
// 
// Inputs: none
// Outputs: none
//
extern "C" void WINAPI OPX_CloseClient();

/////////////////////////////////////////////////////////////////////////////////////////////////

// OPX_GetGlobalParameters: returns basic info on sources and system parameters.
//
// Inputs: 
// - params: pointer to a client-allocated OPX_GlobalParams structure
// Outputs:
// - params: on return, is filled in with parameter values; see definition of
//   OPX_GlobalParams above.
// 
extern "C" int32_t WINAPI OPX_GetGlobalParameters(OPX_GlobalParams* params);

/////////////////////////////////////////////////////////////////////////////////////////////////

// OPX_GetSourceInfoByNumber: Given a source number, returns basic info about the source.
//
// Input: 
// - sourceNum: number of the source whose info is being requested
// Outputs:
// - sourceName: null-terminated string containing the source name
//   * Note: caller must declare a char array of size 256 and pass a pointer to this array
// - sourceType: one of the values SPIKE_TYPE, CONTINUOUS_TYPE, EVENT_TYPE, or OTHER_TYPE
// - numChans: number of channels in the source
// - linearStartChan: starting channel number for the source, within the linear array of channels
//   of the specified type
//
extern "C" int32_t WINAPI OPX_GetSourceInfoByNumber(int32_t sourceNum, char* sourceName, int32_t* sourceType, 
                                                    int32_t* numChans, int32_t* linearStartChan);

// OPX_GetSourceInfoByName: Given a source name, returns basic info about the source.
//
// Input: 
// - sourceName: null-terminated string containing the source's name, e.g. "SPKC"
// Outputs: See OPX_GetSourceInfoByNumber
//
extern "C" int32_t WINAPI OPX_GetSourceInfoByName(char* sourceName, int32_t* sourceNum, int32_t* sourceType, 
                                                  int32_t* numChans, int32_t* linearStartChan);

/////////////////////////////////////////////////////////////////////////////////////////////////

// OPX_GetSpikeSourceInfoByNumber: Given a spike source number, returns spike-source specific info.
//
// Input: 
// - sourceNum: number of the spike source whose info is being requested
// - sourceName: caller-allocated char array of length 256
// Outputs:
// - sourceName: null-terminated string containing the source's name
// - rate: sample rate of the spike source 
// - voltage scaler: value which converts integer spike waveform values to volts
// - trodality: 1 = single electrode, 2 = stereotrode, 4 = tetrode, etc
// - ptsPerWaveform: number of points in each spike waveform
// - preThreshPts: number of waveform points before the threshold crossing
//
extern "C" int32_t WINAPI OPX_GetSpikeSourceInfoByNumber(int32_t sourceNum, char* sourceName, 
                                                         double* rate, double* voltageScaler, int32_t* trodality, 
                                                         int32_t* ptsPerWaveform, int32_t* preThreshPts);

// OPX_GetSpikeSourceInfoByName: Given a spike source name, returns spike-source specific info.
//
// Input: 
// - sourceName: null-terminated string containing the source's name, e.g. "SPK"
// Outputs: See OPX_GetSpikeSourceInfoByNumber
//
extern "C" int32_t WINAPI OPX_GetSpikeSourceInfoByName(char* sourceName, int32_t* sourceNum, 
                                                       double* rate, double* voltageScaler, 
                                                       int32_t* trodality, int32_t* ptsPerWaveform, int32_t* preThreshPts);

/////////////////////////////////////////////////////////////////////////////////////////////////

// OPX_GetContSourceInfoByNumber: Given a continuous source, returns continuous-source specific info.
//                                                      
// Input: 
// - sourceNum: number of the continuous source whose info is being requested
// Outputs:
// - rate: sample rate of the continuous source 
// - voltage scaler: value which converts integer continuous sample values to volts
//
extern "C" int32_t WINAPI OPX_GetContSourceInfoByNumber(int32_t sourceNum, char* sourceName, double* rate, double* voltageScaler);

// OPX_GetContSourceInfoByName: Given a continuous source name, returns continuous-source specific info.
//                                                      
// Input: 
// - sourceName: null-terminated string containing the source's name, e.g. "FP"
// Outputs: See OPX_GetContSourceInfoByNumber
//
extern "C" int32_t WINAPI OPX_GetContSourceInfoByName(char* sourceName, int32_t* sourceNum, 
                                                      double* rate, double* voltageScaler);

/////////////////////////////////////////////////////////////////////////////////////////////////

// OPX_GetSourceChanInfoByNumber: Given a source number and source-relative channel number,
// returns info for that source channel.
//
// Input:
// - sourceNum: number of the source whose info is being requested
// - sourceChan: source-relative channel number
// Outputs:
// - chanName: null-terminated string containing the channel name
// - rate: sample rate of the channel (typically the same for all channels in a source)
// - voltage scaler: value which converts integer waveform or continuous sample values to volts
// - enabled: 1 if channel is enabled, 0 if disabled
// * Note: for event sources, the rate and voltageScaler values can be ignored
//                                                      
extern "C" int32_t WINAPI OPX_GetSourceChanInfoByNumber(int32_t sourceNum, int32_t sourceChan, 
                                                        char* chanName, double* rate, double* voltageScaler, 
                                                        int32_t* enabled);

// OPX_GetSourceChanInfoByName: Given a source name and source-relative channel number,
// returns info for that source channel.
//
// Inputs:
// - sourceName: null-terminated string containing the source's name, e.g. "FP"
// - sourceChan: source-relative channel number
// Outputs: See OPX_GetSourceChanInfo
//                                                
extern "C" int32_t WINAPI OPX_GetSourceChanInfoByName(char* sourceName, int32_t sourceChan, 
                                                      char* chanName, double* rate, double* voltageScaler, 
                                                      int32_t* enabled);

// OPX_GetLinearChanInfo: Given a channel type and channel number within that type,
// returns info for that channel.
//
// Inputs:
// - linearChanType: channel type, one of SPIKE_TYPE, CONTINUOUS_TYPE, EVENT_TYPE, or OTHER_TYPE
// - chan: channel number within of the channel types
// Outputs:
// - chanName: null-terminated string containing the channel name
// - rate: sample rate of the channel (typically the same for all channels in a source)
// - voltageScaler: value which converts integer waveform or continuous sample values to volts
// - enabled: 1 if channel is enabled, 0 if disabled
// * Note: for event sources, the rate and voltageScaler values can be ignored
//
extern "C" int32_t WINAPI OPX_GetLinearChanInfo(int32_t linearChanType, int32_t chan, 
                                                char* chanName, double* rate, double* voltageScaler, 
                                                int32_t* enabled);

/////////////////////////////////////////////////////////////////////////////////////////////////

// OPX_GetContSourceChanFilterInfoByNumber: Give a source number and source-relative channel number,
// returns digital filter info for that source channel.
//
// Input:
// - sourceNum: number of the source whose filter info is being requested
// - sourceChan: source-relative channel number
// Output:
// - filterInfo: digital filter info
//
extern "C" int32_t WINAPI OPX_GetContSourceChanFilterInfoByNumber(int32_t sourceNum, int32_t sourceChan, FilterInfo* filterInfo);

// OPX_GetContSourceChanFilterInfoByName: Given a source name and source-relative channel number,
// returns digital filter info for that source channel.
//
// Inputs:
// - sourceName: null-terminated string containing the source's name, e.g. "FP"
// - sourceChan: source-relative channel number
// Output: 
// - filterInfo: digital filter info
//
extern "C" int32_t WINAPI OPX_GetContSourceChanFilterInfoByName(char* sourceName, int32_t sourceChan, FilterInfo* filterInfo);

// OPX_GetContLinearChanFilterInfo: Given a continuous channel number, returns digital filter info for that channel.
//
// Input:
// - chan: continuous channel linear channel number
// Output:
// - filterInfo: digital filter info
//  
extern "C" int32_t WINAPI OPX_GetContLinearChanFilterInfo(int32_t chan, FilterInfo* filterInfo);

/////////////////////////////////////////////////////////////////////////////////////////////////

// OPX_GetSourceVoltageScalersByNumber: Given a source number, fills in an array of voltage scaling values.
// If the source does not support per-channel scaling, all the values will be the same.  If the source
// does not support gain changes, this function only needs to be called once.  If the source does support
// gain changes, the function OPX_GetLastGainChangeTime() can be used to determine when gains have changed.
//
// Input: 
// - sourceNum: source number
// Output:
// - voltageScalers: array of voltage scaling values
//
extern "C" int32_t WINAPI OPX_GetSourceVoltageScalersByNumber(int32_t sourceNum, double* voltageScalers);

// OPX_GetSourceVoltageScalersByName: Given a source name, fills in an array of voltage scaling values.
// If the source does not support per-channel scaling, all the values will be the same.  If the source
// does not support gain changes, this function only needs to be called once.  If the source does support
// gain changes, the function OPX_GetLastGainChangeTime() can be used to determine when gains have changed.
//
// Input: 
// - sourceName: null-terminated string containing the source's name, e.g. "FP"
// Output:
// - voltageScalers: array of voltage scaling values
//
extern "C" int32_t WINAPI OPX_GetSourceVoltageScalersByName(char* sourceName, double* voltageScalers);

/////////////////////////////////////////////////////////////////////////////////////////////////

// OPX_ExcludeSourceByNumber: Exclude the given source from the data sent to this client.
//
// Input:
// - sourceNum: number of the source to be excluded
// Output: none
//
extern "C" int32_t WINAPI OPX_ExcludeSourceByNumber(int32_t sourceNum);

// OPX_ExcludeSourceByName: Exclude the given source from the data sent to this client.
//
// Input:
// - sourceName: name of the source to be excluded
// Output: none
//
extern "C" int32_t WINAPI OPX_ExcludeSourceByName(char* sourceName);

// OPX_ExcludeAllSourcesOfType: Exclude all sources of the given type from the data sent to this client.
//
// Input:
// - sourceType: one of SPIKE_TYPE, CONTINUOUS_TYPE, or EVENT_TYPE
// Output: none
//
extern "C" int32_t WINAPI OPX_ExcludeAllSourcesOfType(int32_t sourceType);

// OPX_IncludeSourceByNumber: Include the given source in the data sent to this client.  
// Cancels any previous exclusions of the source.
//
// Input:
// - sourceNum: number of the source to be included
// Output: none
//
extern "C" int32_t WINAPI OPX_IncludeSourceByNumber(int32_t sourceNum);

// OPX_IncludeSourceByName: Include the given source in the data sent to this client.  
// Cancels any previous exclusions of the source.
//
// Input:
// - sourceName: named of the source to be included
// Output: none
//
extern "C" int32_t WINAPI OPX_IncludeSourceByName(char* sourceName);

// OPX_IncludeAllSourcesOfType: Include all sources of the given type in the data sent to this client.
// Cancels any previous exclusions of the source type.
// Input:
// - sourceType: one of SPIKE_TYPE, CONTINUOUS_TYPE, EVENT_TYPE, or OTHER_TYPE
// Output: none
//
extern "C" int32_t WINAPI OPX_IncludeAllSourcesOfType(int32_t sourceType);

/////////////////////////////////////////////////////////////////////////////////////////////////

// OPX_GetNewData - Get a batch of online client data.
//
// Inputs:
// - pNumDataBlocks - pointer to the maximum number of data blocks that the client will accept per call
// - dataBlocks - caller-allocated array of OPX_DataBlock, to be filled with incoming data; 
//   must be large enough to accept the specified maximum number of blocks 
// Outputs:
// - pNumDataBlocks - the number of elements of the dataBlocks array that were filled
// - dataBlocks - array of OPX_DataBlock client data
// * Note: the OPX_SetDataFormat function determines whether source types or source numbers
//   appear in the data blocks; by default, source numbers are returned
//
extern "C" int32_t WINAPI OPX_GetNewData(int32_t* pNumDataBlocks, OPX_DataBlock* dataBlocks);

// OPX_GetNewDataEx - Similar to OPX_GetNewData, but returns additional information.
//
// Inputs:
// - pNumDataBlocks - see OPX_GetNewData
// - dataBlocks - see OPX_GetNewData
// 
// Outputs:
// - pNumDataBlocks - see OPX_GetNewData
// - dataBlocks - see OPX_GetNewData
// - pNumSpikesRead - the number of elements of dataBlocks of type SPIKE_TYPE
// - pNumContRead - the number of elements of dataBlocks of type CONTINOUS_TYPE
// - pNumEventsRead - the number of elements of dataBlocks of type EVENT_TYPE
// - pNumOtherRead - the number of elements of dataBlocks of type OTHER_TYPE
// - minTimestamp - the smallest timestamp in dataBlocks, in seconds
// - maxTimestamp - the largest timestamp in dataBlocks, in seconds
// * Note: the OPX_SetDataFormat function determines whether source types or source numbers
//   appear in the data blocks; by default, source numbers are used
//
extern "C" int32_t WINAPI OPX_GetNewDataEx(int32_t* pNumDataBlocks, OPX_DataBlock* dataBlocks,
                                           int32_t* pNumSpikesRead, int32_t* pNumContRead, 
                                           int32_t* pNumEventsRead, int32_t* pNumOtherRead,
                                           double *minTimestamp, double *maxTimestamp);

// OPX_GetNewTimestamps - Get a batch of online client data, as separate parallel arrays of timestamps, 
// source numbers or types, channels, and units (or strobed event words); no spike waveforms or continuous 
// data are returned.
//
// Inputs:
// - pNumTimestamps - pointer to the maximum number of timestamps that the client will accept per call
// - timestamps - caller-allocated array to be filled with timestamps; must be large enough 
//   to accept the specified maximum number of timestamps
// - timestampUpper - caller allocated array; same number of elements as timestamp array
// - sourceNumsOrTypes - caller allocated array; same number of elements as timestamp array
// - channels - caller allocated array; same number of elements as timestamp array
// - units - caller allocated array; same number of elements as timestamp array
// Outputs:
// - pNumTimestamps - the number of timestamps returned
// - timestamps - array of spike and event timestamps, in seconds
// - sourceNumsOrTypes - array of source numbers or source types (SPIKE_TYPE or EVENT_TYPE) for each timestamp
// * Note: the OPX_SetDataFormat function determines whether source types or source numbers
//   are returned in the source numbers array; by default, source numbers are returned
// - channels - array of channel numbers for each timestamp
// - units - array of unit numbers (0 = unsorted, 1 = a, 2 = b, etc) for spikes, or
//   for strobed events, the strobed event word values, for each timestamp
// * Note: For a given array index k, the array elements timestamps[k], timestampsUpper[k], 
//   sourceNumsOrTypes[k], channels[k], and units[k] refer to the kth spike or event
//
extern "C" int32_t WINAPI OPX_GetNewTimestamps(int32_t* pNumTimestamps, double* timestamps, 
                                               uint8_t* sourceNumsOrTypes, WORD* channels, WORD* units);

// OPX_SetDataFormat - Sets the format for data returned by OPX_GetNewData, OPX_GetNewDataEx, and
// OPX_GetNewTimestamps.  Data can be formatted in terms of source-number/source-relative-channel
// or source-type/source-type-relative-channel.  The default is source numbers.
//
// Inputs: 
// - sourceNumberOrType: CHANNEL_FORMAT_SOURCE_RELATIVE or CHANNEL_FORMAT_LINEAR
//
extern "C" int32_t WINAPI OPX_SetDataFormat(int32_t format);
                    
/////////////////////////////////////////////////////////////////////////////////////////////////

// OPX_SendClientDataWords - Sends one or more 16 bit data words to OmniPlex 
// Server.  See the Native Client API documentation for more information on
// this function, its usage and limitations.

// Note that these data words will appear in OmniPlex data on source-relative 
// channel (PL2 channel) CPX1, and on PLX strobed event channel (linear 
// channel) 257.  OmniPlex will hardware-timestamp the event words when they
// are received by OmniPlex, but since the data words are read by polling 
// before being timestamped, there will typically be a latency of 
// 10 msec between when a data word is sent and when it is timestamped.  
// All data words sent in a single call to 
//
// If a client sends data words while a DI strobed input is 
// reading actual hardware strobed data, and/or CinePlex is sending in data on 
// the CPX1 strobed channel, there can be conflicts.  
//
// This function can only used by clients that are running on the same PC as
// OmniPlex.  It cannot be used by remote clients via PlexNet.
                 
extern "C" int32_t WINAPI OPX_SendClientDataWords(int16_t dataWords[], int32_t numWords);                       

/////////////////////////////////////////////////////////////////////////////////////////////////

// OPX_Get64BitIntTimestampFromDataBlock - Extract a 64 bit integer timestamp (in units of ticks)
// from the 40 bit integer timestamp in an OPX_DataBlock.
//
// Inputs:
// - dataBlock: pointer to a data block
// Outputs:
// - function return value is the 64 bit integer timestamp, in units of ticks
//
extern "C" uint64_t WINAPI OPX_Get64BitIntTimestampFromDataBlock(OPX_DataBlock* dataBlock);

// OPX_Make64BitIntTimestampFromUpperLower - Combine the lower 32 bits and upper 8 bits of a 40 bit
// integer timestamp into a 64 bit integer timestamp (in units of ticks).
//
// Inputs:
// - timestampLower: lower 32 bits of the 40 bit timestamp
// - timestampUpper: upper 8 bits of the 40 bit timestamp
//
// Outputs:
// - function return value is the 64 bit integer timestamp, in units of ticks
//
extern "C" uint64_t WINAPI OPX_Make64BitIntTimestampFromUpperLower(uint32_t timestampLower, uint8_t timestampUpper);

// OPX_Get64BitDoubleTimestampFromDataBlock - Extract a 64 bit double precision floating point
// timestamp (in units of seconds) from the 40 bit integer timestamp in an OPX_DataBlock.
//
// Inputs:
// - dataBlock: pointer to a data block
// Outputs:
// - function return value is the 64 bit double precision floating point timestamp, in units of seconds
//
extern "C" double WINAPI OPX_Get64BitDoubleTimestampFromDataBlock(OPX_DataBlock* dataBlock);

// OPX_Make64BitDoubleTimestampFromUpperLower - Combine the lower 32 bits and upper 8 bits of a 40 bit
// integer timestamp into a 64 bit double precision floating point timestamp (in units of seconds).
//
// Inputs:
// - timestampLower: lower 32 bits of the 40 bit timestamp
// - timestampUpper: upper 8 bits of the 40 bit timestamp
//
// Outputs:
// - function return value is the 64 bit double precision floating point timestamp, in units of seconds
//
extern "C" double WINAPI OPX_Make64BitDoubleTimestampFromUpperLower(uint32_t timestampLower, uint8_t timestampUpper);

/////////////////////////////////////////////////////////////////////////////////////////////////

// OPX_GetWaitHandle - Get a handle to be used when waiting for new data (see OPX_WaitForNewData).
// A client should only call this function once, not every time it calls OPX_WaitForNewData.
//
// Inputs: none
// Output: function return value is the handle
//
extern "C" uint32_t WINAPI OPX_GetWaitHandle();

// OPX_WaitForNewData - Wait for new client data.  The client will block (wait) without 
// using CPU time until either new data is available, or the specified timeout elapses.
// 
// Inputs: 
// - hWait - wait handle (see OPX_GetWaitHandle)
// - timeoutMSecs - timeout interval in milliseconds
// Outputs: none
//
extern "C" int32_t WINAPI OPX_WaitForNewData(uint32_t hWait, uint32_t timeoutMSecs);

// OPX_GetOPXSystemStatus - Get the current state of the OmniPlex system.
//
// Inputs: none
// Outputs: function return value is the current OmniPlex state, one of the values:
//   OPX_DAQ_STOPPED: data acquisition is not running
//   OPX_DAQ_STARTED: data acquisition is running
//   OPX_RECORDING: data acquistion is running and OmniPlex is recording
//   OPX_RECORDING_PAUSED: OmniPlex is recording but recording is paused
//
extern "C" int32_t WINAPI OPX_GetOPXSystemStatus();

// OPX_WaitForOPXDAQReady - Wait for OmniPlex data acquisition to begin, or for a
// specified timeout interval to elapse.  Note that this is not the same as waiting for
// new data to become available, after data acquisition has started.
// 
// Input: 
// - timeoutMSecs - timeout interval in milliseconds
// Output: none
//
extern "C" int32_t WINAPI OPX_WaitForOPXDAQReady(uint32_t timeoutMSecs);

// OPX_ClearData - Reads and discards client data, until no more data is immediately 
// available, or until a specified timeout interval elapses.  This is useful when a client
// needs to avoid processing a potentially large "backlog" of data, for example, at startup 
// or if it desires to only occasionally read a "sample" of the incoming data.
//
// Input: 
// - timeoutMSecs - timeout interval in milliseconds
// Outputs: none
// 
extern "C" int32_t WINAPI OPX_ClearData(uint32_t timeoutMSecs);

/////////////////////////////////////////////////////////////////////////////////////////////////

// OPX_GetLastParameterUpdateTime - Get the machine time (Windows time) when any client-accessible 
// OmniPlex parameter was updated.  Clients can use this function to determine whether a  
// parameter has changed since the previous time they called the function.  Note that machine time
// is not the same as acquisition time ("timestamp time").
//
// Inputs: none
// Output: the function return value is the time of the most recent parameter update 
//
extern "C" double WINAPI OPX_GetLastParameterUpdateTime(void);

// OPX_GetLastGainChangeTime - Get the machine time (Windows time) when any source or channel
// gain changed.  Clients can use this function to determine whether one or more gain values
// (and therefore voltage scalers) has changed since the previous time they called the function.  
// Note that machine time is not the same as acquisition time ("timestamp time").
//
// Inputs: none
// Output: the function return value is the time of the most recent gain change
//
extern "C" double WINAPI OPX_GetLastGainChangeTime(void);

// OPX_GetLastWaitEventTime - Get the machine time (Windows time) when new client data was most 
// recently available.  Clients can use this function to determine whether new client data was made
// available since the previous time they called the function.  However, for lowest latency, clients
// should use the OPX_WaitForNewData function.  Note that machine time is not the same as acquisition 
// time ("timestamp time").
//
// Inputs: none
// Output: the function return value is the time when new client data was most recently made available,
// i.e. when clients who were waiting on OPX_WaitForNewData were allowed to continue execution.
//
extern "C" double WINAPI OPX_GetLastWaitEventTime(void);

// OPX_GetLocalMachineTime - Returns the current machine time (Windows time).  Note that machine time 
// is not the same as acquisition time ("timestamp time").
//
// Inputs: none
// Outputs: the function return value is the current machine time.
//
extern "C" double WINAPI OPX_GetLocalMachineTime(void);

/////////////////////////////////////////////////////////////////////////////////////////////////

// OPX_SourceNameToSourceNumber - Convert a source name to the equivalent source number.
//
// Input: 
// - sourceName - source name, e.g. "WB"
// Output:
// - function return value is the source number
//
extern "C" int32_t WINAPI OPX_SourceNameToSourceNumber(char* sourceName);

// OPX_SourceNumberToSourceName - Convert a source number to the equivalent source name.
//
// Input:
// - sourceNum - source number
// Output:
// - sourceName - caller-allocated string (of length at least 255) in which the source name is returned
//
extern "C" int32_t WINAPI OPX_SourceNumberToSourceName(int32_t sourceNum, char* sourceName);

// OPX_SourceChanToLinearChanAndType - Convert a source number and source-relative channel number to
// a source type and a channel number within one of the linear channel number ranges.
//
// Inputs:
// - sourceNum - source number
// - sourceChan - channel number within the specified source
// Outputs:
// - linearChanType - channel type (SPIKE_TYPE, CONTINUOUS_TYPE, EVENT_TYPE, or OTHER_TYPE)
// - linearChan - channel number within the channel range of the returned channel type
//
extern "C" int32_t WINAPI OPX_SourceChanToLinearChanAndType(int32_t sourceNum, int32_t sourceChan,
                                                            int32_t* linearChanType, int32_t* linearChan);

// OPX_SourceChanToLinearChanAndTypeByName - Similar to OPX_SourceChanToLinearChanAndType, but 
// the source is specified by name rather than by number.
//
// Inputs:
// - sourceName - source name
// - sourceChan - channel number within the specified source
// Outputs:
// - linearChanType - channel type (SPIKE_TYPE, CONTINUOUS_TYPE, EVENT_TYPE, or OTHER_TYPE)
// - linearChan - channel number within the channel range of the returned channel type
//
extern "C" int32_t WINAPI OPX_SourceChanToLinearChanAndTypeByName(char* sourceName, int32_t sourceChan,
                                                                  int32_t* linearChanType, int32_t* linearChan);

// OPX_LinearChanAndTypeToSourceChan - Convert a channel type and channel number within that channel 
// type's channel range into the equivalent source number, source name, and source-relative channel.
//
// Inputs: 
// - linearChanType - channel type (SPIKE_TYPE, CONTINUOUS_TYPE, EVENT_TYPE, or OTHER_TYPE)
// - linearChan - channel number within the channel range of the specified channel type
// Outputs:
// - sourceNum - source number
// - sourceName - source name
// - sourceChan - source-relative channel number
//
extern "C" int32_t WINAPI OPX_LinearChanAndTypeToSourceChan(int32_t linearChanType, int32_t linearChan, 
                                                            int32_t* sourceNum, char* sourceName, int32_t* sourceChan);


/////////////////////////////////////////////////////////////////////////////////////////////////

//
// Error codes returned by API functions
//

// No error.
#define OPX_ERROR_NOERROR (0)

// One of the client datapools that OmniPlex uses to communicate with clients 
// was not found.  Check to make sure that OmniPlex Server is running.
#define OPX_ERROR_NODATAPOOL1 (-1)
#define OPX_ERROR_NODATAPOOL2 (-2)

// An attempt to allocate memory failed.
#define OPX_ERROR_NOMEM (-3)

// A bad channel type / source type was passed to an API function.  Valid types are
// SPIKE_TYPE, CONTINUOUS_TYPE, EVENT_TYPE, and OTHER_TYPE.
#define OPX_ERROR_BADCHANTYPE (-4)

// An invalid source number was passed to an API function.  Make sure that you are 
// passing a source number obtained by a call to OPX_GetGlobalParameters.  Note that
// source numbers do not necessarily start at 1 or form a contiguous range of 
// source numbers.
#define OPX_ERROR_BADSOURCENUM (-5)

// An invalid source-relative channel number was passed to an API function.  
// Valid channel numbers are in the range 1..NumberOfChansForSource
#define OPX_ERROR_BADSOURCECHAN (-6)

// An invalid linear channel number was passed to an API function.  
// Valid linear channel numbers start at 0 for channels of CONTINUOUS_TYPE
// and start at 1 for all other channel types.
#define OPX_ERROR_BADLINEARCHAN (-7)

// An invalid data format was passed to an API function.  Valid formats are 
// CHANNEL_FORMAT_LINEAR and CHANNEL_FORMAT_SOURCE_RELATIVE.
#define OPX_ERROR_BADDATAFORMAT (-8)

// A null (zero) parameter was passed to an API function which does not allow a
// null value for that parameter.
#define OPX_ERROR_NULLPARAMETER (-9)

// The requested mapping, for example, between a source name and a source number, 
// could not be performed, possibly because one or more parameters were invalid.
#define OPX_ERROR_MAPPINGFAILED (-10)

// The client failed to initialize.  Either OPX_InitClient was not called before
// any other client API function, or it was called but returned an error code.  
// Attempts to call API functions after a failed initialization will return this error.
#define OPX_INIT_FAILED (-11)

// The wait handle could not be opened.  Make sure that OmniPlex Server is running.
#define OPX_ERROR_NOWAITHANDLE (-12)

// The specified timeout interval elapsed/expired.
#define OPX_ERROR_TIMEOUT (-13)

// OPX_ClearData returned before it was able to clear all the available data.
#define OPX_ERROR_NOTCLEARED (-14)

// OPX_SendClientDataWords was unable to open the CinePlex device client, which is
// used for sending data words to Server
#define OPX_ERROR_OPEN_DLL_FAILED (-15)

// The available data was larger than the return buffer.  The caller should call
// the OPX_Get* function again to get more of the available data.
#define OPX_ERROR_NOT_ALL_DATA_WAS_RETURNED (-16)

#define OPX_ERROR_UNKNOWN (-255)

/////////////////////////////////////////////////////////////////////////////////////////////////
