//------------------------------------------------------------------------------
// <copyright file="DesignerSerializationVisibility.cs" company="Microsoft">
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

    /// <devdoc>
    ///    <para>Specifies the visibility a property has to the design time
    ///          serializer.
    ///    </para>
    /// </devdoc>
    [System.Runtime.InteropServices.ComVisible(true)]
    public enum DesignerSerializationVisibility {
    
        /// <devdoc>
        ///    <para>The code generator will not produce code for the object.</para>
        /// </devdoc>
        Hidden,
        
        /// <devdoc>
        ///    <para>The code generator will produce code for the object.</para>
        /// </devdoc>
        Visible,
        
        /// <devdoc>
        ///    <para>The code generator will produce code for the contents of the object, rather than for the object itself.</para>
        /// </devdoc>
        Content
    }
}
