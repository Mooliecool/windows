//------------------------------------------------------------------------------
// <copyright file="IIntellisenseBuilder.cs" company="Microsoft">
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

    /// <devdoc>
    /// 
    /// </devdoc>
    public interface IIntellisenseBuilder {

        /// <devdoc>
        /// Return a localized name.
        /// </devdoc>
        string Name { get; }

        /// <devdoc>
        /// Show the builder and return a boolean indicating whether value should be replaced with newValue
        /// - false if the user cancels for example
        ///
        /// language - indicates which language service is calling the builder
        /// value - expression being edited
        /// newValue - return the new value
        /// </devdoc> 
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1045:DoNotPassTypesByReference")]
        bool Show(string language, string value, ref string newValue);       
    }
}
