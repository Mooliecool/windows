//------------------------------------------------------------------------------
// <copyright file="INestedContainer.cs" company="Microsoft">
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
    ///     A "nested container" is an object that logically contains zero or more child
    ///     components and is controlled (owned) by some parent component.
    ///    
    ///     In this context, "containment" refers to logical containment, not visual
    ///     containment.  Components and containers can be used in a variety of
    ///     scenarios, including both visual and non-visual scenarios.
    /// </devdoc>
    public interface INestedContainer : IContainer {

        /// <devdoc>
        ///     The component that owns this nested container.
        /// </devdoc>
        IComponent Owner { get; }
    }
}

