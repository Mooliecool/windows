//---------------------------------------------------------------------------
//
// <copyright file="EventMap.cs" company="Microsoft">
//    Copyright (C) Microsoft Corporation.  All rights reserved.
// </copyright>
// 
//
// Description: 
// Accessibility event map classes are used to determine if, and how many
// listeners there are for events and property changes.
//
// History:  
//  07/23/2003 : BrendanM Ported to WCP
//
//---------------------------------------------------------------------------

using System;
using System.Collections;
using System.Windows;
using System.Windows.Automation;
using System.Windows.Automation.Peers;

using SR=MS.Internal.PresentationCore.SR;
using SRID=MS.Internal.PresentationCore.SRID;

namespace MS.Internal.Automation
{   
    // Manages the event map that is used to determine if there are Automation 
    // clients interested in specific events.  
    internal static class EventMap
    {

        private class EventInfo
        {
            internal EventInfo()
            {
                NumberOfListeners = 1;
            }

            internal int NumberOfListeners;
        }

        // Never inline, as we don't want to unnecessarily link the automation DLL.
        [System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
        private static bool IsKnownEvent(int id)
        {
            if (    id == AutomationElementIdentifiers.ToolTipOpenedEvent.Id
                ||  id == AutomationElementIdentifiers.ToolTipClosedEvent.Id
                ||  id == AutomationElementIdentifiers.MenuOpenedEvent.Id
                ||  id == AutomationElementIdentifiers.MenuClosedEvent.Id
                ||  id == AutomationElementIdentifiers.AutomationFocusChangedEvent.Id
                ||  id == InvokePatternIdentifiers.InvokedEvent.Id
                ||  id == SelectionItemPatternIdentifiers.ElementAddedToSelectionEvent.Id
                ||  id == SelectionItemPatternIdentifiers.ElementRemovedFromSelectionEvent.Id
                ||  id == SelectionItemPatternIdentifiers.ElementSelectedEvent.Id
                ||  id == SelectionPatternIdentifiers.InvalidatedEvent.Id
                ||  id == TextPatternIdentifiers.TextSelectionChangedEvent.Id
                ||  id == TextPatternIdentifiers.TextChangedEvent.Id
                ||  id == AutomationElementIdentifiers.AsyncContentLoadedEvent.Id
                ||  id == AutomationElementIdentifiers.AutomationPropertyChangedEvent.Id
                ||  id == AutomationElementIdentifiers.StructureChangedEvent.Id
                ||  ((SynchronizedInputPatternIdentifiers.InputReachedTargetEvent != null) && (id == SynchronizedInputPatternIdentifiers.InputReachedTargetEvent.Id))
                ||  ((SynchronizedInputPatternIdentifiers.InputReachedOtherElementEvent != null) && (id == SynchronizedInputPatternIdentifiers.InputReachedOtherElementEvent.Id))
                ||  ((SynchronizedInputPatternIdentifiers.InputDiscardedEvent != null) && (id == SynchronizedInputPatternIdentifiers.InputDiscardedEvent.Id)))
            {
                return true;
            }

            return false;
        }

        // Never inline, as we don't want to unnecessarily link the automation DLL.
        [System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
        private static AutomationEvent GetRegisteredEventObjectHelper(AutomationEvents eventId)
        {
            AutomationEvent eventObject = null;

            switch(eventId)
            {
                case AutomationEvents.ToolTipOpened:                                        eventObject = AutomationElementIdentifiers.ToolTipOpenedEvent; break;
                case AutomationEvents.ToolTipClosed:                                        eventObject = AutomationElementIdentifiers.ToolTipClosedEvent; break;
                case AutomationEvents.MenuOpened:                                           eventObject = AutomationElementIdentifiers.MenuOpenedEvent; break;
                case AutomationEvents.MenuClosed:                                           eventObject = AutomationElementIdentifiers.MenuClosedEvent; break;
                case AutomationEvents.AutomationFocusChanged:                               eventObject = AutomationElementIdentifiers.AutomationFocusChangedEvent; break;
                case AutomationEvents.InvokePatternOnInvoked:                               eventObject = InvokePatternIdentifiers.InvokedEvent; break;
                case AutomationEvents.SelectionItemPatternOnElementAddedToSelection:        eventObject = SelectionItemPatternIdentifiers.ElementAddedToSelectionEvent; break;
                case AutomationEvents.SelectionItemPatternOnElementRemovedFromSelection:    eventObject = SelectionItemPatternIdentifiers.ElementRemovedFromSelectionEvent; break;
                case AutomationEvents.SelectionItemPatternOnElementSelected:                eventObject = SelectionItemPatternIdentifiers.ElementSelectedEvent; break;
                case AutomationEvents.SelectionPatternOnInvalidated:                        eventObject = SelectionPatternIdentifiers.InvalidatedEvent; break;
                case AutomationEvents.TextPatternOnTextSelectionChanged:                    eventObject = TextPatternIdentifiers.TextSelectionChangedEvent; break;
                case AutomationEvents.TextPatternOnTextChanged:                             eventObject = TextPatternIdentifiers.TextChangedEvent; break;
                case AutomationEvents.AsyncContentLoaded:                                   eventObject = AutomationElementIdentifiers.AsyncContentLoadedEvent; break;
                case AutomationEvents.PropertyChanged:                                      eventObject = AutomationElementIdentifiers.AutomationPropertyChangedEvent; break;
                case AutomationEvents.StructureChanged:                                     eventObject = AutomationElementIdentifiers.StructureChangedEvent; break;
                case AutomationEvents.InputReachedTarget:                                   eventObject = SynchronizedInputPatternIdentifiers.InputReachedTargetEvent; break;
                case AutomationEvents.InputReachedOtherElement:                             eventObject = SynchronizedInputPatternIdentifiers.InputReachedOtherElementEvent; break;
                case AutomationEvents.InputDiscarded:                                       eventObject = SynchronizedInputPatternIdentifiers.InputDiscardedEvent; break;

                default:
                    throw new ArgumentException(SR.Get(SRID.Automation_InvalidEventId), "eventId");
            }

            if ((eventObject != null) && (!_eventsTable.ContainsKey(eventObject.Id)))
            {
                eventObject = null;
            }

            return (eventObject);
        }

        internal static void AddEvent(int idEvent)
        {
            lock (_lock)
            {
                if (_eventsTable == null)
                    _eventsTable = new Hashtable(20, .1f);

                if (_eventsTable.ContainsKey(idEvent))
                {
                    EventInfo info = (EventInfo)_eventsTable[idEvent];
                    info.NumberOfListeners++;
                }
                //  to avoid unbound memory allocations, 
                //  register only events that we recognize
                else if (IsKnownEvent(idEvent))
                {
                    _eventsTable[idEvent] = new EventInfo();
                }
            }
        }

        internal static void RemoveEvent(int idEvent)
        {
            lock (_lock)
            {
                if (_eventsTable != null)
                {
                    // Decrement the count of listeners for this event
                    if (_eventsTable.ContainsKey(idEvent))
                    {
                        EventInfo info = (EventInfo)_eventsTable[idEvent];

                        // Update or remove the entry based on remaining listeners
                        info.NumberOfListeners--;
                        if (info.NumberOfListeners <= 0)
                        {
                            _eventsTable.Remove(idEvent);

                            // If no more entries exist kill the table
                            if (_eventsTable.Count == 0)
                            {
                                _eventsTable = null;
                            }
                        }
                    }
                }
            }
        }
        
        //  Unlike GetRegisteredEvent below, 
        //  HasRegisteredEvent does NOT cause automation DLLs loading
        internal static bool HasRegisteredEvent(AutomationEvents eventId)
        {
            lock (_lock)
            {
                if (_eventsTable != null && _eventsTable.Count != 0)
                {
                    return (GetRegisteredEventObjectHelper(eventId) != null);
                }
            }
            return (false);
        }

        internal static AutomationEvent GetRegisteredEvent(AutomationEvents eventId)
        {
            lock (_lock)
            {
                if (_eventsTable != null && _eventsTable.Count != 0)
                {
                    return (GetRegisteredEventObjectHelper(eventId));
                }
            }

            return (null);
        }

        private static Hashtable _eventsTable;        // key=event id, data=listener count
        private readonly static object _lock = new object();
    }
}
