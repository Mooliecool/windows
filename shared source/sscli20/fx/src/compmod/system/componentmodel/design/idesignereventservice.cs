//------------------------------------------------------------------------------
// <copyright file="IDesignerEventService.cs" company="Microsoft">
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

namespace System.ComponentModel.Design {
    using System.Diagnostics;
    using System;
    using System.ComponentModel;
    using Microsoft.Win32;

    /// <devdoc>
    ///    <para>Provides global
    ///       event notifications and the ability to create designers.</para>
    /// </devdoc>
    public interface IDesignerEventService {

        /// <devdoc>
        ///    <para>
        ///       Gets the currently active designer.
        ///    </para>
        /// </devdoc>
        IDesignerHost ActiveDesigner { get; }

        /// <devdoc>
        ///    <para>
        ///       Gets or
        ///       sets a collection of running design documents in the development environment.
        ///    </para>
        /// </devdoc>
        DesignerCollection Designers { get; }
        
        /// <devdoc>
        ///    <para>
        ///       Adds an event that will be raised when the currently active designer
        ///       changes.
        ///    </para>
        /// </devdoc>
        event ActiveDesignerEventHandler ActiveDesignerChanged;

        /// <devdoc>
        ///    <para>
        ///       Adds an event that will be raised when a designer is created.
        ///    </para>
        /// </devdoc>
        event DesignerEventHandler DesignerCreated;
        
        /// <devdoc>
        ///    <para>
        ///       Adds an event that will be raised when a designer is disposed.
        ///    </para>
        /// </devdoc>
        event DesignerEventHandler DesignerDisposed;
        
        /// <devdoc>
        ///    <para>
        ///       Adds an event that will be raised when the global selection changes.
        ///    </para>
        /// </devdoc>
        event EventHandler SelectionChanged;
    }
}

