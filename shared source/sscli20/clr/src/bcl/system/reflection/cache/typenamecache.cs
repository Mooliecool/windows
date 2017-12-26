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
** Class: TypeNameCache
**
**
** Purpose: Highly performant caching for type names.
**
**
============================================================
namespace System.Reflection.Cache {

    using System;
    using System.Reflection;

    internal class TypeNameCache {
        private static TypeNameCache m_cache = null;

        private TypeNameStruct[] m_data;

        internal const int CacheSize = 919;

        internal TypeNameCache() {
            BCLDebug.Trace("CACHE", "[TypeNameCache.ctor]Allocating a TypeName Cache with ", CacheSize, " elements.");
            m_data = new TypeNameStruct[CacheSize];
        }

        internal static int CalculateHash(IntPtr hashKey) {
            //We know that the bottom two bits of a 32-bit pointer are going to be zero, so
            //let's not include them in the value that we hash.
#if WIN32
            return unchecked((int)((((uint)(int)hashKey)>>2)%(uint)CacheSize));
#else
            return unchecked((int)((((ulong)(long)hashKey)>>3)%(ulong)CacheSize));
#endif
        }

        internal String GetTypeName(IntPtr hashKey) {
            int iPos = CalculateHash(hashKey);
            BCLDebug.Assert(iPos<CacheSize, "[TypeNameCache]iPos<CacheSize");
            BCLDebug.Assert(hashKey!=IntPtr.Zero, "[TypeNameCache]hashKey!=0");

            TypeNameStruct t;
            t.HashKey = m_data[iPos].HashKey;
            t.TypeName = m_data[iPos].TypeName;

            if (t.HashKey==hashKey && m_data[iPos].HashKey==t.HashKey) {
                BCLDebug.Trace("CACHE", "[TypeNameCache.GetTypeName]Hit. Hash key: ", hashKey, 
                               ". Value: ", t.TypeName);
                return t.TypeName;
            }
            BCLDebug.Trace("CACHE", "[TypeNameCache.GetTypeName]Miss. Hash key: ", hashKey);
            return null;
        }

        internal void AddValue(IntPtr hashKey, String typeName) {
            int iPos = CalculateHash(hashKey);
            BCLDebug.Assert(iPos<CacheSize, "[TypeNameCache]iPos<CacheSize");
            
            lock(typeof(TypeNameCache)) {
                m_data[iPos].HashKey = IntPtr.Zero; //Invalidate the bucket.
                m_data[iPos].TypeName = typeName;   //Set the type name.
                m_data[iPos].HashKey = hashKey;     //Revalidate the bucket.
            }

            BCLDebug.Trace("CACHE", "[TypeNameCache.AddValue]Added Item.  Hash key: ", hashKey, " Value: ", typeName);
        }

        internal static TypeNameCache GetCache() {
            if (m_cache==null) {
                lock (typeof(TypeNameCache)) {
                    if (m_cache==null) {
                        m_cache = new TypeNameCache();
                    }
                }
            }
            return m_cache;
        }
    }
}
*/









