//---------------------------------------------------------------------------
//
// <copyright file="ReaderWriterLockWrapper.cs" company="Microsoft">
//    Copyright (C) Microsoft Corporation.  All rights reserved.
// </copyright>
//
//
// Description:
// Wrapper that allows a ReaderWriterLock to work with C#'s using() clause
//
// History:
//  07/23/2003 : BrendanM Ported to WCP
//
//---------------------------------------------------------------------------


using System;
using System.Threading;
using MS.Internal.WindowsBase;

namespace MS.Internal
{
    // Wrapper that allows a ReaderWriterLock to work with C#'s using() clause
    [FriendAccessAllowed] // Built into Base, used by Core and Framework.
    internal class ReaderWriterLockWrapper
    {
        //------------------------------------------------------
        //
        //  Constructors
        //
        //------------------------------------------------------

        #region Constructors

        internal ReaderWriterLockWrapper()
        {
            _rwLock = new ReaderWriterLock();
            _awr = new AutoWriterRelease(_rwLock);
            _arr = new AutoReaderRelease(_rwLock);
        }

        #endregion Constructors

        //------------------------------------------------------
        //
        //  Internal Properties
        //
        //------------------------------------------------------

        #region Internal Properties

        internal IDisposable WriteLock
        {
            get
            {
                _rwLock.AcquireWriterLock(Timeout.Infinite);
                return _awr;
            }
        }

        internal IDisposable ReadLock
        {
            get
            {
                _rwLock.AcquireReaderLock(Timeout.Infinite);
                return _arr;
            }
        }

        #endregion Internal Properties

        //------------------------------------------------------
        //
        //  Private Fields
        //
        //------------------------------------------------------

        #region Private Fields

        private ReaderWriterLock _rwLock;
        private AutoReaderRelease _arr;
        private AutoWriterRelease _awr;

        #endregion Private Fields

        //------------------------------------------------------
        //
        //  Private Classes & Structs
        //
        //------------------------------------------------------

        #region Private Classes & Structs

        private struct AutoWriterRelease : IDisposable
        {
            public AutoWriterRelease(ReaderWriterLock rwLock)
            {
                _lock = rwLock;
            }

            public void Dispose()
            {
                _lock.ReleaseWriterLock();
            }

            private ReaderWriterLock _lock;
        }

        private struct AutoReaderRelease : IDisposable
        {
            public AutoReaderRelease(ReaderWriterLock rwLock)
            {
                _lock = rwLock;
            }

            public void Dispose()
            {
                _lock.ReleaseReaderLock();
            }

            private ReaderWriterLock _lock;
        }
        #endregion Private Classes
    }
}




