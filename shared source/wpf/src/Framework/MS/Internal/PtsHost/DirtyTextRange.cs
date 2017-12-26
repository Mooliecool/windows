//---------------------------------------------------------------------------
//
// Copyright (C) Microsoft Corporation.  All rights reserved.
// 
// File: DirtyTextRange.cs
//
// Description: Dirty text range describes change in the TextContainer. 
//
// History:  
//  06/06/2003 : [....] - created.
//
//---------------------------------------------------------------------------

using System;
using System.Windows.Controls;
using System.Windows.Documents;

namespace MS.Internal.PtsHost
{
    // ----------------------------------------------------------------------
    // Dirty text range describes change in the TextContainer.
    // ----------------------------------------------------------------------
    internal struct DirtyTextRange
    {
        // ------------------------------------------------------------------
        // Constructor
        //
        //      startIndex - Index of the starting position of the change.
        //      positionsAdded - Number of characters added.
        //      positionsRemoved - Number of characters removed.
        // ------------------------------------------------------------------
        internal DirtyTextRange(int startIndex, int positionsAdded, int positionsRemoved)
        {
            StartIndex = startIndex;
            PositionsAdded = positionsAdded;
            PositionsRemoved = positionsRemoved;
        }

        // ------------------------------------------------------------------
        // Constructor
        //
        //      change - TextContainer change data.
        // ------------------------------------------------------------------
        internal DirtyTextRange(TextContainerChangeEventArgs change)
        {
            StartIndex = change.ITextPosition.Offset;

            PositionsAdded = 0;
            PositionsRemoved = 0;

            switch (change.TextChange)
            {
                case TextChangeType.ContentAdded:
                    PositionsAdded = change.Count;
                    break;

                case TextChangeType.ContentRemoved:
                    PositionsRemoved = change.Count;
                    break;

                case TextChangeType.PropertyModified:
                    PositionsAdded = change.Count;
                    PositionsRemoved = change.Count;
                    break;
            }
        }

        // ------------------------------------------------------------------
        // Index of the starting position of the change.
        // ------------------------------------------------------------------
        internal int StartIndex;

        // ------------------------------------------------------------------
        // Number of characters added.
        // ------------------------------------------------------------------
        internal int PositionsAdded;

        // ------------------------------------------------------------------
        // Number of characters removed.
        // ------------------------------------------------------------------
        internal int PositionsRemoved;
    }
}
