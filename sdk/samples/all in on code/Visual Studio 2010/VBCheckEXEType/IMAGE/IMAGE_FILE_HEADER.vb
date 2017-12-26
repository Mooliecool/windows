'*************************** Module Header ******************************'
' Module Name:  IMAGE_FILE_HEADER.vb
' Project:	    VBCheckEXEType
' Copyright (c) Microsoft Corporation.
' 
' Represents the COFF header format. 
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

    Public Structure IMAGE_FILE_HEADER
        ''' <summary>
        ''' The target CPU for this executable. 
        ''' Common values are: IMAGE_FILE_MACHINE_I386    0x014c // Intel 386
        '''                    IMAGE_FILE_MACHINE_IA64    0x0200 // Intel 64
        '''                    IMAGE_FILE_MACHINE_AMD64   0x8664 // AMD 64 
        ''' </summary>
        Public Machine As UInt16

        ''' <summary>
        ''' Indicates how many sections are in the section table. The section table
        ''' immediately follows the IMAGE_NT_HEADERS.
        ''' </summary>
        Public NumberOfSections As UInt16

        ''' <summary>
        ''' Indicates the time when the file was created. This value is the number 
        ''' of seconds since January 1, 1970, Greenwich Mean Time (GMT). This value 
        ''' is a more accurate indicator of when the file was created than is the 
        ''' file system date/time.
        ''' </summary>
        Public TimeDateStamp As UInt32

        ''' <summary>
        ''' The file offset of the COFF symbol table, described in section 5.4 of the 
        ''' Microsoft specification. COFF symbol tables are relatively rare in PE files,
        ''' as newer debug formats have taken over. Prior to Visual Studio .NET, 
        ''' a COFF symbol table could be created by specifying the linker switch
        ''' /DEBUGTYPE:COFF. COFF symbol tables are almost always found in OBJ files. 
        ''' Set to 0 if no symbol table is present.
        ''' </summary>
        Public PointerToSymbolTable As UInt32

        ''' <summary>
        ''' Number of symbols in the COFF symbol table, if present. COFF symbols are a 
        ''' fixed size structure, and this field is needed to find the end of the COFF 
        ''' symbols. Immediately following the COFF symbols is a string table used to 
        ''' hold longer symbol names.
        ''' </summary>
        Public NumberOfSymbols As UInt32

        ''' <summary>
        ''' The size of the optional data that follows the IMAGE_FILE_HEADER. In PE files,
        ''' this data is the IMAGE_OPTIONAL_HEADER. This size is different depending on 
        ''' whether it's a 32 or 64-bit file. For 32-bit PE files, this field is usually 
        ''' 224. For 64-bit PE32+ files, it's usually 240. However, these sizes are just 
        ''' minimum values, and larger values could appear.
        ''' </summary>
        Public SizeOfOptionalHeader As UInt16

        ''' <summary>
        ''' A set of bit flags indicating attributes of the file. 
        ''' </summary>
        Public Characteristics As IMAGE_FILE_Flag

    End Structure

End Namespace
