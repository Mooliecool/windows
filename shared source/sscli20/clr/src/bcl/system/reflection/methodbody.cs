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
using System.Globalization;
using System.Collections.Generic;

namespace System.Reflection
{   
    [Flags()]
    [System.Runtime.InteropServices.ComVisible(true)]
    public enum ExceptionHandlingClauseOptions: int
    {
        Clause = 0x0,
        Filter = 0x1,
        Finally = 0x2,
        Fault = 0x4,
    }
    
    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class ExceptionHandlingClause
    {
        #region Private costructor
        // This class can only be created from inside the EE.
        private ExceptionHandlingClause() { }
        #endregion
        
        #region Private Data Members
        private MethodBody m_methodBody;
        private ExceptionHandlingClauseOptions m_flags;
        private int m_tryOffset;
        private int m_tryLength;
        private int m_handlerOffset;
        private int m_handlerLength;
        private int m_catchMetadataToken;
        private int m_filterOffset;
        #endregion

        #region Public Members
        public ExceptionHandlingClauseOptions Flags { get { return m_flags; } }
        public int TryOffset { get { return m_tryOffset; } }
        public int TryLength { get { return m_tryLength; } }
        public int HandlerOffset { get { return m_handlerOffset; } }
        public int HandlerLength { get { return m_handlerLength; } }
        
        public int FilterOffset 
        { 
            get 
            { 
                if (m_flags != ExceptionHandlingClauseOptions.Filter)
                    throw new InvalidOperationException(Environment.GetResourceString("Arg_EHClauseNotFilter"));
            
                return m_filterOffset; 
            } 
        }
        
        public Type CatchType
        { 
            get 
            {
                if (m_flags != ExceptionHandlingClauseOptions.Clause)
                    throw new InvalidOperationException(Environment.GetResourceString("Arg_EHClauseNotClause"));
            
                Type type = null;

                if (!MetadataToken.IsNullToken(m_catchMetadataToken))
                {
                    Type declaringType = m_methodBody.m_methodBase.DeclaringType;
                    Module module = (declaringType == null) ? m_methodBody.m_methodBase.Module : declaringType.Module;
                    type = module.ResolveType(m_catchMetadataToken, (declaringType == null) ? null : declaringType.GetGenericArguments(), 
                        m_methodBody.m_methodBase is MethodInfo ? m_methodBody.m_methodBase.GetGenericArguments() : null);
                }

                return type;
            }
        }        
        #endregion     
        
        #region Object Overrides
        public override string ToString()
        {
            if (Flags == ExceptionHandlingClauseOptions.Clause)
            {
                return String.Format(CultureInfo.CurrentUICulture, 
                    "Flags={0}, TryOffset={1}, TryLength={2}, HandlerOffset={3}, HandlerLength={4}, CatchType={5}",
                    Flags, TryOffset, TryLength, HandlerOffset, HandlerLength, CatchType);
            }
            
            if (Flags == ExceptionHandlingClauseOptions.Filter)
            {
                return String.Format(CultureInfo.CurrentUICulture, 
                    "Flags={0}, TryOffset={1}, TryLength={2}, HandlerOffset={3}, HandlerLength={4}, FilterOffset={5}",
                    Flags, TryOffset, TryLength, HandlerOffset, HandlerLength, FilterOffset);
            }
            
            return String.Format(CultureInfo.CurrentUICulture, 
                "Flags={0}, TryOffset={1}, TryLength={2}, HandlerOffset={3}, HandlerLength={4}",
                Flags, TryOffset, TryLength, HandlerOffset, HandlerLength);
            
        }
        #endregion
    }
    
    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class MethodBody
    {
        #region Private costructor
        // This class can only be created from inside the EE.
        private MethodBody() { }
        #endregion
        
        #region Private Data Members
        private byte[] m_IL;
        private ExceptionHandlingClause[] m_exceptionHandlingClauses;
        private LocalVariableInfo[] m_localVariables;
        internal MethodBase m_methodBase;
        private int m_localSignatureMetadataToken;
        private int m_maxStackSize;
        private bool m_initLocals;   
        #endregion

        #region Public Members
        public int LocalSignatureMetadataToken { get { return m_localSignatureMetadataToken; } }
        public IList<LocalVariableInfo> LocalVariables { get { return Array.AsReadOnly(m_localVariables); } }
        public int MaxStackSize { get { return m_maxStackSize; } }
        public bool InitLocals { get { return m_initLocals; } }       
        public byte[] GetILAsByteArray() { return m_IL; }
        public IList<ExceptionHandlingClause> ExceptionHandlingClauses { get { return Array.AsReadOnly(m_exceptionHandlingClauses); } } 
        #endregion
    }   

    [System.Runtime.InteropServices.ComVisible(true)]
    public class LocalVariableInfo
    {
        #region Private Data Members
        private int m_isPinned;
        private int m_localIndex;
        private RuntimeTypeHandle m_typeHandle;
        #endregion

        #region Constructor
        internal LocalVariableInfo() { }
        #endregion

        #region Object Overrides
        public override string ToString()
        {
            string toString = LocalType.ToString() + " (" + LocalIndex + ")";
            
            if (IsPinned)
                toString += " (pinned)";

            return toString;
        }
        #endregion

        #region Public Members
        public virtual Type LocalType { get { BCLDebug.Assert(m_typeHandle != null, "type handle must be set!");  return m_typeHandle.GetRuntimeType(); } }
        public virtual bool IsPinned { get { return m_isPinned != 0; } }
        public virtual int LocalIndex { get { return m_localIndex; } }
        #endregion
    }
}

