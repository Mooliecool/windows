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
    public interface IRemoteArgumentEnumerableContract : IContract
    {
        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        IRemoteArgumentEnumeratorContract GetEnumeratorContract();
    }

    public interface IRemoteArgumentCollectionContract : IRemoteArgumentEnumerableContract
    {
        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        int GetCount();
    }

    public interface IRemoteArgumentEnumeratorContract : IContract
    {
        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        RemoteArgument GetCurrent();
        void Reset();
        bool MoveNext();
    }

    public interface IRemoteArgumentArrayContract : IRemoteArgumentCollectionContract
    {
        RemoteArgument GetItem(int index);
        void SetItem(int index, RemoteArgument value);
    }

    public interface IRemoteArgumentArrayListContract : IRemoteArgumentArrayContract
    {
        void Add(RemoteArgument newItem);
        void Clear();
        bool Contains(RemoteArgument item);
        int IndexOf(RemoteArgument item);
        void Insert(int index, RemoteArgument item);
        void Remove(RemoteArgument item);
        void RemoveAt(int index);
    }

    public interface IRemoteArgumentDictionaryContract : IRemoteArgumentCollectionContract
    {
        void Add(RemoteArgument key, RemoteArgument value);
        void Clear();
        bool ContainsKey(RemoteArgument key);
        bool Remove(RemoteArgument key);

        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        new IRemoteArgumentDictionaryEnumeratorContract GetEnumeratorContract();

        RemoteArgument GetItem(RemoteArgument key);
        void SetItem(RemoteArgument key, RemoteArgument value);
        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        IRemoteArgumentCollectionContract GetKeys();
        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        IRemoteArgumentCollectionContract GetValues();
    }

    public interface IRemoteArgumentDictionaryEnumeratorContract : IRemoteArgumentEnumeratorContract
    {
        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        RemoteArgumentDictionaryEntry GetEntry();

        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        RemoteArgument GetKey();

        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        RemoteArgument GetValue();
    }
}
