/****************************** Module Header ******************************\
* Module Name:  ExecutableFile.cs
* Project:	    CSCheckEXEType
* Copyright (c) Microsoft Corporation.
* 
* This class represents an executable file. It could get the image file header,
* image optinal header and data directories from the image file. Form these 
* headers, we can get whether this is a console application, whether this is 
* a .Net application and whether this is a 32bit native application. For .NET
* application, it could generate the full display name like 
*  System, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089, processorArchitecture=MSIL
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
using System.IO;
using System.Text;
using System.Collections;
using System.Collections.Generic;

namespace CSCheckEXEType
{
    public class ExecutableFile
    {
        /// <summary>
        /// The path of the executable file.
        /// </summary>
        public string ExeFilePath { get; private set; }

        private IMAGE.IMAGE_FILE_HEADER imageFileHeader;

        /// <summary>
        /// The image file header.
        /// </summary>
        public IMAGE.IMAGE_FILE_HEADER ImageFileHeader
        {
            get { return imageFileHeader; }
        }

        private IMAGE.IMAGE_OPTIONAL_HEADER32 optinalHeader32;

        /// <summary>
        /// The image optinal header for 32bit executable file.
        /// </summary>
        public IMAGE.IMAGE_OPTIONAL_HEADER32 OptinalHeader32
        {
            get { return optinalHeader32; }
        }

        private IMAGE.IMAGE_OPTIONAL_HEADER64 optinalHeader64;

        /// <summary>
        /// The image optinal header for 64bit executable file.
        /// </summary>
        public IMAGE.IMAGE_OPTIONAL_HEADER64 OptinalHeader64
        {
            get { return optinalHeader64; }
        }

        private IMAGE.IMAGE_DATA_DIRECTORY_Values directoryValues;

        /// <summary>
        /// The data directories.
        /// </summary>
        public IMAGE.IMAGE_DATA_DIRECTORY_Values DirectoryValues
        {
            get { return directoryValues; }
        }

        /// <summary>
        /// Specify whether this is a console application. 
        /// </summary>
        public bool IsConsoleApplication
        {
            get
            {
                if (Is32bitImage)
                {
                    return optinalHeader32.Subsystem == 3;
                }
                else
                {
                    return optinalHeader64.Subsystem == 3;
                }
            }
        }

        /// <summary>
        /// Specify whether this is a .Net application. 
        /// </summary>
        public bool IsDotNetAssembly
        {
            get
            {
                return directoryValues.Values[IMAGE.IMAGE_DATA_DIRECTORY_Values.IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR].VirtualAddress != 0;
            }
        }

        /// <summary>
        /// Specify whether this is a 32bit application. 
        /// </summary>
        public bool Is32bitImage { get; private set; }



        public ExecutableFile(string filePath)
        {
            if (!File.Exists(filePath))
            {
                throw new ArgumentException("Process could not be found.");
            }

            this.ExeFilePath = filePath;

            this.ReadHeaders();
        }

        /// <summary>
        /// Read the headers from the image file.
        /// </summary>
        void ReadHeaders()
        {

            FileStream fs = null;
            BinaryReader binReader = null;

            try
            {
                fs = new FileStream(ExeFilePath, FileMode.Open, FileAccess.Read);
                binReader = new BinaryReader(fs);

                // Read the PE Header start offset.
                fs.Position = 0x3C;
                UInt32 headerOffset = binReader.ReadUInt32();

                // Check if the offset is invalid
                if (headerOffset > fs.Length - 5)
                {
                    throw new ApplicationException("Invalid Image Format");
                }

                // Read the PE file signature
                fs.Position = headerOffset;
                UInt32 signature = binReader.ReadUInt32();

                // 0x00004550 is the letters "PE" followed by two terminating zeroes.
                if (signature != 0x00004550)
                {
                    throw new ApplicationException("Invalid Image Format");
                }

                // Read the image file header. 
                this.imageFileHeader.Machine = binReader.ReadUInt16();
                this.imageFileHeader.NumberOfSections = binReader.ReadUInt16();
                this.imageFileHeader.TimeDateStamp = binReader.ReadUInt32();
                this.imageFileHeader.PointerToSymbolTable = binReader.ReadUInt32();
                this.imageFileHeader.NumberOfSymbols = binReader.ReadUInt32();
                this.imageFileHeader.SizeOfOptionalHeader = binReader.ReadUInt16();
                this.imageFileHeader.Characteristics = (IMAGE.IMAGE_FILE_Flag)binReader.ReadUInt16();

                // Determine whether this is a 32bit assembly.
                UInt16 magic = binReader.ReadUInt16();
                if (magic != 0x010B && magic != 0x020B)
                {
                    throw new ApplicationException("Invalid Image Format");
                }

                // Read the IMAGE_OPTIONAL_HEADER32 for 32bit application.
                if (magic == 0x010B)
                {
                    this.Is32bitImage = true;

                    this.optinalHeader32.Magic = magic;
                    this.optinalHeader32.MajorLinkerVersion = binReader.ReadByte();
                    this.optinalHeader32.MinorImageVersion = binReader.ReadByte();
                    this.optinalHeader32.SizeOfCode = binReader.ReadUInt32();
                    this.optinalHeader32.SizeOfInitializedData = binReader.ReadUInt32();
                    this.optinalHeader32.SizeOfUninitializedData = binReader.ReadUInt32();
                    this.optinalHeader32.AddressOfEntryPoint = binReader.ReadUInt32();
                    this.optinalHeader32.BaseOfCode = binReader.ReadUInt32();
                    this.optinalHeader32.BaseOfData = binReader.ReadUInt32();
                    this.optinalHeader32.ImageBase = binReader.ReadUInt32();
                    this.optinalHeader32.SectionAlignment = binReader.ReadUInt32();
                    this.optinalHeader32.FileAlignment = binReader.ReadUInt32();
                    this.optinalHeader32.MajorOperatingSystemVersion = binReader.ReadUInt16();
                    this.optinalHeader32.MinorOperatingSystemVersion = binReader.ReadUInt16();
                    this.optinalHeader32.MajorImageVersion = binReader.ReadUInt16();
                    this.optinalHeader32.MinorImageVersion = binReader.ReadUInt16();
                    this.optinalHeader32.MajorSubsystemVersion = binReader.ReadUInt16();
                    this.optinalHeader32.MinorSubsystemVersion = binReader.ReadUInt16();
                    this.optinalHeader32.Win32VersionValue = binReader.ReadUInt32();
                    this.optinalHeader32.SizeOfImage = binReader.ReadUInt32();
                    this.optinalHeader32.SizeOfHeaders = binReader.ReadUInt32();
                    this.optinalHeader32.CheckSum = binReader.ReadUInt32();
                    this.optinalHeader32.Subsystem = binReader.ReadUInt16();
                    this.optinalHeader32.DllCharacteristics = binReader.ReadUInt16();
                    this.optinalHeader32.SizeOfStackReserve = binReader.ReadUInt32();
                    this.optinalHeader32.SizeOfStackCommit = binReader.ReadUInt32();
                    this.optinalHeader32.SizeOfHeapReserve = binReader.ReadUInt32();
                    this.optinalHeader32.SizeOfHeapCommit = binReader.ReadUInt32();
                    this.optinalHeader32.LoaderFlags = binReader.ReadUInt32();
                    this.optinalHeader32.NumberOfRvaAndSizes = binReader.ReadUInt32();
                }

                // Read the IMAGE_OPTIONAL_HEADER64 for 64bit application.
                else
                {
                    this.Is32bitImage = false;

                    this.optinalHeader64.Magic = magic;
                    this.optinalHeader64.MajorLinkerVersion = binReader.ReadByte();
                    this.optinalHeader64.MinorImageVersion = binReader.ReadByte();
                    this.optinalHeader64.SizeOfCode = binReader.ReadUInt32();
                    this.optinalHeader64.SizeOfInitializedData = binReader.ReadUInt32();
                    this.optinalHeader64.SizeOfUninitializedData = binReader.ReadUInt32();
                    this.optinalHeader64.AddressOfEntryPoint = binReader.ReadUInt32();
                    this.optinalHeader64.BaseOfCode = binReader.ReadUInt32();
                    this.optinalHeader64.ImageBase = binReader.ReadUInt64();
                    this.optinalHeader64.SectionAlignment = binReader.ReadUInt32();
                    this.optinalHeader64.FileAlignment = binReader.ReadUInt32();
                    this.optinalHeader64.MajorOperatingSystemVersion = binReader.ReadUInt16();
                    this.optinalHeader64.MinorOperatingSystemVersion = binReader.ReadUInt16();
                    this.optinalHeader64.MajorImageVersion = binReader.ReadUInt16();
                    this.optinalHeader64.MinorImageVersion = binReader.ReadUInt16();
                    this.optinalHeader64.MajorSubsystemVersion = binReader.ReadUInt16();
                    this.optinalHeader64.MinorSubsystemVersion = binReader.ReadUInt16();
                    this.optinalHeader64.Win32VersionValue = binReader.ReadUInt32();
                    this.optinalHeader64.SizeOfImage = binReader.ReadUInt32();
                    this.optinalHeader64.SizeOfHeaders = binReader.ReadUInt32();
                    this.optinalHeader64.CheckSum = binReader.ReadUInt32();
                    this.optinalHeader64.Subsystem = binReader.ReadUInt16();
                    this.optinalHeader64.DllCharacteristics = binReader.ReadUInt16();
                    this.optinalHeader64.SizeOfStackReserve = binReader.ReadUInt64();
                    this.optinalHeader64.SizeOfStackCommit = binReader.ReadUInt64();
                    this.optinalHeader64.SizeOfHeapReserve = binReader.ReadUInt64();
                    this.optinalHeader64.SizeOfHeapCommit = binReader.ReadUInt64();
                    this.optinalHeader64.LoaderFlags = binReader.ReadUInt32();
                    this.optinalHeader64.NumberOfRvaAndSizes = binReader.ReadUInt32();
                }

                // Read the data directories.
                this.directoryValues = new IMAGE.IMAGE_DATA_DIRECTORY_Values();
                for (int i = 0; i < 16; i++)
                {
                    directoryValues.Values[i].VirtualAddress = binReader.ReadUInt32();
                    directoryValues.Values[i].Size = binReader.ReadUInt32();
                }
            }
            finally
            {

                // Release the IO resource.
                if (binReader != null)
                {
                    binReader.Close();
                }

                if (fs != null)
                {
                    fs.Close();
                }

            }
        }

        /// <summary>
        /// Get the full name of the .Net application.
        /// </summary>
        public string GetFullDisplayName()
        {
            if (!IsDotNetAssembly)
            {
                return ExeFilePath;
            }

            // Get the IReferenceIdentity interface.
            Fusion.IReferenceIdentity referenceIdentity =
               Fusion.NativeMethods.GetAssemblyIdentityFromFile(ExeFilePath,
               ref Fusion.NativeMethods.ReferenceIdentityGuid) as Fusion.IReferenceIdentity;
            Fusion.IIdentityAuthority IdentityAuthority = Fusion.NativeMethods.GetIdentityAuthority();  
      
            string fullName = IdentityAuthority.ReferenceToText(0, referenceIdentity);
            return fullName;
        }

        public Dictionary<string,string> GetAttributes()
        {
            if (!IsDotNetAssembly)
            {
                return null;
            }

            var attributeDictionary = new Dictionary<string, string>();

            // Get the IReferenceIdentity interface.
            Fusion.IReferenceIdentity referenceIdentity =
               Fusion.NativeMethods.GetAssemblyIdentityFromFile(ExeFilePath,
               ref Fusion.NativeMethods.ReferenceIdentityGuid) as Fusion.IReferenceIdentity;

            var enumAttributes = referenceIdentity.EnumAttributes();
            Fusion.IDENTITY_ATTRIBUTE[] IDENTITY_ATTRIBUTEs=new Fusion.IDENTITY_ATTRIBUTE[1024];

            enumAttributes.Next(1024, IDENTITY_ATTRIBUTEs);

            foreach (var IDENTITY_ATTRIBUTE in IDENTITY_ATTRIBUTEs)
            {
                if (!string.IsNullOrEmpty(IDENTITY_ATTRIBUTE.Name))
                {
                    attributeDictionary.Add(IDENTITY_ATTRIBUTE.Name, IDENTITY_ATTRIBUTE.Value);
                }
            }

            return attributeDictionary;
        }

        /// <summary>
        /// Gets the assembly's original .NET Framework compilation version 
        /// (stored in the metadata), given its file path. 
        /// </summary>
        public string GetCompiledRuntimeVersion()
        {
            if (!IsDotNetAssembly)
            {
                return ExeFilePath;
            }

            object metahostInterface=null;
            Hosting.NativeMethods.CLRCreateInstance(
                ref Hosting.NativeMethods.CLSID_CLRMetaHost,
                ref Hosting.NativeMethods.IID_ICLRMetaHost, 
                out metahostInterface);

            if (metahostInterface == null || !(metahostInterface is Hosting.IClrMetaHost))
            {
                throw new ApplicationException("Can not get IClrMetaHost interface.");
            }

            Hosting.IClrMetaHost ClrMetaHost = metahostInterface as Hosting.IClrMetaHost;
            StringBuilder buffer=new StringBuilder(1024);
            uint bufferLength=1024;          
            ClrMetaHost.GetVersionFromFile(this.ExeFilePath, buffer, ref bufferLength);
            string runtimeVersion = buffer.ToString();

            return runtimeVersion;
        }      
    }
}
