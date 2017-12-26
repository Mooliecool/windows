'*************************** Module Header ******************************'
' Module Name:  IMAGE_FILE_Flag.vb
' Project:	    VBCheckEXEType
' Copyright (c) Microsoft Corporation.
' 
' Used in IMAGE_FILE_HEADER.
' 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Namespace IMAGE

    <Flags()>
    Public Enum IMAGE_FILE_Flag
        ' Relocation info stripped from file.
        IMAGE_FILE_RELOCS_STRIPPED = &H1

        ' File is executable  (i.e. no unresolved externel references).
        IMAGE_FILE_EXECUTABLE_IMAGE = &H2

        ' Line nunbers stripped from file.
        IMAGE_FILE_LINE_NUMS_STRIPPED = &H4

        ' Local symbols stripped from file.
        IMAGE_FILE_LOCAL_SYMS_STRIPPED = &H8

        ' Agressively trim working set
        IMAGE_FILE_AGGRESIVE_WS_TRIM = &H10

        ' App can handle >2gb addresses
        IMAGE_FILE_LARGE_ADDRESS_AWARE = &H20

        ' Bytes of machine word are reversed.
        IMAGE_FILE_BYTES_REVERSED_LO = &H80

        ' 32 bit word machine.
        IMAGE_FILE_32BIT_MACHINE = &H100

        ' Debugging info stripped from file in .DBG file
        IMAGE_FILE_DEBUG_STRIPPED = &H200

        ' If Image is on removable media, copy and run from the swap file.
        IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP = &H400

        ' If Image is on Net, copy and run from the swap file.
        IMAGE_FILE_NET_RUN_FROM_SWAP = &H800

        ' System File.
        IMAGE_FILE_SYSTEM = &H1000

        ' File is a DLL.
        IMAGE_FILE_DLL = &H2000

        ' File should only be run on a UP(single-processor) machine
        IMAGE_FILE_UP_SYSTEM_ONLY = &H4000

        ' Bytes of machine word are reversed.
        IMAGE_FILE_BYTES_REVERSED_HI = &H8000
    End Enum

End Namespace
