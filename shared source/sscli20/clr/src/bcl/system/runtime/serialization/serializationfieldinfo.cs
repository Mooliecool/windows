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
** Class: SerializationFieldInfo
**
**
** Purpose: Provides a methods of representing imaginary fields
** which are unique to serialization.  In this case, what we're
** representing is the private members of parent classes.  We
** aggregate the RuntimeFieldInfo associated with this member 
** and return a managled form of the name.  The name that we
** return is .parentname.fieldname
**
**
============================================================*/
using System;
using System.Reflection;
using System.Globalization;

namespace System.Runtime.Serialization {

    internal sealed class SerializationFieldInfo : FieldInfo {

        internal static readonly char   FakeNameSeparatorChar   = '+';
        internal static readonly String FakeNameSeparatorString = "+";

        private RuntimeFieldInfo m_field;
        private String           m_serializationName;

        public override Module Module { get { return m_field.Module; } }
        public override int MetadataToken { get { return m_field.MetadataToken; } } 

        internal SerializationFieldInfo(RuntimeFieldInfo field, String namePrefix) {
            BCLDebug.Assert(field!=null,      "[SerializationFieldInfo.ctor]field!=null");
            BCLDebug.Assert(namePrefix!=null, "[SerializationFieldInfo.ctor]namePrefix!=null");
            
            m_field = field;
            m_serializationName = String.Concat(namePrefix, FakeNameSeparatorString, m_field.Name);
        }

        //
        // MemberInfo methods
        //
        public override String Name {
            get {
                return m_serializationName;
            }
        }

        public override Type DeclaringType {
            get {
                return m_field.DeclaringType;
            }
        }

        public override Type ReflectedType {
            get {
                return m_field.ReflectedType;
            }
        }

        public override Object[] GetCustomAttributes(bool inherit) {
            return m_field.GetCustomAttributes(inherit);
        }

        public override Object[] GetCustomAttributes(Type attributeType, bool inherit) {
            return m_field.GetCustomAttributes(attributeType, inherit);
        }

        public override bool IsDefined(Type attributeType, bool inherit) {
            return m_field.IsDefined(attributeType, inherit);
        }

        //
        // FieldInfo methods
        //
        public override Type FieldType {
            get {
                return m_field.FieldType;
            }
        }
        
        public override Object GetValue(Object obj) {
            return m_field.GetValue(obj);
        }

        internal Object InternalGetValue(Object obj, bool requiresAccessCheck) {
            RtFieldInfo field = m_field as RtFieldInfo;
            if (field != null)
                return field.InternalGetValue(obj, requiresAccessCheck);
            else
                return m_field.GetValue(obj);
        }

        public override void SetValue(Object obj, Object value, BindingFlags invokeAttr, Binder binder, CultureInfo culture) {
            m_field.SetValue(obj, value, invokeAttr, binder, culture);
        }

        internal void InternalSetValue(Object obj, Object value, BindingFlags invokeAttr, Binder binder, CultureInfo culture, bool requiresAccessCheck, bool isBinderDefault) {
            //m_field.InternalSetValue(obj, value, invokeAttr, binder, culture, requiresAccessCheck, isBinderDefault);
            RtFieldInfo field = m_field as RtFieldInfo;
            if (field != null)
                field.InternalSetValue(obj, value, invokeAttr, binder, culture, false);
            else
                m_field.SetValue(obj, value, invokeAttr, binder, culture);
        }

        internal RuntimeFieldInfo FieldInfo {
            get {
                return m_field;
            }
        }

        public override RuntimeFieldHandle FieldHandle {
            get {
                return m_field.FieldHandle;
            }
        }

        public override FieldAttributes Attributes {
            get {
                return m_field.Attributes;
            }
        }

    }
}
