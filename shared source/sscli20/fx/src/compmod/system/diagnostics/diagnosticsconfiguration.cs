//------------------------------------------------------------------------------
// <copyright file="DiagnosticsConfiguration.cs" company="Microsoft">
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

namespace System.Diagnostics {
    using System;
    using System.Reflection;
    using System.Collections;
    using System.Configuration;
    using System.Threading;
    using System.Runtime.Versioning;
    
    internal enum InitState {
        NotInitialized,
        Initializing,
        Initialized
    }

    internal static class DiagnosticsConfiguration {
        private static SystemDiagnosticsSection configSection;
        private static InitState initState = InitState.NotInitialized;

        // setting for Switch.switchSetting
        internal static SwitchElementsCollection SwitchSettings {
            get { 
                Initialize();
                SystemDiagnosticsSection configSectionSav = configSection;
                if (configSectionSav != null)
                    return configSectionSav.Switches;
                else
                    return null;
            }
        }

        // setting for DefaultTraceListener.AssertUIEnabled
        internal static bool AssertUIEnabled {
            get { 
                Initialize();
                SystemDiagnosticsSection configSectionSav = configSection;
                if (configSectionSav != null && configSectionSav.Assert != null)
                    return configSectionSav.Assert.AssertUIEnabled;
                else
                    return true; // the default
            }
        }

        internal static string ConfigFilePath {
            get { 
                Initialize();
                SystemDiagnosticsSection configSectionSav = configSection;
                if (configSectionSav != null) 
                    return configSectionSav.ElementInformation.Source;
                else
                    return string.Empty; // the default
            }
        }

        // setting for DefaultTraceListener.LogFileName
        internal static string LogFileName {
            [ResourceExposure(ResourceScope.Machine)]
            [ResourceConsumption(ResourceScope.Machine)]
            get { 
                Initialize();
                SystemDiagnosticsSection configSectionSav = configSection;
                if (configSectionSav != null && configSectionSav.Assert != null)
                    return configSectionSav.Assert.LogFileName;
                else
                    return string.Empty; // the default
            }
        }

        // setting for TraceInternal.AutoFlush
        internal static bool AutoFlush {
            get { 
                Initialize();
                SystemDiagnosticsSection configSectionSav = configSection;
                if (configSectionSav != null && configSectionSav.Trace != null)
                    return configSectionSav.Trace.AutoFlush;
                else
                    return false; // the default
            }
        }

        // setting for TraceInternal.UseGlobalLock
        internal static bool UseGlobalLock {
            get { 
                Initialize();
                SystemDiagnosticsSection configSectionSav = configSection;
                if (configSectionSav != null && configSectionSav.Trace != null)
                    return configSectionSav.Trace.UseGlobalLock;
                else
                    return true; // the default
            }
        }

        // setting for TraceInternal.IndentSize
        internal static int IndentSize {
            get { 
                Initialize();
                SystemDiagnosticsSection configSectionSav = configSection;
                if (configSectionSav != null && configSectionSav.Trace != null)
                    return configSectionSav.Trace.IndentSize;
                else
                    return 4; // the default
            }
        }


        internal static ListenerElementsCollection SharedListeners {
            get {
                Initialize();
                SystemDiagnosticsSection configSectionSav = configSection;
                if (configSectionSav != null)
                    return configSectionSav.SharedListeners;
                else
                    return null;
            }
        }

        internal static SourceElementsCollection  Sources {
            get {
                Initialize();
                SystemDiagnosticsSection configSectionSav = configSection;
                if (configSectionSav != null && configSectionSav.Sources != null)
                    return configSectionSav.Sources;
                else
                    return null;
            }
        }

        internal static SystemDiagnosticsSection SystemDiagnosticsSection {
            get {
                Initialize();
                return configSection;
            }
        }
        
        private static SystemDiagnosticsSection GetConfigSection() {
            SystemDiagnosticsSection configSection = (SystemDiagnosticsSection) PrivilegedConfigurationManager.GetSection("system.diagnostics");
            return configSection;
        }

        internal static bool IsInitializing() {
            return initState == InitState.Initializing;
        }

        internal static bool IsInitialized() {
            return initState == InitState.Initialized;
        }
            

        internal static bool CanInitialize() {
            return  (initState != InitState.Initializing) && 
                    !ConfigurationManagerInternalFactory.Instance.SetConfigurationSystemInProgress;
        }
        
        internal static void Initialize() {
            // Initialize() is also called by other components outside of Trace (such as PerformanceCounter)
            // as a result using one lock for this critical section and another for Trace API critical sections  
            // (such as Trace.WriteLine) could potentially lead to deadlock between 2 threads that are 
            // executing these critical sections (and consequently obtaining the 2 locks) in the reverse order. 
            // Using the same lock for DiagnosticsConfiguration as well as TraceInternal avoids this issue. 
            // Sequential locks on TraceInternal.critSec by the same thread is a non issue for this critical section.
            lock (TraceInternal.critSec) {

                // because some of the code used to load config also uses diagnostics
                // we can't block them while we initialize from config. Therefore we just
                // return immediately and they just use the default values.
                if (    initState != InitState.NotInitialized || 
                        ConfigurationManagerInternalFactory.Instance.SetConfigurationSystemInProgress) {

                    return;
                }

                initState = InitState.Initializing; // used for preventing recursion
                try {
                    configSection = GetConfigSection();
                }
                finally {
                    initState = InitState.Initialized;
                }
            }
        }

        internal static void Refresh() {
            ConfigurationManager.RefreshSection("system.diagnostics");

            // There might still be some persistant state left behind for 
            // ConfigPropertyCollection (for ex, swtichelements), probably for perf. 
            // We need to explicitly cleanup any unrecognized attributes that we 
            // have added during last deserialization, so that they are re-added 
            // during the next Config.GetSection properly and we get a chance to
            // populate the Attributes collection for re-deserialization. 
            // Another alternative could be to expose the properties collection
            // directly as Attributes collection (currently we keep a local 
            // hashtable which we explicitly need to keep in sycn and hence the 
            // cleanup logic below) but the down side of that would be we need to
            // explicitly compute what is recognized Vs unrecognized from that 
            // collection when we expose the unrecognized Attributes publically
            SystemDiagnosticsSection configSectionSav = configSection;
            if (configSectionSav != null) {

                if (configSectionSav.Switches != null) {
                    foreach (SwitchElement swelem in configSectionSav.Switches)
                        swelem.ResetProperties();
                }

                if (configSectionSav.SharedListeners != null) {
                    foreach (ListenerElement lnelem in configSectionSav.SharedListeners)
                        lnelem.ResetProperties();
                }

                if (configSectionSav.Sources != null) {
                    foreach (SourceElement srelem in configSectionSav.Sources)
                        srelem.ResetProperties();
                }
            }

            configSection = null;
            
            initState = InitState.NotInitialized;
            Initialize();
        }
    }
}

