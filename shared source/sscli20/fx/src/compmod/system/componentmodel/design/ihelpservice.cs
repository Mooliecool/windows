//------------------------------------------------------------------------------
// <copyright file="IHelpService.cs" company="Microsoft">
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
    using System.Runtime.Remoting;
    using System.ComponentModel;

    using System.Diagnostics;

    using System;

    /// <devdoc>
    ///    <para> 
    ///       Provides the Integrated Development Environment (IDE) help
    ///       system with contextual information for the current task.</para>
    /// </devdoc>
    public interface IHelpService {
        /// <devdoc>
        ///    <para>Adds a context attribute to the document.</para>
        /// </devdoc>
        void AddContextAttribute(string name, string value, HelpKeywordType keywordType);
        
        /// <devdoc>
        ///     Clears all existing context attributes from the document.
        /// </devdoc>
        void ClearContextAttributes();
        
        /// <devdoc>
        ///     Creates a Local IHelpService to manage subcontexts.
        /// </devdoc>
        IHelpService CreateLocalContext(HelpContextType contextType);

        /// <devdoc>
        ///    <para>
        ///       Removes a previously added context attribute.
        ///    </para>
        /// </devdoc>
        void RemoveContextAttribute(string name, string value);
        
        /// <devdoc>
        ///     Removes a context that was created with CreateLocalContext
        /// </devdoc>
        void RemoveLocalContext(IHelpService localContext);

        /// <devdoc>
        ///    <para>Shows the help topic that corresponds to the specified keyword.</para>
        /// </devdoc>
        void ShowHelpFromKeyword(string helpKeyword);

        /// <devdoc>
        ///    <para>
        ///       Shows the help topic that corresponds with the specified Url and topic navigation ID.
        ///    </para>
        /// </devdoc>
        void ShowHelpFromUrl(string helpUrl);
    }
}
