/****************************** Module Header ******************************\
* Module Name:  GunShoot.cs
* Project:      CSSL3WriteableBitmap
* Copyright (c) Microsoft Corporation.
* 
* This example demonstrates how to scale two images of different size to the same resolution, as well as how to work with individual pixels.
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
using System.Windows.Media.Imaging;
using System.IO;

namespace CSSL3WriteableBitmap
{
	public partial class GunShoot : UserControl
	{
		private WriteableBitmap _writeableTop;
		private WriteableBitmap _writeableBottom;
		private static int _resultPixelWidth = 800;
		private static int _resultPixelHeight = 600;
		private static int _radius = 50;

		public GunShoot()
		{
			InitializeComponent();
			this._writeableTop = new WriteableBitmap(_resultPixelWidth, _resultPixelHeight);
			this._writeableBottom = new WriteableBitmap(_resultPixelWidth, _resultPixelHeight);
		}

		private void BrowseButton_Click(object sender, RoutedEventArgs e)
		{
			OpenFileDialog ofd = new OpenFileDialog();
			ofd.Multiselect = true;
			ofd.Filter = "Bitmap Images|*.png;*.jpg";
			if (ofd.ShowDialog().Value)
			{
				int fileCount = ofd.Files.Count();
				if (ofd.Files.Count() != 2)
				{
					MessageBox.Show("Please select at exactly 2 images.");
					return;
				}
				this._writeableBottom = new WriteableBitmap(_resultPixelWidth, _resultPixelHeight);
				int resolution = _resultPixelWidth * _resultPixelHeight;
				List<FileInfo> files = ofd.Files.ToList();
				this.ScaleImage(resolution, files[0], this._writeableBottom);
				this.ScaleImage(resolution, files[1], this._writeableTop);
				this.topImage.Source = this._writeableTop;
			}
		}

		/// <summary>
		/// Scales the source image to a fixed resolution.
		/// </summary>
		private void ScaleImage(int resolution, FileInfo file, WriteableBitmap writeableResult)
		{
			using (FileStream fs = file.OpenRead())
			{
				BitmapImage bmpSource = new BitmapImage();
				bmpSource.SetSource(fs);
				WriteableBitmap writeableSource = new WriteableBitmap(bmpSource);
				//Scale the source image to be the same size as the result image
				double ratioX = (double)writeableSource.PixelWidth / (double)writeableResult.PixelWidth;
				double ratioY = (double)writeableSource.PixelHeight / (double)writeableResult.PixelHeight;
				for (int i = 0; i < resolution; i++)
				{
					int yDest = i / _writeableBottom.PixelWidth;
					int xDest = i % _writeableBottom.PixelWidth;
					int y = (int)Math.Floor(yDest * ratioY);
					int x = (int)Math.Floor(xDest * ratioX);
					writeableResult.Pixels[i] = writeableSource.Pixels[y * writeableSource.PixelWidth + x];
				}
			}
		}

		private void topImage_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
		{
			Point clickedPoint = e.GetPosition(this.topImage);
			//Calculate the rectangle in which the gun shoots, so we don't need to iterate through all pixels.
			int topLeftX = (int)clickedPoint.X - _radius;
			if (topLeftX < 0)
			{
				topLeftX = 0;
			}
			int topLeftY = (int)clickedPoint.Y - _radius;
			if (topLeftY < 0)
			{
				topLeftY = 0;
			}
			int bottomRightX = (int)clickedPoint.X + _radius;
			if (bottomRightX > _resultPixelWidth)
			{
				bottomRightX = _resultPixelWidth;
			}
			int bottomRightY = (int)clickedPoint.Y + _radius;
			if (bottomRightY > _resultPixelHeight)
			{
				bottomRightY = _resultPixelHeight;
			}
			for (int y = topLeftY; y < bottomRightY; y++)
			{
				for (int x = topLeftX; x < bottomRightX; x++)
				{
					//Find the position of the current pixel in the pixel's array.
					int position = y * _resultPixelWidth + x;
					//The gun mark is a circle. So let's calculate the distance from the current point to the center point.
					double distance = Math.Sqrt(Math.Pow(clickedPoint.X - x, 2) + Math.Pow(clickedPoint.Y - y, 2));
					//If the distance is less then radius, the current point is in the gun mark's circle.
					if (distance <= _radius)
					{
						//The pixel in the top image.
						uint currentPixel = (uint)this._writeableTop.Pixels[position];
						//The pixel in the bottom image.
						uint blendPixel = (uint)this._writeableBottom.Pixels[position];
						//Let's draw a gun mark for 5 pixels.
						if (distance > _radius - 5)
						{
							//Obtain the RGB value of the pixels. The left side of the right shift operation must be uint, so that 0 will be filled when the operation is performed.
							uint currentPixelR = currentPixel << 8 >> 24;
							uint currentPixelG = currentPixel << 16 >> 24;
							uint currentPixelB = currentPixel << 24 >> 24;
							uint blendPixelR = blendPixel << 8 >> 24;
							uint blendPixelG = blendPixel << 16 >> 24;
							uint blendPixelB = blendPixel << 24 >> 24;
							//Make the pixel a little redder and reduce its alpha to represent the gun mark.
							currentPixelR += 0x00000066;
							if (currentPixelR > 0x000000ff)
							{
								currentPixelR = 0x000000ff;
							}
							double alpha = 0.3;
							//The standard formula to calculate a pixel's value: currentPixel * alpha + thePixelBelowIt * (1 - alpha).
							uint newPixelR = (uint)(currentPixelR * alpha + blendPixelR * (1d - alpha));
							uint newPixelG = (uint)(currentPixelG * alpha + blendPixelG * (1d - alpha));
							uint newPixelB = (uint)(currentPixelB * alpha + blendPixelB * (1d - alpha));
							uint newPixel= (newPixelR << 16) + (newPixelG << 8) + newPixelB + 0xff000000;
							this._writeableTop.Pixels[position] = (int)newPixel;
						}
						//For the center of the gun mark, just copy the bottom image's value.
						else
						{
							this._writeableTop.Pixels[position] = this._writeableBottom.Pixels[position];
						}
					}
				}
			}
			this.topImage.Source = this._writeableTop;
		}
	}
}
