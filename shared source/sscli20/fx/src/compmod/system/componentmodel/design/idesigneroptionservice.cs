//------------------------------------------------------------------------------
// <copyright file="IDesignerOptionService.cs" company="Microsoft">
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
namespace System.ComponentModel.Design {
    using System.ComponentModel;

    using System.Diagnostics;
        
        /// <devdoc>
        ///    <para>
        ///       Provides access
        ///       to get and set option values for a designer.
        ///    </para>
        /// </devdoc>
        public interface IDesignerOptionService{
        
            /// <devdoc>
            ///    <para>Gets the value of an option defined in this package.</para>
            /// </devdoc>
            object GetOptionValue(string pageName, string valueName);
            
            /// <devdoc>
            ///    <para>Sets the value of an option defined in this package.</para>
            /// </devdoc>
            void SetOptionValue(string pageName, string valueName, object value);
        }
}

