//------------------------------------------------------------------------------
/// <copyright from='2004' to='2005' company='Microsoft Corporation'>
///    Copyright (c) Microsoft Corporation. All Rights Reserved.
///    Information Contained Herein is Proprietary and Confidential.
/// </copyright>
//------------------------------------------------------------------------------

using System;
using System.Diagnostics.CodeAnalysis;

namespace System.AddIn.Contract.Collections
{

    public interface IEnumerableContract<C> : IContract
        where C : IContract
    {
        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        IEnumeratorContract<C> GetEnumeratorContract();
    }

    public interface ICollectionContract<C> : IEnumerableContract<C>
        where C : IContract
    {
        void Add(C item);
        void Clear();
        bool Contains(C item);
        void CopyTo(C[] array, int arrayIndex);
        bool Remove(C item);

        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        int GetCount();
        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        bool GetIsReadOnly();
    }

    public interface IEnumeratorContract<C> : IContract
        where C : IContract
    {
        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        C GetCurrent();
        bool MoveNext();
        void Reset();
    }

    public interface IArrayContract<C> : IEnumerableContract<C>
        where C : IContract
    {
        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        C GetItem(int index);
        void SetItem(int index, C value);
        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        int GetCount();        
    }

    public interface IListContract<C> : ICollectionContract<C>
        where C : IContract
    {
        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        C GetItem(int index);
        void SetItem(int index, C value);

        int IndexOf(C item);
        void Insert(int index, C item);
        void RemoveAt(int index);
    }
}
