/****************************** Module Header ******************************\
Module Name:  ServerActivatedObjectProxy.cs
Project:      CSRemotingClient
Copyright (c) Microsoft Corporation.

ServerActivatedObjectProxy.cs defines the proxy of the server-activated 
types. The proxy has no implementation of the types' methods and properties.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;


namespace RemotingShared
{
    /// <summary>
    /// The proxy of the server-activated type for .NET Remoting.
    /// </summary>
    internal class ServerActivatedObject : MarshalByRefObject
    {
        public float FloatProperty
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        public virtual string GetRemoteObjectType()
        {
            throw new NotImplementedException();
        }

        public void GetProcessThreadID(out uint processId, out uint threadId)
        {
            throw new NotImplementedException();
        }
    }


    /// <summary>
    /// The proxy of the SingleCall server-activated type for .NET Remoting.
    /// </summary>
    internal class SingleCallObject : ServerActivatedObject
    {
    }


    /// <summary>
    /// The proxy of the Singleton server-activated type for .NET Remoting.
    /// </summary>
    internal class SingletonObject : ServerActivatedObject
    {
    }
}
