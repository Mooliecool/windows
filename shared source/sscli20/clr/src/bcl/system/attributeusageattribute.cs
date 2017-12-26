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
** Class:  AttributeUsageAttribute
**
**
** Purpose: The class denotes how to specify the usage of an attribute
**          
**
===========================================================*/
namespace System {

	using System.Reflection;
	/* By default, attributes are inherited and multiple attributes are not allowed */
    [AttributeUsage(AttributeTargets.Class, Inherited = true),Serializable()]
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class AttributeUsageAttribute : Attribute
	{
        internal AttributeTargets m_attributeTarget = AttributeTargets.All; // Defaults to all
        internal bool m_allowMultiple = false; // Defaults to false
        internal bool m_inherited = true; // Defaults to true
    
		internal static AttributeUsageAttribute Default = new AttributeUsageAttribute(AttributeTargets.All);

       //Constructors 
        public AttributeUsageAttribute(AttributeTargets validOn) {
            m_attributeTarget = validOn;
        }
       internal AttributeUsageAttribute(AttributeTargets validOn, bool allowMultiple, bool inherited) {
           m_attributeTarget = validOn;
           m_allowMultiple = allowMultiple;
           m_inherited = inherited;
       }
    
	   
       //Properties 
        public AttributeTargets ValidOn 
		{
           get{ return m_attributeTarget; }
	    }
    
        public bool AllowMultiple 
		{
            get { return m_allowMultiple; }
            set { m_allowMultiple = value; }
        }
    
        public bool Inherited 
		{
            get { return m_inherited; }
            set { m_inherited = value; }
        }
	}
}
