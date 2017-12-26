//------------------------------------------------------------------------------
// <copyright file="IListSource.cs" company="Microsoft">
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

namespace System.ComponentModel {

    using System;
    using Microsoft.Win32;
    using System.Collections;

    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    [
    TypeConverterAttribute("System.Windows.Forms.Design.DataSourceConverter, " + AssemblyRef.SystemDesign),
    Editor("System.Windows.Forms.Design.DataSourceListEditor, " + AssemblyRef.SystemDesign, "System.Drawing.Design.UITypeEditor, " + AssemblyRef.SystemDrawing),
    MergableProperty(false)
    ]
    public interface IListSource {

        bool ContainsListCollection { get; }

        IList GetList();
    }
}
