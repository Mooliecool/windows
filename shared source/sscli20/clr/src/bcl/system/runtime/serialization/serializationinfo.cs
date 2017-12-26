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
** Class:  SerializationInfo
**
**
** Purpose: The structure for holding all of the data needed
**          for object serialization and deserialization.
**
**
===========================================================*/
namespace System.Runtime.Serialization {
    
    using System;
    using System.Reflection;
    using System.Runtime.Remoting;
    using System.Runtime.Remoting.Proxies;
    using System.Globalization;

[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class SerializationInfo {
        private const int defaultSize = 4;
        
        internal String[]     m_members;
        internal Object[]     m_data;
        internal Type[]       m_types;
        internal String       m_fullTypeName;
        internal int          m_currMember;
        internal String       m_assemName;
        internal IFormatterConverter m_converter;
        
        [CLSCompliant(false)]
        public SerializationInfo(Type type, IFormatterConverter converter) {
            if (null==type) {
                throw new ArgumentNullException("type");
            }

            if (converter==null) {
                throw new ArgumentNullException("converter");
            }
            
            
            m_fullTypeName = type.FullName;
            m_assemName = type.Module.Assembly.FullName;
            BCLDebug.Assert(m_fullTypeName!=null, "[SerializationInfo.ctor]m_fullTypeName!=null");
            BCLDebug.Assert(m_assemName!=null, "[SerializationInfo.ctor]m_assemName!=null");
            
            m_members= new String[defaultSize];
            m_data   = new Object[defaultSize]; 
            m_types  = new Type[defaultSize];

            m_converter = converter;
            
            m_currMember = 0;

        }

        public String FullTypeName {
            get {
                return m_fullTypeName;
            }
            set {
                if (null==value) {
                    throw new ArgumentNullException("value");
                }
                m_fullTypeName = value;
            }
        }

        public String AssemblyName {
            get {
                return m_assemName;
            }

            set {
                if (null==value) {
                    throw new ArgumentNullException("value");
                }
                m_assemName = value;
            }
        }

        public void SetType(Type type) {
            if (type==null) {
                throw new ArgumentNullException("type");
            }
            m_fullTypeName = type.FullName;
            m_assemName = type.Module.Assembly.FullName;
            BCLDebug.Assert(m_fullTypeName!=null, "[SerializationInfo.ctor]m_fullTypeName!=null");
            BCLDebug.Assert(m_assemName!=null, "[SerializationInfo.ctor]m_assemName!=null");
        }


        public int MemberCount {
            get {
                return m_currMember;
            }
        }

        public SerializationInfoEnumerator GetEnumerator() {
            return new SerializationInfoEnumerator(m_members, m_data, m_types, m_currMember);
        }


        private void ExpandArrays() {
            int newSize;
            BCLDebug.Assert(m_members.Length == m_currMember, "[SerializationInfo.ExpandArrays]m_members.Length == m_currMember");

            newSize = (m_currMember * 2);

            //
            // In the pathological case, we may wrap
            //
            if (newSize<m_currMember) {
                if (Int32.MaxValue>m_currMember) {
                    newSize = Int32.MaxValue;
                }
            }

            //
            // Allocate more space and copy the data
            //
            String[] newMembers = new String[newSize];
            Object[] newData    = new Object[newSize];
            Type[]   newTypes   = new Type[newSize];

            Array.Copy(m_members, newMembers, m_currMember);
            Array.Copy(m_data, newData, m_currMember);
            Array.Copy(m_types, newTypes, m_currMember);

            //
            // Assign the new arrys back to the member vars.
            //
            m_members = newMembers;
            m_data    = newData;
            m_types   = newTypes;
        }

        public void AddValue(String name, Object value, Type type) {
            if (null==name) {
                throw new ArgumentNullException("name");
            }

            if (null==type) {
                throw new ArgumentNullException("type");
            }

            //
            // Walk until we find a member by the same name or until
            // we reach the end.  If we find a member by the same name,
            // throw.
            for (int i=0; i<m_currMember; i++) {
                if (m_members[i].Equals(name)) {
                    BCLDebug.Trace("SER", "[SerializationInfo.AddValue]Tried to add ", name, " twice to the SI.");
                    
                    throw new SerializationException(Environment.GetResourceString("Serialization_SameNameTwice"));
                }
            }
            
            AddValue(name, value, type, m_currMember);

        }

        public void AddValue(String name, Object value) {
            if (null==value) {
                AddValue(name, value, typeof(Object));
            } else {
                AddValue(name, value, value.GetType());
            }
        }

        public void AddValue(String name, bool value) {
            AddValue(name, (Object)value, typeof(bool));
        }

        public void AddValue(String name, char value) {
            AddValue(name, (Object)value, typeof(char));
        }
		

		[CLSCompliant(false)]
        public void AddValue(String name, sbyte value) {
            AddValue(name, (Object)value, typeof(sbyte));
        }

        public void AddValue(String name, byte value) {
            AddValue(name, (Object)value, typeof(byte));
        }

        public void AddValue(String name, short value) {
            AddValue(name, (Object)value, typeof(short));
        }

		[CLSCompliant(false)]
        public void AddValue(String name, ushort value) {
            AddValue(name, (Object)value, typeof(ushort));
        }

        public void AddValue(String name, int value) {
            AddValue(name, (Object)value, typeof(int));
        }

		[CLSCompliant(false)]
        public void AddValue(String name, uint value) {
            AddValue(name, (Object)value, typeof(uint));
        }

        public void AddValue(String name, long value) {
            AddValue(name, (Object)value, typeof(long));
        }

		[CLSCompliant(false)]
        public void AddValue(String name, ulong value) {
            AddValue(name, (Object)value, typeof(ulong));
        }

        public void AddValue(String name, float value) {
            AddValue(name, (Object)value, typeof(float));
        }

        public void AddValue(String name, double value) {
            AddValue(name, (Object)value, typeof(double));
        }

        public void AddValue(String name, decimal value) {
            AddValue(name, (Object)value, typeof(decimal));
        }

        public void AddValue(String name, DateTime value) {
            AddValue(name, (Object)value, typeof(DateTime));
        }

        internal void AddValue(String name, Object value, Type type, int index) {
            //
            // If we need to expand the arrays, do so.
            //
            if (index>=m_members.Length) {
                ExpandArrays();
            }
            
            //
            // Add the data and then advance the counter.
            //
            m_members[index] = name;
            m_data[index]    = value;
            m_types[index]   = type;
            m_currMember++;
        }

        /*=================================UpdateValue==================================
        **Action: Finds the value if it exists in the current data.  If it does, we replace
        **        the values, if not, we append it to the end.  This is useful to the 
        **        ObjectManager when it's performing fixups, but shouldn't be used by 
        **        clients.  Exposing out this functionality would allow children to overwrite
        **        their parent's values.
        **Returns: void
        **Arguments: name  -- the name of the data to be updated.
        **           value -- the new value.
        **           type  -- the type of the data being added.
        **Exceptions: None.  All error checking is done with asserts.
        ==============================================================================*/
        internal void UpdateValue(String name, Object value, Type type) {
            BCLDebug.Assert(null!=name,  "[SerializationInfo.UpdateValue]name!=null");
            BCLDebug.Assert(null!=value, "[SerializationInfo.UpdateValue]value!=null");
            BCLDebug.Assert(null!=type,  "[SerializationInfo.UpdateValue]type!=null");
            
            int index = FindElement(name);
            if (index<0) {
                AddValue(name, value, type, m_currMember);
            } else {
                m_members[index] = name;
                m_data[index]    = value;
                m_types[index]   = type;
            }

        }

        private int FindElement (String name) {
            if (null==name) {
                throw new ArgumentNullException("name");
            }
            BCLDebug.Trace("SER", "[SerializationInfo.FindElement]Looking for ", name, " CurrMember is: ", m_currMember);
            for (int i=0; i<m_currMember; i++) {
                BCLDebug.Assert(m_members[i]!=null, "[SerializationInfo.FindElement]Null Member in String array.");
                if (m_members[i].Equals(name)) {
                    return i;
                }
            }
            return -1;
        }

        /*==================================GetElement==================================
        **Action: Use FindElement to get the location of a particular member and then return
        **        the value of the element at that location.  The type of the member is
        **        returned in the foundType field.
        **Returns: The value of the element at the position associated with name.
        **Arguments: name -- the name of the element to find.
        **           foundType -- the type of the element associated with the given name.
        **Exceptions: None.  FindElement does null checking and throws for elements not 
        **            found.
        ==============================================================================*/
        private Object GetElement(String name, out Type foundType) {
            int index = FindElement(name);
            if (index==-1) {
                throw new SerializationException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("Serialization_NotFound"), name));
            }

            BCLDebug.Assert(index < m_data.Length, "[SerializationInfo.GetElement]index<m_data.Length");
            BCLDebug.Assert(index < m_types.Length,"[SerializationInfo.GetElement]index<m_types.Length");

            foundType = m_types[index];
            BCLDebug.Assert(foundType!=null, "[SerializationInfo.GetElement]foundType!=null");
            return m_data[index];
        }

[System.Runtime.InteropServices.ComVisible(true)]
        private Object GetElementNoThrow(String name, out Type foundType) {
            int index = FindElement(name);
            if (index==-1) {
                foundType = null;
                return null;
            }

            BCLDebug.Assert(index < m_data.Length, "[SerializationInfo.GetElement]index<m_data.Length");
            BCLDebug.Assert(index < m_types.Length,"[SerializationInfo.GetElement]index<m_types.Length");

            foundType = m_types[index];
            BCLDebug.Assert(foundType!=null, "[SerializationInfo.GetElement]foundType!=null");
            return m_data[index];
        }

        //
        // The user should call one of these getters to get the data back in the 
        // form requested.  
        //

        public Object   GetValue(String name, Type type) {
            Type foundType;
            Object value;
            
            if (null==type) {
                throw new ArgumentNullException("type");
            }

            value = GetElement(name, out foundType);
            if (RemotingServices.IsTransparentProxy(value)) {
                RealProxy proxy = RemotingServices.GetRealProxy(value);
                if (RemotingServices.ProxyCheckCast(proxy, type))
                    return value;
            } else if (foundType==type || type.IsAssignableFrom(foundType) || value==null) {
                return value;
            }

            BCLDebug.Assert(m_converter!=null, "[SerializationInfo.GetValue]m_converter!=null");

            return m_converter.Convert(value, type);
        }

[System.Runtime.InteropServices.ComVisible(true)]
        internal Object GetValueNoThrow(String name, Type type) {
            Type foundType;
            Object value;
            
            BCLDebug.Assert(null != type, "[SerializationInfo.GetValue]type ==null");

            value = GetElementNoThrow(name, out foundType);
            if (value == null) 
                return null;
            if (RemotingServices.IsTransparentProxy(value)) {
                RealProxy proxy = RemotingServices.GetRealProxy(value);
                if (RemotingServices.ProxyCheckCast(proxy, type))
                    return value;
            } else if (foundType==type || type.IsAssignableFrom(foundType) || value==null) {
                return value;
            }

            BCLDebug.Assert(m_converter!=null, "[SerializationInfo.GetValue]m_converter!=null");

            return m_converter.Convert(value, type);
        }

        public bool GetBoolean(String name) {
            Type foundType;
            Object value;
            
            value = GetElement(name, out foundType);
            if (foundType == typeof(bool)) {
                return (bool)value;
            }
            return m_converter.ToBoolean(value);
        }

        public char     GetChar(String name) {
            Type foundType;
            Object value;
            
            value = GetElement(name, out foundType);
            if (foundType == typeof(char)) {
                return (char)value;
            }
            return m_converter.ToChar(value);
        }

		[CLSCompliant(false)]
        public sbyte    GetSByte(String name) {
            Type foundType;
            Object value;
            
            value = GetElement(name, out foundType);
            if (foundType == typeof(sbyte)) {
                return (sbyte)value;
            }
            return m_converter.ToSByte(value);
        }

        public byte     GetByte(String name) {
            Type foundType;
            Object value;
            
            value = GetElement(name, out foundType);
            if (foundType == typeof(byte)) {
                return (byte)value;
            }
            return m_converter.ToByte(value);
        }

        public short    GetInt16(String name) {
            Type foundType;
            Object value;
            
            value = GetElement(name, out foundType);
            if (foundType == typeof(short)) {
                return (short)value;
            }
            return m_converter.ToInt16(value);
        }

		[CLSCompliant(false)]
        public ushort   GetUInt16(String name) {
            Type foundType;
            Object value;
            
            value = GetElement(name, out foundType);
            if (foundType == typeof(ushort)) {
                return (ushort)value;
            }
            return m_converter.ToUInt16(value);
        }

        public int      GetInt32(String name) {
            Type foundType;
            Object value;
            
            value = GetElement(name, out foundType);
            if (foundType == typeof(int)) {
                return (int)value;
            }
            return m_converter.ToInt32(value);
        }

		[CLSCompliant(false)]
        public uint     GetUInt32(String name) {
            Type foundType;
            Object value;
            
            value = GetElement(name, out foundType);
            if (foundType == typeof(uint)) {
                return (uint)value;
            }
            return m_converter.ToUInt32(value);
        }

        public long     GetInt64(String name) {
            Type foundType;
            Object value;
            
            value = GetElement(name, out foundType);
            if (foundType == typeof(long)) {
                return (long)value;
            }
            return m_converter.ToInt64(value);
        }

		[CLSCompliant(false)]
        public ulong    GetUInt64(String name) {
            Type foundType;
            Object value;
            
            value = GetElement(name, out foundType);
            if (foundType == typeof(ulong)) {
                return (ulong)value;
            }
            return m_converter.ToUInt64(value);
        }

        public float    GetSingle(String name) {
            Type foundType;
            Object value;
            
            value = GetElement(name, out foundType);
            if (foundType == typeof(float)) {
                return (float)value;
            }
            return m_converter.ToSingle(value);
        }


        public double   GetDouble(String name) {
            Type foundType;
            Object value;
            
            value = GetElement(name, out foundType);
            if (foundType == typeof(double)) {
                return (double)value;
            }
            return m_converter.ToDouble(value);
        }

        public decimal GetDecimal(String name) {
            Type foundType;
            Object value;
            
            value = GetElement(name, out foundType);
            if (foundType == typeof(decimal)) {
                return (decimal)value;
            }
            return m_converter.ToDecimal(value);
        }

        public DateTime GetDateTime(String name) {
            Type foundType;
            Object value;
            
            value = GetElement(name, out foundType);
            if (foundType == typeof(DateTime)) {
                return (DateTime)value;
            }
            return m_converter.ToDateTime(value);
        }

        public String   GetString(String name) {
            Type foundType;
            Object value;
            
            value = GetElement(name, out foundType);
            if (foundType == typeof(String) || value==null) {
                return (String)value;
            }
            return m_converter.ToString(value);
        }

        internal string[] MemberNames
        {
            get {
                return m_members;
                }

        }

        internal object[] MemberValues
        {
            get {
                return m_data;
                }
        }

    }
}
