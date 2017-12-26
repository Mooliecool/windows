//------------------------------------------------------------------------------
/// <copyright from='2004' to='2005' company='Microsoft Corporation'>
///    Copyright (c) Microsoft Corporation. All Rights Reserved.
///    Information Contained Herein is Proprietary and Confidential.
/// </copyright>
//------------------------------------------------------------------------------

using System;
using System.AddIn.Contract.Collections;
using System.Diagnostics.CodeAnalysis;
using System.Runtime.Serialization;

namespace System.AddIn.Contract
{
    public interface ISerializableObjectContract : IRemoteArgumentDictionaryContract
    {
        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        string GetCanonicalName();

        [SuppressMessage("Microsoft.Design", "CA1024:UsePropertiesWhereAppropriate",
            Justification = "Indigo does not support properties")]
        SerializableObjectData GetSerializableObjectData();
    }
}
