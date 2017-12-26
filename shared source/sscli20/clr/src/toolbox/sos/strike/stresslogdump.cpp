// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==

#include "strike.h"
#include "util.h"
#include <stdio.h>      
#include <ctype.h>

#ifndef STRESS_LOG
#define STRESS_LOG
#endif // STRESS_LOG
#define STRESS_LOG_READONLY
#include "StressLog.h"

BOOL g_bDacBroken = FALSE;


/*********************************************************************************/
static const WCHAR* getTime(const FILETIME* time, __out_ecount (buffLen) WCHAR* buff, int buffLen) {

		buff = L"BAD TIME";
    
	return buff;
}

/*********************************************************************************/
static inline __int64& toInt64(FILETIME& t) {
	return *((__int64 *) &t);
}

/*********************************************************************************/
ThreadStressLog* ThreadStressLog::FindLatestThreadLog() const {
    const ThreadStressLog* latestLog = 0;
    for (const ThreadStressLog* ptr = this; ptr != NULL; ptr = ptr->next) {
        if (ptr->readPtr != NULL)
            if (latestLog == 0 || ptr->readPtr->timeStamp > latestLog->readPtr->timeStamp)
                latestLog = ptr;
    }
    return const_cast<ThreadStressLog*>(latestLog);
}

char* getFacilityName(DWORD_PTR lf)
{
    struct FacilityName_t { size_t lf; char* lfName; };
    #define DEFINE_LOG_FACILITY(logname, value) {logname, #logname},
    static FacilityName_t facilities[] =
    {
        #include <loglf.h>
        { LF_ALWAYS, "LF_ALWAYS" }
    };
    static char buff[1024] = "`";
    if ( lf == LF_ALL )
        return "`ALL`";
    else 
    {
        buff[1] = '\0';
        for ( int i = 0; i < 32; ++i )
        {
            if ( lf & 0x1 )
            {
                strcat ( buff, &(facilities[i].lfName[3]) );
                strcat ( buff, "`" );
            }
            lf >>= 1;
        }
        return buff;
    }
}

/***********************************************************************************/
/* recognize special pretty printing instructions in the format string             */
/* Note that this function might have side effect such that args array value might */
/* be altered if format string contains %s                                         */
void formatOutput(struct IDebugDataSpaces* memCallBack, __in FILE* file, __inout __inout_z char* format, unsigned threadId, double timeStamp, DWORD_PTR facility, __in void** args)
{
    fprintf(file, "%4x %13.9f : ", threadId, timeStamp);
    fprintf(file, "%-20s ", getFacilityName ( facility ));

    CQuickBytes fullname;
    char* ptr = format;
    void** argsPtr = args;
    wchar_t buff[2048];
    static char formatCopy[256];
    
    char *strBuf = NULL;
    int iStrArgCount = 0;
    int iArgCount = 0;
    
    strcpy_s(formatCopy, _countof(formatCopy), format);
    for(;;) {
        char c = *ptr++;
        if (c == 0)
            break;
        if (c == '{')           // Reverse the '{' 's because the log is displayed backwards
            ptr[-1] = '}';
        else if (c == '}')
            ptr[-1] = '{';
        else if (c == '%') {
            argsPtr++;          // This format will consume one of the args
            if (*ptr == '%') {
                ptr++;          // skip the whole %%
                --argsPtr;      // except for a %% 
            }
            else if (*ptr == 'p') { // It is a %p
                ptr++;
                if (isalpha(*ptr)) {    // It is a special %p formatter
                        // Print the string up to that point
                    c = *ptr;
                    *ptr = 0;       // Terminate the string temporarily
                    fprintf(file, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
                    *ptr = c;       // Put it back  

                        // move the argument pointers past the part the was printed
                    format = ptr + 1;
                    args = argsPtr;
                    iArgCount = -1;
                    iStrArgCount = 0;
                    DWORD_PTR arg = DWORD_PTR(argsPtr[-1]);

                    switch (c) {

                        case 'M':   // format as a method Desc
                            if (g_bDacBroken)
                            {
                                fprintf(file," (MethodDesc: %p)",arg);
                            }
                            else
                            {
                                if (!IsMethodDesc(arg)) {
                                    if (arg != 0) 
                                        fprintf(file, " (BAD Method)");
                                }
                                else 
                                {
                                    DacpMethodDescData MethodDescData;
                                    MethodDescData.Request(g_clrData,(CLRDATA_ADDRESS)arg);

                                    static WCHAR wszNameBuffer[1024]; // should be large enough
                                    if (DacpMethodDescData::GetMethodName(g_clrData, arg, 1024, wszNameBuffer) != S_OK)
                                    {
                                        wcscpy_s(wszNameBuffer, _countof(wszNameBuffer),L"UNKNOWN METHODDESC");
                                    }

                                    wcscpy_s(buff, _countof(buff), wszNameBuffer);
                                    fprintf(file, " (%S)", wszNameBuffer);
                                }
                            }
                            break;

							// fall through
                        case 'T':       // format as a MethodTable
                            if (g_bDacBroken)
                            {
                                fprintf(file, "(MethodTable: %p)",arg);
                            }
                            else
                            {
                                if (arg & 3) {
                                    arg &= ~3;      // GC steals the lower bits for its own use during GC.  
                                    fprintf(file, " Low Bit(s) Set");
                                }
                                if (!IsMethodTable(arg))
                                    fprintf(file, " (BAD MethodTable)");
                                else {
                                    NameForMT_s (arg, g_mdName, mdNameLen);
                                    fprintf(file, " (%S)", g_mdName);
                                }
                            }
                            break;

                        case 'V': {     // format as a C vtable pointer 
                            char Symbol[1024];
                            ULONG64 Displacement;
                            HRESULT hr = g_ExtSymbols->GetNameByOffset(arg, Symbol, 1024, NULL, &Displacement);
                            if (SUCCEEDED(hr) && Symbol[0] != '\0' && Displacement == 0) 
                                fprintf(file, " (%s)", Symbol);
                            else 
                                fprintf(file, " (Unknown VTable)");
                            }
                            break;
                        case 'K':
                            {   // format a frame in stack trace
                                char Symbol[1024];
                                ULONG64 Displacement;
                                HRESULT hr = g_ExtSymbols->GetNameByOffset (arg, Symbol, 1024, NULL, &Displacement);
                                if (SUCCEEDED (hr) && Symbol[0] != '\0') 
                                {
                                    fprintf (file, " (%s", Symbol);
                                    if (Displacement)
                                    {
                                        fprintf (file, "+%#x", Displacement);
                                    }
                                    fprintf (file, ")");
                                }
                                else 
                                    fprintf (file, " (Unknown function)");    
                            }
                            break;
                        default:
                            format = ptr;   // Just print the character. 
                    }
                }				
            }
            else if (*ptr == 's') {

                HRESULT     hr;

                // don't need to check strBuf since stack overflow exception
                // will be thrown instead. Do a memory allocation on stack
                ULONG capacity_strBuf = 256;
                strBuf = (char *)_alloca(capacity_strBuf);
                
                // We only support two string arguments for now. 
                // This is where the string arg is at
                // arg[iStrArgCount]
                hr = memCallBack->ReadVirtual(ULONG64((char* )args[iArgCount]), strBuf, capacity_strBuf, 0);
                if (hr != S_OK) 
                {
                    strcpy_s(strBuf, capacity_strBuf, "(#Could not read address of string#)");           
                }

                args[iArgCount] = strBuf;                    
                iStrArgCount++;
                
            }
            iArgCount++;
        }
    }
        // Print anything after the last special format instruction.
    fprintf(file, format, args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
    fprintf(file, "\n");
}


/*********************************************************************************/
HRESULT StressLog::Dump(ULONG64 outProcLog, const char* fileName, struct IDebugDataSpaces* memCallBack) {

    ULONG64 g_hThisInst;

    // Fetch the circular buffer bookeeping data 
    StressLog inProcLog;
    HRESULT hr = memCallBack->ReadVirtual(outProcLog, &inProcLog, sizeof(StressLog), 0);
    if (hr != S_OK) return hr;
    if (inProcLog.logs == NULL || inProcLog.hMod == NULL) {
		ExtOut ( "----- No thread logs in the image: The stress log was probably not initialized correctly. -----\n");
              return S_FALSE;
    }

    g_hThisInst = (ULONG64) inProcLog.hMod;

    ExtOut("Writing to file: %s\n", fileName);
    ExtOut("Stress log in module 0x%p\n", g_hThisInst);
    ExtOut("Stress log address = 0x%p\n", (void*) outProcLog);

    // Fetch the circular buffers for each thread into the 'logs' list
    ThreadStressLog* logs = 0;

    ULONG64 outProcPtr = (ULONG64)(inProcLog.logs);
    ThreadStressLog* inProcPtr;
    ThreadStressLog** logsPtr = &logs;
    int threadCtr = 0;
	unsigned __int64 lastTimeStamp = 0;		// timestmap of last log entry
    while(outProcPtr != 0) {
        inProcPtr = new ThreadStressLog;
        hr = memCallBack->ReadVirtual(outProcPtr, inProcPtr, sizeof (*inProcPtr), 0);
        if (hr != S_OK || inProcPtr->chunkListHead == NULL)
        {
            delete inProcPtr;
            goto FREE_MEM;
        }

        ULONG64 outProcListHead = (ULONG64)(inProcPtr->chunkListHead);
        ULONG64 outProcChunkPtr = outProcListHead;
        StressLogChunk ** chunksPtr = &inProcPtr->chunkListHead;
        StressLogChunk * inProcPrevChunkPtr = NULL;
        BOOL curPtrInitialized = FALSE;
        do
        {
            StressLogChunk * inProcChunkPtr = new StressLogChunk;
            hr = memCallBack->ReadVirtual (outProcChunkPtr, inProcChunkPtr, sizeof (*inProcChunkPtr), NULL);
            if (hr != S_OK || !inProcChunkPtr->IsValid ())
            {
                if (hr != S_OK)
                    ExtOut ("ReadVirtual failed with code hr = %x.\n", hr );
                else
                    ExtOut ("Invalid stress log chunk: %p", outProcChunkPtr);

                // Now cleanup
                delete inProcChunkPtr;
                // if this is the first time through, inProcPtr->chunkListHead may still contain
                // the out-of-process value for the chunk pointer.  NULL it to avoid AVs
                if ((ULONG64)inProcPtr->chunkListHead == outProcListHead)
                   inProcPtr->chunkListHead = NULL; 
                delete inProcPtr;
                goto FREE_MEM;
            }

            if (!curPtrInitialized && outProcChunkPtr == (ULONG64) (inProcPtr->curWriteChunk))
            {
                inProcPtr->curPtr = (StressMsg *)((BYTE *)inProcChunkPtr + ((BYTE *)inProcPtr->curPtr - (BYTE *)inProcPtr->curWriteChunk));
                inProcPtr->curWriteChunk = inProcChunkPtr;
                curPtrInitialized = TRUE;
            }
            
            outProcChunkPtr = (ULONG64)(inProcChunkPtr->next);
            *chunksPtr = inProcChunkPtr;
            chunksPtr = &inProcChunkPtr->next;
            inProcChunkPtr->prev = inProcPrevChunkPtr;
            inProcPrevChunkPtr = inProcChunkPtr;

            if (outProcChunkPtr == outProcListHead)
            {
                inProcChunkPtr->next = inProcPtr->chunkListHead;
                inProcPtr->chunkListHead->prev = inProcChunkPtr;
                inProcPtr->chunkListTail = inProcChunkPtr;
            }           
        } while (outProcChunkPtr != outProcListHead);
        
        if (!curPtrInitialized)
        {
            delete inProcPtr;
            goto FREE_MEM;
        }

        inProcPtr->Activate ();
        if (inProcPtr->readPtr->timeStamp > lastTimeStamp)
        {
	    lastTimeStamp = inProcPtr->readPtr->timeStamp;
        }

        outProcPtr = (ULONG64)(inProcPtr->next);
        *logsPtr = inProcPtr;
        logsPtr = &inProcPtr->next;
        threadCtr++;
    }

    FILE* file = NULL;
    if (fopen_s(&file, fileName, "w") != 0) {
        hr = GetLastError();
        goto FREE_MEM;
    }
    hr = S_FALSE;       // return false if there are no message to print to the log

    fprintf(file, "STRESS LOG:\n"
              "    facilitiesToLog  = 0x%x\n"
              "    levelToLog       = %d\n"
              "    MaxLogSizePerThread = 0x%x (%d)\n"
              "    MaxTotalLogSize = 0x%x (%d)\n"
              "    CurrentTotalLogChunk = %d\n"
              "    ThreadsWithLogs  = %d\n",
        inProcLog.facilitiesToLog, inProcLog.levelToLog, inProcLog.MaxSizePerThread, inProcLog.MaxSizePerThread,
        inProcLog.MaxSizeTotal, inProcLog.MaxSizeTotal, inProcLog.totalChunk, threadCtr);

	FILETIME endTime;
	double totalSecs  = ((double) (lastTimeStamp - inProcLog.startTimeStamp)) / inProcLog.tickFrequency;
	toInt64(endTime) = toInt64(inProcLog.startTime) + ((__int64) (totalSecs * 1.0E7));

	WCHAR timeBuff[64];
    fprintf(file, "    Clock frequency  = %5.3f GHz\n", inProcLog.tickFrequency / 1.0E9);
    fprintf(file, "    Start time         %S\n", getTime(&inProcLog.startTime, timeBuff, 64));
    fprintf(file, "    Last message time  %S\n", getTime(&endTime, timeBuff, 64));
	fprintf(file, "    Total elapsed time %5.3f sec\n", totalSecs);

    fprintf(file, "\nTHREAD  TIMESTAMP     FACILITY                              MESSAGE\n");
    fprintf(file, "  ID  (sec from start)\n");
    fprintf(file, "--------------------------------------------------------------------------------------\n");
    char format[257];
    format[256] = format[0] = 0;
    void** args;
    unsigned msgCtr = 0;
    for (;;) {
        ThreadStressLog* latestLog = logs->FindLatestThreadLog();

        if (IsInterrupt()) {
            fprintf(file, "----- Interrupted by user -----\n");
            break;
        }

        if (latestLog == 0) {
            break;
        }
        StressMsg* latestMsg = latestLog->readPtr;
        if (latestMsg->formatOffset != 0 && !latestLog->CompletedDump()) {
            hr = memCallBack->ReadVirtual((ULONG64)((((ULONG64)latestMsg->formatOffset) << 2) + g_hThisInst), format, 256, 0);
            if (hr != S_OK) 
                strcpy_s(format, _countof(format), "Could not read address of format string");

			double deltaTime = ((double) (latestMsg->timeStamp - inProcLog.startTimeStamp)) / inProcLog.tickFrequency;
            if (strcmp(format, ThreadStressLog::TaskSwitchMsg()) == 0)
            {
                fprintf (file, "Task was switched from %x\n", (unsigned)(size_t)latestMsg->args[0]);
                latestLog->threadId = (unsigned)(size_t)latestMsg->args[0];
            }
            else {
                args = latestMsg->args;
                formatOutput(memCallBack, file, format, latestLog->threadId, deltaTime, latestMsg->facility, args);
            }
            msgCtr++;
        }

        latestLog->readPtr = latestLog->AdvanceRead();
        if (latestLog->CompletedDump())
        {
            latestLog->readPtr = NULL;
            fprintf(file, "------------ Last message from thread %x -----------\n", latestLog->threadId);
        }

        if (msgCtr % 64 == 0) 
        {
            ExtOut(".");        // to indicate progress
            if (msgCtr % (64*64) == 0) 
                ExtOut("\n");   
        }
    }
    ExtOut("\n");

    fprintf(file, "---------------------------- %d total entries ------------------------------------\n", msgCtr);
    fclose(file);

FREE_MEM:
    // clean up the 'logs' list
    while (logs) {
        ThreadStressLog* temp = logs;
        logs = logs->next;
        delete temp;
    }

    return hr;
}

