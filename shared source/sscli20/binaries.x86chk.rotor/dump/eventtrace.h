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
/*++

Module Name:

    eventtrace.h

Abstract:

    This module implements Event Tracing for Windows support


Revision History:
    August 2002 -                                       - Created

--*/

#ifndef _ETWTRACER_HXX_
#define _ETWTRACER_HXX_


#define ETW_TRACER_BUILD 2195       // Earliest Build ETW Tracing works on
#define ETW_ENABLED_MAJORVER 6      // OS versions >= to this we enable ETW
										// registration by default, since on XP
										// and Windows 2003, registration is too slow.

#define ETWMAX_TRACE_LEVEL 6       // Maximum Number of Trace Levels supported

#define TRACE_LEVEL_NONE        0   // Tracing is not on
#define TRACE_LEVEL_FATAL       1   // Abnormal exit or termination
#define TRACE_LEVEL_ERROR       2   // Severe errors that need logging
#define TRACE_LEVEL_WARNING     3   // Warnings such as allocation failure
#define TRACE_LEVEL_INFORMATION 4   // Includes non-error cases such as Entry-Exit
#define TRACE_LEVEL_VERBOSE     5   // Detailed traces from intermediate steps

// Provider trace flags
#define TRACE_FLAG_GC                   0x00000001  // GC stats
#define TRACE_FLAG_THREADPOOL_QUEUE     0x00000002  // threadpool queueing stats
#define TRACE_FLAG_FUSION               0x00000004  // assembly binding logging
#define TRACE_FLAG_STARTUP              0x80000000  // startup events (PRIVATE)


//
//--> Define this Provider's Event Types. 
//

//
// Collection transaction.
//
// NOTE: These define's cannot change value, unless you also change the MOF.
// The safe thing is to just never reuse if you happen to get rid of an event.

// GC
#define ETW_TYPE_GC_START_COLLECTION    1   // start GC
#define ETW_TYPE_GC_END_COLLECTION      2   // end GC
#define ETW_TYPE_GC_END_MARK_PHASE      3   // end of mark
#define ETW_TYPE_GC_DECISION            4   // did we choose mark/compact or sweep?
#define ETW_TYPE_GC_HEAP                5   // heap data
#define ETW_TYPE_GC_CREATE_SEGMENT      6   // create new segment
#define ETW_TYPE_GC_FREE_SEGMENT        7   // free segment, returning memory to OS
#define ETW_TYPE_GC_RESUME              8   // resume from suspension
#define ETW_TYPE_GC_PROMOTED            9   // promoted memory
#define ETW_TYPE_GC_SUSPEND             10  // suspend EE
#define ETW_TYPE_GC_ALLOCATION_TICK     11  // allocation tick
#define ETW_TYPE_GC_CREATE_CONCURRENT_THREAD 12  //create concurrent thread
#define ETW_TYPE_GC_TERMINATE_CONCURRENT_THREAD 13 //when terminate due to timeout
#define ETW_TYPE_GC_SETTINGS 14             //describe GC settings on start
#define ETW_TYPE_GC_FINALIZED 15            //fires when all finalizers have been called 




// ThreadPool
#define ETW_TYPE_THREADPOOL_CREATE_WORKER   1
#define ETW_TYPE_THREADPOOL_TERM_WORKER     2

#define ETW_TYPE_THREADPOOL_CREATE_IO       1
#define ETW_TYPE_THREADPOOL_TERM_IO         2

#define ETW_TYPE_THREADPOOL_RETIRE_WORKER   1
#define ETW_TYPE_THREADPOOL_UNRETIRE_WORKER 2

#define ETW_TYPE_THREADPOOL_RETIRE_IO       1
#define ETW_TYPE_THREADPOOL_UNRETIRE_IO     2

#define ETW_TYPE_THREADPOOL_SUSPENDTHREAD   1
#define ETW_TYPE_THREADPOOL_RESUMETHREAD    2

// Exceptions
#define ETW_TYPE_EXCEPTION_THROWN           1

// Monitor
#define ETW_TYPE_MON_CONTENTION             1

// Startup event codes
//
// Events count by two because in some places
// we signal end(event) as event+1
//
#define ETW_TYPE_STARTUP_EESTARTUP           0
#define ETW_TYPE_STARTUP_EECONFIG_SETUP      2
#define ETW_TYPE_STARTUP_LD_SYS_BASES        4
#define ETW_TYPE_STARTUP_EXEC_EXE            6
#define ETW_TYPE_STARTUP_MAIN                8
#define ETW_TYPE_STARTUP_APPLY_POLICY        10
#define ETW_TYPE_STARTUP_LDLIB_SHFOLDER      12
#define ETW_TYPE_STARTUP_PRESTUBWORKER       14
#define ETW_TYPE_STARTUP_GETINSTALLATION     16
#define ETW_TYPE_STARTUP_OPENHMODULE         18
#define ETW_TYPE_STARTUP_EXPLICITBIND        20
#define ETW_TYPE_STARTUP_PARSEXML            22
#define ETW_TYPE_STARTUP_INITDEFAULTDOMAIN   24
#define ETW_TYPE_STARTUP_INITSECURITY        26
#define ETW_TYPE_STARTUP_ALLOWBINDINGREDIRS  28
#define ETW_TYPE_STARTUP_EECONFIG_SYNC       30
#define ETW_TYPE_STARTUP_FUSION_BINDING      32      
#define ETW_TYPE_STARTUP_LOADERCATCHALL      34      // Assembly Loading
#define ETW_TYPE_STARTUP_FUSIONINIT          36
#define ETW_TYPE_STARTUP_FUSIONAPPCTX        38
#define ETW_TYPE_STARTUP_FUSION2EECALL       40
#define ETW_TYPE_STARTUP_SECURITYCATCHALL    42

// Fusion binding log
#define ETW_TYPE_FUSION_START          0x01
#define ETW_TYPE_FUSION_END            0x02
#define ETW_TYPE_FUSION_HEADER         0x03
#define ETW_TYPE_FUSION_DEFAULT        0x10
#define ETW_TYPE_FUSION_NGEN           0x11
#define ETW_TYPE_FUSION_RESULT_DEFAULT 0x20
#define ETW_TYPE_FUSION_RESULT_NGEN    0x21

//
// GC data.
//
// NOTE: This matches the MOF.
// The fields are also intentionally arranged biggest to smallest, to avoid having
// to guess how the MOF is aligned.
//

typedef union st_GCEventInfo {

    typedef struct _GenerationInfo {
        ULONGLONG GenerationSize;
        ULONGLONG TotalPromotedSize;        
    
    } GenerationInfo;

    struct {
        GenerationInfo GenInfo[4]; // the heap info on gen0, gen1, gen2 and the large object heap.
        ULONGLONG 	   FinalizationPromotedSize; //not available per generation
        ULONG      	   FinalizationPromotedCount; //not available per generation
        ULONG          PinnedObjectCount;
        ULONG          SinkBlockCount;
        ULONG          GCHandleCount;

    } HeapStats;

    typedef enum _HeapType {
        SMALL_OBJECT_HEAP, LARGE_OBJECT_HEAP, READ_ONLY_HEAP
    } HeapType;
    struct {
        ULONGLONG Address;
        ULONGLONG Size;
        HeapType Type;
    } GCCreateSegment;

    struct {
        ULONGLONG Address;
    } GCFreeSegment;
    struct {
        ULONG Count;
        UINT Depth;
    } GCEnd;

    typedef enum _AllocationKind {
        AllocationSmall = 0,
        AllocationLarge
    }AllocationKind;
    struct {
        ULONG Allocation;
        AllocationKind Kind;
    } AllocationTick;

    typedef  enum _GC_REASON { 
        GC_ALLOC = 0 , GC_INDUCED = 1 , GC_LOWMEMORY = 2
    } GC_REASON;
    struct {
        ULONG Count;
        GC_REASON Reason;
    } GCStart;

    struct {
        ULONG Count; // how many finalizers we called.
    } GCFinalizers;

    struct {
        BOOL DoCompact;
    } CompactOrSweep;

    struct {
        UINT Reason;
    } SuspendEE;

    struct {

        BOOL ServerGC; // TRUE means it’s server GC; FALSE means it’s workstation.
        ULONGLONG SegmentSize; 
        ULONGLONG LargeObjectSegmentSize; 

    } GCSettings;


    
} ETW_GC_INFO, *PETW_GC_INFO;

//
// Threadpool data.
//
// NOTE: This matches the MOF.
// The fields are also intentionally arranged biggest to smallest, to avoid having
// to guess how the MOF is aligned.
//
typedef union st_ThreadpoolEventInfo {
    struct {
        ULONG Count;
        ULONG NumRetired;
    } WorkerThread;
    struct {
        ULONG Count;
        ULONG NumRetired;
    } IOThread;
    struct {
        ULONG ThreadID;
        ULONG CPUUtil;
    } SuspendResume;
    
} ETW_THREADPOOL_INFO, *PETW_THREADPOOL_INFO;

//
//--> Define any transaction Guids used
//
DEFINE_GUID ( /* 044973cd-251f-4dff-a3e9-9d6307286b05 */
    GCEventGuid,
    0x044973cd,
    0x251f,
    0x4dff,
    0xa3, 0xe9, 0x9d, 0x63, 0x07, 0x28, 0x6b, 0x05
  );

DEFINE_GUID ( /* cfc4ba53-fb42-4757-8b70-5f5d51fee2f4 */
    WorkerThreadCreateGuid,
    0xcfc4ba53,
    0xfb42,
    0x4757,
    0x8b, 0x70, 0x5f, 0x5d, 0x51, 0xfe, 0xe2, 0xf4
  );

DEFINE_GUID ( /* efdf1eac-1d5d-4e84-893a-19b80f692176 */
    WorkerThreadRetireGuid,
    0xefdf1eac,
    0x1d5d,
    0x4e84,
    0x89, 0x3a, 0x19, 0xb8, 0x0f, 0x69, 0x21, 0x76
  );

DEFINE_GUID ( /* c71408de-42cc-4f81-9c93-b8912abf2a0f */
    IOThreadCreateGuid,
    0xc71408de,
    0x42cc,
    0x4f81,
    0x9c, 0x93, 0xb8, 0x91, 0x2a, 0xbf, 0x2a, 0x0f
  );

DEFINE_GUID ( /* 840c8456-6457-4eb7-9cd0-d28f01c64f5e */
    IOThreadRetireGuid,
    0x840c8456,
    0x6457,
    0x4eb7,
    0x9c, 0xd0, 0xd2, 0x8f, 0x01, 0xc6, 0x4f, 0x5e
  );

DEFINE_GUID ( /* c424b3e3-2ae0-416e-a039-410c5d8e5f14 */
    ThreadpoolSuspendGuid,
    0xc424b3e3,
    0x2ae0,
    0x416e,
    0xa0, 0x39, 0x41, 0x0c, 0x5d, 0x8e, 0x5f, 0x14
  );

DEFINE_GUID ( /* 300ce105-86d1-41f8-b9d2-83fcbff32d99 */
    ExceptionGuid,
    0x300ce105,
    0x86d1,
    0x41f8,
    0xb9, 0xd2, 0x83, 0xfc, 0xbf, 0xf3, 0x2d, 0x99
  );

DEFINE_GUID ( /* 561410f5-a138-4ab3-945e-516483cddfbc */
    MonitorGuid,
    0x561410f5,
    0xa138,
    0x4ab3,
    0x94, 0x5e, 0x51, 0x64, 0x83, 0xcd, 0xdf, 0xbc
  );

DEFINE_GUID(// {02D08A4F-FD01-4538-989B-03E437B950F4}
    StartupGuid,
    0x2d08a4f,
    0xfd01,
    0x4538,
    0x98, 0x9b, 0x3, 0xe4, 0x37, 0xb9, 0x50, 0xf4);

DEFINE_GUID( /* 842e6683-9da0-482d-8138-a785e39a2de3 */
    FusionGuid,
    0x842e6683,
    0x9da0,
    0x482d,
    0x81, 0x38, 0xa7, 0x85, 0xe3, 0x9a, 0x2d, 0xe3
    );


class CEtwTracer {
private:

public:

    /* Dummy implementation for FEATURE_PAL
     ***********************************************/

    ULONG Register()
    {
        LEAF_CONTRACT;
        return ERROR_SUCCESS;
    }

    ULONG UnRegister()
    {
        LEAF_CONTRACT;
        return ERROR_SUCCESS;
    }

    ULONG EtwTraceEvent(IN LPCGUID pGuid,
                        IN ULONG EventType,
                        IN PVOID Source,
                        IN ULONG Length)
    {
        LEAF_CONTRACT;
        return 0;
    }

    BOOL TraceEnabled() { LEAF_CONTRACT;  return FALSE; }
    
    BOOL TraceEnabled(IN ULONG Level) { LEAF_CONTRACT; return FALSE; }

    BOOL TraceFlagEnabled(IN ULONG Flag) { LEAF_CONTRACT; return FALSE; }


};



//
// The ONE and only ONE global instantiation of this class
//
extern CEtwTracer *  g_pEtwTracer;

#define ETW_IS_TRACE_ON(level) ( (g_pEtwTracer != NULL) && (g_pEtwTracer->TraceEnabled(level)) ) 
#define ETW_IS_FLAG_ON(flag) ( (g_pEtwTracer != NULL) && (g_pEtwTracer->TraceFlagEnabled(flag)) ) 



class ETWTraceStartup {
    ULONG event;
public:
    ETWTraceStartup(ULONG event_) {
        event = event_;
        TraceEvent(event);
    }
    ~ETWTraceStartup() {
        TraceEvent(event+1);
    }
    static void TraceEvent(ULONG event) {
        if (ETW_IS_TRACE_ON(TRACE_LEVEL_INFORMATION) &&
            ETW_IS_FLAG_ON(TRACE_FLAG_STARTUP))
        {
            g_pEtwTracer->EtwTraceEvent(&StartupGuid, event, NULL, 0);
        }
    }
};


#endif //_ETWTRACER_HXX_
