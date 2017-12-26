/****************************** Module Header ******************************\
 * Module Name:  MINIDUMP_TYPE.cs
 * Project:      CSCreateMiniDump
 * Copyright (c) Microsoft Corporation.
 * 
 * Identifies the type of information that will be written to the minidump file 
 * by the MiniDumpWriteDump function.
 * 
 * To create a MiniDump, we often use MiniDumpWithIndirectlyReferencedMemory and
 * MiniDumpScanMemory. 
 * 
 * See http://msdn.microsoft.com/en-us/library/ms680519(VS.85).aspx for the detailed
 * information.
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

namespace CSCreateMiniDump.MiniDump
{
    enum MINIDUMP_TYPE
    {
        MiniDumpWithDataSegs = 0x00000001,
        MiniDumpWithFullMemory = 0x00000002,
        MiniDumpWithHandleData = 0x00000004,
        MiniDumpFilterMemory = 0x00000008,
        MiniDumpScanMemory = 0x00000010,
        MiniDumpWithUnloadedModules = 0x00000020,
        MiniDumpWithIndirectlyReferencedMemory = 0x00000040,
        MiniDumpFilterModulePaths = 0x00000080,
        MiniDumpWithProcessThreadData = 0x00000100,
        MiniDumpWithPrivateReadWriteMemory = 0x00000200,
        MiniDumpWithoutOptionalData = 0x00000400,
        MiniDumpWithFullMemoryInfo = 0x00000800,
        MiniDumpWithThreadInfo = 0x00001000,
        MiniDumpWithCodeSegs = 0x00002000,
        MiniDumpWithoutAuxiliaryState = 0x00004000,
        MiniDumpWithFullAuxiliaryState = 0x00008000,
        MiniDumpValidTypeFlags = 0x0000ffff,
    }
}
