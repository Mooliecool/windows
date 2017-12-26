//------------------------------------------------------------------------------
// <copyright file="TypeDescriptionProviderAttribute.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>                                                                
//------------------------------------------------------------------------------

namespace System.ComponentModel 
{

    using System;
    using System.Security.Permissions;

    /// <devdoc>
    /// </devdoc>
    [AttributeUsage(AttributeTargets.Class, Inherited = true)]
    public sealed class TypeDescriptionProviderAttribute : Attribute 
    {
        private string _typeName;

        /// <devdoc>
        ///     Creates a new TypeDescriptionProviderAttribute object.
        /// </devdoc>
        public TypeDescriptionProviderAttribute(string typeName)
        {
            if (typeName == null)
            {
                throw new ArgumentNullException("typeName");
            }

            _typeName = typeName;
        }
    
        /// <devdoc>
        ///     Creates a new TypeDescriptionProviderAttribute object.
        /// </devdoc>
        public TypeDescriptionProviderAttribute(Type type)
        {
            if (type == null)
            {
                throw new ArgumentNullException("type");
            }

            _typeName = type.AssemblyQualifiedName;
        }

        /// <devdoc>
        ///     The TypeName property returns the assembly qualified type name 
        ///     for the type description provider.
        /// </devdoc>
        public string TypeName
        {
            get
            {
                return _typeName;
            }
        }
    }
}

