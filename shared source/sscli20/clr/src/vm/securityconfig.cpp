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
////////////////////////////////////////////////////////////////////////////////
//
//   File:          SecurityConfig.cpp
//
//
//   Purpose:       Native implementation for security config access and manipulation
//
//   Date created : August 30, 2000
//
////////////////////////////////////////////////////////////////////////////////

// The security config system resides outside of the rest
// of the config system since our needs are different.  The
// unmanaged portion of the security config system is only
// concerned with data file/cache file pairs, not what they
// are used for.  It performs all the duties of reading data
// from the disk, saving data back to the disk, and maintaining
// the policy and quick cache data structures.
//
// FILE FORMAT
//
// The data file is a purely opaque blob for the unmanaged
// code; however, the cache file is constructed and maintained
// completely in the unmanaged code.  It's format is as follows:
//
// CacheHeader
//  |
//  +-- dummyFileTime (FILETIME, 8 bytes) = this exists to make sure we don't read old format cache files. Must be set to {1, 0}.
//  |
//  +-- version (DWORD) = The version of this config file.
//  |
//  +-- configFileTime (FILETIME, 8 bytes) = The file time of the config file associated with this cache file.
//  |
//  +-- isSecurityOn (DWORD, 4 bytes) = This is currently not used.
//  |
//  +-- quickCache (DWORD, 4 bytes) = Used as a bitfield to maintain the information for the QuickCache.  See the QuickCache section for more details.
//  |
//  +-- registryExtensionsInfo (struct RegistryExtensionsInfo) = Indicates whether this cache file was generated in the presence of registry extensions.
//  |
//  +-- numEntries (DWORD, 4 bytes) = The number of policy cache entries in the latter portion of this cache file.
//  |
//  +-- sizeConfig (DWORD, 4 bytes) = The size of the config information stored in the latter portion of this cache file.
//
// Config Data (if any)
//     The cache file can include an entire copy of this
//     information in the adjoining config file.  This is
//     necessary since the cache often allows us to make
//     policy decisions without having parsed the data in 
//     the config file.  In order to guarantee that the config
//     data used by this process is not altered in the
//     meantime, we need to store the data in a readonly
//     location.  Due to the design of the caching system
//     the cache file is locked when it is opened and therefore
//     is the perfect place to store this information.  The
//     other alternative is to hold it in memory, but since
//     this can amount to many kilobytes of data we decided
//     on this design.
//
// List of CacheEntries
//  |
//  +-- CacheEntry
//  |    |
//  |    +-- numItemsInKey (DWORD, 4 bytes) = The number of evidence objects serialized in the key blob
//  |    |
//  |    +-- keySize (DWORD, 4 bytes) = The number of bytes in the key blob.
//  |    |
//  |    +-- dataSize (DWORD, 4 bytes) = The number of bytes in the data blob.
//  |    |     
//  |    +-- keyBlob (raw) = A raw blob representing the serialized evidence.
//  |    |
//  |    +-- dataBlob (raw) = A raw blob representing an XML serialized PolicyStatement
//  |
//  +-- ...

#include "common.h"
#include "securityconfig.h"

// Header version of the cache file.
#define CONFIG_VERSION 2
// This controls the maximum size of the cache file.
#define MAX_CACHEFILE_SIZE (1 << 20)

#define SIZE_OF_ENTRY( X )   sizeof( CacheEntryHeader ) + X->header.keySize + X->header.dataSize
#define MAX_NUM_LENGTH 16

WCHAR* SecurityConfig::wcscatDWORD( __out_ecount(cchdst) __out_z WCHAR* dst, size_t cchdst, DWORD num )
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END

    _ASSERTE( SecurityConfig::dataLock_.OwnedByCurrentThread() );

    static WCHAR buffer[MAX_NUM_LENGTH];

    buffer[MAX_NUM_LENGTH-1] = L'\0';

    size_t index = MAX_NUM_LENGTH-2;

    if (num == 0)
    {
        buffer[index--] = L'0';
    }
    else
    {
        while (num != 0)
        {
            buffer[index--] = (WCHAR)(L'0' + (num % 10));
            num = num / 10;
        }
    }

    wcscat_s( dst, cchdst, buffer + index + 1 );

    return dst;
}

inline WCHAR * Wszdup(const WCHAR * str)
{
    size_t len = wcslen(str) + 1;
    WCHAR * ret = new WCHAR[len];
    wcscpy_s(ret, len, str);
    return ret;
}

struct CacheHeader
{
    FILETIME dummyFileTime;
    DWORD version;
    FILETIME configFileTime;
    DWORD isSecurityOn, quickCache;
    SecurityConfig::RegistryExtensionsInfo registryExtensionsInfo;
    DWORD numEntries, sizeConfig;

    CacheHeader() : isSecurityOn( (DWORD) -1 ), quickCache( 0 ), numEntries( 0 ), sizeConfig( 0 )
    {
        WRAPPER_CONTRACT;
        memset( &this->configFileTime, 0, sizeof( configFileTime ) );
        dummyFileTime.dwLowDateTime = 1;
        dummyFileTime.dwHighDateTime = 0;
        version = CONFIG_VERSION;
        memset(&registryExtensionsInfo, 0, sizeof(registryExtensionsInfo));
        _ASSERTE( IsValid() && "CacheHeader constructor should make it valid" );
    };

    bool IsValid()
    {
        return dummyFileTime.dwLowDateTime == 1 &&
               dummyFileTime.dwHighDateTime == 0 &&
               version == CONFIG_VERSION;
    }
};

struct CacheEntryHeader
{
    DWORD numItemsInKey;
    DWORD keySize;
    DWORD dataSize;
};

struct CacheEntry
{
    CacheEntryHeader header;
    BYTE* key;
    BYTE* data;
    DWORD cachePosition;
    BOOL used;

    CacheEntry() : key( NULL ), data( NULL ), used( FALSE ) 
    {
        LEAF_CONTRACT;
    };

    ~CacheEntry( void )
    {
        WRAPPER_CONTRACT;
        delete [] key;
        delete [] data;
    }
};

struct Data
{
    enum State
    {
        None = 0x0,
        UsingCacheFile = 0x1,
        CopyCacheFile = 0x2,
        CacheUpdated = 0x4,
        UsingConfigFile = 0x10,
        CacheExhausted = 0x20,
        NewConfigFile = 0x40
    };

    INT32 id;
    WCHAR* configFileName;
    WCHAR* cacheFileName;
    WCHAR* cacheFileNameTemp;

    LPBYTE configData;
    DWORD  configDataSize;
    FILETIME configFileTime;
    FILETIME cacheFileTime;
    CacheHeader header;
    ArrayList* oldCacheEntries;
    ArrayList* newCacheEntries;
    State state;
    DWORD cacheCurrentPosition;
    HANDLE cache;
    PBYTE configBuffer;
    DWORD  sizeConfig;
    SecurityConfig::ConfigRetval initRetval;
    DWORD newEntriesSize;

    Data( INT32 id )
        : id( id ),
          configFileName( NULL ),
          cacheFileName( NULL ),
          configData( NULL ),
          oldCacheEntries( new ArrayList ),
          newCacheEntries( new ArrayList ),
          state( Data::None ),
          cache( INVALID_HANDLE_VALUE ),
          configBuffer( NULL ),
          newEntriesSize( 0 )
    {
        LEAF_CONTRACT;
    }

    Data( INT32 id, STRINGREF* configFile )
        : id( id ),
          cacheFileName( NULL ),
          configData( NULL ),
          oldCacheEntries( new ArrayList ),
          newCacheEntries( new ArrayList ),
          state( Data::None ),
          cache( INVALID_HANDLE_VALUE ),
          configBuffer( NULL ),
          newEntriesSize( 0 )
    {
        CONTRACTL {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(*configFile != NULL);
        } CONTRACTL_END;

        configFileName = Wszdup( (*configFile)->GetBuffer() );
        cacheFileName = NULL;
        cacheFileNameTemp = NULL;
    }

    Data( INT32 id, STRINGREF* configFile, STRINGREF* cacheFile )
        : id( id ),
          configData( NULL ),
          oldCacheEntries( new ArrayList ),
          newCacheEntries( new ArrayList ),
          state( Data::None ),
          cache( INVALID_HANDLE_VALUE ),
          configBuffer( NULL ),
          newEntriesSize( 0 )
    {
        CONTRACTL {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(*configFile != NULL);
        } CONTRACTL_END;

        configFileName = Wszdup( (*configFile)->GetBuffer() );

        if (cacheFile != NULL)
        {
            // Since temp cache files can stick around even after the process that
            // created them, we want to make sure they are fairly unique (if they
            // aren't, we'll just fail to save cache information, which is not good
            // but it won't cause anyone to crash or anything).  The unique name
            // algorithm used here is to append the process id and tick count to
            // the name of the cache file.

            cacheFileName = Wszdup( (*cacheFile)->GetBuffer() );
            size_t len = wcslen( cacheFileName ) + 1 + 2 * MAX_NUM_LENGTH;
            cacheFileNameTemp = new WCHAR[len];
            wcscpy_s( cacheFileNameTemp, len, cacheFileName );
            wcscat_s( cacheFileNameTemp, len, L"." );
            SecurityConfig::wcscatDWORD( cacheFileNameTemp, len, GetCurrentProcessId() );
            wcscat_s( cacheFileNameTemp, len, L"." );
            SecurityConfig::wcscatDWORD( cacheFileNameTemp, len, GetTickCount() );
        }
        else
        {
            cacheFileName = NULL;
            cacheFileNameTemp = NULL;
        }
    }

    Data( INT32 id, const WCHAR* configFile, const WCHAR* cacheFile )
        : id( id ),
          configData( NULL ),
          oldCacheEntries( new ArrayList ),
          newCacheEntries( new ArrayList ),
          state( Data::None ),
          cache( INVALID_HANDLE_VALUE ),
          configBuffer( NULL ),
          newEntriesSize( 0 )

    {
        CONTRACTL {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(*configFile != NULL);
        } CONTRACTL_END;

        configFileName = Wszdup( configFile );

        if (cacheFile != NULL)
        {
            cacheFileName = Wszdup( cacheFile );
            size_t len = wcslen( cacheFileName ) + 1 + 2 * MAX_NUM_LENGTH;
            cacheFileNameTemp = new WCHAR[len];
            wcscpy_s( cacheFileNameTemp, len, cacheFileName );
            wcscat_s( cacheFileNameTemp, len, L"." );
            SecurityConfig::wcscatDWORD( cacheFileNameTemp, len, GetCurrentProcessId() );
            wcscat_s( cacheFileNameTemp, len, L"." );
            SecurityConfig::wcscatDWORD( cacheFileNameTemp, len, GetTickCount() );
        }
        else
        {
            cacheFileName = NULL;
            cacheFileNameTemp = NULL;
        }
    }

    void Reset( void )
    {
        CONTRACTL {
            THROWS;
            GC_NOTRIGGER;
            MODE_ANY;
        } CONTRACTL_END;

        delete [] configBuffer;
        configBuffer = NULL;

        if (cache != INVALID_HANDLE_VALUE)
        {
            CloseHandle( cache );
            cache = INVALID_HANDLE_VALUE;
        }

        if (cacheFileNameTemp != NULL)
        {
            // Note: we don't check a return value here as the worst thing that
            // happens is we leave a spurious cache file.

            WszDeleteFile( cacheFileNameTemp );
        }

        if (configData != NULL)
            delete [] configData;
        configData = NULL;

        DeleteAllEntries();
        header = CacheHeader();

        oldCacheEntries = new ArrayList();
        newCacheEntries = new ArrayList();

    }

    void Cleanup( void )
    {
        CONTRACTL {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
        } CONTRACTL_END;

        if (cache != INVALID_HANDLE_VALUE)
        {
            CloseHandle( cache );
            cache = INVALID_HANDLE_VALUE;
        }

        if (cacheFileNameTemp != NULL)
        {
            // Note: we don't check a return value here as the worst thing that
            // happens is we leave a spurious cache file.

            WszDeleteFile( cacheFileNameTemp );
        }
    }

    ~Data( void )
    {
        CONTRACTL {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
        } CONTRACTL_END;

        Cleanup();
        delete [] configBuffer;

        delete [] configFileName;
        delete [] cacheFileName;
        delete [] cacheFileNameTemp;

        if (configData != NULL)
            delete [] configData;
        DeleteAllEntries();
    }

    void DeleteAllEntries( void );
};

void Data::DeleteAllEntries( void )
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    } CONTRACTL_END;

    ArrayList::Iterator iter;

    if (oldCacheEntries != NULL)
    {
        iter = oldCacheEntries->Iterate();

        while (iter.Next())
        {
            delete (CacheEntry*) iter.GetElement();
        }

        delete oldCacheEntries;
        oldCacheEntries = NULL;
    }

    if (newCacheEntries != NULL)
    {
        iter = newCacheEntries->Iterate();

        while (iter.Next())
        {
            delete (CacheEntry*) iter.GetElement();
        }

        delete newCacheEntries;
        newCacheEntries = NULL;
    }
}

void* SecurityConfig::GetData( INT32 id )
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END

    ArrayList::Iterator iter = entries_.Iterate();

    while (iter.Next())
    {
        Data* data = (Data*)iter.GetElement();

        if (data->id == id)
        {
            return data;
        }
    }

    return NULL;
}

SecurityConfig::RegistryExtensionsInfo SecurityConfig::s_registryExtensionsInfo;

#define POLICY_EXTENSIONS_CONFIG_KEY_W L"Software\\Microsoft\\.NETFramework\\Security\\Policy\\Extensions\\NamedPermissionSets"
void SecurityConfig::ReadRegistryExtensionsInfo () {
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;

    GCX_PREEMP();

    memset(&s_registryExtensionsInfo, 0, sizeof(s_registryExtensionsInfo));

    // No support for sandbox extensions on Rotor.
    s_registryExtensionsInfo.eStatus = NoExtensions;
}

static BOOL CacheOutOfDate( FILETIME* configFileTime, __in_z WCHAR* configFileName, __in_opt __in_z WCHAR* cacheFileName )
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_PREEMPTIVE;
    }
    CONTRACTL_END

    BOOL retval = TRUE;
    BOOL deleteFile = FALSE;

    HandleHolder config(WszCreateFile( configFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL ));

    if (config.GetValue() == INVALID_HANDLE_VALUE)
    {
        goto CLEANUP;
    }

    // Get the last write time for both files.

    FILETIME newConfigTime;

    if (!GetFileTime( config.GetValue(), NULL, NULL, &newConfigTime ))
    {
        goto CLEANUP;
    }

    if (CompareFileTime( configFileTime, &newConfigTime ) != 0)
    {
        // Cache is dated.  Delete the cache.
        deleteFile = TRUE;
        goto CLEANUP;
    }

    retval = FALSE;

CLEANUP:
    // Note: deleting this file is a perf optimization so that
    // we don't have to do this file time comparison next time.
    // Therefore, if it fails for some reason we just loss a
    // little perf.

    if (deleteFile && cacheFileName != NULL)
        WszDeleteFile( cacheFileName );

    return retval;
}

static BOOL CacheOutOfDate( FILETIME* cacheFileTime, HANDLE cache, __in_opt __in_z WCHAR* cacheFileName )
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_PREEMPTIVE;
    }
    CONTRACTL_END

    BOOL retval = TRUE;

    // Get the last write time for both files.

    FILETIME newCacheTime;

    if (!GetFileTime( cache, NULL, NULL, &newCacheTime ))
    {
        goto CLEANUP;
    }

    if (CompareFileTime( cacheFileTime, &newCacheTime ) != 0)
    {
        // Cache is dated.  Delete the cache.
        // Note: deleting this file is a perf optimization so that
        // we don't have to do this file time comparison next time.
        // Therefore, if it fails for some reason we just loss a
        // little perf.

        if (cacheFileName != NULL)
        {
            CloseHandle( cache );
            WszDeleteFile( cacheFileName );
        }
        goto CLEANUP;
    }

    retval = FALSE;

CLEANUP:
    return retval;
}

static BOOL CacheOutOfDate( FILETIME* configTime, FILETIME* cachedConfigTime )
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_PREEMPTIVE;
    }
    CONTRACTL_END

    DWORD result = CompareFileTime( configTime, cachedConfigTime );

    return result != 0;
}

static DWORD GetShareFlags()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END

    DWORD shareFlags;

    shareFlags = FILE_SHARE_READ | FILE_SHARE_DELETE;

    return shareFlags;
}

static DWORD WriteFileData( HANDLE file, PBYTE data, DWORD size )
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END

    DWORD totalBytesWritten = 0;
    DWORD bytesWritten;

    do
    {
        if (WriteFile( file, data, size - totalBytesWritten, &bytesWritten, NULL ) == 0)
        {
            return E_FAIL;
        }
        if (bytesWritten == 0)
        {
            return E_FAIL;
        }
        totalBytesWritten += bytesWritten;
    } while (totalBytesWritten < size);

    return S_OK;
}

// the data argument to this function can be a pointer to GC heap. 
// We do ensure cooperative mode before we call this function using a pointer to GC heap,
// so we can't change GC mode inside this function. 
// Greg will look into the ways to pin the object.

static DWORD ReadFileData( HANDLE file, PBYTE data, DWORD size )
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END

    DWORD totalBytesRead = 0;
    DWORD bytesRead;
    do
    {
        if (ReadFile( file, data, size - totalBytesRead, &bytesRead, NULL ) == 0)
        {
            return E_FAIL;
        }

        if (bytesRead == 0)
        {
            return E_FAIL;
        }
        
        totalBytesRead += bytesRead;
        
    } while (totalBytesRead < size);
    
    return S_OK;
}

SecurityConfig::ConfigRetval SecurityConfig::InitData( INT32 id, const WCHAR* configFileName, const WCHAR* cacheFileName )
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END

    Data* data = (Data*)GetData( id );
    if (data != NULL)
    {
        return data->initRetval;
    }

    if (configFileName == NULL || wcslen( configFileName ) == 0)
    {
        return NoFile;
    }

    {
        CrstHolder ch( &dataLock_ );
        data = new (nothrow) Data( id, configFileName, cacheFileName );
    }

    if (data == NULL)
    {
         return NoFile;
    }

    return InitData( data, TRUE );
}


SecurityConfig::ConfigRetval SecurityConfig::InitData( void* configDataParam, BOOL addToList )
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END

    GCX_PREEMP();

    _ASSERTE( configDataParam != NULL );

    Data* data = (Data*) configDataParam;
    DWORD cacheSize;
    DWORD configSize;
    ConfigRetval retval = NoFile;
    DWORD shareFlags;

    shareFlags = GetShareFlags();

    // Crack open the config file.

    HandleHolder config(WszCreateFile( data->configFileName, GENERIC_READ, shareFlags, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL ));
    if (config == INVALID_HANDLE_VALUE || !GetFileTime( config, NULL, NULL, &data->configFileTime ))
    {
        memset( &data->configFileTime, 0, sizeof( data->configFileTime ) );
    }
    else
    {
        data->state = (Data::State)(Data::UsingConfigFile | data->state);
    }

    // If we want a cache file, try to open that up.
    // Note: we do not use a holder for data->cache because the new holder for data will
    // delete the entire data structure which includes closing this handle as necessary.

    if (data->cacheFileName != NULL)
        data->cache = WszCreateFile( data->cacheFileName, GENERIC_READ, shareFlags, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    
    if (data->cache == INVALID_HANDLE_VALUE)
    {
        goto READ_DATA;
    }

    // Validate that the cache file is in a good form by checking
    // that it is at least big enough to contain a header.

    cacheSize = SafeGetFileSize( data->cache, NULL );
    
    if (cacheSize == 0xFFFFFFFF)
    {
        goto READ_DATA;
    }
    
    if (cacheSize < sizeof( CacheHeader ))
    {
        goto READ_DATA;
    }

    // Finally read the data from the file into the buffer.
    
    if (ReadFileData( data->cache, (BYTE*)&data->header, sizeof( CacheHeader ) ) != S_OK)
    {
        goto READ_DATA;
    }

    if (!data->header.IsValid())
    {
        goto READ_DATA;
    }

    // Check to make sure the cache file and the config file
    // match up by comparing the actual file time of the config
    // file and the config file time stored in the cache file.

    if (CacheOutOfDate( &data->configFileTime, &data->header.configFileTime ))
    {
        goto READ_DATA;
    }

    if (!GetFileTime( data->cache, NULL, NULL, &data->cacheFileTime ))
    {
        goto READ_DATA;
    }

    // Set the file pointer to after both the header and config data (if any) so
    // that we are ready to read cache entries.

    if (SetFilePointer( data->cache, sizeof( CacheHeader ) + data->header.sizeConfig, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER)
    {
        goto READ_DATA;
    }

    data->cacheCurrentPosition = sizeof( CacheHeader ) + data->header.sizeConfig;
    data->state = (Data::State)(Data::UsingCacheFile | Data::CopyCacheFile | data->state);

    retval = (ConfigRetval)(retval | CacheFile);

READ_DATA:
    // If we are not using the cache file but we successfully opened it, we need
    // to close it now.  In addition, we need to reset the cache information
    // stored in the Data object to make sure there is no spill over.

    if (data->cache != INVALID_HANDLE_VALUE && (data->state & Data::UsingCacheFile) == 0)
    {
        CloseHandle( data->cache );
        data->header = CacheHeader();
        data->cache = INVALID_HANDLE_VALUE;
    }

    if (config != INVALID_HANDLE_VALUE)
    {
        configSize = SafeGetFileSize( config, NULL );
    
        if (configSize == 0xFFFFFFFF)
        {
            goto ADD_DATA;
        }

        // Be paranoid and only use the cache file version if we find that it has the correct sized
        // blob in it.

        if ((data->state & Data::UsingCacheFile) != 0 && configSize == data->header.sizeConfig)
        {
            goto ADD_DATA;
        }
        else
        {
            GCX_COOP();

            if (data->cache != INVALID_HANDLE_VALUE)
            {
                CloseHandle( data->cache );
                data->header = CacheHeader();
                data->cache = INVALID_HANDLE_VALUE;
                data->state = (Data::State)(data->state & ~(Data::UsingCacheFile));
            }

            data->configData = new BYTE[configSize];
            if (ReadFileData( config, data->configData, configSize ) != S_OK)
            {
                goto ADD_DATA;
            }
            data->configDataSize = configSize;
        }
        retval = (ConfigRetval)(retval | ConfigFile);
    }

ADD_DATA:
    {
        GCX_PREEMP();
        CrstHolder ch(&dataLock_);

        if (addToList)
        {
            IfFailThrow(entries_.Append(data));
        }
    }

    _ASSERTE( data );
    data->initRetval = retval;

    return retval;

};

static CacheEntry* LoadNextEntry( HANDLE cache, Data* data )
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_PREEMPTIVE;
    }
    CONTRACTL_END

    if ((data->state & Data::CacheExhausted) != 0)
        return NULL;

    NewHolder<CacheEntry> entry(new CacheEntry());

    if (SetFilePointer( cache, data->cacheCurrentPosition, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER)
    {
        return NULL;
    }

        if (ReadFileData( cache, (BYTE*)&entry.GetValue()->header, sizeof( CacheEntryHeader ) ) != S_OK)
    {
        return NULL;
    }

    entry.GetValue()->cachePosition = data->cacheCurrentPosition + sizeof( entry.GetValue()->header );

    data->cacheCurrentPosition += sizeof( entry.GetValue()->header ) + entry.GetValue()->header.keySize + entry.GetValue()->header.dataSize;

    if (SetFilePointer( cache, entry.GetValue()->header.keySize + entry->header.dataSize, NULL, FILE_CURRENT ) == INVALID_SET_FILE_POINTER)
    {
        return NULL;
    }

    // We append a partially populated entry. CompareEntry is robust enough to handle this.
    IfFailThrow(data->oldCacheEntries->Append( entry ));

    return entry.Extract();
}

static BOOL WriteEntry( HANDLE cache, CacheEntry* entry, HANDLE oldCache = NULL )
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_PREEMPTIVE;
    }
    CONTRACTL_END

    if (WriteFileData( cache, (BYTE*)&entry->header, sizeof( CacheEntryHeader ) ) != S_OK)
    {
        return FALSE;
    }

    if (entry->key == NULL)
    {
        _ASSERTE (oldCache != NULL);

        // We were lazy in reading the entry. Read the key now.
        entry->key = new BYTE[entry->header.keySize];

        _ASSERTE (cache != INVALID_HANDLE_VALUE);

        if (SetFilePointer( oldCache, entry->cachePosition, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER)
            return NULL;

        if (ReadFileData( oldCache, entry->key, entry->header.keySize ) != S_OK)
        {
            return NULL;
        }

        entry->cachePosition += entry->header.keySize;
    }

    _ASSERTE( entry->key != NULL );

    if (entry->data == NULL)
    {
        _ASSERTE (oldCache != NULL);

        // We were lazy in reading the entry. Read the data also.
        entry->data = new BYTE[entry->header.dataSize];

        if (SetFilePointer( oldCache, entry->cachePosition, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER)
            return NULL;

        if (ReadFileData( oldCache, entry->data, entry->header.dataSize ) != S_OK)
            return NULL;

        entry->cachePosition += entry->header.dataSize;
    }

    _ASSERT( entry->data != NULL );

    if (WriteFileData( cache, entry->key, entry->header.keySize ) != S_OK)
    {
        return FALSE;
    }

    if (WriteFileData( cache, entry->data, entry->header.dataSize ) != S_OK)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL SecurityConfig::SaveCacheData( INT32 id )
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END

    GCX_PREEMP();

    // Note: this function should only be called at EEShutdown time.
    // This is because we need to close the current cache file in
    // order to delete it.  If it ever became necessary to do
    // cache saves while a process we still executing managed code
    // it should be possible to create a locking scheme for usage
    // of the cache handle with very little reordering of the below
    // (as it should always be possible for us to have a live copy of
    // the file and yet still be making the swap).

    HandleHolder cache;
    HandleHolder config;
    CacheHeader header;
    BOOL retval = FALSE;
    BOOL fWriteSucceeded = FALSE;
    DWORD numEntriesWritten = 0;
    DWORD amountWritten = 0;
    DWORD sizeConfig = 0;
    NewHolder<BYTE> configBuffer;
    BOOL useConfigData = FALSE;

    Data* data = (Data*)GetData( id );

    // If there is not data by the id or there is no
    // cache file name associated with the data, then fail.

    if (data == NULL || data->cacheFileName == NULL)
        return FALSE;

    // If we haven't added anything new to the cache
    // then just return success.

    if ((data->state & Data::CacheUpdated) == 0)
        return TRUE;

    // If the config file has changed since the process started
    // then our cache data is no longer valid.  We'll just
    // return success in this case.

    if ((data->state & Data::UsingConfigFile) != 0 && CacheOutOfDate( &data->configFileTime, data->configFileName, NULL ))
        return TRUE;

    DWORD fileNameLength = (DWORD)wcslen( data->cacheFileName );

    NewHolder<WCHAR> newFileName(new WCHAR[fileNameLength + 5]);

    swprintf_s( newFileName.GetValue(), fileNameLength + 5, L"%s%s", data->cacheFileName, L".new" );

    cache.Assign( WszCreateFile( newFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ) ); 

    for (DWORD RetryCount = 0; RetryCount < 5; RetryCount++)
    {
        if (cache != INVALID_HANDLE_VALUE)
        {
            break;
        }
        else
        {
            DWORD error = GetLastError();

            if (error == ERROR_PATH_NOT_FOUND)
            {
                // The directory does not exist, iterate through and try to create it.

                WCHAR* currentChar = newFileName;

                // Skip the first backslash

                while (*currentChar != L'\0')
                {
                    if (*currentChar == L'\\' || *currentChar == L'/')
                    {
                        currentChar++;
                        break;
                    }
                    currentChar++;
                }

                // Iterate through trying to create each subdirectory.

                while (*currentChar != L'\0')
                {
                    if (*currentChar == L'\\' || *currentChar == L'/')
                    {
                        *currentChar = L'\0';

                        if (!WszCreateDirectory( newFileName, NULL ))
                        {
                            error = GetLastError();

                            if (error != ERROR_ACCESS_DENIED && error != ERROR_INVALID_NAME && error != ERROR_ALREADY_EXISTS)
                            {
                                goto CLEANUP;
                            }
                        }

                        *currentChar = L'\\';
                    }
                    currentChar++;
                }

                // Try the file creation again
                continue;
            }
        }

        // CreateFile failed.  Sleep a little and retry, in case a
        // virus scanner caused the creation to fail.
        ClrSleepEx(10, FALSE);
    }

    if (cache.GetValue() == INVALID_HANDLE_VALUE)
        goto CLEANUP;

    // This code seems complicated only because of the
    // number of cases that we are trying to handle.  All we
    // are trying to do is determine the amount of space to
    // leave for the config information.

    // If we saved out a new config file during this run, use
    // the config size stored in the Data object itself.

    {
        GCX_COOP();

        if (data->configData != NULL)
        {
            useConfigData = TRUE;
        }
    }

    if ((data->state & Data::NewConfigFile) != 0)
    {
        sizeConfig = data->sizeConfig;
    }

    // If we have a cache file, then use the size stored in the
    // cache header.

    else if ((data->state & Data::UsingCacheFile) != 0)
    {
        sizeConfig = data->header.sizeConfig;
    }

    // If we read in the config data, use the size of the
    // managed byte array that it is stored in.

    else if (useConfigData)
    {
        sizeConfig = data->configDataSize;
    }

    // Otherwise, check the config file itself to get the size.

    else
    {
        DWORD shareFlags;

        shareFlags = GetShareFlags();

        config.Assign( WszCreateFile( data->configFileName, GENERIC_READ, shareFlags, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL ) );

        if (config == INVALID_HANDLE_VALUE)
        {
            sizeConfig = 0;
        }
        else
        {
            sizeConfig = SafeGetFileSize( config, NULL );

            if (sizeConfig == 0xFFFFFFFF)
            {
                sizeConfig = 0;
            }
        }
    }

    // First write the entries.

    if (SetFilePointer( cache, sizeof( CacheHeader ) + sizeConfig, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER)
    {
        goto CLEANUP;
    }

    // We're going to write out the cache entries in a modified
    // least recently used order, throwing out any that end up
    // taking us past our hardcoded max file size.

    {
        // First, write the entries from the cache file that were used.
        // We do this because presumably these are system assemblies
        // and other assemblies used by a number of applications.

        ArrayList::Iterator iter;

        if ((data->state & Data::UsingCacheFile) != 0)
        {
            iter = data->oldCacheEntries->Iterate();

            while (iter.Next() && amountWritten < MAX_CACHEFILE_SIZE)
            {
                CacheEntry* currentEntry = (CacheEntry*)iter.GetElement();

                if (currentEntry->used)
                {
                    if(!WriteEntry( cache, currentEntry, data->cache ))
                    {
                        goto CLEANUP;
                    }

                    amountWritten += SIZE_OF_ENTRY( currentEntry );
                    numEntriesWritten++;
                }
            }
        }

        // Second, write any new cache entries to the file.  These are
        // more likely to be assemblies specific to this app.

        iter = data->newCacheEntries->Iterate();

        while (iter.Next() && amountWritten < MAX_CACHEFILE_SIZE)
        {
            CacheEntry* currentEntry = (CacheEntry*)iter.GetElement();

            if (!WriteEntry( cache, currentEntry ))
            {
                goto CLEANUP;
            }

            amountWritten += SIZE_OF_ENTRY( currentEntry );
            numEntriesWritten++;
        }

        // Third, if we are using the cache file, write the old entries
        // that were not used this time around.

        if ((data->state & Data::UsingCacheFile) != 0)
        {
            // First, write the ones that we already have partially loaded

            iter = data->oldCacheEntries->Iterate();

            while (iter.Next() && amountWritten < MAX_CACHEFILE_SIZE)
            {
                CacheEntry* currentEntry = (CacheEntry*)iter.GetElement();

                if (!currentEntry->used)
                {
                    if(!WriteEntry( cache, currentEntry, data->cache ))
                    {
                        goto CLEANUP;
                    }

                    amountWritten += SIZE_OF_ENTRY( currentEntry );
                    numEntriesWritten++;
                }
            }

            while (amountWritten < MAX_CACHEFILE_SIZE)
            {
                CacheEntry* entry = LoadNextEntry( data->cache, data );

                if (entry == NULL)
                    break;

                if (!WriteEntry( cache, entry, data->cache ))
                {
                    goto CLEANUP;
                }

                amountWritten += SIZE_OF_ENTRY( entry );
                numEntriesWritten++;
            }
        }

        fWriteSucceeded = TRUE;
    }


    if (!fWriteSucceeded)
    {
        CloseHandle( cache.GetValue() );
        cache.SuppressRelease();
        WszDeleteFile( newFileName );
        goto CLEANUP;
    }

    // End with writing the header.

    header.configFileTime = data->configFileTime;
    header.isSecurityOn = 1;
    header.numEntries = numEntriesWritten;
    header.quickCache = data->header.quickCache;
    header.registryExtensionsInfo = SecurityConfig::s_registryExtensionsInfo;
    header.sizeConfig = sizeConfig;

    if (SetFilePointer( cache, 0, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER)
    {
        // Couldn't move to the beginning of the file
        goto CLEANUP;
    }

    if (WriteFileData( cache, (PBYTE)&header, sizeof( header ) ) != S_OK)
    {
        // Couldn't write header info.
        goto CLEANUP;
    }

    if (sizeConfig != 0)
    {
        if ((data->state & Data::NewConfigFile) != 0)
        {
            if (WriteFileData( cache, data->configBuffer, sizeConfig ) != S_OK)
            {
                goto CLEANUP;
            }
        }
        else
        {
            if (data->configData != NULL)
            {
                if (WriteFileData( cache, data->configData, sizeConfig ) != S_OK)
                {
                    goto CLEANUP;
                }
            }
            else if ((data->state & Data::UsingCacheFile) != 0)
            {
                GCX_PREEMP();

                configBuffer.Assign( new BYTE[sizeConfig] );

                if (SetFilePointer( data->cache, sizeof( CacheHeader ), NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER)
                {
                    goto CLEANUP;
                }

                if (ReadFileData( data->cache, configBuffer.GetValue(), sizeConfig ) != S_OK)
                {
                    goto CLEANUP;
                }

                if (WriteFileData( cache, configBuffer.GetValue(), sizeConfig ) != S_OK)
                {
                    goto CLEANUP;
                }
            }
            else
            {
                GCX_PREEMP();

                configBuffer.Assign( new BYTE[sizeConfig] );

                if (SetFilePointer( config, 0, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER)
                {
                    goto CLEANUP;
                }

                if (ReadFileData( config, configBuffer.GetValue(), sizeConfig ) != S_OK)
                {
                    goto CLEANUP;
                }

                if (WriteFileData( cache, configBuffer.GetValue(), sizeConfig ) != S_OK)
                {
                    goto CLEANUP;
                }
            }
        }
    }

    // Flush the file buffers to make sure
    // we get full write through.

    FlushFileBuffers( cache.GetValue() );

    CloseHandle( cache );
    cache.SuppressRelease();
    CloseHandle( data->cache );
    data->cache = INVALID_HANDLE_VALUE;

    // Move the existing file out of the way
    // Note: use MoveFile because we know it will never cross
    // device boundaries.

    // Note: the delete file can fail, but we can't really do anything
    // if it does so just ignore any failures.
    WszDeleteFile( data->cacheFileNameTemp );

    // Try to move the existing cache file out of the way.  However, if we can't
    // then try to delete it.  If it can't be deleted then just bail out.
    if (!WszMoveFile( data->cacheFileName, data->cacheFileNameTemp ) &&
        (!Assembly::FileNotFound(HRESULT_FROM_WIN32(GetLastError()))) &&
        !WszDeleteFile( data->cacheFileName ))
    {
        if (!Assembly::FileNotFound(HRESULT_FROM_WIN32(GetLastError())))
            goto CLEANUP;
    }

    // Move the new file into position

    if (!WszMoveFile( newFileName, data->cacheFileName ))
    {
        goto CLEANUP;
    }

    retval = TRUE;

CLEANUP:
    if (retval)
        cache.SuppressRelease();

    return retval;

}

FCIMPL1(void, SecurityConfig::EcallResetCacheData, INT32 id)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;        
    } CONTRACTL_END;

    HELPER_METHOD_FRAME_BEGIN_0();
    //-[autocvtpro]-------------------------------------------------------

    ResetCacheData( id );

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

void SecurityConfig::ResetCacheData( INT32 id )
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END

    Data* data = (Data*)GetData( id );

    if (data == NULL)
        return;

    {
        GCX_PREEMP();
        CrstHolder ch(&dataLock_);

        data->DeleteAllEntries();

        data->oldCacheEntries = new ArrayList;
        data->newCacheEntries = new ArrayList;

        data->header = CacheHeader();
        data->state = (Data::State)(~(Data::CopyCacheFile | Data::UsingCacheFile) & data->state);

        DWORD shareFlags;

        shareFlags = GetShareFlags();

        HandleHolder config(WszCreateFile( data->configFileName, GENERIC_READ, shareFlags, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL ));

        if (config.GetValue() == INVALID_HANDLE_VALUE)
            return;

        VERIFY(GetFileTime( config, NULL, NULL, &data->configFileTime ));
        VERIFY(GetFileTime( config, NULL, NULL, &data->header.configFileTime ));
    }
}


FCIMPL4(FC_BOOL_RET, SecurityConfig::EcallSaveDataByte, StringObject* pathUNSAFE, U1Array* dataUNSAFE, DWORD dwOffset, DWORD dwLength)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;

    BOOL retVal = FALSE;
    U1ARRAYREF dataSAFE = (U1ARRAYREF) dataUNSAFE;
    STRINGREF pathSAFE = (STRINGREF) pathUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_2(dataSAFE, pathSAFE);

    NewArrayHolder<BYTE> pb(new BYTE[dwLength]);
    memcpyNoGCRefs(pb, dataSAFE->GetDirectPointerToNonObjectElements() + dwOffset, dwLength);

    int cchPathChar = pathSAFE->GetStringLength();
    WCHAR* pwszPath = new WCHAR[cchPathChar + 1];
    memcpy (pwszPath, pathSAFE->GetBuffer(), cchPathChar * sizeof(WCHAR));
    pwszPath[cchPathChar] = L'\0';

    retVal = SaveData(pwszPath, pb, dwLength);

    HELPER_METHOD_FRAME_END();
    FC_RETURN_BOOL(retVal);
}
FCIMPLEND

BOOL SecurityConfig::SaveData(__in_z WCHAR* wszConfigPath, LPBYTE pbData, DWORD cbData)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END

    GCX_PREEMP();

    HandleHolder newFile(INVALID_HANDLE_VALUE);

    int RetryCount;
    DWORD error = 0;
    BOOL retval = FALSE;
    DWORD fileNameLength = (DWORD) wcslen(wszConfigPath);

    NewHolder<WCHAR> newFileName(new WCHAR[fileNameLength + 5]);
    NewHolder<WCHAR> oldFileName(new WCHAR[fileNameLength + 5]);

    swprintf_s( newFileName.GetValue(), fileNameLength + 5, L"%s%s", wszConfigPath, L".new" );
    swprintf_s( oldFileName.GetValue(), fileNameLength + 5, L"%s%s", wszConfigPath, L".old" );

    // Create the new file.
    for (RetryCount = 0; RetryCount < 5; RetryCount++) {
        newFile.Assign( WszCreateFile( newFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ) );
    
        if (newFile != INVALID_HANDLE_VALUE)
            break;
        else
        {
            error = GetLastError();

            if (error == ERROR_PATH_NOT_FOUND)
            {
                // The directory does not exist, iterate through and try to create it.

                WCHAR* currentChar = newFileName;

                // Skip the first backslash

                while (*currentChar != L'\0')
                {
                    if (*currentChar == L'\\' || *currentChar == L'/')
                    {
                        currentChar++;
                        break;
                    }
                    currentChar++;
                }

                // Iterate through trying to create each subdirectory.

                while (*currentChar != L'\0')
                {
                    if (*currentChar == L'\\' || *currentChar == L'/')
                    {
                        *currentChar = L'\0';

                        if (!WszCreateDirectory( newFileName, NULL ))
                        {
                            error = GetLastError();

                            if (error != ERROR_ACCESS_DENIED && error != ERROR_ALREADY_EXISTS)
                            {
                                goto CLEANUP;
                            }
                        }

                        *currentChar = L'\\';
                    }
                    currentChar++;
                }

                // Try the file creation again
                continue;
            }
        }

        // CreateFile failed.  Sleep a little and retry, in case a
        // virus scanner caused the creation to fail.
        ClrSleepEx(10, FALSE);
    }

    if (newFile == INVALID_HANDLE_VALUE) {
        goto CLEANUP;
    }

    // Write the data into it.
    if (WriteFileData(newFile.GetValue(), pbData, cbData) != S_OK)
    {
        // Write failed, destroy the file and bail.
        // Note: if the delete fails, we always do a CREATE_NEW
        // for this file so that should take care of it.  If not
        // we'll fail to write out future cache files.
        CloseHandle( newFile.GetValue() );
        newFile.SuppressRelease();
        WszDeleteFile( newFileName );
        goto CLEANUP;
    }

    FlushFileBuffers( newFile.GetValue() );
    CloseHandle( newFile.GetValue() );
    newFile.SuppressRelease();

    // Move the existing file out of the way
    if (!WszMoveFileEx( wszConfigPath, oldFileName, MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED ))
    {
        // If move fails for a reason other than not being able to find the file, bail out.
        // Also, if the old file didn't exist, we have no need to delete it.

        if (!Assembly::FileNotFound(HRESULT_FROM_WIN32(GetLastError())))
        {
            if (!WszDeleteFile(wszConfigPath))
                goto CLEANUP;
        }
    }

    // Move the new file into position

    if (!WszMoveFileEx( newFileName, wszConfigPath, MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED ))
        goto CLEANUP;

    retval = TRUE;

CLEANUP:
    return retval;
}

FCIMPL1(FC_BOOL_RET, SecurityConfig::EcallRecoverData, INT32 id)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;

    BOOL retVal = 0;
    HELPER_METHOD_FRAME_BEGIN_RET_0();
    //-[autocvtpro]-------------------------------------------------------

    retVal = RecoverData( id );

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
    FC_RETURN_BOOL(retVal);
}
FCIMPLEND

BOOL SecurityConfig::RecoverData( INT32 id )
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END

    Data* data = (Data*)GetData( id );

    BOOL retval = FALSE;

    if (data == NULL)
        return retval;

    {
        GCX_PREEMP();
        DWORD fileNameLength = (DWORD)wcslen( data->configFileName );

        NewHolder<WCHAR> tempFileName(new WCHAR[fileNameLength + 10]);
        NewHolder<WCHAR> oldFileName(new WCHAR[fileNameLength + 5]);

        swprintf_s( tempFileName.GetValue(), fileNameLength + 10, L"%s%s", data->configFileName, L".old.temp" );
        swprintf_s( oldFileName.GetValue(), fileNameLength + 5, L"%s%s", data->configFileName, L".old" );

        HandleHolder oldFile(WszCreateFile( oldFileName, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL ));

        if (oldFile.GetValue() == INVALID_HANDLE_VALUE)
        {
            goto CLEANUP;
        }

        CloseHandle( oldFile );
        oldFile.SuppressRelease();

        if (!WszMoveFile( data->configFileName, tempFileName ))
        {
            goto CLEANUP;
        }

        if (!WszMoveFile( oldFileName, data->configFileName ))
        {
            goto CLEANUP;
        }

        if (!WszMoveFile( tempFileName, oldFileName ))
        {
            goto CLEANUP;
        }
    }

    // We need to do some work to reset the unmanaged data object
    // so that the managed side of things behaves like you'd expect.
    // This basically means cleaning up the open resources and
    // doing the work to init on a different set of files.

    data->Reset();
    InitData( data, FALSE );

    retval = TRUE;

CLEANUP:
    return retval;
}

BOOL SecurityConfig::GetQuickCacheEntry( INT32 id, QuickCacheEntryType type )
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END

    //
    // If there is no config file for this level, then we'll assume the default
    // security policy is in effect. This could happen for example if there is
    // user profile loaded or if the config file is not present.
    //

    Data* data = (Data*)GetData( id );
    if (data == NULL || ((data->state & Data::UsingConfigFile) == 0))
        return (type == FullTrustZoneMyComputer); // MyComputer gets FT by default.

    if ((data->state & Data::UsingCacheFile) == 0)
        return FALSE;

    return (data->header.quickCache & type);
}

FCIMPL2(void, SecurityConfig::EcallSetQuickCache, INT32 id, QuickCacheEntryType type)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;

    HELPER_METHOD_FRAME_BEGIN_0();

    SetQuickCache(id, type);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

void SecurityConfig::SetQuickCache( INT32 id, QuickCacheEntryType type )
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END

    Data* data = (Data*)GetData( id );

    if (data == NULL)
        return;

    if ((DWORD) type != data->header.quickCache)
    {
        GCX_PREEMP();
        CrstHolder ch(&dataLock_);

        data->state = (Data::State)(Data::CacheUpdated | data->state);
        data->header.quickCache = type;
    }
}

static HANDLE OpenCacheFile( Data* data )
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_PREEMPTIVE;
    }
    CONTRACTL_END

    CrstHolder ch(&SecurityConfig::dataLock_);

    if (data->cache != INVALID_HANDLE_VALUE)
        return data->cache;

    _ASSERTE( FALSE && "This case should never happen" );

    data->cache = WszCreateFile( data->cacheFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    if (data->cache == INVALID_HANDLE_VALUE)
        return NULL;

    // Check whether the cache has changed since we first looked at it.
    // If it has but the config file hasn't, then we need to start fresh.
    // However, if the config file has changed then we have to ignore it.

    if (CacheOutOfDate( &data->cacheFileTime, data->cache, NULL ))
    {
        if (CacheOutOfDate( &data->configFileTime, data->configFileName, NULL ))
            return NULL;

        if (ReadFileData( data->cache, (BYTE*)&data->header, sizeof( CacheHeader ) ) != S_OK)
            return NULL;

        data->cacheCurrentPosition = sizeof( CacheHeader );

        if (data->oldCacheEntries != NULL)
        {
            ArrayList::Iterator iter = data->oldCacheEntries->Iterate();
            while (iter.Next())
            {
                delete (CacheEntry*)iter.GetElement();
            }
            delete data->oldCacheEntries;
            data->oldCacheEntries = new ArrayList();
        }
    }

    return data->cache;
}

static BYTE* CompareEntry( CacheEntry* entry, DWORD numEvidence, DWORD evidenceSize, BYTE* evidenceBlock, HANDLE cache, DWORD* size)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_PREEMPTIVE;
    }
    CONTRACTL_END

    _ASSERTE (entry);

    if (entry->header.numItemsInKey == numEvidence &&
        entry->header.keySize == evidenceSize)
    {
        if (entry->key == NULL)
        {
            // We were lazy in reading the entry. Read the key now.
            entry->key = new BYTE[entry->header.keySize];

            _ASSERTE (cache != INVALID_HANDLE_VALUE);

            if (SetFilePointer( cache, entry->cachePosition, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER)
                return NULL;

            if (ReadFileData( cache, entry->key, entry->header.keySize ) != S_OK)
                return NULL;

            entry->cachePosition += entry->header.keySize;
        }

        _ASSERTE (entry->key);

        if (memcmp( entry->key, evidenceBlock, entry->header.keySize ) == 0)
        {
            if (entry->data == NULL)
            {
                // We were lazy in reading the entry. Read the data also.
                entry->data = new BYTE[entry->header.dataSize];

                if (SetFilePointer( cache, entry->cachePosition, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER)
                    return NULL;

                if (ReadFileData( cache, entry->data, entry->header.dataSize ) != S_OK)
                    return NULL;

                entry->cachePosition += entry->header.dataSize;
            }

            entry->used = TRUE;
            *size = entry->header.dataSize;

            return entry->data;
        }
    }
    return NULL;
}

FCIMPL4(FC_BOOL_RET, SecurityConfig::GetCacheEntry, INT32 id, DWORD numEvidence, CHARArray* evidenceUNSAFE, U1ARRAYREF* policy)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;

    BOOL retVal = false;
    CHARARRAYREF evidence = (CHARARRAYREF) evidenceUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_1(evidence);
    //-[autocvtpro]-------------------------------------------------------

    HANDLE cache = INVALID_HANDLE_VALUE;
    BOOL success = FALSE;
    DWORD size = (DWORD) -1;

    DWORD evidenceSize = evidence->GetNumComponents() * sizeof( WCHAR );
    NewHolder<BYTE> evidenceBlock(new BYTE[evidenceSize]);
    memcpyNoGCRefs( evidenceBlock.GetValue(), evidence->GetDirectPointerToNonObjectElements(), evidenceSize );

    *policy = NULL;

    BYTE* retval = NULL;

    Data* data = (Data*)GetData( id );

    {
        GCX_PREEMP();

        switch (s_registryExtensionsInfo.eStatus) {
        case NoExtensions:
            //
            // If registry extensions were removed from the registry since we last saved
            // the cache file, then we can't use the grant sets stored in the cache.
            //
            if (data->header.registryExtensionsInfo.eStatus != NoExtensions)
                goto CLEANUP;
            break;
        case AccessSuccess:
            //
            // check to see if policy extensions were added to the registry since
            // we last generated the security cache and configuration files.
            //
            if ((data->header.registryExtensionsInfo.eStatus != AccessSuccess) ||
                CompareFileTime(&s_registryExtensionsInfo.ftLastWriteTime, &data->header.registryExtensionsInfo.ftLastWriteTime) != 0)
                goto CLEANUP;
            break;
        case AccessFailure:
            goto CLEANUP;
            break;
        default:
            _ASSERTE(!"s_registryExtensionsInfo.eStatus was not properly initialized");
        }

        ArrayList::Iterator iter;

        if (data == NULL)
        {
            goto CLEANUP;
        }

        if ((data->state & Data::UsingCacheFile) == 0)
        {
            // We know we don't have anything in the config file, so
            // let's just look through the new entries to make sure we
            // aren't getting any repeats.

            // Then try the existing new entries

            iter = data->newCacheEntries->Iterate();

            while (iter.Next())
            {
                // newCacheEntries do not need the cache file so pass in NULL.
                retval = CompareEntry( (CacheEntry*)iter.GetElement(), numEvidence, evidenceSize, evidenceBlock, NULL, &size );

                if (retval != NULL)
                {
                    success = TRUE;
                    goto CLEANUP;
                }
            }

            goto CLEANUP;
        }

        // Its possible that the old entries were not read in completely
        // so we keep the cache file open before iterating through the
        // old entries.

        cache = OpenCacheFile( data );

        if ( cache == NULL )
        {
            goto CLEANUP;
        }

        // First, iterator over the old entries

        {
            CrstHolder ch(&dataLock_);

            iter = data->oldCacheEntries->Iterate();
            while (iter.Next())
            {
                retval = CompareEntry( (CacheEntry*)iter.GetElement(), numEvidence, evidenceSize, evidenceBlock, cache, &size );
                if (retval != NULL)
                {
                    success = TRUE;
                    goto CLEANUP;
                }
            }

            ch.Release();

            // Then try the existing new entries
            iter = data->newCacheEntries->Iterate();
            while (iter.Next())
            {
                // newCacheEntries do not need the cache file so pass in NULL.
                retval = CompareEntry( (CacheEntry*)iter.GetElement(), numEvidence, evidenceSize, evidenceBlock, NULL, &size );
                if (retval != NULL)
                {
                    success = TRUE;
                    goto CLEANUP;
                }
            }

            // Finally, try loading existing entries from the file

            ch.Acquire();

            if (SetFilePointer( cache, data->cacheCurrentPosition, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER)
            {
                goto CLEANUP;
            }

            do
            {
                CacheEntry* entry = LoadNextEntry( cache, data );
                if (entry == NULL)
                {
                    data->state = (Data::State)(Data::CacheExhausted | data->state);
                    break;
                }

                retval = CompareEntry( entry, numEvidence, evidenceSize, evidenceBlock, cache, &size );
                if (retval != NULL)
                {
                    success = TRUE;
                    break;
                }
            } while (TRUE);

            // LockHolder goes out of scope here
        }
    }

CLEANUP:
    if (success && retval != NULL)
    {
        _ASSERTE( size != (DWORD) -1 );
        _ASSERTE( GetThread()->PreemptiveGCDisabled() );
        *policy = (U1ARRAYREF)AllocatePrimitiveArray(ELEMENT_TYPE_U1, size);
        memcpyNoGCRefs( (*policy)->GetDirectPointerToNonObjectElements(), retval, size );
    }

    retVal = success;

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
    FC_RETURN_BOOL(retVal);
}
FCIMPLEND

FCIMPL4(void, SecurityConfig::AddCacheEntry, INT32 id, DWORD numEvidence, CHARArray* evidenceUNSAFE, U1Array* policyUNSAFE)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;

    CHARARRAYREF evidence = (CHARARRAYREF) evidenceUNSAFE;
    U1ARRAYREF policy = (U1ARRAYREF) policyUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_2(evidence, policy);
    //-[autocvtpro]-------------------------------------------------------

    Data* data = (Data*)GetData( id );

    DWORD sizeOfEntry = 0;
    NewHolder<CacheEntry> entry;

    if (data == NULL)
    {
        goto lExit;
    }

    // In order to limit how large a long running app can become,
    // we limit the total memory held by the new cache entries list.
    // For now this limit corresponds with how large the max cache file
    // can be.

    sizeOfEntry = sizeof( WCHAR ) * evidence->GetNumComponents() +
                  sizeof( BYTE ) * policy->GetNumComponents() +
                  sizeof( CacheEntryHeader );

    if (data->newEntriesSize + sizeOfEntry >= MAX_CACHEFILE_SIZE)
    {
        goto lExit;
    }

    entry = new CacheEntry();

    entry->header.numItemsInKey = numEvidence;
    entry->header.keySize = sizeof( WCHAR ) * evidence->GetNumComponents();
    entry->header.dataSize = sizeof( BYTE ) * policy->GetNumComponents();

    entry->key = new BYTE[entry->header.keySize];
    entry->data = new BYTE[entry->header.dataSize];

    memcpyNoGCRefs( entry->key, evidence->GetDirectPointerToNonObjectElements(), entry->header.keySize );
    memcpyNoGCRefs( entry->data, policy->GetDirectPointerToNonObjectElements(), entry->header.dataSize );

    {
        GCX_PREEMP();
        CrstHolder ch(&dataLock_);

        // Check the size again to handle the race.
        if (data->newEntriesSize + sizeOfEntry < MAX_CACHEFILE_SIZE)
        {
            data->state = (Data::State)(Data::CacheUpdated | data->state);
            IfFailThrow(data->newCacheEntries->Append( entry.GetValue() ));
            entry.SuppressRelease();
            data->newEntriesSize += sizeOfEntry;
        }
    }

lExit: ;
    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

ArrayListStatic SecurityConfig::entries_;
CrstStatic      SecurityConfig::dataLock_;

void SecurityConfig::Init( void )
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END

    dataLock_.Init("Security Policy Cache Lock", CrstSecurityPolicyCache);
    entries_.Init();
}

void SecurityConfig::Cleanup( void )
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END

    ArrayList::Iterator iter = entries_.Iterate();

    GCX_PREEMP();

    CrstHolder ch(&dataLock_);

    while (iter.Next())
    {
        ((Data*) iter.GetElement())->Cleanup();
    }
}

void SecurityConfig::Delete( void )
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END

    ArrayList::Iterator iter = entries_.Iterate();

    while (iter.Next())
    {
        delete (Data*) iter.GetElement();
    }

    entries_.Destroy();
    dataLock_.Destroy();
}

FCIMPL0(Object*, SecurityConfig::EcallGetMachineDirectory)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;        
    } CONTRACTL_END;

    STRINGREF refRetVal = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);
    //-[autocvtpro]-------------------------------------------------------

    
    WCHAR machine[MAX_PATH];
    HRESULT hr = GetMachineDirectory(machine, MAX_PATH);
    if (FAILED(hr))
        ThrowHR(hr);

    refRetVal = COMString::NewString( machine );

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND

FCIMPL0(Object*, SecurityConfig::EcallGetUserDirectory)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;

    STRINGREF refRetVal = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, refRetVal);
    //-[autocvtpro]-------------------------------------------------------

    WCHAR user[MAX_PATH];
    size_t userCount = MAX_PATH;

    BOOL result = GetUserDirectory(user, userCount);
    if (result)
        refRetVal = COMString::NewString( user );

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND

HRESULT SecurityConfig::GetMachineDirectory(__out_ecount(bufferCount) __out_z WCHAR* buffer, size_t bufferCount)
{
    CONTRACTL 
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END

    GCX_PREEMP();

    HRESULT hr;

    DWORD length = (DWORD)bufferCount;
    hr = GetInternalSystemDirectory(buffer, &length);
    if (FAILED(hr))
        return hr;
    
    // Make sure we have enough buffer to concat the string.
    // Note the length including the terminating zero. 
    if((bufferCount - wcslen(buffer) - 1) < wcslen(L"config\\"))
        return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);

    wcscat_s(buffer, bufferCount, L"config\\");
    return S_OK;
}

BOOL SecurityConfig::GetVIUserDirectory(__out_ecount(bufferCount) __out_z WCHAR* buffer, size_t bufferCount)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END

    GCX_PREEMP();

    WCHAR scratchBuffer[MAX_PATH];
    BOOL retval = FALSE;


    if (!PAL_GetUserConfigurationDirectory(buffer, (UINT)bufferCount))
        goto CLEANUP;


    wcscpy_s( scratchBuffer, COUNTOF(scratchBuffer), L"\\Microsoft\\CLR Security Config\\" );

    if (bufferCount < wcslen( buffer ) + wcslen( scratchBuffer ) + 1)
    {
        goto CLEANUP;
    }

    wcscat_s( buffer, bufferCount, scratchBuffer );

    retval = TRUE;

CLEANUP:
    return retval;
}

BOOL SecurityConfig::GetUserDirectory(__out_ecount(bufferCount) __out_z WCHAR* buffer, size_t bufferCount)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END

    GCX_PREEMP();

    StackSString ssScratchBuffer;
    BOOL retval = FALSE;

    WCHAR* wszScratchBuffer = ssScratchBuffer.OpenUnicodeBuffer( (COUNT_T)bufferCount );
    retval = GetVIUserDirectory(wszScratchBuffer, bufferCount);
    ssScratchBuffer.CloseBuffer( (COUNT_T)wcslen( wszScratchBuffer ) );

    if (!retval)
        return retval;

    ssScratchBuffer.Append( L"v" );
    ssScratchBuffer.Append( VER_FILEVERSION_STR_L );
    ssScratchBuffer.Append( L"\\" );

#if !defined(_X86_)
    ssScratchBuffer.Append( L"64bit\\" );
#endif // !_X86_

    if (ssScratchBuffer.GetCount() + 1 > bufferCount)
        return FALSE;

    wcscpy_s( buffer, bufferCount, ssScratchBuffer.GetUnicode() );

    return TRUE;
}

BOOL SecurityConfig::WriteToEventLog( __in_z WCHAR* message )
{
    WRAPPER_CONTRACT;

    return FALSE;
}


FCIMPL1(FC_BOOL_RET, SecurityConfig::EcallWriteToEventLog, StringObject* messageUNSAFE)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;        
    } CONTRACTL_END;

    BOOL retVal = FALSE;
    STRINGREF message = (STRINGREF) messageUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_1(message);
    //-[autocvtpro]-------------------------------------------------------

    WCHAR messageBuf[1024];
    WCHAR* wszMessage;
    size_t cchMessage;

    {
        GCX_COOP();
        if (message->GetStringLength() + 1 > COUNTOF(messageBuf))
        {
            cchMessage = message->GetStringLength() + 1;
            wszMessage = new (nothrow) WCHAR[cchMessage];
        }
        else
        {
            wszMessage = messageBuf;
            cchMessage = COUNTOF(wszMessage);
        }
    }

    if (wszMessage != NULL)
    {
        wcsncpy_s( wszMessage, COUNTOF(messageBuf), message->GetBuffer(), message->GetStringLength() + 1);

        retVal = WriteToEventLog( wszMessage );

        if (wszMessage != messageBuf)
        {
            delete [] wszMessage;
        }
    }

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
    FC_RETURN_BOOL(retVal);
}
FCIMPLEND

#ifdef _DEBUG
FCIMPL2(HRESULT, SecurityConfig::DebugOut, StringObject* fileUNSAFE, StringObject* messageUNSAFE)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

    HRESULT retVal = E_FAIL;
    STRINGREF fileName = (STRINGREF) fileUNSAFE;
    STRINGREF message = (STRINGREF) messageUNSAFE;

    HELPER_METHOD_FRAME_BEGIN_RET_2(fileName, message);
    //-[autocvtpro]-------------------------------------------------------

    SString ssFileName;
    SString ssMessage;

    fileName->GetSString( ssFileName );
    message->GetSString( ssMessage );

    DWORD ccMessage = message->GetStringLength();

    GCX_PREEMP();
    HandleHolder file(WszCreateFile( ssFileName.GetUnicode(), GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ));

    if (file == INVALID_HANDLE_VALUE)
    {
        goto lExit;
    }

    SetFilePointer( file, 0, NULL, FILE_END );

    DWORD bytesWritten;
    if (!WriteFile( file, ssMessage.GetUnicode(), ccMessage * sizeof(WCHAR), &bytesWritten, NULL ))
    {
        goto lExit;
    }

    if (ccMessage * sizeof(WCHAR) != bytesWritten)
    {
        goto lExit;
    }

    retVal = S_OK;

lExit: ;
    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
    return retVal;
}
FCIMPLEND
#endif
