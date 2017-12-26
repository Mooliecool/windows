/// ==++==
// 
//   Copyright (c) Microsoft Corporation.  All rights reserved.
// 
// ==--==

using System;

namespace System.AddIn.Contract
{
    /// <summary>
    /// An interface for iterating over a collection across an appdomain boundary.
    /// </summary>
    public interface IEnumeratorContract<T> : IDisposable
    {
        // Note there are no properties here because Windows Communications Foundation doesn't
        // support them, and we may use WCF in the future.
        T GetCurrent();
        bool MoveNext();
        void Reset();
    }
}

