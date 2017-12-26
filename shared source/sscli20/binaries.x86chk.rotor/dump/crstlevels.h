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


//===============================================================================================
// This file declares all of the levels used by our hierarchical critical sections.
// The rule is that you may take a critical sections only in the order of decreasing
// leval rankings (i.e. you can take a rank 40 crst and then take a rank 20 crst, in that order,
// but not vice versa.)
//
// You'll notice that this file only defines the names - not the actual rankings. We don't
// want to put the actual rankings here because this file is included in global header files
// and we went to the trouble of refactoring all this precisely so we don't have to
// trigger a complete source rebuild just to adjust a crst level ranking.
//
// The actual rankings are now in src\vm\crst.cpp. The values that used to be members of the
// CrstLevel enum are now just pointers to static strings (i.e. CrstClassInit is now the string
// "CrstClassInit.") Mscorwks.dll will resolve these string names into the actual rankings at runtime
// rather than at compiletime.
//
// To change the rank of an existing level, just go to crst.cpp where you'll see a lot of global
// variable initializations that look like this:
//
//      UINT CrstFusionListRanking = 86;    // Note the "Ranking" suffix added to these names
//      UINT CrstFusionLogRanking  = 99;
//
//      ...etc.
//
// Just change the appropriate number.
//
// If you want to the change the ranking as an experiment, there's even an easier way. You can
// override a specific ranking just by setting an extra regkey. For example, to change
// CrstFusionLog from "99" to "42", set the regkey:
//
//      HKLM\Software\Microsoft\.NETFramework\CrstFusionLog = 42
//
// This lets you try out various rankings without even rebuilding the runtime. Once you and the
// bvt's are satisifed that the order is correct, you must put the actual numbers into crst.cpp.
//
// To ADD a new level, you must update both this file and crst. To add a new crst level called
// BANANA, for example, add this line to this file:
//
//      DEFINE_CRST_LEVEL(CrstBanana)
//
// and add a line to crst.cpp
//
//      UINT CrstBananaRanking = <whatever>;
//
// The second step is mandatory - if you fail to do it, you will get a link error when building
// mscorwks.
// 
//
// 
// 
//===============================================================================================






// Fusion critical sections.
//
// These critical sections are ordered correctly with respect to each other
// but have not yet been ordered with respect to the places the CLR and
// Fusion interact (reading metadata, strong name signature validation).
// When these code paths have their crsts levelled these levels will
// probably change.
//

DEFINE_CRST_LEVEL(CrstFusionList                     )
DEFINE_CRST_LEVEL(CrstFusionLog                      )
DEFINE_CRST_LEVEL(CrstFusionSingleUse                )
DEFINE_CRST_LEVEL(CrstFusionAsmImprt                 )
DEFINE_CRST_LEVEL(CrstFusionHistory                  )    
DEFINE_CRST_LEVEL(CrstFusionModImprt                 )    
DEFINE_CRST_LEVEL(CrstFusionNgenIndex                )    

DEFINE_CRST_LEVEL(CrstFusionClb                      )    
DEFINE_CRST_LEVEL(CrstFusionPcyCache                 )    
    
DEFINE_CRST_LEVEL(CrstFusionLoadContext              )    
DEFINE_CRST_LEVEL(CrstFusionConfigSettings           )    

DEFINE_CRST_LEVEL(CrstFusionNgenIndexPool            )    
DEFINE_CRST_LEVEL(CrstFusionGACAsmPool               )    
DEFINE_CRST_LEVEL(CrstFusionNIAsmPool                )    

DEFINE_CRST_LEVEL(CrstFusionDownload                 )    
DEFINE_CRST_LEVEL(CrstFusionAssemblyDownload         )       // must be more than CrstThreadStore

DEFINE_CRST_LEVEL(CrstFusionAppCtx                   )     
DEFINE_CRST_LEVEL(CrstFusionClosure                  )     
DEFINE_CRST_LEVEL(CrstFusionBindResult               )     

DEFINE_CRST_LEVEL(CrstDummy                          )             // For internal use only. Not a true level.

DEFINE_CRST_LEVEL(CrstGCMemoryPressure               )             // used by the AddMemoryPressure/RemoveMemoryPressure apis
DEFINE_CRST_LEVEL(CrstStubDispatchCache              )
DEFINE_CRST_LEVEL(CrstPEImageView                    )
DEFINE_CRST_LEVEL(CrstStrongName                     )             // Some crypto APIs are non thread safe. This Crst is used to serialize these operations.
DEFINE_CRST_LEVEL(CrstSaveModuleProfileData          )
DEFINE_CRST_LEVEL(CrstIbcProfile                     )             // For the ibc profiler - should not enter any other critical section while in here 
DEFINE_CRST_LEVEL(CrstIJWFixupData                   )    
DEFINE_CRST_LEVEL(CrstPEImage                        )       
DEFINE_CRST_LEVEL(CrstObjectList                     )             // For the object list
DEFINE_CRST_LEVEL(CrstThreadIdDispenser              )             // For the thin lock thread ids - needs to be less than CrstThreadStore
DEFINE_CRST_LEVEL(CrstMda                            )             // For inilization of mda after dynamic activation
DEFINE_CRST_LEVEL(CrstCer                            )             // For manipulating per-module hash tables used by Critical Execution Regions
DEFINE_CRST_LEVEL(CrstUniqueStack                    )             // For FastGCStress, which should be lower than CrstLeaf
DEFINE_CRST_LEVEL(CrstOnEventManager                 )             // For CCLROnEventManager
DEFINE_CRST_LEVEL(CrstInterfaceVTableMap             )             // synchronize access to InterfaceVTableMap
DEFINE_CRST_LEVEL(CrstStubInterceptor                )             // stub tracker (debug       )    
DEFINE_CRST_LEVEL(CrstStubTracker                    )             // stub tracker (debug       )    
DEFINE_CRST_LEVEL(CrstSyncBlockCache                 )             // allocate a SyncBlock to an object -- taken inside CrstHandleTable
DEFINE_CRST_LEVEL(CrstHandleTable                    )             // allocate / release a handle (called inside CrstSingleUseLock       )    
DEFINE_CRST_LEVEL(CrstCompressedStackRef             )             // Reference counting lock for the compressed stack
DEFINE_CRST_LEVEL(CrstExecuteManRangeLock            )    
DEFINE_CRST_LEVEL(CrstSyncHashLock                   )             // used for synchronized access to a hash table
DEFINE_CRST_LEVEL(CrstSingleUseLock                  )             // one time initialization of data, locks use this level
DEFINE_CRST_LEVEL(CrstModule                         )    
DEFINE_CRST_LEVEL(CrstModuleLookupTable              )    
DEFINE_CRST_LEVEL(CrstArgBasedStubCache              )    
DEFINE_CRST_LEVEL(CrstThread                         )             // used during e.g. thread suspend
DEFINE_CRST_LEVEL(CrstMLCache                        )    
DEFINE_CRST_LEVEL(CrstUMThunkHash                    )    
DEFINE_CRST_LEVEL(CrstMUThunkHash                    )    
DEFINE_CRST_LEVEL(CrstReflection                     )             // Reflection memory setup
DEFINE_CRST_LEVEL(CrstADCompressedStackRegistry      )            // lock for protecting access to the AD domain compressed stack list
DEFINE_CRST_LEVEL(CrstCompressedStackTransition      )            // State transition lock for the compressed stack
DEFINE_CRST_LEVEL(CrstSecurityPolicyCache            )             // For Security policy cache
DEFINE_CRST_LEVEL(CrstSecurityStackwalkCache         )             // For security stackwalk cache
DEFINE_CRST_LEVEL(CrstRCWCache                       )             // For RCWCache
DEFINE_CRST_LEVEL(CrstSigConvert                     )             // convert a gsig_ from text to binary
DEFINE_CRST_LEVEL(CrstCompressedStackListCleanup     )          // List cleanup lock for the compressed stack
DEFINE_CRST_LEVEL(CrstJumpStubCache                  )    
DEFINE_CRST_LEVEL(CrstPatchEntryPoint                )    
DEFINE_CRST_LEVEL(CrstThreadStore                    )             // used to e.g. iterate over threads in system


DEFINE_CRST_LEVEL(CrstDebuggerFavorLock              )    
DEFINE_CRST_LEVEL(CrstDebuggerHeapLock               )    
DEFINE_CRST_LEVEL(CrstDebuggerJitInfo                )             // Debugger Jit Info lock
DEFINE_CRST_LEVEL(CrstDebuggerMutex                  )             // Main Debugger lock, used in synchronization.
DEFINE_CRST_LEVEL(CrstDebuggerControllerMutex        )             // lock around DebuggerController stuff.

    

DEFINE_CRST_LEVEL(CrstIsJMCMethod                    )             // Debugger lock
DEFINE_CRST_LEVEL(CrstReDacl                         )             // lock to hold when redacl
DEFINE_CRST_LEVEL(CrstISymUnmanagedReader           )               // lcok you need for getting ISymUnmanagedReader

DEFINE_CRST_LEVEL(CrstThreadpoolWorker               )             // For threadpool worker
DEFINE_CRST_LEVEL(CrstThreadpoolWaitThreads          )             // For threadpool wait thread
DEFINE_CRST_LEVEL(CrstThreadpoolTimerQueue           )             // For threadpool timer queue
DEFINE_CRST_LEVEL(CrstThreadpoolEventCache           )             // For threadpool event cache
DEFINE_CRST_LEVEL(CrstAppDomainCache                 )     
DEFINE_CRST_LEVEL(CrstMethodJitLock                  )    
DEFINE_CRST_LEVEL(CrstExecuteManLock                 )    
DEFINE_CRST_LEVEL(CrstSystemDomain                   )    
DEFINE_CRST_LEVEL(CrstAppDomainHandleTable           )             // A lock to protect the large heap handle table at the app domain level
DEFINE_CRST_LEVEL(CrstGlobalStrLiteralMap            )             // A lock to protect the global string literal map.
DEFINE_CRST_LEVEL(CrstAppDomainStrLiteralMap         )             // A lock to protect the app domain specific string literal map.
DEFINE_CRST_LEVEL(CrstCompilationDomain              )    

DEFINE_CRST_LEVEL(CrstDomainLocalBlock               )    
DEFINE_CRST_LEVEL(CrstModuleFixup                    )    
 
DEFINE_CRST_LEVEL(CrstClassInit                      )             // Class initializers
DEFINE_CRST_LEVEL(CrstThreadDomainLocalStore         )             // used to update the thread's domain local store list
DEFINE_CRST_LEVEL(CrstEventStore                     )             // A lock to protect the store for events used for Object::Wait
DEFINE_CRST_LEVEL(CrstSharedAssemblyCreate           )             // Creating shared assemblies should be just below assembly loading
DEFINE_CRST_LEVEL(CrstAssemblyLoader                 )             // DO NOT place another crst at this level
DEFINE_CRST_LEVEL(CrstSharedBaseDomain               )    
DEFINE_CRST_LEVEL(CrstSystemBaseDomain               )    
DEFINE_CRST_LEVEL(CrstBaseDomain                     )    
DEFINE_CRST_LEVEL(CrstCtxVTable                      )             // increase the size of context proxy vtable
DEFINE_CRST_LEVEL(CrstClassHash                      )    
DEFINE_CRST_LEVEL(CrstClassloaderRequestQueue        )    
DEFINE_CRST_LEVEL(CrstWrapperTemplate                )             // Create a wrapper template for a class
DEFINE_CRST_LEVEL(CrstCOMWrapperCache                )     
DEFINE_CRST_LEVEL(CrstILStubGen                      )             // IL generation for IL stubs
DEFINE_CRST_LEVEL(CrstRemoting                       )             // Remoting infrastructure
DEFINE_CRST_LEVEL(CrstInterop                        )     
DEFINE_CRST_LEVEL(CrstClassFactInfoHash              )             // Class factory hash lookup
DEFINE_CRST_LEVEL(CrstStartup                        )             // Initializes and uninitializes the EE
    
DEFINE_CRST_LEVEL(CrstSynchronized                   )            // an object is Synchronized

DEFINE_CRST_LEVEL(CrstMetadataTracker                )
DEFINE_CRST_LEVEL(CrstLoaderHeap                     )
DEFINE_CRST_LEVEL(CrstPendingTypeLoadEntry           )
DEFINE_CRST_LEVEL(CrstCCompRC                        )
DEFINE_CRST_LEVEL(CrstJitPerf                        )
DEFINE_CRST_LEVEL(CrstMemoryReport                   )
DEFINE_CRST_LEVEL(CrstInstMethodHashTable            )
DEFINE_CRST_LEVEL(CrstUnresolvedClassLock            )
DEFINE_CRST_LEVEL(CrstAvailableClass                 )
DEFINE_CRST_LEVEL(CrstAvailableParamTypes            )
DEFINE_CRST_LEVEL(CrstListLock                       )
DEFINE_CRST_LEVEL(CrstTPMethodTable                  )
DEFINE_CRST_LEVEL(CrstXMLParser                      )
DEFINE_CRST_LEVEL(CrstXMLParserManaged               )

DEFINE_CRST_LEVEL(CrstLockedRangeList                )
DEFINE_CRST_LEVEL(CrstStressLog                      )
DEFINE_CRST_LEVEL(CrstInteropData                    )
DEFINE_CRST_LEVEL(CrstSystemDomainDelayedUnloadList  )
DEFINE_CRST_LEVEL(CrstWatson                         )
DEFINE_CRST_LEVEL(CrstDynamicMT                      )
DEFINE_CRST_LEVEL(CrstCodeHeapList                   )
DEFINE_CRST_LEVEL(CrstPinHandle                      )
DEFINE_CRST_LEVEL(CrstSpecialStatics                 )
DEFINE_CRST_LEVEL(CrstIJWHash                        )
DEFINE_CRST_LEVEL(CrstExternalRelocs                 )
DEFINE_CRST_LEVEL(CrstStubUnwindInfoHeapSegments     )
DEFINE_CRST_LEVEL(CrstDeadlockDetection              )
DEFINE_CRST_LEVEL(CrstPublisherCertificate           )
DEFINE_CRST_LEVEL(CrstModIntPairList                 )
DEFINE_CRST_LEVEL(CrstRVAOverrides                   )

// These should not be used in any checked in sources. They exist simply
// so that if you need to add a new crstlevel, you can use one of these temporarily
// until it's convenient for you to hit crstlevels.h and do a total source rebuild.
DEFINE_CRST_LEVEL(CrstSpare1                         )
DEFINE_CRST_LEVEL(CrstSpare2                         )
DEFINE_CRST_LEVEL(CrstSpare3                         )
DEFINE_CRST_LEVEL(CrstSpare4                         )
DEFINE_CRST_LEVEL(CrstSpare5                         )
DEFINE_CRST_LEVEL(CrstSpare6                         )
DEFINE_CRST_LEVEL(CrstSpare7                         )
DEFINE_CRST_LEVEL(CrstSpare8                         )
DEFINE_CRST_LEVEL(CrstSpare9                         )



