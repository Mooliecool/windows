/****************************** Module Header ******************************\
* Module Name:  Screenshot.cs
* Project:      CSSL3WriteableBitmap
* Copyright (c) Microsoft Corporation.
* 
* This example demonstrates the screenshot feature of WriteableBitmap, as well as its limitations. It also implements a bmp encoder.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 9/15/2009 19:47 Yilun Luo Created
\***************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Windows.Threading;
using System.Windows.Media.Imaging;
using System.IO;

namespace CSSL3WriteableBitmap
{
	public partial class Screenshot : UserControl
	{
		private DispatcherTimer _timer;

		public Screenshot()
		{
			InitializeComponent();
		}

		private void UserControl_Loaded(object sender, System.Windows.RoutedEventArgs e)
		{
			this.sb.Begin();
			this._timer = new DispatcherTimer();
			this._timer.Interval = TimeSpan.FromMilliseconds(10);
			this._timer.Tick += new EventHandler(_timer_Tick);
			this._timer.Start();
		}

		void _timer_Tick(object sender, EventArgs e)
		{
			WriteableBitmap bmp = new WriteableBitmap(uiElementSource, uiElementSource.RenderTransform);
			this.img.Source = bmp;
		}

		/// <summary>
		/// Construct a bmp file from the screenshot. For information about bmp file format, please refer to http://en.wikipedia.org/wiki/BMP_file_format.
		/// Note the limitations of WriteableBitmap's screenshot feature: Do not support projection. If the image has a RenderTransform, all empty pixels are rendered with black (000000).
		/// </summary>
		private void SaveFileButton_Click(object sender, System.Windows.RoutedEventArgs e)
		{
			SaveFileDialog sfd = new SaveFileDialog();
			sfd.Filter = "Bitmap Images|*.bmp";
			if (sfd.ShowDialog().Value)
			{
				Stream stream = sfd.OpenFile();
				WriteableBitmap bmp = new WriteableBitmap(uiElementSource, uiElementSource.RenderTransform);
				byte[] header = new byte[54];
				// B
				header[0] = 0x42;
				// M
				header[1] = 0x4D;
				// Padding for 4 byte alignment (Could be a value other than zero).
				int padding = bmp.PixelWidth % 4;
				byte[] paddingData = new byte[padding];
				// Calculate the size of the bitmap. Note the data is stored in the reverse order in any binary file.
				int size = bmp.Pixels.Length * 3 + 54 + padding * bmp.PixelHeight;
				this.Write4ByteBinaryData(header, 2, size);
				// The offset where the bitmap data (pixels) can be found.
				this.Write4ByteBinaryData(header, 10, 54);
				// The number of bytes in the header (from this point).
				this.Write4ByteBinaryData(header, 14, 40);
				// The width and height of the bitmap in pixels
				this.Write4ByteBinaryData(header, 18, bmp.PixelWidth);
				this.Write4ByteBinaryData(header, 22, bmp.PixelHeight);
				// 1 plane being used.
				header[26] = 1;
				// The number of bits/pixel is 24. We do not take alpha into account.
				this.Write2ByteBinaryData(header, 28, 24);
				//The size of the raw BMP data (after this header)
				this.Write4ByteBinaryData(header, 34, 16);
				// The horizontal resolution of the image. 96 per inch is 3780 per meter.
				this.Write4ByteBinaryData(header, 38, 3780);
				// The vertical resolution of the image.
				this.Write4ByteBinaryData(header, 42, 3780);
				stream.Write(header, 0, 54);
				int x = 0;
				foreach (int pixel in bmp.Pixels)
				{
					stream.WriteByte((byte)(pixel << 24 >> 24));
					stream.WriteByte((byte)(pixel << 16 >> 24));
					stream.WriteByte((byte)(pixel << 8 >> 24));
					x++;
					if (x == bmp.PixelWidth)
					{
						stream.Write(paddingData, 0, padding);
						x = 0;
					}
				}
				stream.Close();
			}
		}

		/// <summary>
		/// Write the next 2 bytes with the integer data.
		/// </summary>
		private void Write2ByteBinaryData(byte[] header, int start, int data)
		{
			header[start] = (byte)(data << 24 >> 24);
			header[start + 1] = (byte)(data << 16 >> 24);
		}

		/// <summary>
		/// Write the next 4 bytes with the integer data.
		/// </summary>
		private void Write4ByteBinaryData(byte[] header, int start, int data)
		{
			header[start] = (byte)(data << 24 >> 24);
			header[start + 1] = (byte)(data << 16 >> 24);
			header[start + 2] = (byte)(data << 8 >> 24);
			header[start + 3] = (byte)(data >> 24);
		}
	}
}
