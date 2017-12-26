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
** Class:   ValueType
**
**
** Purpose: Base class for all value classes.
**
**
===========================================================*/
namespace System {
    using System;
    using System.Reflection;
    using System.Runtime.CompilerServices;

    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public abstract class ValueType {

        public override bool Equals (Object obj) {
            BCLDebug.Perf(false, "ValueType::Equals is not fast.  "+this.GetType().FullName+" should override Equals(Object)");
            if (null==obj) {
                return false;
            }
            RuntimeType thisType = (RuntimeType)this.GetType();
            RuntimeType thatType = (RuntimeType)obj.GetType();

            if (thatType!=thisType) {
                return false;
            }

            Object thisObj = (Object)this;
            Object thisResult, thatResult;

            // if there are no GC references in this object we can avoid reflection 
            // and do a fast memcmp
            if (CanCompareBits(this))
                return FastEqualsCheck(thisObj, obj);

            FieldInfo[] thisFields = thisType.GetFields(BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic);
            
            for (int i=0; i<thisFields.Length; i++) {
                thisResult = ((RtFieldInfo)thisFields[i]).InternalGetValue(thisObj,false);
                thatResult = ((RtFieldInfo)thisFields[i]).InternalGetValue(obj, false);
                
                if (thisResult == null) {
                    if (thatResult != null)
                        return false;
                }
                else
                if (!thisResult.Equals(thatResult)) {
                    return false;
                }
            }

            return true;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern bool CanCompareBits(Object obj);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern bool FastEqualsCheck(Object a, Object b);

        /*=================================GetHashCode==================================
        **Action: Our algorithm for returning the hashcode is a little bit complex.  We look
        **        for the first non-static field and get it's hashcode.  If the type has no
        **        non-static fields, we return the hashcode of the type.  We can't take the
        **        hashcode of a static member because if that member is of the same type as
        **        the original type, we'll end up in an infinite loop.
        **Returns: The hashcode for the type.
        **Arguments: None.
        **Exceptions: None.
        ==============================================================================*/
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern override int GetHashCode();

        public override String ToString() {
            return this.GetType().ToString();
        }
    }
}
