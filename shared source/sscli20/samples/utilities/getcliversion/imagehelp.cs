//------------------------------------------------------------------------------
// <copyright file="imagehelp.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>                                                                
//------------------------------------------------------------------------------

using System;

namespace DevService.Util {

	public struct IMAGE_DOS_HEADER
	{      // DOS .EXE header
    	public ushort   e_magic;                     // 00 Magic number
    	public ushort   e_cblp;                      // 02 Bytes on last page of file
    	public ushort   e_cp;                        // 04 Pages in file
    	public ushort   e_crlc;                      // 06 Relocations
    	public ushort   e_cparhdr;                   // 08 Size of header in paragraphs
    	public ushort   e_minalloc;                  // 0A Minimum extra paragraphs needed
    	public ushort   e_maxalloc;                  // 0C Maximum extra paragraphs needed
    	public ushort   e_ss;                        // 0E Initial (relative) SS value
    	public ushort   e_sp;                        // 10 Initial SP value
    	public ushort   e_csum;                      // 12 Checksum
    	public ushort   e_ip;                        // 14 Initial IP value
    	public ushort   e_cs;                        // 16 Initial (relative) CS value
    	public ushort   e_lfarlc;                    // 18 File address of relocation table
    	public ushort   e_ovno;                      // 1A Overlay number
    	public ushort[] e_res;                       // 1C Reserved words size 4
    	public ushort   e_oemid;                     // OEM identifier (for e_oeminfo)
    	public ushort   e_oeminfo;                   // OEM information; e_oemid specific
    	public ushort[] e_res2;                      // Reserved words size 10
    	public uint     e_lfanew;                    // File address of new exe header

	}//struct IMAGE_DOS_HEADER

	public struct IMAGE_NT_HEADERS32
	{
		public uint Signature;
	    public IMAGE_FILE_HEADER FileHeader;
	    public IMAGE_OPTIONAL_HEADER32 OptionalHeader;
	}//struct IMAGE_NT_HEADERS32

	public struct IMAGE_FILE_HEADER
	{
	  public ushort  Machine;
	  public ushort  NumberOfSections;
	  public ulong   TimeDateStamp;
	  public ulong   PointerToSymbolTable;
	  public ulong   NumberOfSymbols;
	  public ushort  SizeOfOptionalHeader;
    public ushort  Characteristics;
	}//struct IMAGE_FILE_HEADER

	public struct IMAGE_OPTIONAL_HEADER32
	{
		public ushort  Magic;
		public byte   MajorLinkerVersion;
		public byte   MinorLinkerVersion;
		public uint   SizeOfCode;
		public uint   SizeOfInitializedData;
		public uint   SizeOfUninitializedData;
		public uint   AddressOfEntryPoint;
		public uint   BaseOfCode;
		public uint   BaseOfData;

		//
		// NT additional fields.
		//

		public uint   ImageBase;
		public uint   SectionAlignment;
		public uint   FileAlignment;
		public ushort  MajorOperatingSystemVersion;
		public ushort  MinorOperatingSystemVersion;
		public ushort  MajorImageVersion;
		public ushort  MinorImageVersion;
		public ushort  MajorSubsystemVersion;
		public ushort  MinorSubsystemVersion;
		public uint   Win32VersionValue;
		public uint   SizeOfImage;
		public uint   SizeOfHeaders;
		public uint   CheckSum;
		public ushort  Subsystem;
		public ushort  DllCharacteristics;
		public uint   SizeOfStackReserve;
		public uint   SizeOfStackCommit;
		public uint   SizeOfHeapReserve;
		public uint   SizeOfHeapCommit;
		public uint   LoaderFlags;
		public uint   NumberOfRvaAndSizes;
		public IMAGE_DATA_DIRECTORY[] DataDirectory;
	}//struct IMAGE_OPTIONAL_HEADER32


	public struct IMAGE_DATA_DIRECTORY
	{
    public uint   VirtualAddress;
    public uint   Size;
  }//struct IMAGE_DATA_DIRECTORY


	public struct IMAGE_SECTION_HEADER
	{
    	public byte[]   Name;  // size is 8
        public uint   PhysicalAddress;
        public uint   VirtualSize;
		public uint   VirtualAddress;
		public uint   SizeOfRawData;
		public uint   PointerToRawData;
		public uint   PointerToRelocations;
		public uint   PointerToLinenumbers;
		public ushort  NumberOfRelocations;
		public ushort  NumberOfLinenumbers;
		public uint   Characteristics;
	}//struct IMAGE_SECTION_HEADER


	public struct IMAGE_COR20_HEADER
	{
		// Header versioning
		public uint			cb;
		public ushort			MajorRuntimeVersion;
		public ushort			MinorRuntimeVersion;

		// Symbol table and startup information
		public IMAGE_DATA_DIRECTORY    MetaData;
		public uint            Flags;
		public uint            EntryPointToken;

		// Binding information
		public IMAGE_DATA_DIRECTORY    Resources;
		public IMAGE_DATA_DIRECTORY    StrongNameSignature;

		// Regular fixup and binding information
		public IMAGE_DATA_DIRECTORY    CodeManagerTable;
		public IMAGE_DATA_DIRECTORY    VTableFixups;
		public IMAGE_DATA_DIRECTORY    ExportAddressTableJumps;

		// Precompiled image info (internal use only - set to zero)
		public IMAGE_DATA_DIRECTORY    ManagedNativeHeader;

	}//struct IMAGE_COR20_HEADER


	public struct STORAGESIGNATURE
	{
		public ulong lSignature;				// "Magic" signature.
		public ushort iMajorVer;				// Major file version.
		public ushort iMinorVer;				// Minor file version.
		public uint ExtraData;
		public uint   iVersionString;
		public string VersionString;
//		public ushort ignored;
//		public ushort count;

	}//struct STORAGESIGNATURE

	public struct STORAGEHEADER
	{
		public byte		fFlags;					// STGHDR_xxx flags.
		public byte		pad;
		public ushort	iStreams;				// How many streams are there.
	}//struct STORAGEHEADER


	public struct STORAGESTREAM
	{
		public const int MAXSTREAMNAME = 32;
		public ulong		iOffset;				// Offset in file for this stream.
		public ulong 		iSize;					// Size of the file.
		public char[]		rcName;	// Start of name, null terminated.
	}//struct STORAGESTREAM

}//namespace
