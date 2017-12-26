//------------------------------------------------------------------------------
// <copyright file="InheritanceLevel.cs" company="Microsoft">
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
    
    /// <devdoc>
    ///    <para>
    ///       Specifies
    ///       numeric IDs for different inheritance levels.
    ///    </para>
    /// </devdoc>
    public enum InheritanceLevel {
    
        /// <devdoc>
        ///      Indicates that the object is inherited.
        /// </devdoc>
        Inherited = 1,
        
        /// <devdoc>
        ///    <para>
        ///       Indicates that the object is inherited, but has read-only access.
        ///    </para>
        /// </devdoc>
        InheritedReadOnly = 2,
        
        /// <devdoc>
        ///      Indicates that the object is not inherited.
        /// </devdoc>
        NotInherited = 3,
    }
}

