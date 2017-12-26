//------------------------------------------------------------------------------
// <copyright file="_ConnectionGroup.cs" company="Microsoft">
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

namespace System.Net {
    using System.Collections;
    using System.Diagnostics;
    using System.Threading;

    //
    // ConnectionGroup groups a list of connections within the ServerPoint context,
    //   this used to keep context for things such as proxies or seperate clients.
    //
    internal class ConnectionGroup {

        //
        // Members
        //
        private const int DefaultConnectionListSize = 3;

        private ServicePoint        m_ServicePoint;
        private string              m_Name;
        private int                 m_ConnectionLimit;
        private ArrayList           m_ConnectionList;
        private object              m_Event;
        private Queue               m_AuthenticationRequestQueue;
        internal bool               m_AuthenticationGroup;
        private HttpAbortDelegate   m_AbortDelegate;
        private bool                m_NtlmNegGroup;
        private int                 m_IISVersion = -1;
        

        //
        // Constructors
        //
        internal ConnectionGroup(ServicePoint servicePoint, string connName) {
            m_ServicePoint      = servicePoint;
            m_ConnectionLimit   = servicePoint.ConnectionLimit;
            m_ConnectionList    = new ArrayList(DefaultConnectionListSize); //it may grow beyond
            m_Name              = MakeQueryStr(connName);
            m_AbortDelegate     = new HttpAbortDelegate(Abort);
            GlobalLog.Print("ConnectionGroup::.ctor m_ConnectionLimit:" + m_ConnectionLimit.ToString());
        }

        //
        // Accessors
        //
        internal ServicePoint ServicePoint {
            get {
                return m_ServicePoint;
            }
        }

        internal int CurrentConnections {
            get {
                return m_ConnectionList.Count;
            }
        }

        internal int ConnectionLimit {
            get {
                return m_ConnectionLimit;
            }
            set {
                m_ConnectionLimit = value;
                PruneExcesiveConnections();
                GlobalLog.Print("ConnectionGroup::ConnectionLimit.set m_ConnectionLimit:" + m_ConnectionLimit.ToString());
            }
        }

        private ManualResetEvent AsyncWaitHandle {
            get {
                if (m_Event == null) {
                    //
                    // lazy allocation of the event:
                    // if this property is never accessed this object is never created
                    //
                    Interlocked.CompareExchange(ref m_Event, new ManualResetEvent(false), null);
                }

                ManualResetEvent castedEvent = (ManualResetEvent)m_Event;

                return castedEvent;
            }
        }

        private Queue AuthenticationRequestQueue {
            get {
                if (m_AuthenticationRequestQueue == null) {
                    lock (m_ConnectionList) {
                        if (m_AuthenticationRequestQueue == null) {
                            m_AuthenticationRequestQueue = new Queue();
                        }
                    }
                }
                return m_AuthenticationRequestQueue;
            }
            set {
                m_AuthenticationRequestQueue = value;
            }
        }

        //
        // Methods
        //

        internal static string MakeQueryStr(string connName) {
            return ((connName == null) ? "" : connName);
        }


        /// <devdoc>
        ///    <para>
        ///       These methods are made available to the underlying Connection
        ///       object so that we don't leak them because we're keeping a local
        ///       reference in our m_ConnectionList.
        ///       Called by the Connection's constructor
        ///    </para>
        /// </devdoc>
        internal void Associate(Connection connection) {
            lock (m_ConnectionList) {
                m_ConnectionList.Add(connection);
            }
            GlobalLog.Print("ConnectionGroup::Associate() Connection:" + connection.GetHashCode());
        }



        /// <devdoc>
        ///    <para>
        ///       Used by the Connection's explicit finalizer (note this is
        ///       not a destructor, since that's never calld unless we
        ///       remove reference to the object from our internal list)
        ///    </para>
        /// </devdoc>
        internal void Disassociate(Connection connection) {
            lock (m_ConnectionList) {
                m_ConnectionList.Remove(connection);
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Called when a connection is idle and ready to process new requests
        ///    </para>
        /// </devdoc>
        internal void ConnectionGoneIdle() {
            if (m_AuthenticationGroup) {
                lock (m_ConnectionList) {
                    GlobalLog.Print("ConnectionGroup::ConnectionGoneIdle() setting the event");
                    AsyncWaitHandle.Set();
                }
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Causes an abort of any aborted requests waiting in the ConnectionGroup
        ///    </para>
        /// </devdoc>
        private bool Abort(HttpWebRequest request, WebException webException)
        {
            lock (m_ConnectionList)
            {
                AsyncWaitHandle.Set();
            }
            return true;
        }

        /// <devdoc>
        ///    <para>
        ///       Removes aborted requests from our queue.
        ///    </para>
        /// </devdoc>
        private void PruneAbortedRequests() {
            lock (m_ConnectionList) {
                Queue updatedQueue = new Queue();
                foreach(HttpWebRequest request in AuthenticationRequestQueue) {
                    if (!request.Aborted) {
                        updatedQueue.Enqueue(request);
                    }
                }
                AuthenticationRequestQueue = updatedQueue;
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Removes extra connections that are found when reducing the connection limit
        ///    </para>
        /// </devdoc>
        private void PruneExcesiveConnections() {
            lock(m_ConnectionList) {
                int connectionLimit = ConnectionLimit;
                if (CurrentConnections > connectionLimit) {
                    int numberToPrune = CurrentConnections - connectionLimit;
                    for (int i=0; i<numberToPrune; i++) {
                        ((Connection)m_ConnectionList[i]).CloseOnIdle();
                    }
                    m_ConnectionList.RemoveRange(0, numberToPrune);
                }
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Forces all connections on the ConnectionGroup to not be KeepAlive.
        ///    </para>
        /// </devdoc>
        internal void DisableKeepAliveOnConnections() {
            // The timer thread is allowed to call this.  (It doesn't call user code and doesn't block.)
            GlobalLog.ThreadContract(ThreadKinds.Unknown, ThreadKinds.SafeSources | ThreadKinds.Timer, "ConnectionGroup#" + ValidationHelper.HashString(this) + "::DisableKeepAliveOnConnections");

            lock (m_ConnectionList) {
                GlobalLog.Print("ConnectionGroup#" + ValidationHelper.HashString(this) + "::DisableKeepAliveOnConnections() Name = " + m_Name + ", Count:" + m_ConnectionList.Count);
                foreach (Connection currentConnection in m_ConnectionList) {
                    //
                    // For each Connection set KeepAlive to false
                    //
                    GlobalLog.Print("ConnectionGroup#" + ValidationHelper.HashString(this) + "::DisableKeepAliveOnConnections() setting KeepAlive to false Connection#" + ValidationHelper.HashString(currentConnection));
                    currentConnection.CloseOnIdle();
                }
                m_ConnectionList.Clear();
            }
        }



        private Connection FindMatchingConnection(HttpWebRequest request, string connName, out Connection leastbusyConnection) {
            int minBusyCount = Int32.MaxValue;
            bool freeConnectionsAvail = false;

            leastbusyConnection = null;

            lock (m_ConnectionList) {

                //
                // go through the list of open connections to this service point and pick
                // the first empty one or, if none is empty, pick the least busy one.
                //
                minBusyCount = Int32.MaxValue;
                foreach (Connection currentConnection in m_ConnectionList) {
                    GlobalLog.Print("ConnectionGroup::FindMatchingConnection currentConnection.BusyCount:" + currentConnection.BusyCount.ToString());

                    if (currentConnection.LockedRequest == request) {
                        leastbusyConnection = currentConnection;
                        return currentConnection;
                    }

                    GlobalLog.Print("ConnectionGroup::FindMatchingConnection: lockedRequest# " + ((currentConnection.LockedRequest == null) ? "null" : currentConnection.LockedRequest.GetHashCode().ToString()));
                    if (currentConnection.BusyCount < minBusyCount && currentConnection.LockedRequest == null) {
                        leastbusyConnection = currentConnection;
                        minBusyCount = currentConnection.BusyCount;
                        if (minBusyCount == 0) {
                            freeConnectionsAvail = true;
                        }
                    }
                }

                //
                // If there is NOT a Connection free, then we allocate a new Connection
                //
                if (!freeConnectionsAvail && CurrentConnections < ConnectionLimit) {
                    //
                    // If we can create a new connection, then do it,
                    // this may have complications in pipeling because
                    // we may wish to optimize this case by actually
                    // using existing connections, rather than creating new ones
                    //
                    // Note: this implicately results in a this.Associate being called.
                    //

                    GlobalLog.Print("ConnectionGroup::FindMatchingConnection [returning new Connection] CurrentConnections:" + CurrentConnections.ToString() + " ConnectionLimit:" + ConnectionLimit.ToString());
                    leastbusyConnection = new Connection(this);
                }
            }

            return null; // only if we have a locked Connection that matches can return non-null
        }

        private Connection FindConnectionAuthenticationGroup(HttpWebRequest request, string connName) {
            Connection leastBusyConnection = null;

            GlobalLog.Print("ConnectionGroup::FindConnectionAuthenticationGroup [" + connName + "] for request#" + request.GetHashCode() +", m_ConnectionList.Count:" + m_ConnectionList.Count.ToString());

            //
            // First try and find a free Connection (i.e. one not busy with Authentication handshake)
            //   or try to find a Request that has already locked a specific Connection,
            //   if a matching Connection is found, then we're done
            //
            lock (m_ConnectionList) {
                Connection matchingConnection;
                matchingConnection = FindMatchingConnection(request, connName, out leastBusyConnection);
                if (matchingConnection != null) {
                    return matchingConnection;
                }
                if (AuthenticationRequestQueue.Count == 0) {
                    if (leastBusyConnection != null) {
                        if (request.LockConnection) {
                            m_NtlmNegGroup = true;
                            m_IISVersion = leastBusyConnection.IISVersion;
                        }
                        if(request.LockConnection || (m_NtlmNegGroup && !request.Pipelined && request.UnsafeOrProxyAuthenticatedConnectionSharing && m_IISVersion >= 6)){
                            GlobalLog.Print("Assigning New Locked Request#" + request.GetHashCode().ToString());
                            leastBusyConnection.LockedRequest = request;
                        }
                        return leastBusyConnection;
                    }
                }
                else if (leastBusyConnection != null) {
                    AsyncWaitHandle.Set();
                }
                AuthenticationRequestQueue.Enqueue(request);
            }

            //
            // If all the Connections are busy, then we queue ourselves and need to wait.   As soon as
            //   one of the Connections are free, we grab the lock, and see if we find ourselves
            //   at the head of the queue.  If not, we loop backaround.
            //   Care is taken to examine the request when we wakeup, in case the request is aborted.
            //
            while (true) {
                GlobalLog.Print("waiting");
                request.AbortDelegate = m_AbortDelegate;

                if (!request.Aborted)
                    AsyncWaitHandle.WaitOne();

                GlobalLog.Print("wait up");
                lock(m_ConnectionList) {
                    if (request.Aborted)
                    {
                        PruneAbortedRequests();
                        // Note that request is not on any connection and it will not be submitted
                        return null;
                    }

                    FindMatchingConnection(request, connName, out leastBusyConnection);
                    if (AuthenticationRequestQueue.Peek() == request) {
                        GlobalLog.Print("dequeue");
                        AuthenticationRequestQueue.Dequeue();
                        if (leastBusyConnection != null) {
                            if (request.LockConnection) {
                                m_NtlmNegGroup = true;
                                m_IISVersion = leastBusyConnection.IISVersion;
                            }
                            if(request.LockConnection || (m_NtlmNegGroup  && !request.Pipelined && request.UnsafeOrProxyAuthenticatedConnectionSharing && m_IISVersion >= 6)){
                                leastBusyConnection.LockedRequest = request;
                            }
                            
                            return leastBusyConnection;
                        }
                        AuthenticationRequestQueue.Enqueue(request);
                    }
                    if (leastBusyConnection == null) {
                        AsyncWaitHandle.Reset();
                    }
                }
            }
        }

        /// <devdoc>
        ///    <para>
        ///       Used by the ServicePoint to find a free or new Connection
        ///       for use in making Requests.  Under NTLM and Negotiate requests,
        ///       this function depricates itself and switches the object over to
        ///       using a new code path (see FindConnectionAuthenticationGroup).
        ///    </para>
        /// </devdoc>
        internal Connection FindConnection(HttpWebRequest request, string connName) {
            Connection leastbusyConnection = null;
            Connection newConnection = null;
            bool freeConnectionsAvail = false;

            if (m_AuthenticationGroup || request.LockConnection) {
                m_AuthenticationGroup = true;
                return FindConnectionAuthenticationGroup(request, connName);
            }

            GlobalLog.Print("ConnectionGroup::FindConnection [" + connName + "] m_ConnectionList.Count:" + m_ConnectionList.Count.ToString());

            lock (m_ConnectionList) {

                //
                // go through the list of open connections to this service point and pick
                // the first empty one or, if none is empty, pick the least busy one.
                //
                int minBusyCount = Int32.MaxValue;
                foreach (Connection currentConnection in m_ConnectionList) {
                    GlobalLog.Print("ConnectionGroup::FindConnection currentConnection.BusyCount:" + currentConnection.BusyCount.ToString());
                    if (currentConnection.BusyCount < minBusyCount) {
                        leastbusyConnection = currentConnection;
                        minBusyCount = currentConnection.BusyCount;
                        if (minBusyCount == 0) {
                            freeConnectionsAvail = true;
                            break;
                        }
                    }
                }


                //
                // If there is NOT a Connection free, then we allocate a new Connection
                //
                if (!freeConnectionsAvail && CurrentConnections < ConnectionLimit) {
                    //
                    // If we can create a new connection, then do it,
                    // this may have complications in pipeling because
                    // we may wish to optimize this case by actually
                    // using existing connections, rather than creating new ones
                    //
                    // Note: this implicately results in a this.Associate being called.
                    //

                    GlobalLog.Print("ConnectionGroup::FindConnection [returning new Connection] freeConnectionsAvail:" + freeConnectionsAvail.ToString() + " CurrentConnections:" + CurrentConnections.ToString() + " ConnectionLimit:" + ConnectionLimit.ToString());
                    newConnection = new Connection(this);
                }
                else {
                    //
                    // All connections are busy, use the least busy one
                    //

                    GlobalLog.Print("ConnectionGroup::FindConnection [returning leastbusyConnection] freeConnectionsAvail:" + freeConnectionsAvail.ToString() + " CurrentConnections:" + CurrentConnections.ToString() + " ConnectionLimit:" + ConnectionLimit.ToString());
                    GlobalLog.Assert(leastbusyConnection != null, "Connect.leastbusyConnection != null|All connections have BusyCount equal to Int32.MaxValue.");

                    newConnection = leastbusyConnection;
                }
            }

            return newConnection;
        }


        [System.Diagnostics.Conditional("DEBUG")]
        internal void Debug(int requestHash) {
            foreach(Connection connection in  m_ConnectionList) {
                connection.Debug(requestHash);
            }
        }
    }
}
