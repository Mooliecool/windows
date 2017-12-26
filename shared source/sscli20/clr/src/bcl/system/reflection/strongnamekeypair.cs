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
/*============================================================
**
** File:    StrongNameKeyPair.cs
**
**
** Purpose: Encapsulate access to a public/private key pair
**          used to sign strong name assemblies.
**
**
===========================================================*/
namespace System.Reflection
{

    using System;
    using System.IO;
    using System.Runtime.CompilerServices;
    using System.Runtime.Serialization;
    using System.Security.Permissions;

    [Serializable()]
[System.Runtime.InteropServices.ComVisible(true)]
    public class StrongNameKeyPair : IDeserializationCallback, ISerializable 
    {
        private bool    _keyPairExported;
        private byte[]  _keyPairArray;
        private String  _keyPairContainer;
        private byte[]  _publicKey;

        // Build key pair from file.
        [SecurityPermissionAttribute(SecurityAction.Demand, Flags=SecurityPermissionFlag.UnmanagedCode)]
        public StrongNameKeyPair(FileStream keyPairFile)
        {
            if (keyPairFile == null)
                throw new ArgumentNullException("keyPairFile");

            int length = (int)keyPairFile.Length;
            _keyPairArray = new byte[length];
            keyPairFile.Read(_keyPairArray, 0, length);

            _keyPairExported = true;
        }

        // Build key pair from byte array in memory.
        [SecurityPermissionAttribute(SecurityAction.Demand, Flags=SecurityPermissionFlag.UnmanagedCode)]
        public StrongNameKeyPair(byte[] keyPairArray)
        {
            if (keyPairArray == null)
                throw new ArgumentNullException("keyPairArray");

            _keyPairArray = new byte[keyPairArray.Length];
            Array.Copy(keyPairArray, _keyPairArray, keyPairArray.Length);

            _keyPairExported = true;
        }

        // Reference key pair in named key container.
        [SecurityPermissionAttribute(SecurityAction.Demand, Flags=SecurityPermissionFlag.UnmanagedCode)]
        public StrongNameKeyPair(String keyPairContainer)
        {
            if (keyPairContainer == null)
                throw new ArgumentNullException("keyPairContainer");

            _keyPairContainer = keyPairContainer;

            _keyPairExported = false;
        }

        [SecurityPermissionAttribute(SecurityAction.Demand, Flags=SecurityPermissionFlag.UnmanagedCode)]
        protected StrongNameKeyPair (SerializationInfo info, StreamingContext context) {
            _keyPairExported = (bool) info.GetValue("_keyPairExported", typeof(bool));
            _keyPairArray = (byte[]) info.GetValue("_keyPairArray", typeof(byte[]));
            _keyPairContainer = (string) info.GetValue("_keyPairContainer", typeof(string));
            _publicKey = (byte[]) info.GetValue("_publicKey", typeof(byte[]));
        }

        // Get the public portion of the key pair.
        public byte[] PublicKey
        {
            get
            {
                if (_publicKey == null)
                {
                    _publicKey = nGetPublicKey(_keyPairExported, _keyPairArray, _keyPairContainer);
                }

                byte[] publicKey = new byte[_publicKey.Length];
                Array.Copy(_publicKey, publicKey, _publicKey.Length);

                return publicKey;
            }
        }

        /// <internalonly/>
        void ISerializable.GetObjectData (SerializationInfo info, StreamingContext context) {
            info.AddValue("_keyPairExported", _keyPairExported);
            info.AddValue("_keyPairArray", _keyPairArray);
            info.AddValue("_keyPairContainer", _keyPairContainer);
            info.AddValue("_publicKey", _publicKey);
        }

        /// <internalonly/>
        void IDeserializationCallback.OnDeserialization (Object sender) {}

        // Internal routine used to retrieve key pair info from unmanaged code.
        private bool GetKeyPair(out Object arrayOrContainer)
        {
            arrayOrContainer = _keyPairExported ? (Object)_keyPairArray : (Object)_keyPairContainer;
            return _keyPairExported;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern byte[] nGetPublicKey(bool exported, byte[] array, String container);
    }
}
