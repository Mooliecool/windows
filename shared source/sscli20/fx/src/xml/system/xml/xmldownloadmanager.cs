//------------------------------------------------------------------------------
// <copyright file="XmlDownloadManager.cs" company="Microsoft">
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

namespace System.Xml {

    using System;
    using System.IO;
    using System.Net;
    using System.Security;
    using System.Xml.Schema;
    using System.Collections;
    using System.Security.Policy;

//
// XmlDownloadManager
//
    internal class XmlDownloadManager {
        Hashtable connections;

        internal Stream GetStream( Uri uri, ICredentials credentials ) {
            if ( uri.Scheme == "file" ) {
                return new FileStream( uri.LocalPath, FileMode.Open, FileAccess.Read, FileShare.Read, 1 );
            }
            else {
                return GetNonFileStream( uri, credentials );
            }
        }

        private Stream GetNonFileStream( Uri uri, ICredentials credentials ) {
            WebRequest req = WebRequest.Create( uri );
            if ( credentials != null ) {
                req.Credentials = credentials;
            }
            WebResponse resp = req.GetResponse();
            HttpWebRequest webReq = req as HttpWebRequest;
            if ( webReq != null ) {
                lock ( this ) {
                    if ( connections == null ) {
                        connections = new Hashtable();
                    }
                    OpenedHost openedHost = (OpenedHost)connections[webReq.Address.Host];
                    if ( openedHost == null ) {
                        openedHost = new OpenedHost();
                    }

                    if ( openedHost.nonCachedConnectionsCount < webReq.ServicePoint.ConnectionLimit - 1 ) {
                        // we are not close to connection limit -> don't cache the stream
                        if ( openedHost.nonCachedConnectionsCount == 0 ) {
                            connections.Add( webReq.Address.Host, openedHost );
                        }
                        openedHost.nonCachedConnectionsCount++;
                        return new XmlRegisteredNonCachedStream( resp.GetResponseStream(), this, webReq.Address.Host );
                    }
                    else {
                        // cache the stream and save the connection for the next request
                        return new XmlCachedStream( resp.ResponseUri, resp.GetResponseStream() );
                    }
                }
            }
            else {
                return resp.GetResponseStream();
            }
        }

        internal void Remove( string host ) {
            lock ( this ) {
                OpenedHost openedHost = (OpenedHost)connections[host];
                if ( openedHost != null ) {
                    if ( --openedHost.nonCachedConnectionsCount == 0 ) {
                        connections.Remove( host );
                    }
                }
            }
        }

    }

//
// OpenedHost
//
    internal class OpenedHost {
        internal int nonCachedConnectionsCount;
    }

//
// XmlRegisteredNonCachedStream
//
    internal class XmlRegisteredNonCachedStream : Stream {
        protected Stream stream;
        XmlDownloadManager downloadManager;
        string host;

        internal XmlRegisteredNonCachedStream( Stream stream, XmlDownloadManager downloadManager, string host ) {
            this.stream = stream;
            this.downloadManager = downloadManager;
            this.host = host;
        }

        ~XmlRegisteredNonCachedStream() {
            if ( downloadManager != null ) {
                downloadManager.Remove( host );
            }
            stream = null;
            // The base class, Stream, provides its own finalizer
        } 

        protected override void Dispose( bool disposing ) {
            try {
                if ( disposing && stream != null ) {
                    if ( downloadManager != null ) {
                        downloadManager.Remove( host );
                    }
                    stream.Close();
                }
                stream = null;
                GC.SuppressFinalize( this ); // do not call finalizer
            }
            finally {
                base.Dispose( disposing );
            }
        }

        //
        // Stream
        //
        public override IAsyncResult BeginRead( byte[] buffer, int offset, int count, AsyncCallback callback, object state ) {
            return stream.BeginRead( buffer, offset, count, callback, state );
        }

        public override IAsyncResult BeginWrite( byte[] buffer, int offset, int count, AsyncCallback callback, object state ) {
            return BeginWrite( buffer, offset, count, callback, state );
        }

        public override int EndRead( IAsyncResult asyncResult ) {
            return stream.EndRead( asyncResult );
        }

        public override void EndWrite( IAsyncResult asyncResult ) {
            stream.EndWrite( asyncResult );
        }

        public override void Flush() {
            stream.Flush();
        }

        public override int Read( byte[] buffer, int offset, int count ) {
            return stream.Read( buffer, offset, count );
        }

        public override int ReadByte() {
            return stream.ReadByte();
        }

        public override long Seek( long offset, SeekOrigin origin ) {
            return stream.Seek( offset, origin );
        }

        public override void SetLength( long value ) {
            stream.SetLength( value );
        }

        public override void Write( byte[] buffer, int offset, int count ) {
            stream.Write( buffer, offset, count );
        }

        public override void WriteByte( byte value ) {
            stream.WriteByte( value );
        }

        public override Boolean CanRead {
            get { return stream.CanRead; }
        }

        public override Boolean CanSeek {
            get { return stream.CanSeek; }
        }

        public override Boolean CanWrite {
            get { return stream.CanWrite; }
        }

        public override long Length {
            get { return stream.Length; }
        }

        public override long Position {
            get { return stream.Position; }
            set { stream.Position = value; }
        }
    }

//
// XmlCachedStream
//
    internal class XmlCachedStream : MemoryStream {
        private const int MoveBufferSize = 4096;

        private Uri uri;

        internal XmlCachedStream( Uri uri, Stream stream ) 
            : base() {

            this.uri = uri;

            try {
                byte[] bytes = new byte[MoveBufferSize];
                int read = 0;
                while ( ( read = stream.Read( bytes, 0, MoveBufferSize ) ) > 0 ) {
                    this.Write( bytes, 0, read );
                }
                base.Position = 0;
            }
            finally {
                stream.Close();
            }
        }
    }
}
