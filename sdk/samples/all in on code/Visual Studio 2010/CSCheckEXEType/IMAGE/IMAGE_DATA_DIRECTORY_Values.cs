/****************************** Module Header ******************************\
* Module Name:  IMAGE_DATA_DIRECTORY_Values.cs
* Project:	    CSCheckEXEType
* Copyright (c) Microsoft Corporation.
* 
* The DataDirectory is an array of 16 structures. Each array entry has a 
* predefined meaning for what it refers to. The IMAGE_DIRECTORY_ENTRY_ xxx 
* #defines are array indexes into the DataDirectory (from 0 to 15).
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


namespace CSCheckEXEType.IMAGE
{
    public class IMAGE_DATA_DIRECTORY_Values
    {
        // Export Directory
        public const int IMAGE_DIRECTORY_ENTRY_EXPORT = 0;

        // Import Directory
        public const int IMAGE_DIRECTORY_ENTRY_IMPORT = 1;

        // Resource Directory
        public const int IMAGE_DIRECTORY_ENTRY_RESOURCE = 2;

        // Exception Directory
        public const int IMAGE_DIRECTORY_ENTRY_EXCEPTION = 3;

        // Security Directory
        public const int IMAGE_DIRECTORY_ENTRY_SECURITY = 4;

        // Base Relocation Table
        public const int IMAGE_DIRECTORY_ENTRY_BASERELOC = 5;

        // Debug Directory
        public const int IMAGE_DIRECTORY_ENTRY_DEBUG = 6;

        // Architecture Specific Data
        public const int IMAGE_DIRECTORY_ENTRY_ARCHITECTURE = 7;

        // RVA of GP
        public const int IMAGE_DIRECTORY_ENTRY_GLOBALPTR = 8;

        // TLS Directory
        public const int IMAGE_DIRECTORY_ENTRY_TLS = 9;

        // Load Configuration Directory
        public const int IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG = 10;

        // Bound Import Directory in headers
        public const int IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT = 11;

        // Import Address Table
        public const int IMAGE_DIRECTORY_ENTRY_IAT = 12;

        // Delay Load Import Descriptors
        public const int IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT = 13;

        // COM Runtime descriptor 
        public const int IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR = 14;

        public IMAGE_DATA_DIRECTORY[] Values { get; set; }
 
        public IMAGE_DATA_DIRECTORY_Values()
        {
            Values = new IMAGE_DATA_DIRECTORY[16];
        }
    }

}
