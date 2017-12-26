//---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------
#region Using directives

using System;
using System.Text;

#endregion

namespace Microsoft.Samples.MsmqSample
{
    /// <summary>
    /// Compact Disc class that represents a CD
    /// </summary>
    public class CompactDisc
    {
        // Title of the CD
        private string titleValue;
        //Constructor
        public CompactDisc(string title)
        {
            this.titleValue = title;
        }
        //Default Constructor
        public CompactDisc()
        {
        }
        public string Title
        {
            get
            {
                return titleValue;
            }
            set
            {
                titleValue = value;
            }
        }
    }
}
