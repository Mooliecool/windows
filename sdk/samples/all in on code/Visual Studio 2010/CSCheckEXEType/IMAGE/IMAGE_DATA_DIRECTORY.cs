/****************************** Module Header ******************************\
* Module Name:  IMAGE_DATA_DIRECTORY.cs
* Project:	    CSCheckEXEType
* Copyright (c) Microsoft Corporation.
* 
* Represents the data directory. 
* 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;

namespace CSCheckEXEType.IMAGE
{
    public struct IMAGE_DATA_DIRECTORY
    {

        // RVA of the data
        public UInt32 VirtualAddress;

        // Size of the data
        public UInt32 Size;

    }
}
