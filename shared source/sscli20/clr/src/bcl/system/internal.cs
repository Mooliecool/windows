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
** This file exists to contain miscellaneous module-level attributes
** and other miscellaneous stuff.
**
**
** 
===========================================================*/
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using System.Collections.Generic;
using System.Reflection;


[assembly:DefaultDependencyAttribute(LoadHint.Always)]
// mscorlib would like to have its literal strings frozen if possible
[assembly: System.Runtime.CompilerServices.StringFreezingAttribute()]

namespace System
{
    static class Internal
    {
        // This method is purely an aid for NGen to statically deduce which
        // instantiations to save in the ngen image.
        // Otherwise, the JIT-compiler gets used, which is bad for working-set.
        // Note that IBC can provide this information too.
        // However, this helps in keeping the JIT-compiler out even for
        // test scenarios which do not use IBC.
        // This can be removed after V2, when we implement other schemes
        // of keeping the JIT-compiler out for generic instantiations.

        static void CommonlyUsedGenericInstantiations_HACK()
        {
            // Make absolutely sure we include some of the most common 
            // instantiations here in mscorlib's ngen image.
            // Note that reference type instantiations are already included
            // automatically for us.

            new ArraySegment<byte>(new byte[1], 0, 0);

            new Dictionary<Char, Object>();
            new Dictionary<Int16, IntPtr>();
            new Dictionary<Int32, Byte>();
            new Dictionary<Int32, Int32>();
            new Dictionary<Int32, Object>();
            new Dictionary<IntPtr, Int16>();
            new Dictionary<Object, Char>();
            new Dictionary<Object, Guid>();
            new Dictionary<Object, Int32>();

            NullableHelper_HACK<Boolean>();
            NullableHelper_HACK<Byte>();
            NullableHelper_HACK<Char>();
            NullableHelper_HACK<DateTime>(); 
            NullableHelper_HACK<Decimal>(); 
            NullableHelper_HACK<Double>();
            NullableHelper_HACK<Guid>();
            NullableHelper_HACK<Int16>();
            NullableHelper_HACK<Int32>();
            NullableHelper_HACK<Int64>();
            NullableHelper_HACK<Single>();
            NullableHelper_HACK<TimeSpan>();

            new List<Boolean>();
            new List<Byte>();
            new List<DateTime>();
            new List<Decimal>();
            new List<Double>();
            new List<Guid>();
            new List<Int16>();
            new List<Int32>();
            new List<Int64>();
            new List<TimeSpan>();
            new List<SByte>();
            new List<Single>();
            new List<UInt16>();
            new List<UInt32>();
            new List<UInt64>();
            new List<KeyValuePair<Object, Object>>();

            RuntimeType.RuntimeTypeCache.Prejitinit_HACK();

            new CerArrayList<RuntimeMethodInfo>(0);
            new CerArrayList<RuntimeConstructorInfo>(0);
            new CerArrayList<RuntimePropertyInfo>(0);
            new CerArrayList<RuntimeEventInfo>(0);
            new CerArrayList<RuntimeFieldInfo>(0);
            new CerArrayList<RuntimeType>(0);

            new KeyValuePair<Char, UInt16>('\0', UInt16.MinValue);
            new KeyValuePair<UInt16, Double>(UInt16.MinValue, Double.MinValue);


            SZArrayHelper_HACK<Boolean>(null);
            SZArrayHelper_HACK<Byte>(null);
            SZArrayHelper_HACK<DateTime>(null);
            SZArrayHelper_HACK<Decimal>(null);
            SZArrayHelper_HACK<Double>(null);
            SZArrayHelper_HACK<Guid>(null);
            SZArrayHelper_HACK<Int16>(null);
            SZArrayHelper_HACK<Int32>(null);
            SZArrayHelper_HACK<Int64>(null);
            SZArrayHelper_HACK<TimeSpan>(null);
            SZArrayHelper_HACK<SByte>(null);
            SZArrayHelper_HACK<Single>(null);
            SZArrayHelper_HACK<UInt16>(null);
            SZArrayHelper_HACK<UInt32>(null);
            SZArrayHelper_HACK<UInt64>(null);

            SZArrayHelper_HACK<CustomAttributeTypedArgument>(null);
            SZArrayHelper_HACK<CustomAttributeNamedArgument>(null);
        }

        static T NullableHelper_HACK<T>() where T : struct
        {
            Nullable.Compare<T>(null, null);    
            Nullable.Equals<T>(null, null); 
            Nullable<T> nullable = new Nullable<T>();
            return nullable.GetValueOrDefault();
        }       

        static void SZArrayHelper_HACK<T>(SZArrayHelper oSZArrayHelper)
        {
            // Instantiate common methods for IList implementation on Array
            oSZArrayHelper.get_Count<T>();
            oSZArrayHelper.get_Item<T>(0);
            oSZArrayHelper.GetEnumerator<T>();
        }
    }
}
