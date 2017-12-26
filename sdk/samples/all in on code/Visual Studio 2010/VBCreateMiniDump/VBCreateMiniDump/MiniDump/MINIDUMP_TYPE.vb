'*************************** Module Header ******************************'
' Module Name:  MINIDUMP_TYPE.vb
' Project:      VBCreateMiniDump
' Copyright (c) Microsoft Corporation.
' 
' Identifies the type of information that will be written to the minidump file 
' by the MiniDumpWriteDump function.
' 
' To create a MiniDump, we often use MiniDumpWithIndirectlyReferencedMemory and
' MiniDumpScanMemory. 
' 
' See http://msdn.microsoft.com/en-us/library/ms680519(VS.85).aspx for the detailed
' information.
' 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'**************************************************************************'

Namespace MiniDump

    Friend Enum MINIDUMP_TYPE
        MiniDumpWithDataSegs = &H1
        MiniDumpWithFullMemory = &H2
        MiniDumpWithHandleData = &H4
        MiniDumpFilterMemory = &H8
        MiniDumpScanMemory = &H10
        MiniDumpWithUnloadedModules = &H20
        MiniDumpWithIndirectlyReferencedMemory = &H40
        MiniDumpFilterModulePaths = &H80
        MiniDumpWithProcessThreadData = &H100
        MiniDumpWithPrivateReadWriteMemory = &H200
        MiniDumpWithoutOptionalData = &H400
        MiniDumpWithFullMemoryInfo = &H800
        MiniDumpWithThreadInfo = &H1000
        MiniDumpWithCodeSegs = &H2000
        MiniDumpWithoutAuxiliaryState = &H4000
        MiniDumpWithFullAuxiliaryState = &H8000
        MiniDumpValidTypeFlags = &HFFFF
    End Enum

End Namespace

