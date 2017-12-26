//------------------------------------------------------------------------------
// <copyright fieldInfole="_AutoWebProxyScriptEngine.cs" company="Microsoft">
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

namespace System.Net
{
    using System.IO;
    using System.Collections;
    using System.Collections.Specialized;
    using System.Threading;
    using System.Text;
    using System.Net.Cache;
    using System.Globalization;
    using System.Net.Configuration;
    using System.Security.Permissions;

    enum AutoWebProxyState {
        Uninitialized = 0,
        DiscoveryFailure = 1,
        DiscoverySuccess = 2,
        DownloadFailure = 3,
        DownloadSuccess = 4,
        CompilationFailure = 5,
        CompilationSuccess = 6,
        ExecutionFailure = 7,
        ExecutionSuccess = 8,
    }

    /// <summary>
    ///    Simple EXE host for the AutoWebProxyScriptEngine. Pushes the contents of a script file
    ///    into the engine and executes it.  Exposes the JScript model used in IE 3.2 - 6,
    ///    for resolving which proxy to use.
    /// </summary>
    internal class AutoWebProxyScriptEngine {
        private static readonly char[] splitChars = new char[]{';'};

        private static TimerThread.Queue s_TimerQueue;
        private static readonly TimerThread.Callback s_TimerCallback = new TimerThread.Callback(RequestTimeoutCallback);
        private static readonly WaitCallback s_AbortWrapper = new WaitCallback(AbortWrapper);

        private bool automaticallyDetectSettings;
        private Uri automaticConfigurationScript;

        private AutoWebProxyScriptWrapper scriptInstance;
        internal AutoWebProxyState state;
        private Uri engineScriptLocation;
        private WebProxy webProxy;

        private RequestCache backupCache;

        // Used by abortable lock.
        private bool m_LockHeld;
        private WebRequest m_LockedRequest;

        private bool m_UseRegistry;


        [SecurityPermission(SecurityAction.Assert, Flags = SecurityPermissionFlag.ControlPrincipal)]
        internal AutoWebProxyScriptEngine(WebProxy proxy, bool useRegistry)
        {
            webProxy = proxy;
            m_UseRegistry = useRegistry;

            backupCache = new SingleItemRequestCache(RequestCacheManager.IsCachingEnabled);
        }


        // AutoWebProxyScriptEngine has special abortable locking.  No one should ever lock (this) except the locking helper methods below.
        private static class SyncStatus
        {
            internal const int Unlocked      = 0;
            internal const int Locking       = 1;
            internal const int LockOwner     = 2;
            internal const int RequestOwner  = 3;
            internal const int AbortedLocked = 4;
            internal const int Aborted       = 5;
        }

        private void EnterLock(ref int syncStatus)
        {
            if (syncStatus == SyncStatus.Unlocked)
            {
                lock (this)
                {
                    if (syncStatus != SyncStatus.Aborted)
                    {
                        syncStatus = SyncStatus.Locking;
                        while (true)
                        {
                            if (!m_LockHeld)
                            {
                                syncStatus = SyncStatus.LockOwner;
                                m_LockHeld = true;
                                return;
                            }
                            Monitor.Wait(this);
                            if (syncStatus == SyncStatus.Aborted)
                            {
                                Monitor.Pulse(this);  // This is to ensure that a Pulse meant to let someone take the lock isn't lost.
                                return;
                            }
                        }
                    }
                }
            }
        }

        private void ExitLock(ref int syncStatus)
        {
            if (syncStatus != SyncStatus.Unlocked && syncStatus != SyncStatus.Aborted)
            {
                lock (this)
                {
                    if (syncStatus == SyncStatus.RequestOwner)
                    {
                        m_LockedRequest = null;
                    }
                    m_LockHeld = false;
                    if (syncStatus == SyncStatus.AbortedLocked)
                    {
                        state = AutoWebProxyState.Uninitialized;
                        syncStatus = SyncStatus.Aborted;
                    }
                    else
                    {
                        syncStatus = SyncStatus.Unlocked;
                    }
                    Monitor.Pulse(this);
                }
            }
        }

        private void LockRequest(WebRequest request, ref int syncStatus)
        {
            lock (this)
            {
                switch (syncStatus)
                {
                    case SyncStatus.LockOwner:
                        m_LockedRequest = request;
                        syncStatus = SyncStatus.RequestOwner;
                        break;

                    case SyncStatus.RequestOwner:
                        m_LockedRequest = request;
                        break;
                }
            }
        }

        internal void Abort(ref int syncStatus)
        {
            lock (this)
            {
                switch (syncStatus)
                {
                    case SyncStatus.Unlocked:
                        syncStatus = SyncStatus.Aborted;
                        break;

                    case SyncStatus.Locking:
                        syncStatus = SyncStatus.Aborted;
                        Monitor.PulseAll(this);
                        break;

                    case SyncStatus.LockOwner:
                        syncStatus = SyncStatus.AbortedLocked;
                        break;

                    case SyncStatus.RequestOwner:
                        ThreadPool.UnsafeQueueUserWorkItem(s_AbortWrapper, m_LockedRequest);
                        syncStatus = SyncStatus.AbortedLocked;
                        m_LockedRequest = null;
                        break;
                }
            }
        }
        // End of locking helper methods.



        // The lock is always held while these three are modified.
        internal bool AutomaticallyDetectSettings
        {
            set
            {
                if (automaticallyDetectSettings != value)
                {
                    state = AutoWebProxyState.Uninitialized;
                    automaticallyDetectSettings = value;
                }
            }
        }

        internal Uri AutomaticConfigurationScript
        {
            set
            {
                if (!object.Equals(automaticConfigurationScript, value))
                {
                    automaticConfigurationScript = value;

                    {
                        state = AutoWebProxyState.Uninitialized;
                    }
                }
            }
        }


        // from wininet.h
        //
        //  #define INTERNET_MAX_PATH_LENGTH        2048
        //  #define INTERNET_MAX_PROTOCOL_NAME      "gopher"    // longest protocol name
        //  #define INTERNET_MAX_URL_LENGTH         ((sizeof(INTERNET_MAX_PROTOCOL_NAME) - 1) \
        //                                          + sizeof("://") \
        //                                          + INTERNET_MAX_PATH_LENGTH)
        //
        private const int MaximumProxyStringLength = 2058;

        /// <devdoc>
        ///     <para>
        ///         Called to discover script location. This performs
        ///         autodetection using the method specified in the detectFlags.
        ///     </para>
        /// </devdoc>
        private static unsafe Uri SafeDetectAutoProxyUrl(uint discoveryMethod)
        {
            Uri autoProxy = null;


            return autoProxy;
        }

        internal StringCollection GetProxies(Uri destination, bool returnFirstOnly, out AutoWebProxyState autoWebProxyState)
        {
            int syncStatus = SyncStatus.Unlocked;
            return GetProxies(destination, returnFirstOnly, out autoWebProxyState, ref syncStatus);
        }

        internal StringCollection GetProxies(Uri destination, bool returnFirstOnly, out AutoWebProxyState autoWebProxyState, ref int syncStatus)
        {
            GlobalLog.Print("AutoWebProxyScriptEngine#" + ValidationHelper.HashString(this) + "::GetProxies() state:" + ValidationHelper.ToString(state));


            if (state==AutoWebProxyState.DiscoveryFailure) {
                // No engine will be available anyway, shortcut the call.
                autoWebProxyState = state;
                return null;
            }

            // This whole thing has to be locked, both to prevent simultaneous downloading / compilation, and
            // because the script isn't threadsafe.
            string scriptReturn = null;
            try
            {
                EnterLock(ref syncStatus);
                if (syncStatus != SyncStatus.LockOwner)
                {
                    // This is typically because a download got aborted.
                    autoWebProxyState = AutoWebProxyState.DownloadFailure;
                    return null;
                }

                autoWebProxyState = EnsureEngineAvailable(ref syncStatus);
                if (autoWebProxyState != AutoWebProxyState.CompilationSuccess)
                {
                    // the script can't run, say we're not ready and bypass
                    return null;
                }
                autoWebProxyState = AutoWebProxyState.ExecutionFailure;
                try {
                    scriptReturn = scriptInstance.FindProxyForURL(destination.ToString(), destination.Host);
                    autoWebProxyState = AutoWebProxyState.ExecutionSuccess;
                    GlobalLog.Print("AutoWebProxyScriptEngine#" + ValidationHelper.HashString(this) + "::GetProxies() calling ExecuteFindProxyForURL() for destination:" + ValidationHelper.ToString(destination) + " returned scriptReturn:" + ValidationHelper.ToString(scriptReturn));
                }
                catch (Exception exception) {
                    if (NclUtilities.IsFatal(exception)) throw;
                    if(Logging.On)Logging.PrintWarning(Logging.Web, SR.GetString(SR.net_log_proxy_script_execution_error, exception));
                    GlobalLog.Print("AutoWebProxyScriptEngine#" + ValidationHelper.HashString(this) + "::GetProxies() calling ExecuteFindProxyForURL() for destination:" + ValidationHelper.ToString(destination) + " threw:" + ValidationHelper.ToString(exception));
                }
            }
            finally
            {
                ExitLock(ref syncStatus);
            }

            if (autoWebProxyState==AutoWebProxyState.ExecutionFailure) {
                // the script failed at runtime, say we're not ready and bypass
                return null;
            }
            StringCollection proxies = ParseScriptReturn(scriptReturn, destination, returnFirstOnly);
            GlobalLog.Print("AutoWebProxyScriptEngine#" + ValidationHelper.HashString(this) + "::GetProxies() proxies:" + ValidationHelper.ToString(proxies));
            return proxies;
        }

        /// <devdoc>
        ///     <para>
        ///         Ensures that (if state is AutoWebProxyState.CompilationSuccess) there is an engine available to execute script.
        ///         Figures out the script location (might discover if needed).
        ///         Calls DownloadAndCompile().
        ///     </para>
        /// </devdoc>
        private AutoWebProxyState EnsureEngineAvailable(ref int syncStatus)
        {
            GlobalLog.Enter("AutoWebProxyScriptEngine#" + ValidationHelper.HashString(this) + "::EnsureEngineAvailable");
            AutoWebProxyScriptWrapper newScriptInstance;

            if (state == AutoWebProxyState.Uninitialized || engineScriptLocation == null)
            {

                // Either Auto-Detect wasn't enabled or something failed with it.  Try the manual script location.
                if (automaticConfigurationScript != null)
                {
                    GlobalLog.Print("AutoWebProxyScriptEngine#" + ValidationHelper.HashString(this) + "::EnsureEngineAvailable() using automaticConfigurationScript:" + ValidationHelper.ToString(automaticConfigurationScript) + " engineScriptLocation:" + ValidationHelper.ToString(engineScriptLocation));
                    state = AutoWebProxyState.DiscoverySuccess;
                    if (automaticConfigurationScript.Equals(engineScriptLocation))
                    {
                        state = AutoWebProxyState.CompilationSuccess;
                        GlobalLog.Leave("AutoWebProxyScriptEngine#" + ValidationHelper.HashString(this) + "::EnsureEngineAvailable", ValidationHelper.ToString(state));
                        return state;
                    }
                    state = DownloadAndCompile(automaticConfigurationScript, out newScriptInstance, ref syncStatus);
                    if (state == AutoWebProxyState.CompilationSuccess)
                    {
                        UpdateScriptInstance(newScriptInstance);
                        engineScriptLocation = automaticConfigurationScript;
                        GlobalLog.Leave("AutoWebProxyScriptEngine#" + ValidationHelper.HashString(this) + "::EnsureEngineAvailable", ValidationHelper.ToString(state));
                        return state;
                    }
                }
            }
            else
            {
                // We always want to call DownloadAndCompile to check the expiration.
                GlobalLog.Print("AutoWebProxyScriptEngine#" + ValidationHelper.HashString(this) + "::EnsureEngineAvailable() state:" + state + " engineScriptLocation:" + ValidationHelper.ToString(engineScriptLocation));
                state = AutoWebProxyState.DiscoverySuccess;
                state = DownloadAndCompile(engineScriptLocation, out newScriptInstance, ref syncStatus);
                if (state == AutoWebProxyState.CompilationSuccess)
                {
                    UpdateScriptInstance(newScriptInstance);
                    GlobalLog.Leave("AutoWebProxyScriptEngine#" + ValidationHelper.HashString(this) + "::EnsureEngineAvailable", ValidationHelper.ToString(state));
                    return state;
                }

                // There's still an opportunity to fail over to the automaticConfigurationScript.
                if (!engineScriptLocation.Equals(automaticConfigurationScript))
                {
                    GlobalLog.Print("AutoWebProxyScriptEngine#" + ValidationHelper.HashString(this) + "::EnsureEngineAvailable() Update failed.  Falling back to automaticConfigurationScript:" + ValidationHelper.ToString(automaticConfigurationScript));
                    state = AutoWebProxyState.DiscoverySuccess;
                    state = DownloadAndCompile(automaticConfigurationScript, out newScriptInstance, ref syncStatus);
                    if (state == AutoWebProxyState.CompilationSuccess)
                    {
                        UpdateScriptInstance(newScriptInstance);
                        engineScriptLocation = automaticConfigurationScript;
                        GlobalLog.Leave("AutoWebProxyScriptEngine#" + ValidationHelper.HashString(this) + "::EnsureEngineAvailable", ValidationHelper.ToString(state));
                        return state;
                    }
                }
            }

            // Everything failed.  Set this instance to mostly-dead.  It will wake up again if there's a reg/connectoid change.
            GlobalLog.Print("AutoWebProxyScriptEngine#" + ValidationHelper.HashString(this) + "::EnsureEngineAvailable() All failed.");
            state = AutoWebProxyState.DiscoveryFailure;
            UpdateScriptInstance(null);
            engineScriptLocation = null;

            GlobalLog.Leave("AutoWebProxyScriptEngine#" + ValidationHelper.HashString(this) + "::EnsureEngineAvailable", ValidationHelper.ToString(state));
            return state;
        }

        void UpdateScriptInstance(AutoWebProxyScriptWrapper newScriptInstance) {
            GlobalLog.Print("AutoWebProxyScriptEngine#" + ValidationHelper.HashString(this) + "::UpdateScriptInstance() updating scriptInstance#" + ValidationHelper.HashString(scriptInstance) + " to newScriptInstance#" + ValidationHelper.HashString(newScriptInstance));

            if (scriptInstance == newScriptInstance)
            {
                return;
            }

            if (scriptInstance!=null) {
                GlobalLog.Print("AutoWebProxyScriptEngine#" + ValidationHelper.HashString(this) + "::UpdateScriptInstance() Closing engine.");
                scriptInstance.Close();
            }
            scriptInstance = newScriptInstance;
        }

        /// <devdoc>
        ///     <para>
        ///         Downloads and compiles the script from a given Uri.
        ///         This code can be called by config for a downloaded control, we need to assert.
        ///         This code is called holding the lock.
        ///     </para>
        /// </devdoc>
        private AutoWebProxyState DownloadAndCompile(Uri location, out AutoWebProxyScriptWrapper newScriptInstance, ref int syncStatus)
        {
            GlobalLog.Print("AutoWebProxyScriptEngine#" + ValidationHelper.HashString(this) + "::DownloadAndCompile() location:" + ValidationHelper.ToString(location));
            AutoWebProxyState newState = AutoWebProxyState.DownloadFailure;
            WebResponse response = null;
            TimerThread.Timer timer = null;
            newScriptInstance = null;

            // Can't assert this in declarative form (DCR?). This Assert() is needed to be able to create the request to download the proxy script.
            ExceptionHelper.WebPermissionUnrestricted.Assert();
            try {
                // here we have a reentrance issue due to config load.
                WebRequest request = WebRequest.Create(location);
                request.Timeout = Timeout.Infinite;
                request.CachePolicy = new RequestCachePolicy(RequestCacheLevel.Default);
                request.ConnectionGroupName = "__WebProxyScript";
                
                // We have an opportunity here, if caching is disabled AppDomain-wide, to override it with a
                // custom, trivial cache-provider to get a similar semantic.
                //
                // We also want to have a backup caching key in the case when IE has locked an expired script response
                //
                if (request.CacheProtocol != null)
                {
                    GlobalLog.Print("AutoWebProxyScriptEngine#" + ValidationHelper.HashString(this) + "::DownloadAndCompile() Using backup caching.");
                    request.CacheProtocol = new RequestCacheProtocol(backupCache, request.CacheProtocol.Validator);
                }

                HttpWebRequest httpWebRequest = request as HttpWebRequest;
                if (httpWebRequest!=null)
                {
                    httpWebRequest.Accept = "*/*";
                    httpWebRequest.UserAgent = this.GetType().FullName + "/" + Environment.Version;
                    httpWebRequest.KeepAlive = false;
                    httpWebRequest.Pipelined = false;
                    httpWebRequest.InternalConnectionGroup = true;
                }
                else
                {
                    FtpWebRequest ftpWebRequest = request as FtpWebRequest;
                    if (ftpWebRequest!=null)
                    {
                        ftpWebRequest.KeepAlive = false;
                    }
                }

                // Use no proxy, default cache - initiate the download.
                request.Proxy = null;
                request.Credentials = webProxy.Credentials;

                // Set this up with the abortable lock to abort this too.
                LockRequest(request, ref syncStatus);
                if (syncStatus != SyncStatus.RequestOwner)
                {
                    throw new WebException(NetRes.GetWebStatusString("net_requestaborted", WebExceptionStatus.RequestCanceled), WebExceptionStatus.RequestCanceled);
                }

                // Use our own timeout timer so that it can encompass the whole request, not just the headers.
                if (s_TimerQueue == null)
                {
                    s_TimerQueue = TimerThread.GetOrCreateQueue(SettingsSectionInternal.Section.DownloadTimeout);
                }
                timer = s_TimerQueue.CreateTimer(s_TimerCallback, request);
                response = request.GetResponse();

                // Check Last Modified.
                DateTime lastModified = DateTime.MinValue;
                HttpWebResponse httpResponse = response as HttpWebResponse;
                if (httpResponse != null)
                {
                    lastModified = httpResponse.LastModified;
                }
                else
                {
                    FtpWebResponse ftpResponse = response as FtpWebResponse;
                    if (ftpResponse != null)
                    {
                        lastModified = ftpResponse.LastModified;
                    }
                }
                GlobalLog.Print("AutoWebProxyScriptEngine#" + ValidationHelper.HashString(this) + "::DownloadAndCompile() lastModified:" + lastModified.ToString() + " (script):" + (scriptInstance == null ? "(null)" : scriptInstance.LastModified.ToString()));
                if (scriptInstance != null && lastModified != DateTime.MinValue && scriptInstance.LastModified == lastModified)
                {
                    newScriptInstance = scriptInstance;
                    newState = AutoWebProxyState.CompilationSuccess;
                }
                else
                {
                    string scriptBody = null;
                    byte[] scriptBuffer = null;
                    using (Stream responseStream = response.GetResponseStream())
                    {
                        SingleItemRequestCache.ReadOnlyStream ros = responseStream as SingleItemRequestCache.ReadOnlyStream;
                        if (ros != null)
                        {
                            scriptBuffer = ros.Buffer;
                        }
                        if (scriptInstance != null && scriptBuffer != null && scriptBuffer == scriptInstance.Buffer)
                        {
                            scriptInstance.LastModified = lastModified;
                            newScriptInstance = scriptInstance;
                            newState = AutoWebProxyState.CompilationSuccess;
                            GlobalLog.Print("AutoWebProxyScriptEngine#" + ValidationHelper.HashString(this) + "::DownloadAndCompile() Buffer matched - reusing engine.");
                        }
                        else
                        {
                            using (StreamReader streamReader = new StreamReader(responseStream))
                            {
                                scriptBody = streamReader.ReadToEnd();
                            }
                        }
                    }

                    WebResponse tempResponse = response;
                    response = null;
                    tempResponse.Close();
                    timer.Cancel();
                    timer = null;

                    if (newState != AutoWebProxyState.CompilationSuccess)
                    {
                        newState = AutoWebProxyState.DownloadSuccess;

                        GlobalLog.Print("AutoWebProxyScriptEngine#" + ValidationHelper.HashString(this) + "::DownloadAndCompile() IsFromCache:" + tempResponse.IsFromCache.ToString() + " scriptInstance:" + ValidationHelper.HashString(scriptInstance));
                        if (scriptInstance != null && scriptBody == scriptInstance.ScriptBody)
                        {
                            GlobalLog.Print("AutoWebProxyScriptEngine#" + ValidationHelper.HashString(this) + "::DownloadAndCompile() Script matched - using existing engine.");
                            scriptInstance.LastModified = lastModified;
                            if (scriptBuffer != null)
                            {
                                scriptInstance.Buffer = scriptBuffer;
                            }
                            newScriptInstance = scriptInstance;
                            newState = AutoWebProxyState.CompilationSuccess;
                        }
                        else
                        {
                            GlobalLog.Print("AutoWebProxyScriptEngine#" + ValidationHelper.HashString(this) + "::DownloadAndCompile() Creating AutoWebProxyScriptWrapper.");
                            newScriptInstance = new AutoWebProxyScriptWrapper();
                            newScriptInstance.LastModified = lastModified;
                            newState = newScriptInstance.Compile(location, scriptBody, scriptBuffer);
                        }
                    }
                }
            }
            catch (Exception exception)
            {
                if (NclUtilities.IsFatal(exception)) throw;
                if(Logging.On)Logging.PrintWarning(Logging.Web, SR.GetString(SR.net_log_proxy_script_download_compile_error, exception));
                GlobalLog.Print("AutoWebProxyScriptEngine#" + ValidationHelper.HashString(this) + "::DownloadAndCompile() Download() threw:" + ValidationHelper.ToString(exception));
            }
            finally
            {
                if (timer != null)
                {
                    timer.Cancel();
                }

                //                                                                                 
                try
                {
                    if (response != null)
                    {
                        response.Close();
                    }
                }
                finally
                {
                    WebPermission.RevertAssert();
                }
            }
            if (newState!=AutoWebProxyState.CompilationSuccess) {
                newScriptInstance = null;
            }
            GlobalLog.Print("AutoWebProxyScriptEngine#" + ValidationHelper.HashString(this) + "::DownloadAndCompile() retuning newState:" + ValidationHelper.ToString(newState));
            return newState;
        }

        // RequestTimeoutCallback - Called by the TimerThread to abort a request.  This just posts ThreadPool work item - Abort() does too
        // much to be done on the timer thread (timer thread should never block or call user code).
        private static void RequestTimeoutCallback(TimerThread.Timer timer, int timeNoticed, object context)
        {
            ThreadPool.UnsafeQueueUserWorkItem(s_AbortWrapper, context);
        }

        private static void AbortWrapper(object context)
        {
#if DEBUG
            GlobalLog.SetThreadSource(ThreadKinds.Worker);
            using (GlobalLog.SetThreadKind(ThreadKinds.System)) {
#endif
            ((WebRequest) context).Abort();
#if DEBUG
            }
#endif
        }

        private StringCollection ParseScriptReturn(string scriptReturn, Uri destination, bool returnFirstOnly) {
            if (scriptReturn == null)
            {
                return new StringCollection();
            }
            StringCollection proxies = new StringCollection();
            string[] proxyListStrings = scriptReturn.Split(splitChars);
            string proxyAuthority;
            foreach (string s in proxyListStrings)
            {
                string proxyString = s.Trim(' ');
                if (!proxyString.StartsWith("PROXY ", StringComparison.InvariantCultureIgnoreCase))
                {
                    if (string.Compare("DIRECT", proxyString, StringComparison.InvariantCultureIgnoreCase) == 0)
                    {
                        proxyAuthority = null;
                    }
                    else
                    {
                        continue;
                    }
                }
                else
                {
                    proxyAuthority = proxyString.Substring(6).TrimStart(' ');
                    Uri uri = null;
                    bool tryParse = Uri.TryCreate("http://" + proxyAuthority, UriKind.Absolute, out uri);
                    if (!tryParse || uri.UserInfo.Length>0 || uri.HostNameType==UriHostNameType.Basic || uri.AbsolutePath.Length!=1 || proxyAuthority[proxyAuthority.Length-1]=='/' || proxyAuthority[proxyAuthority.Length-1]=='#' || proxyAuthority[proxyAuthority.Length-1]=='?') {
                        continue;
                    }
                }
                proxies.Add(proxyAuthority);
                if (returnFirstOnly) {
                    break;
                }
            }
            return proxies;
        }

        internal void Close() {
        }


    }
}
