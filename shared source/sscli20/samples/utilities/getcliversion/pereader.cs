//------------------------------------------------------------------------------
// <copyright file="pereader.cs" company="Microsoft">
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
using System.IO;
using System.Collections;
using System.Globalization;
using System.Text;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters;
using System.Runtime.Serialization.Formatters.Binary;

namespace DevService.Util 
{

  public class PEReader
  {
    public IMAGE_DOS_HEADER dosHeader;
    public IMAGE_NT_HEADERS32 win32header;
    public IMAGE_SECTION_HEADER[] secheaders;
    public IMAGE_COR20_HEADER corheader;
    public STORAGESIGNATURE mdstorsig;
    public STORAGEHEADER  mdstorhdr;
    public STORAGESTREAM[] mdstreams;
    public ArrayList strings;
    public Stream fstream;
    public BinaryReader bread;
    public BinaryWriter bwrite;
    public ulong metadataOffset;
    public uint m_sigverMaxSize;

    public string GetSignatureVersion()
    {
      return mdstorsig.VersionString;
    }

    public void SetSignatureVersion(string verstring)
    {
      if (verstring.Length > m_sigverMaxSize)
      {
        throw new Exception("Version string will not fit");
      }

      bwrite.BaseStream.Seek((long)metadataOffset + 12, SeekOrigin.Begin);
      bwrite.Write(m_sigverMaxSize);

      byte[] tempstring = new byte[m_sigverMaxSize];

      for (int i = 0; i < tempstring.Length; i++)
      {
        tempstring[i] = 0;
      }

      for (int i = 0; i < verstring.Length; i++)
      {
        tempstring[i] = (byte)verstring[i];
      }

      bwrite.Write(tempstring);

    }

    public void Close()
    {
      fstream.Flush();
      fstream.Close();
    }

    public PEReader(string filename, bool fWrite)
    {
      strings = new ArrayList();
      fstream = File.Open(filename, FileMode.Open, 
        fWrite ? FileAccess.ReadWrite : FileAccess.Read, 
        fWrite ? FileShare.None : FileShare.Read);
      bread = new BinaryReader(fstream);
      
      if (fWrite)
        bwrite = new BinaryWriter(fstream);

      dosHeader.e_magic = (ushort)bread.ReadUInt16();                     // Magic number
      dosHeader.e_cblp = (ushort)bread.ReadUInt16();                      // Bytes on last page of file
      dosHeader.e_cp = (ushort)bread.ReadUInt16();                        // Pages in file
      dosHeader.e_crlc = (ushort)bread.ReadUInt16();                      // Relocations
      dosHeader.e_cparhdr = (ushort)bread.ReadUInt16();                   // Size of header in paragraphs
      dosHeader.e_minalloc = (ushort)bread.ReadUInt16();                  // Minimum extra paragraphs needed
      dosHeader.e_maxalloc = (ushort)bread.ReadUInt16();                  // Maximum extra paragraphs needed
      dosHeader.e_ss = (ushort)bread.ReadUInt16();                        // Initial (relative) SS value
      dosHeader.e_sp = (ushort)bread.ReadUInt16();                        // Initial SP value
      dosHeader.e_csum = (ushort)bread.ReadUInt16();                      // Checksum
      dosHeader.e_ip = (ushort)bread.ReadUInt16();                        // Initial IP value
      dosHeader.e_cs = (ushort)bread.ReadUInt16();                        // Initial (relative) CS value
      dosHeader.e_lfarlc = (ushort)bread.ReadUInt16();                    // File address of relocation table
      dosHeader.e_ovno = (ushort)bread.ReadUInt16();                      // Overlay number
      dosHeader.e_res = new ushort[4];                    // Reserved words size 4

      for (int i = 0; i < dosHeader.e_res.Length; i++)
      {
        dosHeader.e_res[i]  = (ushort)bread.ReadUInt16();
      }


      dosHeader.e_oemid = (ushort)bread.ReadUInt16();                     // OEM identifier (for e_oeminfo)
      dosHeader.e_oeminfo = (ushort)bread.ReadUInt16();                   // OEM information; e_oemid specific

      dosHeader.e_res2 = new ushort[10];                  // Reserved words size 10

      for (int i = 0; i < dosHeader.e_res2.Length; i++)
      {
        dosHeader.e_res2[i] = (ushort)bread.ReadUInt16();                  // Reserved words size 10
      }


      dosHeader.e_lfanew = (uint)bread.ReadUInt32();

#if VERBOSE
			Console.WriteLine(dosHeader.e_lfanew);
#endif
      bread.BaseStream.Seek(dosHeader.e_lfanew, SeekOrigin.Begin);


      win32header.Signature = (uint)bread.ReadUInt32();
      win32header.FileHeader.Machine = (ushort)bread.ReadUInt16();
      win32header.FileHeader.NumberOfSections = (ushort)bread.ReadUInt16();
      win32header.FileHeader.TimeDateStamp = (uint)bread.ReadUInt32();
      win32header.FileHeader.PointerToSymbolTable = (uint)bread.ReadUInt32();
      win32header.FileHeader.NumberOfSymbols = (uint)bread.ReadUInt32();
      win32header.FileHeader.SizeOfOptionalHeader =(ushort)bread.ReadUInt16();
      win32header.FileHeader.Characteristics = (ushort)bread.ReadUInt16();

#if VERBOSE
			Console.WriteLine(win32header.Signature);
			Console.WriteLine(win32header.FileHeader.Machine);
			Console.WriteLine(win32header.FileHeader.NumberOfSections);
			Console.WriteLine(win32header.FileHeader.TimeDateStamp);
			Console.WriteLine(win32header.FileHeader.PointerToSymbolTable);
			Console.WriteLine(win32header.FileHeader.NumberOfSymbols);
			Console.WriteLine(win32header.FileHeader.SizeOfOptionalHeader);
		 	Console.WriteLine(win32header.FileHeader.Characteristics);
#endif
      // Get the Current position of the stream
      long Optionalposition = bread.BaseStream.Position;
      secheaders = new IMAGE_SECTION_HEADER[win32header.FileHeader.NumberOfSections];


      // read the Optional Header

      win32header.OptionalHeader.Magic = (ushort)bread.ReadUInt16();
      win32header.OptionalHeader.MajorLinkerVersion = (byte)bread.ReadByte();
      win32header.OptionalHeader.MinorLinkerVersion = (byte)bread.ReadByte();
      win32header.OptionalHeader.SizeOfCode = (uint)bread.ReadUInt32();
      win32header.OptionalHeader.SizeOfInitializedData = (uint)bread.ReadUInt32();
      win32header.OptionalHeader.SizeOfUninitializedData = (uint)bread.ReadUInt32();
      win32header.OptionalHeader.AddressOfEntryPoint = (uint)bread.ReadUInt32();
      win32header.OptionalHeader.BaseOfCode = (uint)bread.ReadUInt32();
      win32header.OptionalHeader.BaseOfData = (uint)bread.ReadUInt32();

      //
      // NT additional fields.
      //

      win32header.OptionalHeader.ImageBase = (uint)bread.ReadUInt32();
      win32header.OptionalHeader.SectionAlignment = (uint)bread.ReadUInt32();
      win32header.OptionalHeader.FileAlignment = (uint)bread.ReadUInt32();
      win32header.OptionalHeader.MajorOperatingSystemVersion = (ushort)bread.ReadUInt16();
      win32header.OptionalHeader.MinorOperatingSystemVersion = (ushort)bread.ReadUInt16();
      win32header.OptionalHeader.MajorImageVersion = (ushort)bread.ReadUInt16();
      win32header.OptionalHeader.MinorImageVersion = (ushort)bread.ReadUInt16();
      win32header.OptionalHeader.MajorSubsystemVersion = (ushort)bread.ReadUInt16();
      win32header.OptionalHeader.MinorSubsystemVersion = (ushort)bread.ReadUInt16();
      win32header.OptionalHeader.Win32VersionValue = (uint)bread.ReadUInt32();
      win32header.OptionalHeader.SizeOfImage = (uint)bread.ReadUInt32();
      win32header.OptionalHeader.SizeOfHeaders = (uint)bread.ReadUInt32();
      win32header.OptionalHeader.CheckSum = (uint)bread.ReadUInt32();
      win32header.OptionalHeader.Subsystem = (ushort)bread.ReadUInt16();
      win32header.OptionalHeader.DllCharacteristics = (ushort)bread.ReadUInt16();
      win32header.OptionalHeader.SizeOfStackReserve = (uint)bread.ReadUInt32();
      win32header.OptionalHeader.SizeOfStackCommit = (uint)bread.ReadUInt32();
      win32header.OptionalHeader.SizeOfHeapReserve = (uint)bread.ReadUInt32();
      win32header.OptionalHeader.SizeOfHeapCommit = (uint)bread.ReadUInt32();
      win32header.OptionalHeader.LoaderFlags = (uint)bread.ReadUInt32();
      win32header.OptionalHeader.NumberOfRvaAndSizes = (uint)bread.ReadUInt32();

      // read the Data DIRS
      win32header.OptionalHeader.DataDirectory = new IMAGE_DATA_DIRECTORY[(int)win32header.OptionalHeader.NumberOfRvaAndSizes];

      for(int i = 0; i < win32header.OptionalHeader.DataDirectory.Length; i++)
      {
        win32header.OptionalHeader.DataDirectory[i].VirtualAddress = (uint)bread.ReadUInt32();
        win32header.OptionalHeader.DataDirectory[i].Size = (uint)bread.ReadUInt32();
      }

      if (win32header.OptionalHeader.NumberOfRvaAndSizes <13)// we have metadata
        throw new Exception("No metadata");

#if VERBOSE
				Console.WriteLine("We have metadata");
#endif

      for(int i = 0; i < win32header.OptionalHeader.DataDirectory.Length; i++)
      {
#if VERBOSE
				Console.WriteLine(i);
				Console.WriteLine(win32header.OptionalHeader.DataDirectory[i].VirtualAddress);
				Console.WriteLine(win32header.OptionalHeader.DataDirectory[i].Size);
#endif
      }

      // Seek to the First Section
      bread.BaseStream.Seek(Optionalposition + win32header.FileHeader.SizeOfOptionalHeader, SeekOrigin.Begin);

      // Read the section headers

      for (int i = 0; i < secheaders.Length; i++)
      {
        secheaders[i].Name = new byte[8];  // size is 8

        for (int x = 0; x < secheaders[i].Name.Length; x++)
        {
          secheaders[i].Name[x] = bread.ReadByte();
        }
        secheaders[i].PhysicalAddress = (uint)bread.ReadUInt32();
        secheaders[i].VirtualSize = secheaders[i].PhysicalAddress;
        secheaders[i].VirtualAddress = (uint)bread.ReadUInt32();
        secheaders[i].SizeOfRawData = (uint)bread.ReadUInt32();
        secheaders[i].PointerToRawData = (uint)bread.ReadUInt32();
        secheaders[i].PointerToRelocations = (uint)bread.ReadUInt32();
        secheaders[i].PointerToLinenumbers = (uint)bread.ReadUInt32();
        secheaders[i].NumberOfRelocations = (ushort)bread.ReadUInt16();
        secheaders[i].NumberOfLinenumbers = (ushort)bread.ReadUInt16();
        secheaders[i].Characteristics = (uint)bread.ReadUInt32();
      }

      // Get the offset to the COR HEADER
      ulong corheaderoffset  = RVAtoOffset(win32header.OptionalHeader.DataDirectory[14].VirtualAddress);

      bread.BaseStream.Seek((long)corheaderoffset, SeekOrigin.Begin);

      corheader.cb = (uint)bread.ReadUInt32();
      corheader.MajorRuntimeVersion = (ushort)bread.ReadUInt16();
      corheader.MinorRuntimeVersion = (ushort)bread.ReadUInt16();

      // Symbol table and startup information
      corheader.MetaData.VirtualAddress = (uint)bread.ReadUInt32();
      corheader.MetaData.Size = (uint)bread.ReadUInt32();

      corheader.Flags = (uint)bread.ReadUInt32();
      corheader.EntryPointToken = (uint)bread.ReadUInt32();

      // Binding information
      corheader.Resources.VirtualAddress = (uint)bread.ReadUInt32();
      corheader.Resources.Size = (uint)bread.ReadUInt32();

      corheader.StrongNameSignature.VirtualAddress = (uint)bread.ReadUInt32();
      corheader.StrongNameSignature.Size = (uint)bread.ReadUInt32();

      // Regular fixup and binding information
      corheader.CodeManagerTable.VirtualAddress = (uint)bread.ReadUInt32();
      corheader.CodeManagerTable.Size = (uint)bread.ReadUInt32();

      corheader.VTableFixups.VirtualAddress = (uint)bread.ReadUInt32();
      corheader.VTableFixups.Size = (uint)bread.ReadUInt32();

      corheader.ExportAddressTableJumps.VirtualAddress = (uint)bread.ReadUInt32();
      corheader.ExportAddressTableJumps.Size = (uint)bread.ReadUInt32();

      // Precompiled image info (internal use only - set to zero)
      //corheader.IMAGE_DATA_DIRECTORY    ManagedNativeHeader;

      metadataOffset = RVAtoOffset(corheader.MetaData.VirtualAddress);
#if VERBOSE
			Console.WriteLine("Metadata Address " + corheader.MetaData.VirtualAddress );
			Console.WriteLine("Metadata offset" + metadataOffset);
#endif


      bread.BaseStream.Seek((long)metadataOffset, SeekOrigin.Begin);

      mdstorsig.lSignature = (uint)bread.ReadUInt32();				// "Magic" signature.
      mdstorsig.iMajorVer = (ushort)bread.ReadUInt16();				// Major file version.
      mdstorsig.iMinorVer = (ushort)bread.ReadUInt16();				// Minor file version.
      mdstorsig.ExtraData      = (uint)bread.ReadUInt32();
      mdstorsig.iVersionString  = (uint)bread.ReadUInt32();
      uint siglen = mdstorsig.iVersionString;
      if (mdstorsig.iVersionString %4 != 0)
      {
        siglen += 4 - (mdstorsig.iVersionString%4);
      }
      m_sigverMaxSize = siglen;

      byte[] sigtemp;// = new byte[siglen];
      sigtemp = bread.ReadBytes((int)siglen);
      char[] sigchar = new char[mdstorsig.iVersionString];

      for (int z = 0; z < mdstorsig.iVersionString; z++)
      {
        sigchar[z] = (char)sigtemp[z];
      }

      mdstorsig.VersionString = new string(sigchar);


#if VERBOSE
			Console.WriteLine("Metadata");
			Console.WriteLine(mdstorsig.lSignature);
			Console.WriteLine(mdstorsig.iMajorVer);
			Console.WriteLine(mdstorsig.iMinorVer);


#endif

      mdstorhdr.fFlags = (byte)bread.ReadByte();					// STGHDR_xxx flags.
      mdstorhdr.pad = (byte)bread.ReadByte();
      mdstorhdr.iStreams = (ushort)bread.ReadUInt16();				// How many streams are there.

#if VERBOSE

			Console.WriteLine("Metadata Storage");
			Console.WriteLine(mdstorhdr.fFlags);
			Console.WriteLine(mdstorhdr.pad);
			Console.WriteLine(mdstorhdr.iStreams);
#endif

      mdstreams = new STORAGESTREAM[mdstorhdr.iStreams];

      for(int i = 0; i < mdstreams.Length; i++)
      {
        byte temp = 0;
        int idx = 0;
        mdstreams[i].iOffset = (uint)bread.ReadUInt32();				// Offset in file for this stream.
        mdstreams[i].iSize = (uint)bread.ReadUInt32();					// Size of the file.
        mdstreams[i].rcName = new char[STORAGESTREAM.MAXSTREAMNAME];	// Start of name, null terminated.
        while((temp = bread.ReadByte()) != 0)
        {
          mdstreams[i].rcName[idx] = (char)temp;
          idx++;
        }

        if (bread.BaseStream.Position%4 != 0)
        {
          bread.BaseStream.Position += (4-(bread.BaseStream.Position%4));
        }
      }

#if VERBOSE

			Console.WriteLine("Metadata Streams");
			for (int i = 0; i < mdstorhdr.iStreams; i++)
			{
				Console.WriteLine(new string(mdstreams[i].rcName));
				Console.WriteLine(mdstreams[i].iOffset);
				Console.WriteLine(mdstreams[i].iSize);

			}

#endif

    }

    public ulong RVAtoOffset(ulong rva)
    {
      for(int i = 0; i < secheaders.Length; i++)
      {
        if (secheaders[i].VirtualAddress <= rva && secheaders[i].VirtualAddress + secheaders[i].SizeOfRawData >rva)
        {
          return (secheaders[i].PointerToRawData + (rva - secheaders[i].VirtualAddress));
        }
      }

      throw new Exception("RVA is not in image");
    }
  }

}


public class StringData
{
  public string m_string;
  public long m_offset;	// This does not include the ending null

  public override string ToString()
  {
    return m_string + " : " + m_offset;
  }
}
