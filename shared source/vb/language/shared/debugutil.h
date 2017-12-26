//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Helpful debugging utilities
//
//-------------------------------------------------------------------------------------------------
#pragma once

class DebugUtil
{
#if DEBUG
    struct ObjectInfo
    {
        WCHAR NameBuffer[250];
        void* ObjectPointer;

        ObjectInfo(const WCHAR* wszName, void* pThis)
        {
            ObjectPointer = pThis;
            (void)StringCchCopy(NameBuffer, _countof(NameBuffer), wszName);
        }
    };

public:
    DebugUtil();
    ~DebugUtil();

    void TrackCreate(const WCHAR* name, void* pThis);
    void TrackDestroy(void* pThis);
    void Check();

private:
    CriticalSection m_cs;
    DynamicHashTable<void*,ObjectInfo*> m_objectMap;
#endif
};

extern class DebugUtil *g_pvbDebugUtil;

#if DEBUG

#define DUTIL_TRACK_CREATE(className) do { if ( g_pvbDebugUtil ) g_pvbDebugUtil->TrackCreate(_T(#className), this); } while(0)
#define DUTIL_TRACK_DESTROY() do { if ( g_pvbDebugUtil ) g_pvbDebugUtil->TrackDestroy(this); } while(0)
#define DUTIL_TRACK_CHECK() do { if ( g_pvbDebugUtil ) g_pvbDebugUtil->Check();} while(0)

#else

#define DUTIL_TRACK_CREATE(className) 
#define DUTIL_TRACK_DESTROY() 
#define DUTIL_TRACK_CHECK()

#endif
