//------------------------------------------------------------------------------
// <copyright file="TraceSource.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>
//------------------------------------------------------------------------------

using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Threading;
using System.Configuration;
using System.Security;
using System.Security.Permissions;

namespace System.Diagnostics {
    public class TraceSource {
        private static List<WeakReference> tracesources =  new List<WeakReference>();
        
        private readonly TraceEventCache manager = new TraceEventCache();
        private SourceSwitch internalSwitch;
        private TraceListenerCollection listeners;
        private StringDictionary attributes;
        private SourceLevels switchLevel;
        private string sourceName;
        internal bool _initCalled = false;   // Whether we've called Initialize already.
        
        public TraceSource(string name) 
            : this(name, SourceLevels.Off) {
        }
        
        public TraceSource(string name, SourceLevels defaultLevel) {
            if (name == null)
                throw new ArgumentNullException("name");
            if (name.Length == 0)
                throw new ArgumentException("name");

            sourceName = name;
            switchLevel = defaultLevel;

            // Delay load config to avoid perf (and working set) issues in retail

            // Add a weakreference to this source
            lock(tracesources) {
                tracesources.Add(new WeakReference(this));
            }
        }

        private void Initialize() {
            if (!_initCalled) {
                lock(this) {
                    if (_initCalled)
                        return;
                    
                    SourceElementsCollection sourceElements = DiagnosticsConfiguration.Sources;
    
                    if (sourceElements != null) {
                        SourceElement sourceElement = sourceElements[sourceName];
                        if (sourceElement != null) {
                            if (!String.IsNullOrEmpty(sourceElement.SwitchName)) {
                                CreateSwitch(sourceElement.SwitchType, sourceElement.SwitchName);
                            }
                            else {
                                CreateSwitch(sourceElement.SwitchType, sourceName);
    
                                if (!String.IsNullOrEmpty(sourceElement.SwitchValue)) 
                                    internalSwitch.Level = (SourceLevels) Enum.Parse(typeof(SourceLevels), sourceElement.SwitchValue);
                            }

                            listeners = sourceElement.Listeners.GetRuntimeObject();
    
                            attributes = new StringDictionary();
                            TraceUtils.VerifyAttributes(sourceElement.Attributes, GetSupportedAttributes(), this);
                            attributes.contents = sourceElement.Attributes;
                        }
                        else
                            NoConfigInit();
                    }
                    else
                        NoConfigInit();
    
                    _initCalled = true;
                }
            }
        }
         
        private void NoConfigInit() {
            internalSwitch = new SourceSwitch(sourceName, switchLevel.ToString());
            listeners = new TraceListenerCollection();
            listeners.Add(new DefaultTraceListener());
            attributes = null;
        }
        
        [SecurityPermission(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.UnmanagedCode)]
        public void Close() {
            // No need to call Initialize()
            if (listeners != null) {
                // Use global lock
                lock (TraceInternal.critSec) {
                    foreach (TraceListener listener in listeners) {
                        listener.Close();
                    }
                }
            }
        }

        public void Flush() {
            // No need to call Initialize()
            if (listeners != null) {
                if (TraceInternal.UseGlobalLock) {
                    lock (TraceInternal.critSec) {
                        foreach (TraceListener listener in listeners) {
                            listener.Flush();
                        }
                    }
                }
                else {
                    foreach (TraceListener listener in listeners) {
                        if (!listener.IsThreadSafe) {
                            lock (listener) {
                                listener.Flush();
                            }
                        }
                        else {
                            listener.Flush();
                        }
                    }            
                }
            }
        }

        virtual protected internal string[] GetSupportedAttributes() {
            return null;
        }

        internal static void RefreshAll() {
            lock (tracesources) {
                for (int i=0; i<tracesources.Count; i++) {
                    TraceSource tracesource = ((TraceSource) tracesources[i].Target);
                    if (tracesource != null) {
                        tracesource.Refresh();
                    }
                }
            }
        }
        
        internal void Refresh() {
            if (!_initCalled) {
                Initialize();
                return;
            }

            SourceElementsCollection sources = DiagnosticsConfiguration.Sources;

            if (sources != null) {
                SourceElement sourceElement = sources[Name];
                if (sourceElement != null) {

                    // first check if the type changed
                    if ((String.IsNullOrEmpty(sourceElement.SwitchType) && internalSwitch.GetType() != typeof(SourceSwitch)) ||
                         (sourceElement.SwitchType != internalSwitch.GetType().AssemblyQualifiedName)) {

                        if (!String.IsNullOrEmpty(sourceElement.SwitchName)) {
                            CreateSwitch(sourceElement.SwitchType, sourceElement.SwitchName);
                        }
                        else {
                            CreateSwitch(sourceElement.SwitchType, Name);
                        
                            if (!String.IsNullOrEmpty(sourceElement.SwitchValue)) 
                                internalSwitch.Level = (SourceLevels) Enum.Parse(typeof(SourceLevels), sourceElement.SwitchValue);
                        }
                    }
                    else if (!String.IsNullOrEmpty(sourceElement.SwitchName)) {
                        // create a new switch if the name changed, otherwise just refresh. 
                        if (sourceElement.SwitchName != internalSwitch.DisplayName)
                            CreateSwitch(sourceElement.SwitchType, sourceElement.SwitchName);
                        else
                            internalSwitch.Refresh();
                    }
                    else {
                        // the switchValue changed.  Just update our internalSwitch. 
                        if (!String.IsNullOrEmpty(sourceElement.SwitchValue)) 
                            internalSwitch.Level = (SourceLevels) Enum.Parse(typeof(SourceLevels), sourceElement.SwitchValue);
                        else
                            internalSwitch.Level = SourceLevels.Off;
                    }

                    TraceListenerCollection newListenerCollection = new TraceListenerCollection();
                    foreach (ListenerElement listenerElement in sourceElement.Listeners) {
                        TraceListener listener = listeners[listenerElement.Name];
                        if (listener != null) {
                            newListenerCollection.Add(listenerElement.RefreshRuntimeObject(listener));
                        }
                        else {
                            newListenerCollection.Add(listenerElement.GetRuntimeObject());
                        }
                    }

                    TraceUtils.VerifyAttributes(sourceElement.Attributes, GetSupportedAttributes(), this);

                    attributes  = new StringDictionary();
                    attributes.contents = sourceElement.Attributes;

                    listeners = newListenerCollection;
                }
                else {
                    // there was no config, so clear whatever we have.  
                    internalSwitch.Level = switchLevel;
                    listeners.Clear();
                    attributes = null;
                }
            }
        }
        
        [Conditional("TRACE")]
        public void TraceEvent(TraceEventType eventType, int id) {
            // Ensure that config is loaded 
            Initialize();
             
            if (internalSwitch.ShouldTrace(eventType) && listeners != null) {
                if (TraceInternal.UseGlobalLock) {
                    // we lock on the same object that Trace does because we're writing to the same Listeners.
                    lock (TraceInternal.critSec) {
                        for (int i=0; i<listeners.Count; i++) {
                            TraceListener listener = listeners[i];
                            listener.TraceEvent(manager, Name, eventType, id);
                            if (Trace.AutoFlush) listener.Flush();
                        }
                    }
                }
                else {
                    for (int i=0; i<listeners.Count; i++) {
                        TraceListener listener = listeners[i];
                        if (!listener.IsThreadSafe) {
                            lock (listener) {
                                listener.TraceEvent(manager, Name, eventType, id);
                                if (Trace.AutoFlush) listener.Flush();
                            }
                        }
                        else {
                            listener.TraceEvent(manager, Name, eventType, id);
                            if (Trace.AutoFlush) listener.Flush();
                        }
                    }
                }
                manager.Clear();
            }
        }

        [Conditional("TRACE")]
        public void TraceEvent(TraceEventType eventType, int id, string message) {
            // Ensure that config is loaded 
            Initialize();
             
            if (internalSwitch.ShouldTrace(eventType) && listeners != null) {
                if (TraceInternal.UseGlobalLock) {
                    // we lock on the same object that Trace does because we're writing to the same Listeners.
                    lock (TraceInternal.critSec) {
                        for (int i=0; i<listeners.Count; i++) {
                            TraceListener listener = listeners[i];
                            listener.TraceEvent(manager, Name, eventType, id, message);
                            if (Trace.AutoFlush) listener.Flush();
                        }
                    }
                }
                else {
                    for (int i=0; i<listeners.Count; i++) {
                        TraceListener listener = listeners[i];
                        if (!listener.IsThreadSafe) {
                            lock (listener) {
                                listener.TraceEvent(manager, Name, eventType, id, message);
                                if (Trace.AutoFlush) listener.Flush();
                            }
                        }
                        else {
                            listener.TraceEvent(manager, Name, eventType, id, message);
                            if (Trace.AutoFlush) listener.Flush();
                        }
                    }
                }
                manager.Clear();
            }
        }

        [Conditional("TRACE")]
        public void TraceEvent(TraceEventType eventType, int id, string format, params object[] args) {
            // Ensure that config is loaded 
            Initialize();
             
            if (internalSwitch.ShouldTrace(eventType) && listeners != null) {
                if (TraceInternal.UseGlobalLock) {
                    // we lock on the same object that Trace does because we're writing to the same Listeners.
                    lock (TraceInternal.critSec) {
                        for (int i=0; i<listeners.Count; i++) {
                            TraceListener listener = listeners[i];
                            listener.TraceEvent(manager, Name, eventType, id, format, args);
                            if (Trace.AutoFlush) listener.Flush();
                        }
                    }
                }
                else {
                    for (int i=0; i<listeners.Count; i++) {
                        TraceListener listener = listeners[i];
                        if (!listener.IsThreadSafe) {
                            lock (listener) {
                                listener.TraceEvent(manager, Name, eventType, id, format, args);
                                if (Trace.AutoFlush) listener.Flush();
                            }
                        }
                        else {
                            listener.TraceEvent(manager, Name, eventType, id, format, args);
                            if (Trace.AutoFlush) listener.Flush();
                        }
                    }
                }
                manager.Clear();
            }
        }

        [Conditional("TRACE")]
        public void TraceData(TraceEventType eventType, int id, object data) {
            // Ensure that config is loaded 
            Initialize();
             
            if (internalSwitch.ShouldTrace(eventType) && listeners != null) {
                if (TraceInternal.UseGlobalLock) {
                    // we lock on the same object that Trace does because we're writing to the same Listeners.
                    lock (TraceInternal.critSec) {
                        for (int i=0; i<listeners.Count; i++) {
                            TraceListener listener = listeners[i];
                            listener.TraceData(manager, Name, eventType, id, data);
                            if (Trace.AutoFlush) listener.Flush();
                        }
                    }
                }
                else {
                    for (int i=0; i<listeners.Count; i++) {
                        TraceListener listener = listeners[i];
                        if (!listener.IsThreadSafe) {
                            lock (listener) {
                                listener.TraceData(manager, Name, eventType, id, data);
                                if (Trace.AutoFlush) listener.Flush();
                            }
                        }
                        else {
                            listener.TraceData(manager, Name, eventType, id, data);
                            if (Trace.AutoFlush) listener.Flush();
                        }
                    }
                }
                manager.Clear();
            }
        }

        [Conditional("TRACE")]
        public void TraceData(TraceEventType eventType, int id, params object[] data) {
            // Ensure that config is loaded 
            Initialize();
             
            if (internalSwitch.ShouldTrace(eventType) && listeners != null) {
                if (TraceInternal.UseGlobalLock) {
                    // we lock on the same object that Trace does because we're writing to the same Listeners.
                    lock (TraceInternal.critSec) {
                        for (int i=0; i<listeners.Count; i++) {
                            TraceListener listener = listeners[i];
                            listener.TraceData(manager, Name, eventType, id, data);
                            if (Trace.AutoFlush) listener.Flush();
                        }
                    }
                }
                else {
                    for (int i=0; i<listeners.Count; i++) {
                        TraceListener listener = listeners[i];
                        if (!listener.IsThreadSafe) {
                            lock (listener) {
                                listener.TraceData(manager, Name, eventType, id, data);
                                if (Trace.AutoFlush) listener.Flush();
                            }
                        }
                        else {
                            listener.TraceData(manager, Name, eventType, id, data);
                            if (Trace.AutoFlush) listener.Flush();
                        }
                    }
                }
                manager.Clear();
            }
        }

        [Conditional("TRACE")]
        public void TraceInformation(string message) { // eventType= TraceEventType.Info, id=0
            // No need to call Initialize()
            TraceEvent(TraceEventType.Information, 0, message, null);
        }

        [Conditional("TRACE")]
        public void TraceInformation(string format, params object[] args) {
            // No need to call Initialize()
            TraceEvent(TraceEventType.Information, 0, format, args);
        }

        [Conditional("TRACE")]
        public void TraceTransfer(int id, string message, Guid relatedActivityId) {
            // Ensure that config is loaded 
            Initialize();
             
            if (internalSwitch.ShouldTrace(TraceEventType.Transfer) && listeners != null) {
                if (TraceInternal.UseGlobalLock) {
                    // we lock on the same object that Trace does because we're writing to the same Listeners.
                    lock (TraceInternal.critSec) {
                        for (int i=0; i<listeners.Count; i++) {
                            TraceListener listener = listeners[i];
                            listener.TraceTransfer(manager, Name, id, message, relatedActivityId);
                            if (Trace.AutoFlush) listener.Flush();
                        }
                    }
                }
                else {
                    for (int i=0; i<listeners.Count; i++) {
                        TraceListener listener = listeners[i];
                        if (!listener.IsThreadSafe) {
                            lock (listener) {
                                listener.TraceTransfer(manager, Name, id, message, relatedActivityId);
                                if (Trace.AutoFlush) listener.Flush();
                            }
                        }
                        else {
                            listener.TraceTransfer(manager, Name, id, message, relatedActivityId);
                            if (Trace.AutoFlush) listener.Flush();
                        }
                    }
                }
                manager.Clear();
            }
        }

        private void CreateSwitch(string typename, string name) {
            if (!String.IsNullOrEmpty(typename))
                internalSwitch = (SourceSwitch) TraceUtils.GetRuntimeObject(typename, typeof(SourceSwitch), name);
            else
                internalSwitch = new SourceSwitch(name, switchLevel.ToString());
        }
        
        public StringDictionary Attributes {
            get {
                // Ensure that config is loaded 
                Initialize();

                if (attributes == null)
                    attributes  = new StringDictionary();

                return attributes;
            }
        }

        public string Name {
            get {
                return sourceName;
            }
        }

        public TraceListenerCollection Listeners {
            [SecurityPermission(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.UnmanagedCode)]
            get {
                // Ensure that config is loaded 
                Initialize();

                return listeners;
            }
        }

        public SourceSwitch Switch {
            // No need for security demand here. SourceSwitch.set_Level is protected already.
            get {
                // Ensure that config is loaded 
                Initialize();

                return internalSwitch;
            }
            [SecurityPermission(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.UnmanagedCode)]
            set {
                if (value == null)
                    throw new ArgumentNullException("Switch");

                // Ensure that config is loaded 
                Initialize();

                internalSwitch = value;
            }
        }
    }
}
