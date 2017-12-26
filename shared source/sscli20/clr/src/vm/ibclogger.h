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
// IBClogger.H -
//
// Infrastructure for recording touches of EE data structures
//
//

#ifndef IBCLOGGER_H
#define IBCLOGGER_H

#include <holder.h>
#include <sarray.h>
#include <crst.h>
#include <synch.h>


//
// The IBCLogger class records touches of EE data structures.  It is important to
// minimize the overhead of IBC recording on non-recording scenarios.  Our goal is
// for all public methods to be inlined, and that the cost of doing the instrumentation
// check does not exceed one comparison and one branch.
//

class MethodDesc;
class MethodTable;
class EEClass;
class TypeHandle;
struct DispatchSlot;
class Module;
class IBCLogger;

typedef void * HashDatum;

typedef Pair< Module*, mdToken > RidMapLogData;
typedef Pair< BYTE *, ULONG > BlobLogData;


class IBCLoggingDisabler
{
public:
    IBCLoggingDisabler()
    {
    }

    ~IBCLoggingDisabler()
    {
    }
};



class IBCLogger
{
    //
    // Methods for logging EE data structure accesses.  All methods should be defined
    // using the LOGACCESS macros, which creates the wrapper method that calls the
    // helper when instrumentation is enabled.  The public name of these methods should
    // be of the form Log##name##Access where name describes the type of access to be
    // logged.  The private helpers are implemented in IBClogger.cpp.
    //


#define LOGACCESS_PTR(name,type)                        \
public:                                                 \
    void Log##name##Access(type* p) { }                 \

#define LOGACCESS_VALUE(name, type)                     \
public:                                                 \
    void Log##name##Access(type p) { }                  \


    /* Log access to method desc (which adds the method desc to the required list) */
    /* Macro expands to : LogMethodDescAccess(MethodDesc *)*/
    LOGACCESS_PTR(MethodDesc, MethodDesc)

    /* Log access to method code or method header */
    /* Macro expands to : LogMethodCodeAccess(MethodDesc *)*/
    LOGACCESS_PTR(MethodCode, MethodDesc)

    /* Log access to method desc (which addes the method desc to the required list) */
    /* Macro expands to : LogMethodDescWriteAccess(MethodDesc *)*/
    LOGACCESS_PTR(MethodDescWrite,MethodDesc)

    /* Log access to method desc (which adds the method desc to the required list) */
    /* Macro expands to : LogMethodPrecodeAccess(MethodDesc *)*/
    LOGACCESS_PTR(MethodPrecode, MethodDesc)

    /* Log access to method desc (which addes the method desc to the required list) */
    /* Macro expands to : LogMethodPrecodeWriteAccess(MethodDesc *)*/
    LOGACCESS_PTR(MethodPrecodeWrite,MethodDesc)

    /* Log access to gc info */
    /* Macro expands to : LogMethodGCInfoAccess(MethodDesc *)*/
    LOGACCESS_PTR(MethodGCInfo, MethodDesc)

    /* Log access to fcall hash table */
    /* Macro expands to : LogMethodFCallAccess(MethodDesc *)*/
    LOGACCESS_PTR(MethodFCall, MethodDesc)

    /* Log access to method table */
    /* Macro expands to : LogMethodTableAccess(MethodTable *)*/
    LOGACCESS_PTR(MethodTable, MethodTable)

    /* Log access to method table */
    /* Macro expands to : LogTypeMethodTableAccess(TypeHandle const *)*/
    LOGACCESS_PTR(TypeMethodTable, TypeHandle const)

    /* Log write access to method table */
    /* Macro expands to : LogTypeMethodTableWriteableAccess(TypeHandle const *)*/
    LOGACCESS_PTR(TypeMethodTableWriteable, TypeHandle const)

    /* Log access to private (written to) method tables */
    /* Macro expands to : LogMethodTableWriteableDataAccess(MethodTable *)*/
    LOGACCESS_PTR(MethodTableWriteableData,MethodTable)

    /* Log access to private (written to) method tables */
    /* Macro expands to : LogMethodTableWriteableDataWriteAccess(MethodTable *)*/
    LOGACCESS_PTR(MethodTableWriteableDataWrite,MethodTable)

    /* Log access to EEClass */
    /* Macro expands to : LogEEClassAndMethodTableAccess(EEClass *)*/
    LOGACCESS_PTR(EEClassAndMethodTable, EEClass)

    /* Log access to EEClass COW table */
    /* Macro expands to : LogEEClassCOWTableAccess(EEClass *)*/
    LOGACCESS_PTR(EEClassCOWTable, EEClass)

    /* Log access to rid */
    /* Macro expands to : LogRidAccess(MethodDesc *)*/
    LOGACCESS_PTR(Rid, MethodDesc)

    /* Log write access to method table */
    /* Macro expands to : LogMethodTableWriteAccess(MethodTable *)*/
    LOGACCESS_PTR(MethodTableWrite, MethodTable)

    /* Log access to the FieldDescs list in the EEClass */
    /* Macro expands to : LogFieldDescsAccess(EEClass *)*/
    LOGACCESS_PTR(FieldDescs,EEClass)

    /* Log access to the extra data stored for a MethodDesc */
    /* Macro expands to : LogStoredMethodDataAccess(MethodDesc *)*/
    LOGACCESS_PTR(StoredMethodData,MethodDesc)

    /* Log access to the extra data stored for an Enum, accessed by reflection  */
    /* Macro expands to : LogStoredEnumDataAccess(EEClass *)*/
    LOGACCESS_PTR(StoredEnumData,EEClass)

    /* Log access to the MTs dispatch map */
    /* Macro expands to : LogDispatchMapAccess(MethodTable *)*/
    LOGACCESS_PTR(DispatchMap,MethodTable)

    /* Log read access to the MTs dispatch implementation table */
    /* Macro expands to : LogDispatchTableAccess(MethodTable *)*/
    LOGACCESS_PTR(DispatchTable,MethodTable)

    /* Log read access to the MTs dispatch implementation table */
    /* Macro expands to : LogDispatchTableAccess(DispatchSlot *)*/
    LOGACCESS_PTR(DispatchTableSlot,DispatchSlot)

    /* Log write access to the MTs dispatch implementation table */
    /* Macro expands to : LogDispatchTableWriteAccess(MethodTable *)*/
    LOGACCESS_PTR(DispatchTableWrite,MethodTable)

    /* Log write access to the MTs dispatch implementation table */
    /* Macro expands to : LogDispatchTableSlotWriteAccess(DispatchSlot *)*/
    LOGACCESS_PTR(DispatchTableSlotWrite,DispatchSlot)

    /* Log an update to the field marshalers */
    /* Macro expands to : LogFieldMarshalersReadAccess(EEClass *)*/
    LOGACCESS_PTR(FieldMarshalersRead,EEClass)

    /* Log a lookup in the cctor info table */
    /* Macro expands to : LogCCtorInfoReadAccess(MethodTable *)*/
    LOGACCESS_PTR(CCtorInfoRead,MethodTable)

    /* Log a lookup in the class hash table */
    /* Macro expands to : LogClassHashTableAccess(HashDatum *)*/
    LOGACCESS_PTR(ClassHashTable,HashDatum)

    /* Log a lookup of the method list for a CER */
    /* Macro expands to : LogCerMethodListReadAccess(MethodDesc *)*/
    LOGACCESS_PTR(CerMethodListRead,MethodDesc)

    /* Log a metadata access */
    /* Macro expands to : LogMetaDataAccess(void *)*/
    LOGACCESS_PTR(MetaData,void)

    /* Log metadata blob access */
    /* Macro expands to : LogMetaDataBlobAccess( Pair< BYTE *, ULONG > ) */
    LOGACCESS_VALUE( MetaDataBlob, BlobLogData )
    
    /* Log a RVS fielddesc access */
    /* Macro expands to : LogRVADataAccess(FieldDesc *)*/
    LOGACCESS_PTR(RVAData,FieldDesc)

    /* Log a lookup in the type hash table */
    /* Macro expands to : LogTypeHashTableAccess(TypeHandle const *)*/
    LOGACCESS_PTR(TypeHashTable,TypeHandle const)

    /* Log an insert in the type hash table */
    /* Macro expands to : LogTypeHashTableWriteAccess(TypeHandle const *)*/
    LOGACCESS_PTR(TypeHashTableWrite,TypeHandle const)

    /* Log a lookup in the Rid map */
    /* Macro expands to : LogTypeHashTableWriteAccess( Pair< Module*, mdToken > )*/
    LOGACCESS_VALUE( RidMap, RidMapLogData );

public:
    IBCLogger()
        : dwInstrEnabled(0)
        , m_sync(NULL)
    {
    }

public:
    // Methods for enabling/disabling instrumentation.

    void EnableAllInstr();
    void EnableFakeInstr();

    void DisableAllInstr();

    void DisableRidAccessOrderInstr();
    void DisableMethodDescAccessInstr();

    inline BOOL InstrEnabled()
    {
        return (dwInstrEnabled);
    }

    inline BOOL FakeInstrEnabled()
    {
        return (dwFakeInstrEnabled);
    }

private:
    typedef Pair< MethodDesc*, ULONG > MethodAccessData;
    void LogMethodAccessHelper(MethodDesc* pMD, ULONG flagNum);
    static void LogMethodAccessWrapper(IBCLogger* pLogger, void* pData);

    typedef Pair< TypeHandle, ULONG > TypeAccessData;
    void LogTypeAccessHelper(TypeHandle th, ULONG flagNum);
    static void LogTypeAccessWrapper(IBCLogger* pLogger, void* pData);

    Crst * GetSync();

    BOOL MethodDescAccessInstrEnabled();
    BOOL RidAccessInstrEnabled();

private:
    DWORD dwInstrEnabled;
    DWORD dwFakeInstrEnabled;
    
    Crst * volatile m_sync;
};

#endif // IBCLOGGER_H
