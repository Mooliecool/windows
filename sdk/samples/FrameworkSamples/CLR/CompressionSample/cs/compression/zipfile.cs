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

namespace Microsoft.Sample.Compression
{
	/// <summary>
	/// This class represents a Zip archive.  You can ask for the contained
	/// entries, or get an uncompressed file for a file entry.  
	/// </summary>
	public class ZipFile
	{
		List<ZipEntry> zipEntries;		// The collection of entries
		private ZipReader thisReader;
		private ZipWriter thisWriter;

		private Stream baseStream;		// Stream to which the writer writes 
										// both header and data, the reader
										// reads this
		private string zipName;

		/// <summary>
		/// Created a new Zip file with the given name.
		/// </summary>
		/// <param name="method"> Gzip or deflate</param>
		/// <param name="name"> Zip name</param>
		public ZipFile(string name, byte method, FileMode mode)
		{
			try
			{
				zipName = name;
				
				baseStream = new FileStream(zipName, mode);
				thisWriter = new ZipWriter(baseStream);
				thisWriter.Method = method;

				//New File
				thisWriter.WriteSuperHeader(0, method);

				int index1 = zipName.IndexOf(ZipConstants.Dot);
				int index2 = zipName.LastIndexOf(ZipConstants.BackSlash);
				thisReader = new ZipReader(baseStream, zipName.Substring(index2,
						index1 - index2));

				zipEntries = thisReader.GetAllEntries();
				CompressionForm.statusMessage =
					String.Format(
					System.Threading.Thread.CurrentThread.CurrentUICulture,
					ZipConstants.NewMessage, name);
			}
			catch (System.IO.IOException)
			{
				ZipConstants.ShowError(ZipConstants.IOError);
			}
		}

		/// <summary>
		/// Opens a Zip file with the given name.
		/// </summary>
		/// <param name="name"> Zip name</param>
		public ZipFile(string name)
		{
			try
			{
				zipName = name;
				baseStream = new FileStream(zipName, FileMode.Open);
				thisWriter = new ZipWriter(baseStream);

				int index1 = zipName.IndexOf(ZipConstants.Dot);
				int index2 = zipName.LastIndexOf(ZipConstants.BackSlash);
				thisReader = new ZipReader(baseStream, zipName.Substring(index2,
						index1 - index2));

				zipEntries = thisReader.GetAllEntries();
				thisWriter.Method = thisReader.Method;
				if (CompressionForm.statusMessage != String.Empty)
					CompressionForm.statusMessage =
					String.Format(
					System.Threading.Thread.CurrentThread.CurrentUICulture, 
					ZipConstants.OpenMessage, name);
			}		
			catch (IOException)
			{
				ZipConstants.ShowError("Error opening the file");
			}
			catch (ArgumentOutOfRangeException)
			{
				ZipConstants.ShowError(ZipConstants.CorruptedError);
			}
		}


		/// <summary>
		/// Gets offset to which the jump should be made by summing up 
		/// all the individual lengths.
		/// </summary>
		/// <returns>
		/// the offset from SeekOrigin.Begin
		/// </returns>
		private long GetOffset(int index)
		{
			if (index > zipEntries.Count)
				return -1;
			int jump = ZipConstants.SuperHeaderSize;
			int i;
			for (i = 0; i < index - 1; ++i)
			{
				ZipEntry entry = zipEntries[i];
				jump += ZipConstants.FixedHeaderSize + entry.NameLength 
					+ entry.CompressedSize;
			}
			return jump;
		}

		public void Add(string fileName) {
			System.Globalization.CultureInfo ci = 
				System.Threading.Thread.CurrentThread.CurrentUICulture;
			if (fileName.ToLower(ci).Equals(zipName.ToLower(ci)))
			{
				ZipConstants.ShowError("Cannot add the current xip file");
				CompressionForm.statusMessage = String.Empty;
				return;
			}
			ZipEntry entry = new ZipEntry(fileName);
			thisWriter.Add(entry);

			if (CompressionForm.statusMessage.Length != 0)
			{
				zipEntries.Add(entry);
				thisWriter.CloseHeaders((Int16)zipEntries.Count);
			}
		}

		public void Extract(int index, string path) {
			
			if(index < 0 || index >= zipEntries.Count)	
			{
				ZipConstants.ShowError("Argument out of range" +
					"exception");
				return;
			}
			thisReader.Extract(zipEntries[index], GetOffset(index + 1), 
					path);
		}

		public void ExtractAll(string path) {
			thisReader.ExtractAll(zipEntries, path);
		}

		/// <summary>
		/// Closes the ZipFile.  This also closes all input streams given by
		/// this class.  After this is called, no further method should be
		/// called.
		/// </summary>
		public void Close()
		{
			if(baseStream != null)
				baseStream.Close();
		}

		/// <summary>
		/// Gets the entries of compressed files.
		/// </summary>
		/// <returns>
		/// Collection of ZipEntries
		/// </returns>
		public List<ZipEntry> Entries
		{
			get
			{
				return zipEntries;
			}
		}

		public byte CompressionMethod()
		{
			return thisWriter.Method;
		}

		public int CheckFileExists(string fileName)
		{
			System.Globalization.CultureInfo ci =
						System.Threading.Thread.CurrentThread.CurrentUICulture;
			int i = -1;
			foreach (ZipEntry eachEntry in zipEntries)
			{
				++i;
				if (eachEntry.Name.ToLower(ci).Equals(fileName.ToLower(ci)))
				{
					return i;
				}
			}
			return -1;
		}

		/// <summary>
		/// Remove an entry from the archive
		/// </summary>
		/// <param name="index">
		/// The index of the entry that is to be removed
		/// </param>
		private void DeleteEntryFromFile(int index)
		{
			long jump = ZipConstants.SuperHeaderSize;
			for(int i = 0; i < index; ++i)
			{
				jump += ZipConstants.FixedHeaderSize +
					zipEntries[i].NameLength +
					zipEntries[i].CompressedSize;
			}
			ZipEntry entry = zipEntries[index];
			long fileJump = ZipConstants.FixedHeaderSize +
					entry.NameLength +
					entry.CompressedSize;
			baseStream.Seek(jump + fileJump, SeekOrigin.Begin);
			long length = baseStream.Length - fileJump - jump;
			byte[] b = new byte[length];
			baseStream.Read(b, 0, (int)length);
			baseStream.Seek(jump, SeekOrigin.Begin);
			baseStream.Write(b, 0, (int)length);
			baseStream.SetLength(baseStream.Length - fileJump);
			CompressionForm.statusMessage = "Removed successfully";
		}

		/// <summary>
		/// Remove an entry from the archive
		/// </summary>
		/// <param name="index">
		/// The index of the entry that is to be removed
		/// </param>
		public void Remove(int index)
		{
			long jump = ZipConstants.SuperHeaderSize;
			for (int i = 0; i < index; ++i)
			{
				jump += ZipConstants.FixedHeaderSize +
					zipEntries[i].NameLength +
					zipEntries[i].CompressedSize;
			}
			thisWriter.Remove(jump, zipEntries[index]);
			zipEntries.RemoveAt(index);
			if (CompressionForm.statusMessage.Length != 0)
			{
				thisWriter.CloseHeaders((Int16)zipEntries.Count);
			}
		}


	}
}
