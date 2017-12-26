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
** Class:  PropertyBuilder
**
**
** Propertybuilder is for client to define properties for a class
**
** 
===========================================================*/
namespace System.Reflection.Emit {
    
	using System;
	using System.Reflection;
	using CultureInfo = System.Globalization.CultureInfo;
    using System.Security.Permissions;
    using System.Runtime.InteropServices;

    // 
    // A PropertyBuilder is always associated with a TypeBuilder.  The TypeBuilder.DefineProperty
    // method will return a new PropertyBuilder to a client.
    // 
    [HostProtection(MayLeakOnAbort = true)]
    [ClassInterface(ClassInterfaceType.None)]
    [ComDefaultInterface(typeof(_PropertyBuilder))]
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class PropertyBuilder : PropertyInfo, _PropertyBuilder
    { 
    
    	// Make a private constructor so these cannot be constructed externally.
        private PropertyBuilder() {}
    	
        // Constructs a PropertyBuilder.  
    	//
        internal PropertyBuilder(
    		Module			mod,					// the module containing this PropertyBuilder
    		String			name,					// property name
    		SignatureHelper	sig,					// property signature descriptor info
    		PropertyAttributes	attr,				// property attribute such as DefaultProperty, Bindable, DisplayBind, etc
			Type			returnType,				// return type of the property.
    		PropertyToken	prToken,				// the metadata token for this property
            TypeBuilder     containingType)         // the containing type
        {
            if (name == null)
                throw new ArgumentNullException("name");
			if (name.Length == 0)
				throw new ArgumentException(Environment.GetResourceString("Argument_EmptyName"), "name");
            if (name[0] == '\0')
                throw new ArgumentException(Environment.GetResourceString("Argument_IllegalName"), "name");
    		
            m_name = name;
            m_module = mod;
            m_signature = sig;
            m_attributes = attr;
			m_returnType = returnType;
    		m_prToken = prToken;
            m_tkProperty = prToken.Token;
            m_getMethod = null;
            m_setMethod = null;
            m_containingType = containingType;
        }
    
        //************************************************
        // Set the default value of the Property
        //************************************************
        public void SetConstant(Object defaultValue) 
        {
            m_containingType.ThrowIfCreated();
        
            TypeBuilder.SetConstantValue(
                m_module,
                m_prToken.Token,
				m_returnType,
                defaultValue);
        }
        

    	// Return the Token for this property within the TypeBuilder that the
    	// property is defined within.
        public PropertyToken PropertyToken
        {
    		get {return m_prToken;}
        }

        internal override int MetadataTokenInternal
        {
            get 
            {
                return m_tkProperty;
            }
        }
        
        public override Module Module
        {
            get 
            {
                return m_containingType.Module;
            }
        }
    
    	 
    	public void SetGetMethod(MethodBuilder mdBuilder)
    	{
            if (mdBuilder == null)
            {
    			throw new ArgumentNullException("mdBuilder");
            }
            m_containingType.ThrowIfCreated();
    		TypeBuilder.InternalDefineMethodSemantics(
    			m_module,
    			m_prToken.Token,
    			MethodSemanticsAttributes.Getter, 
    			mdBuilder.GetToken().Token);
            m_getMethod = mdBuilder;
    	}
    
    
    	public void SetSetMethod(MethodBuilder mdBuilder)
    	{
            if (mdBuilder == null)
            {
    			throw new ArgumentNullException("mdBuilder");
            }
            m_containingType.ThrowIfCreated();
    		TypeBuilder.InternalDefineMethodSemantics(
    			m_module,
    			m_prToken.Token,
    			MethodSemanticsAttributes.Setter, 
    			mdBuilder.GetToken().Token);
            m_setMethod = mdBuilder;                
    	}

    	public void AddOtherMethod(MethodBuilder mdBuilder)
    	{
            if (mdBuilder == null)
            {
    			throw new ArgumentNullException("mdBuilder");
            }
            m_containingType.ThrowIfCreated();
    		TypeBuilder.InternalDefineMethodSemantics(
    			m_module,
    			m_prToken.Token,
    			MethodSemanticsAttributes.Other, 
    			mdBuilder.GetToken().Token);
    	}
    
		// Use this function if client decides to form the custom attribute blob themselves
[System.Runtime.InteropServices.ComVisible(true)]
    	public void SetCustomAttribute(ConstructorInfo con, byte[] binaryAttribute)
    	{
    		if (con == null)
    			throw new ArgumentNullException("con");
    		if (binaryAttribute == null)
    			throw new ArgumentNullException("binaryAttribute");
    		
            m_containingType.ThrowIfCreated();
            TypeBuilder.InternalCreateCustomAttribute(
                m_prToken.Token,
                ((ModuleBuilder )m_module).GetConstructorToken(con).Token,
                binaryAttribute,
                m_module,
                false);
    	}

		// Use this function if client wishes to build CustomAttribute using CustomAttributeBuilder
        public void SetCustomAttribute(CustomAttributeBuilder customBuilder)
        {
            if (customBuilder == null)
            {
    			throw new ArgumentNullException("customBuilder");
            }
            m_containingType.ThrowIfCreated();
            customBuilder.CreateCustomAttribute((ModuleBuilder)m_module, m_prToken.Token);
        }

		// Not supported functions in dynamic module.
		public override Object GetValue(Object obj,Object[] index)
		{
			throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));
		}

		public override Object GetValue(Object obj,BindingFlags invokeAttr,Binder binder,Object[] index,CultureInfo culture)
		{
			throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));
		}

    	public override void SetValue(Object obj,Object value,Object[] index)
    	{
			throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));
		}

    	public override void SetValue(Object obj,Object value,BindingFlags invokeAttr,Binder binder,Object[] index,CultureInfo culture)
		{
			throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));
		}

		public override MethodInfo[] GetAccessors(bool nonPublic)
		{
			throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));
		}

    	public override MethodInfo GetGetMethod(bool nonPublic)
		{
            if (nonPublic || m_getMethod == null)
			    return m_getMethod;
            // now check to see if m_getMethod is public
            if ((m_getMethod.Attributes & MethodAttributes.Public) == MethodAttributes.Public)
                return m_getMethod;
            return null;
		}

		public override MethodInfo GetSetMethod(bool nonPublic)
		{
            if (nonPublic || m_setMethod == null)
			    return m_setMethod;
            // now check to see if m_setMethod is public
            if ((m_setMethod.Attributes & MethodAttributes.Public) == MethodAttributes.Public)
                return m_setMethod;
            return null;
		}

		public override ParameterInfo[] GetIndexParameters()
		{
			throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));
		}

		public override Type PropertyType {
    		get { return m_returnType; }
    	}

    	public override PropertyAttributes Attributes {
    		get { return m_attributes;}
    	}

    	public override bool CanRead {
    		get { if (m_getMethod != null) return true; else return false; } }

    	public override bool CanWrite {
    		get { if (m_setMethod != null) return true; else return false; }
    	}

        public override Object[] GetCustomAttributes(bool inherit)
		{
			throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));
		}

        public override Object[] GetCustomAttributes(Type attributeType, bool inherit)
		{
			throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));
		}

        public override bool IsDefined (Type attributeType, bool inherit)
		{
			throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));
		}

        void _PropertyBuilder.GetTypeInfoCount(out uint pcTInfo)
        {
            throw new NotImplementedException();
        }

        void _PropertyBuilder.GetTypeInfo(uint iTInfo, uint lcid, IntPtr ppTInfo)
        {
            throw new NotImplementedException();
        }

        void _PropertyBuilder.GetIDsOfNames([In] ref Guid riid, IntPtr rgszNames, uint cNames, uint lcid, IntPtr rgDispId)
        {
            throw new NotImplementedException();
        }

        void _PropertyBuilder.Invoke(uint dispIdMember, [In] ref Guid riid, uint lcid, short wFlags, IntPtr pDispParams, IntPtr pVarResult, IntPtr pExcepInfo, IntPtr puArgErr)
        {
            throw new NotImplementedException();
        }


    	public override String Name {
    		get { return m_name; }
    	}

    	public override Type DeclaringType {
    		get { return m_containingType; }
    	}

    	public override Type ReflectedType {
    		get { return m_containingType; }
    	}

        // These are package private so that TypeBuilder can access them.
        private String				m_name;				// The name of the property
        private PropertyToken	    m_prToken;			// The token of this property
		private int					m_tkProperty;
        private Module				m_module;
        private SignatureHelper		m_signature;
    	private PropertyAttributes  m_attributes;		// property's attribute flags
		private Type				m_returnType;		// property's return type
        private MethodInfo          m_getMethod;
        private MethodInfo          m_setMethod;
        private TypeBuilder         m_containingType;
    }

}
