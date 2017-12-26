//------------------------------------------------------------------------------
// <copyright file="ExtenderProvidedPropertyAttribute.cs" company="Microsoft">
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

/*
 */
namespace System.ComponentModel {

    using System;
    using System.Diagnostics;
    using System.Security.Permissions;    

    /// <internalonly/>
    /// <devdoc>
    ///    <para>
    ///       ExtenderProvidedPropertyAttribute is an attribute that marks that a property
    ///       was actually offered up by and extender provider.
    ///    </para>
    /// </devdoc>
    [AttributeUsage(AttributeTargets.All)]
    public sealed class ExtenderProvidedPropertyAttribute : Attribute {

        private PropertyDescriptor extenderProperty;
        private IExtenderProvider  provider;
        private Type               receiverType;

        /// <devdoc>
        ///     Creates a new ExtenderProvidedPropertyAttribute.
        /// </devdoc>
        internal static ExtenderProvidedPropertyAttribute Create(PropertyDescriptor extenderProperty, Type receiverType, IExtenderProvider provider) {
            ExtenderProvidedPropertyAttribute e = new ExtenderProvidedPropertyAttribute();
            e.extenderProperty = extenderProperty;
            e.receiverType = receiverType;
            e.provider = provider;
            return e;
        }

        /// <devdoc>
        ///     Creates an empty ExtenderProvidedPropertyAttribute.
        /// </devdoc>
        public ExtenderProvidedPropertyAttribute() {
        }

        /// <devdoc>
        ///     PropertyDescriptor of the property that is being provided.
        /// </devdoc>
        public PropertyDescriptor ExtenderProperty {
            get {
                return extenderProperty;
            }
        }

        /// <devdoc>
        ///     Extender provider that is providing the property.
        /// </devdoc>
        public IExtenderProvider Provider {
            get {
                return provider;
            }
        }

        /// <devdoc>
        ///     The type of object that can receive these properties.
        /// </devdoc>
        public Type ReceiverType {
            get {
                return receiverType;
            }
        }

        public override bool Equals(object obj) {
            if (obj == this) {
                return true;
            }

            ExtenderProvidedPropertyAttribute other = obj as ExtenderProvidedPropertyAttribute;

            return (other != null) && other.extenderProperty.Equals(extenderProperty) && other.provider.Equals(provider) && other.receiverType.Equals(receiverType);
        }

        public override int GetHashCode() {
            return base.GetHashCode();
        }
    
        /// <internalonly/>
        /// <devdoc>
        /// </devdoc>
        public override bool IsDefaultAttribute() {
            return receiverType == null;
        }
    }
}
