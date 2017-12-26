//------------------------------------------------------------------------------
// <copyright file="GraphicsState.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

namespace System.Drawing.Drawing2D {

    using System.Diagnostics;

    using System;

    /// <include file='doc\GraphicsState.uex' path='docs/doc[@for="GraphicsState"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public sealed class GraphicsState : MarshalByRefObject {
        internal int nativeState;

        [System.Runtime.TargetedPatchingOptOutAttribute("Performance critical to inline across NGen image boundaries")]
        internal GraphicsState(int nativeState) {
            this.nativeState = nativeState;
        }
    }
}

