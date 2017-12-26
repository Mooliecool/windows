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

namespace Microsoft.Sample.Compression
{
	/// <summary>
	/// This class represents a member of a zip archive.  ZipReader and
	/// ZipWriter will give instances of this class as information
	/// about the members in an archive.  
	/// </summary>

	public class ZipEntry 
	{
		//Private fields
		private Int32 size;
		private Int32 compressedSize;
		private Int32 dosTime;			//Time represented as an Int
		private Int16 nameLength;		//Length of the variable sized name
		private byte[] crc;             //Array of 16 CRC bytes
		private string name;

		/// <summary>
		/// Creates a zip entry with the given name.
		/// </summary>
		/// <param name="name">
		/// the name. May include directory components separated by '/'.
		/// </param>
		public ZipEntry(string name)
		{
			if (name == null) 
			{
				//Wrong entry
				throw new System.ArgumentNullException();
			}
			this.DateTime  = System.DateTime.Now;
			this.Name = name;
			this.size = 0;
			this.compressedSize = 0;
			this.crc = new byte[16];
		}
		

		public Int32 DosTime 
		{
			get 
			{
				return dosTime ;
			}
			set
			{
				this.dosTime = value;
			}
		}

		public Int16 NameLength
		{
			get
			{
				return nameLength;
			}
			set
			{
				//Check if the value is greater than 16 bytes
				if ((UInt16)value > 0xffff)
					throw new ArgumentOutOfRangeException();
				this.nameLength = value;
			}
		}

		/// <summary>
		/// Gets/Sets the time of last modification of the entry.
		/// </summary>
		public DateTime DateTime 
		{
			get 
			{
				int sec  = 2 * (dosTime & 0x1f);
				int min  = (dosTime >> 5) & 0x3f;
				int hrs  = (dosTime >> 11) & 0x1f;
				int day  = (dosTime >> 16) & 0x1f;
				int mon  = ((dosTime >> 21) & 0xf);
				int year = ((dosTime >> 25) & 0x7f) + 1980; /* since 1900 */
				return new System.DateTime(year, mon, day, hrs, min, sec);
			}
			set 
			{
				DosTime = ((Int32)value.Year - 1980 & 0x7f) << 25 | 
				          ((Int32)value.Month) << 21 |
				          ((Int32)value.Day) << 16 |
				          ((Int32)value.Hour) << 11 |
				          ((Int32)value.Minute) << 5 |
				          ((Int32)value.Second) >> 1;
			}
		}
		
		/// <summary>
		/// Returns the entry name.  The path components in the entry are
		/// always separated by slashes ('/').
		/// </summary>
		public string Name 
		{
			get 
			{
				return name;
			}
			set
			{
				//Check if the value is greater than 16 bytes or null
				if (value == null || value.Length > 0xffffL)
					throw new ArgumentOutOfRangeException();

				if (value.Length != 0)
				{
					name = value;
					nameLength = (Int16)value.Length;
				}
			}
		}

		/// <summary>
		/// Gets/Sets the size of the uncompressed data.
		/// </summary>
		/// <exception cref="System.ArgumentException">
		/// if size is not in 0..0xffffffffL
		/// </exception>
		/// <returns>
		/// the size 
		/// </returns>
		public Int32 Size 
		{
			get 
			{
				return size;
			}
			set  
			{
				//Check if the value is greater than 32 bytes
				if ((UInt32)value > 0xffffffffL)
					throw new ArgumentOutOfRangeException();

				this.size  = (Int32)value;
			}
		}
		/// <summary>
		/// Gets/Sets the size of the compressed data.
		/// </summary>
		/// <exception cref="System.ArgumentOutOfRangeException">
		/// if csize is not in 0..0xffffffffUL
		/// </exception>
		/// <returns>
		/// the compressed size.
		/// </returns>
		public Int32 CompressedSize 
		{
			get 
			{
				return compressedSize;
			}
			set 
			{
				//Check if the value is greater than 32 bytes
				if ((UInt32)value > 0xffffffffUL)
					throw new ArgumentOutOfRangeException();

				this.compressedSize = (Int32)value;
				
			}
		}
		/// <summary>
		/// Gets/Sets the crc of the compressed data.
		/// </summary>
		/// <exception cref="System.ArgumentOutOfRangeException">
		/// if crc is not in 16 byte array
		/// </exception>
		/// <returns>
		/// the crc.
		/// </returns>
		public byte[] GetCrc()
		{
			return crc;
		}
		public void SetCrc(byte[] value)
		{
				//Check if the Length of value array is greater than 16
				if (value.Length != crc.Length)
					throw new ArgumentOutOfRangeException();

				crc = value;
		}
		
	}	
}
