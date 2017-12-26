/****************************** Module Header ******************************\
* Module Name:  IMAGE_FILE_Flag.cs
* Project:	    CSCheckEXEType
* Copyright (c) Microsoft Corporation.
* 
* Used in IMAGE_FILE_HEADER.
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
    [Flags]
    public enum IMAGE_FILE_Flag
    {
        // Relocation info stripped from file.
        IMAGE_FILE_RELOCS_STRIPPED = 0x0001,

        // File is executable  (i.e. no unresolved externel references).
        IMAGE_FILE_EXECUTABLE_IMAGE = 0x0002,

        // Line nunbers stripped from file.
        IMAGE_FILE_LINE_NUMS_STRIPPED = 0x0004,

        // Local symbols stripped from file.
        IMAGE_FILE_LOCAL_SYMS_STRIPPED = 0x0008,

        // Agressively trim working set
        IMAGE_FILE_AGGRESIVE_WS_TRIM = 0x0010,

        // App can handle >2gb addresses
        IMAGE_FILE_LARGE_ADDRESS_AWARE = 0x0020,

        // Bytes of machine word are reversed.
        IMAGE_FILE_BYTES_REVERSED_LO = 0x0080,

        // 32 bit word machine.
        IMAGE_FILE_32BIT_MACHINE = 0x0100,

        // Debugging info stripped from file in .DBG file
        IMAGE_FILE_DEBUG_STRIPPED = 0x0200,

        // If Image is on removable media, copy and run from the swap file.
        IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP = 0x0400,

        // If Image is on Net, copy and run from the swap file.
        IMAGE_FILE_NET_RUN_FROM_SWAP = 0x0800,

        // System File.
        IMAGE_FILE_SYSTEM = 0x1000,

        // File is a DLL.
        IMAGE_FILE_DLL = 0x2000,

        // File should only be run on a UP(single-processor) machine
        IMAGE_FILE_UP_SYSTEM_ONLY = 0x4000,

        // Bytes of machine word are reversed.
        IMAGE_FILE_BYTES_REVERSED_HI = 0x8000
    }
}
