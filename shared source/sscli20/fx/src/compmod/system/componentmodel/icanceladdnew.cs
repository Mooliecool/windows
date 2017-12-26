//------------------------------------------------------------------------------
// <copyright file="ICancelAddNew.cs" company="Microsoft">
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

    /// <devdoc>
    ///     Interface implemented by a list that allows the addition of a new item
    ///     to be either cancelled or committed.
    ///
    ///     Note: In some scenarios, specifically Windows Forms complex data binding,
    ///     the list may recieve CancelNew or EndNew calls for items other than the
    ///     new item. These calls should be ignored, ie. the new item should only be
    ///     cancelled or committed when that item's index is specified.
    /// </devdoc>
    public interface ICancelAddNew
    {
        /// <devdoc>
        ///     If a new item has been added to the list, and <paramref name="itemIndex"/> is the position of that item,
        ///     then this method should remove it from the list and cancel the add operation.
        /// </devdoc>
        void CancelNew(int itemIndex);

        /// <devdoc>
        ///     If a new item has been added to the list, and <paramref name="itemIndex"/> is the position of that item,
        ///     then this method should leave it in the list and complete the add operation.
        /// </devdoc>
        void EndNew(int itemIndex);
    }
}
