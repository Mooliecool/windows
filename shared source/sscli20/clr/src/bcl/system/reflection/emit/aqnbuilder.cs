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
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace System.Reflection.Emit
{
    // TypeNameBuilder is NOT thread safe NOR hardened for OOM NOR reliable
    internal class TypeNameBuilder
    {
        internal enum Format
        {
            ToString,
            FullName,
            AssemblyQualifiedName,
        }
        
        #region FCalls
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern IntPtr _CreateTypeNameBuilder();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void _ReleaseTypeNameBuilder(IntPtr pAQN);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void _OpenGenericArguments(IntPtr tnb);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void _CloseGenericArguments(IntPtr tnb);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void _OpenGenericArgument(IntPtr tnb);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void _CloseGenericArgument(IntPtr tnb);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void _AddName(IntPtr tnb, string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void _AddPointer(IntPtr tnb);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void _AddByRef(IntPtr tnb);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void _AddSzArray(IntPtr tnb);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void _AddArray(IntPtr tnb, int rank);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void _AddAssemblySpec(IntPtr tnb, string assemblySpec);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern string _ToString(IntPtr tnb);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void _Clear(IntPtr tnb);
        #endregion

        #region Static Members

        // TypeNameBuilder is NOT thread safe NOR hardened for OOM NOR reliable
        internal static string ToString(Type type, Format format)
        {
            if (format == Format.FullName || format == Format.AssemblyQualifiedName)
            {
                if (!type.IsGenericTypeDefinition && type.ContainsGenericParameters)
                    return null;
            }
            
            TypeNameBuilder tnb = new TypeNameBuilder(_CreateTypeNameBuilder());
            tnb.Clear();
            tnb.ConstructAssemblyQualifiedNameWorker(type, format);
            string toString = tnb.ToString();
            tnb.Dispose();
            return toString;
        }
        #endregion

        #region Private Data Members
        private IntPtr m_typeNameBuilder;
        #endregion

        #region Constructor
        private TypeNameBuilder(IntPtr typeNameBuilder) { m_typeNameBuilder = typeNameBuilder; }
        internal void Dispose() { _ReleaseTypeNameBuilder(m_typeNameBuilder); }
        #endregion

        #region private Members
        private void AddElementType(Type elementType)
        {
            if (elementType.HasElementType)
                AddElementType(elementType.GetElementType());
                
            if (elementType.IsPointer)
                AddPointer();

            else if (elementType.IsByRef)
                AddByRef();

            else if (elementType.IsSzArray)
                AddSzArray();

            else if (elementType.IsArray)
                AddArray(elementType.GetArrayRank());
        }
        
        private void ConstructAssemblyQualifiedNameWorker(Type type, Format format)
        {
            Type rootType = type;

            while (rootType.HasElementType)
                rootType = rootType.GetElementType();

            // Append namespace + nesting + name
            List<Type> nestings = new List<Type>();
            for (Type t = rootType; t != null; t = t.IsGenericParameter ? null : t.DeclaringType)
                nestings.Add(t);
            
            for (int i = nestings.Count - 1; i >= 0; i--)
            {
                Type enclosingType = nestings[i];
                string name = enclosingType.Name;

                if (i == nestings.Count - 1  && enclosingType.Namespace != null && enclosingType.Namespace.Length != 0)
                    name = enclosingType.Namespace + "." + name;

                AddName(name);
            }

            // Append generic arguments
            if (rootType.IsGenericType && (!rootType.IsGenericTypeDefinition || format == Format.ToString))
            {
                Type[] genericArguments = rootType.GetGenericArguments();

                OpenGenericArguments();
                for (int i = 0; i < genericArguments.Length; i++)
                {
                    Format genericArgumentsFormat = format == Format.FullName ? Format.AssemblyQualifiedName : format;
                   
                    OpenGenericArgument();
                    ConstructAssemblyQualifiedNameWorker(genericArguments[i], genericArgumentsFormat);
                    CloseGenericArgument();
                }
                CloseGenericArguments();
            }

            // Append pointer, byRef and array qualifiers
            AddElementType(type);

            if (format == Format.AssemblyQualifiedName)
                AddAssemblySpec(type.Module.Assembly.FullName);
        }
        
        private void OpenGenericArguments() { _OpenGenericArguments(m_typeNameBuilder); }
        private void CloseGenericArguments() { _CloseGenericArguments(m_typeNameBuilder); }
        private void OpenGenericArgument() { _OpenGenericArgument(m_typeNameBuilder); }
        private void CloseGenericArgument() { _CloseGenericArgument(m_typeNameBuilder); }
        private void AddName(string name) { _AddName(m_typeNameBuilder, name); }
        private void AddPointer() { _AddPointer(m_typeNameBuilder); }
        private void AddByRef() { _AddByRef(m_typeNameBuilder); }
        private void AddSzArray() { _AddSzArray(m_typeNameBuilder); }
        private void AddArray(int rank) { _AddArray(m_typeNameBuilder, rank); }
        private void AddAssemblySpec(string assemblySpec) { _AddAssemblySpec(m_typeNameBuilder, assemblySpec); }
        public override string ToString() { return _ToString(m_typeNameBuilder); }
        private void Clear() { _Clear(m_typeNameBuilder); }
        #endregion
    }
}
