//------------------------------------------------------------------------------
// <copyright file="ICustomTypeDescriptor.cs" company="Microsoft">
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
    using System.Runtime.Serialization.Formatters;
    using System.Runtime.InteropServices;
    

    using System.Diagnostics;

    using System;
    using Microsoft.Win32;
    

    /// <devdoc>
    ///    <para>Provides an interface that provides custom type information for an 
    ///       object.</para>
    /// </devdoc>
    public interface ICustomTypeDescriptor {

        /// <devdoc>
        /// <para>Gets a collection of type <see cref='System.Attribute'/> with the attributes 
        ///    for this object.</para>
        /// </devdoc>
        AttributeCollection GetAttributes();

        /// <devdoc>
        ///    <para>Gets the class name of this object.</para>
        /// </devdoc>
        string GetClassName();

        /// <devdoc>
        ///    <para>Gets the name of this object.</para>
        /// </devdoc>
        string GetComponentName();

        /// <devdoc>
        ///    <para>Gets a type converter for this object.</para>
        /// </devdoc>
        TypeConverter GetConverter();

        /// <devdoc>
        ///    <para>Gets the default event for this object.</para>
        /// </devdoc>
        EventDescriptor GetDefaultEvent();


        /// <devdoc>
        ///    <para>Gets the default property for this object.</para>
        /// </devdoc>
        PropertyDescriptor GetDefaultProperty();

        /// <devdoc>
        ///    <para>Gets an editor of the specified type for this object.</para>
        /// </devdoc>
        object GetEditor(Type editorBaseType);

        /// <devdoc>
        ///    <para>Gets the events for this instance of a component.</para>
        /// </devdoc>
        EventDescriptorCollection GetEvents();

        /// <devdoc>
        ///    <para>Gets the events for this instance of a component using the attribute array as a
        ///       filter.</para>
        /// </devdoc>
        EventDescriptorCollection GetEvents(Attribute[] attributes);

        /// <devdoc>
        ///    <para>Gets the properties for this instance of a component.</para>
        /// </devdoc>
        PropertyDescriptorCollection GetProperties();

        /// <devdoc>
        ///    <para>Gets the properties for this instance of a component using the attribute array as a filter.</para>
        /// </devdoc>
        PropertyDescriptorCollection GetProperties(Attribute[] attributes);

        /// <devdoc>
        ///    <para>Gets the object that directly depends on this value being edited.</para>
        /// </devdoc>
        object GetPropertyOwner(PropertyDescriptor pd);
   }
}
