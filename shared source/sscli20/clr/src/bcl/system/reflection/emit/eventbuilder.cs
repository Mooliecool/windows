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
** Class:  EventBuilder
**
**
** Eventbuilder is for client to define eevnts for a class
**
** 
===========================================================*/
namespace System.Reflection.Emit {
    
	using System;
	using System.Reflection;
    using System.Security.Permissions;
    using System.Runtime.InteropServices;
    
    // 
    // A EventBuilder is always associated with a TypeBuilder.  The TypeBuilder.DefineEvent
    // method will return a new EventBuilder to a client.
    // 
    [HostProtection(MayLeakOnAbort = true)]
    [ClassInterface(ClassInterfaceType.None)]
    [ComDefaultInterface(typeof(_EventBuilder))]
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class EventBuilder : _EventBuilder
    { 
    
    	// Make a private constructor so these cannot be constructed externally.
        private EventBuilder() {}
    	
        // Constructs a EventBuilder.  
    	//
        internal EventBuilder(
    		Module			mod,					// the module containing this EventBuilder		
    		String			name,					// Event name
    		EventAttributes attr,					// event attribute such as Public, Private, and Protected defined above
    		int				eventType,				// event type
    		TypeBuilder     type,                   // containing type
            EventToken		evToken)
        {   		
            m_name = name;
            m_module = mod;
            m_attributes = attr;
    		m_evToken = evToken;
            m_type = type;
        }
    
    	// Return the Token for this event within the TypeBuilder that the
    	// event is defined within.
        public EventToken GetEventToken()
        {
            return m_evToken;
        }
    
    	
    	public void SetAddOnMethod(MethodBuilder mdBuilder)
    	{
            if (mdBuilder == null)
            {
    			throw new ArgumentNullException("mdBuilder");
            }
            
            m_type.ThrowIfCreated();
    		TypeBuilder.InternalDefineMethodSemantics(
    			m_module,
    			m_evToken.Token,
    			MethodSemanticsAttributes.AddOn, 
    			mdBuilder.GetToken().Token);
    	}
    
    
    	public void SetRemoveOnMethod(MethodBuilder mdBuilder)
    	{
            if (mdBuilder == null)
            {
    			throw new ArgumentNullException("mdBuilder");
            }
            m_type.ThrowIfCreated();
    		TypeBuilder.InternalDefineMethodSemantics(
    			m_module,
    			m_evToken.Token,
    			MethodSemanticsAttributes.RemoveOn, 
    			mdBuilder.GetToken().Token);
    	}
    
    
    	public void SetRaiseMethod(MethodBuilder mdBuilder)
    	{
            if (mdBuilder == null)
            {
    			throw new ArgumentNullException("mdBuilder");
            }
            m_type.ThrowIfCreated();
    		TypeBuilder.InternalDefineMethodSemantics(
    			m_module,
    			m_evToken.Token,
    			MethodSemanticsAttributes.Fire, 
    			mdBuilder.GetToken().Token);
    	}
    
    
    	public void AddOtherMethod(MethodBuilder mdBuilder)
    	{
            if (mdBuilder == null)
            {
    			throw new ArgumentNullException("mdBuilder");
            }

            m_type.ThrowIfCreated();
    		TypeBuilder.InternalDefineMethodSemantics(
    			m_module,
    			m_evToken.Token,
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
            m_type.ThrowIfCreated();
    		
            TypeBuilder.InternalCreateCustomAttribute(
                m_evToken.Token,
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
            m_type.ThrowIfCreated();
            customBuilder.CreateCustomAttribute((ModuleBuilder)m_module, m_evToken.Token);
        }

        void _EventBuilder.GetTypeInfoCount(out uint pcTInfo)
        {
            throw new NotImplementedException();
        }

        void _EventBuilder.GetTypeInfo(uint iTInfo, uint lcid, IntPtr ppTInfo)
        {
            throw new NotImplementedException();
        }

        void _EventBuilder.GetIDsOfNames([In] ref Guid riid, IntPtr rgszNames, uint cNames, uint lcid, IntPtr rgDispId)
        {
            throw new NotImplementedException();
        }

        void _EventBuilder.Invoke(uint dispIdMember, [In] ref Guid riid, uint lcid, short wFlags, IntPtr pDispParams, IntPtr pVarResult, IntPtr pExcepInfo, IntPtr puArgErr)
        {
            throw new NotImplementedException();
        }


        // These are package private so that TypeBuilder can access them.
        private String				m_name;				// The name of the event
        private EventToken			m_evToken;			// The token of this event
        private Module				m_module;
    	private EventAttributes		m_attributes;
        private TypeBuilder         m_type;       
    }




}
