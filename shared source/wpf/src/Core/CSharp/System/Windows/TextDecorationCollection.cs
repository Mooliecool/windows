//---------------------------------------------------------------------------
//
// <copyright file=GlyphInfoList.cs company=Microsoft>
//    Copyright (C) Microsoft Corporation.  All rights reserved.
// </copyright>
// 
//
// Description: TextDecorationCollection class
//
// History:  
//  10/14/2004: Garyyang Created the file 
//
//---------------------------------------------------------------------------

using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;

using System.Windows.Media.Animation;
using System.Windows.Markup;

using MS.Internal.PresentationCore;

namespace System.Windows
{
    /// <summary>
    /// A collection of text decoration instances
    /// </summary>
    [TypeConverter(typeof(TextDecorationCollectionConverter))]
    [Localizability(LocalizationCategory.None, Readability=Readability.Unreadable)]
    public sealed partial class TextDecorationCollection : Animatable, IList
    {
        /// <summary>
        /// Compare this collection with another TextDecorations.  
        /// </summary>
        /// <param name="textDecorations"> the text decoration collection to be compared </param>
        /// <returns> true if two collections of TextDecorations contain equal TextDecoration objects in the
        /// the same order. false otherwise 
        /// </returns>
        /// <remarks>
        /// The method doesn't check "full" equality as it can not take into account of all the possible 
        /// values associated with the DependencyObject,such as Animation, DataBinding and Attached property. 
        /// It only compares the public properties to serve the specific Framework's needs in inline property 
        /// management and Editing serialization. 
        /// </remarks>        
        [FriendAccessAllowed]   // used by Framework
        internal bool ValueEquals(TextDecorationCollection textDecorations)
        {
            if (textDecorations == null) 
                return false;   // o is either null or not TextDecorations object

            if (this == textDecorations) 
                return true;    // Reference equality.

            if ( this.Count != textDecorations.Count)
                return false;   // Two counts are different.

            // To be considered equal, TextDecorations should be same in the exact order.
            // Order matters because they imply the Z-order of the text decorations on screen.
            // Same set of text decorations drawn with different orders may have different result.
            for (int i = 0; i < this.Count; i++)
            {
                if (!this[i].ValueEquals(textDecorations[i]))
                    return false;
            }            
            return true;                     
        }
     
        /// <summary>
        /// Add a collection of text decorations into the current collection
        /// </summary>
        /// <param name="textDecorations"> The collection to be added </param>
        [CLSCompliant(false)]
        public void Add(IEnumerable<TextDecoration> textDecorations)
        {
            if (textDecorations == null)
            {
                throw new ArgumentNullException("textDecorations");
            }

            foreach(TextDecoration textDecoration in textDecorations)
            {
                Add(textDecoration);
            }                
        }
    }
}
