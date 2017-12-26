// ==++==
// 
//   Copyright (c) Microsoft Corporation.  All rights reserved.
// 
// ==--==
using System;

namespace System.AddIn.Contract
{
    /// <summary>
    /// An interface for passing a collection of items across an appdomain boundary.
    /// </summary>
    public interface IListContract<T> : IContract
    {
        void Add(T item);
        void Clear();
        bool Contains(T item);
        int GetCount();
        IEnumeratorContract<T> GetEnumeratorContract();
        bool GetIsReadOnly();
        T GetItem(int index);
        int IndexOf(T item);
        void Insert(int index, T item);
        bool Remove(T item);
        void RemoveAt(int index);
        void SetItem(int index, T item);
    }
}

