//-----------------------------------------------------------------------
//  This file is part of the Microsoft .NET SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//-----------------------------------------------------------------------
using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;

namespace Microsoft.Sample.Compression
{
	/// <summary>
	/// This is a Reader that reads the files baseStream to get an 
	/// entry in the zip archive one after another.The zip entry contains 
	/// information about the file name, size, compressed size, CRC, etc.
	/// It includes support for GZIP and DEFLATE methods of compression.
	/// <summary>
	public class ZipReader
	{
		private Stream zipStream;		//The zip stream which is instantiated
										//and closed after every operation
		
		private string zipName;			//Name given to the archive
		private Stream baseStream;		//The base stream from which the header 
										//and compressed data are read

		private Int16 numberOfFiles;
		private byte method;

		System.Security.Cryptography.MD5CryptoServiceProvider md5;
										//Required for checking CRC

		/// <summary>
		/// Creates a new Zip input stream, reading a zip archive.
		/// </summary>
		public ZipReader(Stream fileStream, string name)
		{
			zipName = name;
			baseStream = fileStream;
			numberOfFiles = -1;
			method = 255;
			md5 = new System.Security.Cryptography.MD5CryptoServiceProvider();
		}

		/// <summary>
		/// Reads the super header
		/// Super header structure:
		///		number of files - 2 byte
		///		method of compression - 1 byte
		/// </summary>
		/// <exception cref="ArgumentOutOfRangeException">
		/// Thrown if the super header is tampered
		/// </exception>
		/// 
		private void ReadSuperHeader()
		{
			numberOfFiles = ReadLeInt16();
			method = ReadLeByte();
			if (method != ZipConstants.DEFLATE &&
				method != ZipConstants.GZIP)
				throw new ArgumentOutOfRangeException();
		}

		private int ReadBuf(byte[] outBuf, int length)
		{
			return baseStream.Read(outBuf, 0, length);
		}

		/// <summary>
		/// Read a byte from baseStream.
		/// </summary>
		private byte ReadLeByte()
		{
			return (byte)baseStream.ReadByte();
		}

		/// <summary>
		/// Read an unsigned short baseStream little endian byte order.
		/// </summary>
		private Int16 ReadLeInt16()
		{
			return (Int16)( ReadLeByte()|(ReadLeByte() << 8));
		}

		/// <summary>
		/// Read an int baseStream little endian byte order.
		/// </summary>
		private Int32 ReadLeInt32()
		{
			return (UInt16)ReadLeInt16() | ((UInt16)ReadLeInt16() << 16);
		}

		private string ConvertToString(byte[] data)
		{
			return System.Text.Encoding.ASCII.GetString(data, 0, data.Length);
		}

		/// <summary>
		/// Open the next entry from the zip archive, and return its 
		/// description. The method expects the pointer to be intact.
		/// </summary>
		private ZipEntry GetNextEntry()
		{
			ZipEntry currentEntry = null;
			try
			{
				Int32 size = ReadLeInt32();
				if (size == -1)
					return new ZipEntry(String.Empty);

				Int32 csize = ReadLeInt32();
				byte[] crc = new byte[16];
				ReadBuf(crc, crc.Length);

				Int32 dostime = ReadLeInt32();
				Int16 nameLength = ReadLeInt16();

				byte[] buffer = new byte[nameLength];
				ReadBuf(buffer, nameLength);
				string name = ConvertToString(buffer);

				currentEntry = new ZipEntry(name);
				currentEntry.Size = size;
				currentEntry.CompressedSize = csize;
				currentEntry.SetCrc(crc);
				currentEntry.DosTime = dostime;
			}
			catch (ArgumentException)
			{
				ZipConstants.ShowError(ZipConstants.ArgumentError);
			}
			catch (ObjectDisposedException)
			{
				ZipConstants.ShowError(ZipConstants.CloseError);
			}
			return currentEntry;
		}

		/// <summary>
		/// Writes the uncompressed data into the filename in the 
		/// entry. It instantiates a memory stream which will serve 
		/// as a temp store and decompresses it using Gzip Stream or
		/// Deflate stream
		/// </summary>
		private void WriteUncompressedFile(ZipEntry entry, string completePath)
		{
			MemoryStream ms = new MemoryStream();
			try
			{
				byte[] b = new byte[entry.CompressedSize];
				baseStream.Read(b, 0, (int)entry.CompressedSize);
				if (CheckCRC(entry.GetCrc(), b))
					ms.Write(b, 0, b.Length);
				ms.Seek(0, SeekOrigin.Begin);
				if (method == ZipConstants.DEFLATE)
					zipStream = new DeflateStream(ms, 
						CompressionMode.Decompress, false);
				else if (method == ZipConstants.GZIP)
					zipStream = new GZipStream(ms, CompressionMode.Decompress, 
						false);

				int index = entry.Name.LastIndexOf(ZipConstants.BackSlash);
				string name = completePath + entry.Name.Substring(index + 1);
				FileStream rewrite = new FileStream(name, FileMode.Create);
				b = new byte[entry.Size];
				zipStream.Read(b, 0, (int)entry.Size);

				rewrite.Write(b, 0, (int)entry.Size);
				rewrite.Close();
			}
			catch (IOException)
			{
				ZipConstants.ShowError(ZipConstants.IOError);
			}
			catch (ArgumentException)
			{
				ZipConstants.ShowError(ZipConstants.ArgumentError);
			}
			finally
			{
				zipStream.Close();
				ms.Close();
			}
		}

		/// <summary>
		/// Extracts all the entries in the list of entries
		///	</summary>
		/// <param name="zipentries">
		///	List of all the zip entries. Can be empty.
		/// </param>
		
		public void ExtractAll(List<ZipEntry> zipEntries, string startPath) {
			try
			{
				DirectoryInfo dir = new DirectoryInfo(startPath + zipName);
				if (!dir.Exists)
					dir.Create();
				
				int jump = 3;
				baseStream.Seek(jump, SeekOrigin.Begin);

				foreach (ZipEntry entry in zipEntries)
				{
					int index1 = entry.Name.IndexOf(ZipConstants.BackSlash);
					int index2 = entry.Name.LastIndexOf(ZipConstants.BackSlash);
					string relPath = entry.Name.Substring(index1 + 1, 
						index2 - index1);
					if (index1 == 0)
						relPath = String.Empty;

					if (relPath.Length != 0)
						dir.CreateSubdirectory(relPath);

					jump = ZipConstants.FixedHeaderSize + entry.NameLength;
					baseStream.Seek(jump, SeekOrigin.Current);
					WriteUncompressedFile(entry, startPath + zipName +
						ZipConstants.BackSlash + relPath);
				}
				CompressionForm.statusMessage = String.Format(
					System.Threading.Thread.CurrentThread.CurrentUICulture,
					ZipConstants.ExtractMessage, startPath + zipName +
					ZipConstants.BackSlash);
			}
			catch (IOException)
			{
				ZipConstants.ShowError(ZipConstants.IOError);
			}
			catch (OutOfMemoryException)
			{
				ZipConstants.ShowError(ZipConstants.MemoryError);
			}
		}

		/// <summary>
		/// Extracts the specified entry
		///	</summary>
		/// <param name="entry">
		///	The entry that is to be extracted.Cannot be null
		/// </param>
		/// <param name="jump">
		///	The offset from the SeekOrigin.Begin at which the 
		/// comrpessed data is located
		/// </param>
		
		public void Extract(ZipEntry entry, long jump, string startPath)
		{
			try{
				DirectoryInfo dir = new DirectoryInfo(startPath + zipName);
				if (!dir.Exists)
					dir.Create();
				int index1 = entry.Name.IndexOf(ZipConstants.BackSlash);
				int index2 = entry.Name.LastIndexOf(ZipConstants.BackSlash);
				string relPath = entry.Name.Substring(index1 + 1, 
					index2 - index1);
				if (index1 == 0)
					relPath = String.Empty;

				if(relPath.Length != 0)
					dir.CreateSubdirectory(relPath);
				baseStream.Seek(jump, SeekOrigin.Begin);
				jump = ZipConstants.FixedHeaderSize + entry.NameLength;
				baseStream.Seek(jump, SeekOrigin.Current);

				WriteUncompressedFile(entry, startPath + zipName + 
						ZipConstants.BackSlash + relPath);
				CompressionForm.statusMessage = String.Format(
					System.Threading.Thread.CurrentThread.CurrentUICulture,
					ZipConstants.ExtractMessage, startPath + zipName +
					ZipConstants.BackSlash);
			}
			catch(IOException)
			{
				ZipConstants.ShowError(ZipConstants.IOError);
			}
			catch (OutOfMemoryException)
			{
				ZipConstants.ShowError(ZipConstants.MemoryError);
			}
		}

		/// <summary>
		/// Gets all the entries in the file 
		/// </summary>
		/// <returns>
		///	List of all the zip entries
		/// </returns> 
		
		
		public List<ZipEntry> GetAllEntries()
		{
			List<ZipEntry> headers = null;
			try{
				if (method == 255 || numberOfFiles == -1)
				{
					baseStream.Seek(0, SeekOrigin.Begin);
					ReadSuperHeader();
				}
				headers = new List<ZipEntry>(numberOfFiles);
				baseStream.Seek(3, SeekOrigin.Begin);
				for (int i = 0; i < numberOfFiles; ++i)
				{
					ZipEntry entry = GetNextEntry();
					headers.Add(entry);
					baseStream.Seek(entry.CompressedSize, SeekOrigin.Current);
				}
			}
			catch (IOException)
			{
				ZipConstants.ShowError(ZipConstants.IOError);
			}
			
			return headers;
		}
		/// <summary>
		/// Gets the method of compression of the archive.
		/// </summary>
		/// <returns>
		/// the ZipConstants.Deflate or ZipConstants.Gzip
		/// </returns>
		public byte Method
		{
			get
			{
				return method;
			}
		}

		//Check the CRC of the byte array and return true if check successful
		//false otherwise
		private bool CheckCRC(byte[] crc, byte[] data)
		{
			byte[] newCrc = md5.ComputeHash(data);
			for (int i = 0; i < crc.Length; ++i)
			{
				if (crc[i] != newCrc[i])
					return false;
			}
			return true;
		}
	}
}
