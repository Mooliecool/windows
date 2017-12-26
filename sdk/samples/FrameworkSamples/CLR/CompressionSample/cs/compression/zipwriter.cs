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
using System.IO;
using System.IO.Compression;

namespace Microsoft.Sample.Compression
{
	/// <summary>
	/// This is a Writer that writes the files into a zip
	/// archive one after another.  The zip entries contains 
	/// information about the file name, size, compressed size,
	/// CRC, etc.
	/// It includes support for GZIP and DEFLATE compression methods.
	/// </summary>
	public class ZipWriter
	{
		private Stream zipStream;		//The zip stream which is instantiated
										//and closed after every operation
		private Stream baseStream;		//The base stream from which the header 
										//and compressed data are read

		private long offset;
		private byte method;

		System.Security.Cryptography.MD5CryptoServiceProvider md5;
										//Required for creating CRC

		/// <summary>
		/// Creates a new Zip writer, used to write a zip archive.
		/// </summary>
		/// <param name="fileStream">
		/// the output stream to which the zip archive is written.
		/// </param>
		public ZipWriter(Stream fileStream)
		{
			baseStream = fileStream;
			md5 = new System.Security.Cryptography.MD5CryptoServiceProvider();
		}

		/// <summary>
		/// Super Header format
		/// </summary>
		/// <param name="number">Number of files in the archive</param>
		/// <param name="mode">Mode of zipping can be either GZip or Deflate</param>
		public void WriteSuperHeader(Int16 number, byte mode)
		{
			baseStream.Seek(0, SeekOrigin.Begin);
			WriteLeInt16(number);
			baseStream.WriteByte(mode);
		}

		/// <summary>
		/// Writes the superheader
		/// Resets the pointer back to the original position
		/// </summary>
		/// <param name="number">Number of files in the archive</param>

		private void WriteSuperHeader(Int16 number)
		{
			try{
				long pos = baseStream.Position;
				baseStream.Seek(0, SeekOrigin.Begin);
				WriteLeInt16(number);
				baseStream.Seek(pos, SeekOrigin.Begin);
			}
			catch (IOException)
			{
				ZipConstants.ShowError(ZipConstants.IOError);
			}
			catch (ArgumentException)
			{
				ZipConstants.ShowError(ZipConstants.SeekError);
			}
		}

		/// <summary>
		/// Writes a stream of bytes into the stream.
		/// </summary>
		private void WriteBytes(byte []value)
		{
			foreach(byte b in value)
				baseStream.WriteByte(b);
		}
		/// <summary>
		/// Write an int16 in little endian byte order.
		/// </summary>
		private void WriteLeInt16(Int16 value)
		{
			baseStream.WriteByte((byte)value);
			baseStream.WriteByte((byte)(value >> 8));
		}

		/// <summary>
		/// Write an int32 in little endian byte order.
		/// </summary>
		private void WriteLeInt32(Int32 value)
		{
			WriteLeInt16((Int16)value);
			WriteLeInt16((Int16)(value >> 16));
		}

		/// <summary>
		/// Puts the next header in a predefined order
		/// </summary>
		/// <param name="entry">
		/// the ZipEntry which contains all the information
		/// </param>
		private void PutNextHeader(ZipEntry entry)
		{
			try{
				WriteLeInt32(entry.Size);
				//REcord the offset to write proper CRC and compressed size
				offset = baseStream.Position;
				WriteLeInt32(entry.CompressedSize);
				WriteBytes(entry.GetCrc());
				WriteLeInt32(entry.DosTime);
				WriteLeInt16(entry.NameLength);
				byte[] names = ConvertToArray(entry.Name);
				baseStream.Write(names, 0, names.Length);
			}
			catch (IOException)
			{
				ZipConstants.ShowError(ZipConstants.IOError);
			}
			catch (ArgumentException)
			{
				ZipConstants.ShowError(ZipConstants.SeekError);
			}
		}

		/// <summary>
		/// Writes the compressed data into the basestream 
		/// It instantiates a memory stream which will serve 
		/// as a temp store and then compresses it using Gzip Stream or
		/// Deflate stream and writes it to the base stream
		/// </summary>
		private void WriteCompressedFile(FileStream fStream, ZipEntry entry)
		{
			MemoryStream ms = new MemoryStream();
			try
			{
				if (method == ZipConstants.DEFLATE)
					zipStream = new DeflateStream(ms, CompressionMode.Compress, 
						true);
				else if (method == ZipConstants.GZIP)
					zipStream = new GZipStream(ms, CompressionMode.Compress, 
						true);

				byte[] buffer = new byte[fStream.Length];
				fStream.Read(buffer, 0, buffer.Length);
				zipStream.Write(buffer, 0, buffer.Length);
				zipStream.Close();

				byte[] b = new byte[ms.Length];
				ms.Seek(0, SeekOrigin.Begin);
				ms.Read(b, 0, b.Length);
				baseStream.Write(b, 0, b.Length);
				//Go back and write the length and the CRC
				WriteCompressedSizeCRC((int)ms.Length, ComputeMD5(b), entry);
				
			}
			catch (IOException)
			{
				ZipConstants.ShowError(ZipConstants.IOError);
			}
			catch (ArgumentException)
			{
				ZipConstants.ShowError(ZipConstants.SeekError);
			}
			finally
			{
				ms.Close();
			}

		}

		private void WriteCompressedSizeCRC(Int32 value, byte[] crc, 
			ZipEntry entry)
		{
			try{
				entry.CompressedSize = value;
				entry.SetCrc(crc);
				baseStream.Seek(offset, SeekOrigin.Begin);
				WriteLeInt32(entry.CompressedSize);
				WriteBytes(crc);
				//Remove the recorded offset
				offset = -1;
				baseStream.Seek(0, SeekOrigin.End);
			}
			catch (IOException)
			{
				ZipConstants.ShowError(ZipConstants.IOError);
			}
			catch (ArgumentException)
			{
				ZipConstants.ShowError(ZipConstants.ArgumentError);
			}
	}

		private byte[] ConvertToArray(string name)
		{
			return System.Text.Encoding.ASCII.GetBytes(name);
		}

		/// <summary>
		/// Completes the header.This will update the superheader.
		/// </summary>
		/// <param name="numberOfFiles">
		/// The total number of files in the archive
		/// </param>
		public void CloseHeaders(Int16 numberOfFiles)
		{
			WriteSuperHeader(numberOfFiles);
		}

		/// <summary>
		/// Add a new entry to the zip
		/// </summary>
		/// <param name="entry">
		/// The details about the header of the entry
		/// </param>
		public void Add(ZipEntry entry)		{
			FileStream fs = null;
			try
			{
				fs = File.OpenRead(entry.Name);
				entry.Size = (Int32)fs.Length;
				entry.DateTime = File.GetLastWriteTime(entry.Name);
				PutNextHeader(entry);
				WriteCompressedFile(fs, entry);
				CompressionForm.statusMessage = ZipConstants.AddMessage;
			}
			catch (ArgumentOutOfRangeException)
			{
				ZipConstants.ShowError(ZipConstants.ArgumentError);
			}
			catch (ArgumentException)
			{
				
				ZipConstants.ShowError(ZipConstants.FileError);
			}
			catch (FileNotFoundException)
			{
				ZipConstants.ShowError(ZipConstants.FileNotFoundError);
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
		/// Sets default compression method. 
		/// </summary>
		/// <param name = "method">
		/// the Compression method which can be Gzip or deflate.
		/// </param>
		public byte Method
		{
			get
			{
				return method;
			}
			set
			{
				method = value;
			}
		}

		public byte[] ComputeMD5(byte[] input)
		{
			return md5.ComputeHash(input);
		}

		/// <summary>
		/// Remove an entry from the archive
		/// </summary>
		/// <param name="jump">
		/// The offset of the file to be removed
		/// </param>
		public void Remove(long jump, ZipEntry entry)
		{
			try{
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
		catch (ArgumentException)
		{

			ZipConstants.ShowError(ZipConstants.FileError);
		}
		catch (IOException)
		{
			ZipConstants.ShowError(ZipConstants.IOError);
		}
	}
	}
}
