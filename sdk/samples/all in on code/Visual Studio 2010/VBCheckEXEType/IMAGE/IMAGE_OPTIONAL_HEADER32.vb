'*************************** Module Header ******************************'
' Module Name:  IMAGE_OPTIONAL_HEADER32.vb
' Project:	    VBCheckEXEType
' Copyright (c) Microsoft Corporation.
' 
' Represents the optional header format for 32bit application.
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

    Public Structure IMAGE_OPTIONAL_HEADER32
        ''' <summary>
        ''' A signature WORD, identifying what type of header this is. The two most 
        ''' common values are IMAGE_NT_OPTIONAL_HDR32_MAGIC 0x10b and 
        ''' IMAGE_NT_OPTIONAL_HDR64_MAGIC 0x20b.
        ''' </summary>
        Public Magic As UInt16

        ''' <summary>
        ''' The major version of the linker used to build this executable. For PE files
        ''' from the Microsoft linker, this version number corresponds to the Visual 
        ''' Studio version number (for example, version 6 for Visual Studio 6.0).
        ''' </summary>
        Public MajorLinkerVersion As Byte

        ''' <summary>
        ''' The minor version of the linker used to build this executable.
        ''' </summary>
        Public MinorLinkerVersion As Byte

        ''' <summary>
        ''' The combined total size of all sections with the IMAGE_SCN_CNT_CODE attribute.
        ''' </summary>
        Public SizeOfCode As UInt32

        ''' <summary>
        ''' The combined size of all initialized data sections.
        ''' </summary>
        Public SizeOfInitializedData As UInt32

        ''' <summary>
        ''' The size of all sections with the uninitialized data attributes. This field 
        ''' will often be 0, since the linker can append uninitialized data to the end
        ''' of regular data sections.
        ''' </summary>
        Public SizeOfUninitializedData As UInt32

        ''' <summary>
        ''' The RVA of the first code byte in the file that will be executed. For DLLs, 
        ''' this entrypoint is called during process initialization and shutdown and 
        ''' during thread creations/destructions. In most executables, this address 
        ''' doesn't directly point to main, WinMain, or DllMain. Rather, it points to 
        ''' runtime library code that calls the aforementioned functions. This field can 
        ''' be set to 0 in DLLs, and none of the previous notifications will be received.
        ''' The linker /NOENTRY switch sets this field to 0.
        ''' </summary>
        Public AddressOfEntryPoint As UInt32

        ''' <summary>
        ''' The RVA of the first byte of code when loaded in memory.
        ''' </summary>
        Public BaseOfCode As UInt32


        ''' <summary>
        ''' Theoretically, the RVA of the first byte of data when loaded into memory. 
        ''' However, the values for this field are inconsistent with different versions 
        ''' of the Microsoft linker. This field is not present in 64-bit executables.
        ''' </summary>
        Public BaseOfData As UInt32

        ''' <summary>
        ''' The preferred load address of this file in memory. The loader attempts to 
        ''' load the PE file at this address if possible (that is, if nothing else 
        ''' currently occupies that memory, it's aligned properly and at a legal address, 
        ''' and so on). If the executable loads at this address, the loader can skip the 
        ''' step of applying base relocations (described in Part 2 of this article). 
        ''' For EXEs, the default ImageBase is 0x400000. For DLLs, it's 0x10000000. 
        ''' The ImageBase can be set at link time with the /BASE switch, or later with the 
        ''' REBASE utility.
        ''' </summary>
        Public ImageBase As UInt32

        ''' <summary>
        ''' The alignment of sections when loaded into memory. The alignment must be 
        ''' greater or equal to the file alignment field (mentioned next). The default 
        ''' alignment is the page size of the target CPU. For user mode executables to 
        ''' run under Windows 9x or Windows Me, the minimum alignment size is a page (4KB).
        ''' This field can be set with the linker /ALIGN switch.
        ''' </summary>
        Public SectionAlignment As UInt32

        ''' <summary>
        ''' The alignment of sections within the PE file. For x86 executables, this value 
        ''' is usually either 0x200 or 0x1000. The default has changed with different 
        ''' versions of the Microsoft linker. This value must be a power of 2, and if the 
        ''' SectionAlignment is less than the CPU's page size, this field must match the 
        ''' SectionAlignment. The linker switch /OPT:WIN98 sets the file alignment on x86 
        ''' executables to 0x1000, while /OPT:NOWIN98 sets the alignment to 0x200.
        ''' </summary>
        Public FileAlignment As UInt32

        ''' <summary>
        ''' The major version number of the required operating system. With the advent 
        ''' of so many versions of Windows, this field has effectively become irrelevant.
        ''' </summary>
        Public MajorOperatingSystemVersion As UInt16

        ''' <summary>
        ''' The minor version number of the required OS.
        ''' </summary>
        Public MinorOperatingSystemVersion As UInt16

        ''' <summary>
        ''' The major version number of this file. Unused by the system and can be 0. 
        ''' It can be set with the linker /VERSION switch.
        ''' </summary>
        Public MajorImageVersion As UInt16

        ''' <summary>
        ''' The minor version number of this file.
        ''' </summary>
        Public MinorImageVersion As UInt16

        ''' <summary>
        ''' The major version of the operating subsystem needed for this executable. 
        ''' At one time, it was used to indicate that the newer Windows 95 or Windows NT 4.0 
        ''' user interface was required, as opposed to older versions of the Windows NT 
        ''' interface. Today, because of the proliferation of the various versions of 
        ''' Windows, this field is effectively unused by the system and is typically set 
        ''' to the value 4. Set with the linker /SUBSYSTEM switch.
        ''' </summary>
        Public MajorSubsystemVersion As UInt16

        ''' <summary>
        ''' The minor version of the operating subsystem needed for this executable.
        ''' </summary>
        Public MinorSubsystemVersion As UInt16

        ''' <summary>
        ''' Another field that never took off. Typically set to 0.

        ''' </summary>
        Public Win32VersionValue As UInt32

        ''' <summary>
        ''' SizeOfImage contains the RVA that would be assigned to the section following 
        ''' the last section if it existed. This is effectively the amount of memory that 
        ''' the system needs to reserve when loading this file into memory. This field
        ''' must be a multiple of the section alignment.
        ''' </summary>
        Public SizeOfImage As UInt32

        ''' <summary>
        ''' The combined size of the MS-DOS header, PE headers, and section table. All of 
        ''' these items will occur before any code or data sections in the PE file. The 
        ''' value of this field is rounded up to a multiple of the file alignment. 
        ''' </summary>
        Public SizeOfHeaders As UInt32

        ''' <summary>
        ''' The checksum of the image. The CheckSumMappedFile API in IMAGEHLP.DLL can 
        ''' calculate this value. Checksums are required for kernel-mode drivers and some 
        ''' system DLLs. Otherwise, this field can be 0. The checksum is placed in the 
        ''' file when the /RELEASE linker switch is used.
        ''' </summary>
        Public CheckSum As UInt32


        ''' <summary>
        ''' An enum value indicating what subsystem (user interface type) the executable 
        ''' expects. This field is only important for EXEs. Important values include:
        ''' IMAGE_SUBSYSTEM_NATIVE       // Image doesn't require a subsystem
        ''' IMAGE_SUBSYSTEM_WINDOWS_GUI  // Use the Windows GUI
        ''' IMAGE_SUBSYSTEM_WINDOWS_CUI  // Run as a console mode application
        '''                              // When run, the OS creates a console
        '''                              // window for it, and provides stdin,
        '''                              // stdout, and stderr file handles
        '''
        ''' </summary>
        Public Subsystem As UInt16

        ''' <summary>
        ''' Flags indicating characteristics of this DLL. These correspond to the 
        ''' IMAGE_DLLCHARACTERISTICS_xxx fields #defines. Current values are:
        ''' IMAGE_DLLCHARACTERISTICS_NO_BIND		// Do not bind this image
        ''' IMAGE_DLLCHARACTERISTICS_WDM_DRIVER		// Driver uses WDM model
        ''' IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE // When the terminal server loads
        '''                                                // an application that is not
        '''                                                //  Terminal- Services-aware, it
        '''                                                // also loads a DLL that contains
        '''                                                // compatibility code
        ''' </summary>
        Public DllCharacteristics As UInt16

        ''' <summary>
        ''' In EXE files, the maximum size the initial thread in the process can grow to. 
        ''' This is 1MB by default. Not all this memory is committed initially.
        ''' </summary>
        Public SizeOfStackReserve As UInt32

        ''' <summary>
        ''' In EXE files, the amount of memory initially committed to the stack. 
        ''' By default, this field is 4KB.
        ''' </summary>
        Public SizeOfStackCommit As UInt32

        ''' <summary>
        ''' In EXE files, the initial reserved size of the default process heap. 
        ''' This is 1MB by default. In current versions of Windows, however, 
        ''' the heap can grow beyond this size without intervention by the user.
        ''' </summary>
        Public SizeOfHeapReserve As UInt32

        ''' <summary>
        ''' In EXE files, the size of memory committed to the heap. 
        ''' By default, this is 4KB.
        ''' </summary>
        Public SizeOfHeapCommit As UInt32

        ''' <summary>
        ''' This is obsolete.
        ''' </summary>
        Public LoaderFlags As UInt32

        ''' <summary>
        '''  At the end of the IMAGE_NT_HEADERS structure is an array of 
        '''  IMAGE_DATA_DIRECTORY structures. This field contains the number of entries in
        '''  the array. This field has been 16 since the earliest releases of Windows NT.
        ''' </summary>
        Public NumberOfRvaAndSizes As UInt32

    End Structure

End Namespace
