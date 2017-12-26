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
#include "common.h"
#include "eventstore.hpp"
#include "synch.h"

// A class to maintain a pool of available events.

const int EventStoreLength = 8;
class EventStore
{
public: 
    // Note: No constructors/destructors - global instance

    void Init()
    {
        WRAPPER_CONTRACT;

        m_EventStoreCrst.Init("EventStore", CrstEventStore, CRST_UNSAFE_ANYMODE);
        m_Store = NULL;
    }

    void Destroy()
    {
        WRAPPER_CONTRACT;
        
        _ASSERTE (g_fEEShutDown);

        m_EventStoreCrst.Destroy();

        EventStoreElem *walk;
        EventStoreElem *next;

        walk = m_Store;
        while (walk) {
            next = walk->next;
            delete (walk);
            walk = next;
        }
    }

    void StoreHandleForEvent (CLREvent* handle)
    {
        CONTRACTL {
            THROWS;
            GC_NOTRIGGER;
        } CONTRACTL_END;

        _ASSERTE (handle);
        CrstHolder ch(&m_EventStoreCrst);
        if (m_Store == NULL) {
            m_Store = new EventStoreElem ();
        }
        EventStoreElem *walk;
#ifdef _DEBUG
        // See if we have some leakage.
        LONG count = 0;
        walk = m_Store; 
        while (walk) {
            count += walk->AvailableEventCount();
            walk = walk->next;
        }
        // The number of events stored in the pool should be small.
        _ASSERTE (count <= ThreadStore::s_pThreadStore->ThreadCountInEE() * 2 + 10);
#endif
        walk = m_Store;        
        while (walk) {
            if (walk->StoreHandleForEvent (handle) )
                return;
            if (walk->next == NULL) {
                break;
            }
            walk = walk->next;
        }
        if (walk != NULL)
        {
            walk->next = new EventStoreElem ();
            walk->next->hArray[0] = handle;
        }
    }

    CLREvent* GetHandleForEvent ()
    {
        CONTRACTL {
            THROWS;
            GC_NOTRIGGER;
        } CONTRACTL_END;

        CLREvent* handle;
        CrstHolder ch(&m_EventStoreCrst);
        EventStoreElem *walk = m_Store;
        while (walk) {
            handle = walk->GetHandleForEvent();
            if (handle != NULL) {
                return handle;
            }
            walk = walk->next;
        }
        handle = new CLREvent;
        _ASSERTE (handle != NULL);
        handle->CreateManualEvent(TRUE);
        return handle;
    }

private:
    struct EventStoreElem
    {
        CLREvent *hArray[EventStoreLength];
        EventStoreElem *next;
        
        EventStoreElem ()
        {
            LEAF_CONTRACT;

            next = NULL;
            for (int i = 0; i < EventStoreLength; i ++) {
                hArray[i] = NULL;
            }
        }

        ~EventStoreElem ()
        {
            LEAF_CONTRACT;

            for (int i = 0; i < EventStoreLength; i++) {
                if (hArray[i]) {
                    delete hArray[i];
                    hArray[i] = NULL;
                }
            }
        }

        // Store a handle in the current EventStoreElem.  Return TRUE if succeessful.
        // Return FALSE if failed due to no free slot.
        BOOL StoreHandleForEvent (CLREvent* handle)
        {
            LEAF_CONTRACT;

            int i;
            for (i = 0; i < EventStoreLength; i++) {
                if (hArray[i] == NULL) {
                    hArray[i] = handle;
                    return TRUE;
                }
            }
            return FALSE;
        }

        // Get a handle from the current EventStoreElem.
        CLREvent* GetHandleForEvent ()
        {
            LEAF_CONTRACT;

            int i;
            for (i = 0; i < EventStoreLength; i++) {
                if (hArray[i] != NULL) {
                    CLREvent* handle = hArray[i];
                    hArray[i] = NULL;
                    return handle;
                }
            }

            return NULL;
        }

#ifdef _DEBUG
        LONG AvailableEventCount ()
        {
            LEAF_CONTRACT;

            LONG count = 0;
            for (int i = 0; i < EventStoreLength; i++) {
                if (hArray[i] != NULL) {
                    count ++;
                }
            }
            return count;
        }
#endif
    };

    EventStoreElem  *m_Store;
    
    // Critical section for adding and removing event used for Object::Wait
    CrstStatic      m_EventStoreCrst;
};

static EventStore s_EventStore;
 
CLREvent* GetEventFromEventStore()
{
    WRAPPER_CONTRACT;

    return s_EventStore.GetHandleForEvent();
}

void StoreEventToEventStore(CLREvent* hEvent)
{
    WRAPPER_CONTRACT;

    s_EventStore.StoreHandleForEvent(hEvent);
}

void InitEventStore()
{
    WRAPPER_CONTRACT;

    s_EventStore.Init();
}

void TerminateEventStore()
{
    WRAPPER_CONTRACT;

    s_EventStore.Destroy();
}
