// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==

using System;
using System.Runtime.Remoting;
using System.Runtime.Serialization;
using System.Security.Permissions;

namespace System.Reflection 
{
    // This is not serializable because it is a reflection command.
    [Serializable()]
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class Missing: ISerializable
    {
    	public static readonly Missing Value = new Missing();

        #region Constructor
        private Missing() { }
        #endregion

        #region ISerializable
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.SerializationFormatter)]
        void ISerializable.GetObjectData(SerializationInfo info, StreamingContext context)
        {
            if (info == null)
                throw new ArgumentNullException("info");

            UnitySerializationHolder.GetUnitySerializationInfo(info, this);
        }
        #endregion
    }
}
