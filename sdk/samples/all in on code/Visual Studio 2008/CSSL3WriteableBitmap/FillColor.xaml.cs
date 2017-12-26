/****************************** Module Header ******************************\
* Module Name:  FillColor.cs
* Project:      CSSL3WriteableBitmap
* Copyright (c) Microsoft Corporation.
* 
* This example implements a common feature in painting programmers: Fill an area with a solid color. It demonstrates how to apply common algorithms to Silverlight WriteableBitmap, as well as how to avoid stack overflow in very deep recursive methods by simulating the way CLR invokes methods.
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

namespace CSSL3WriteableBitmap
{
	public partial class FillColor : UserControl
	{
		private BitmapSource _source = new BitmapImage(new Uri("Heart Empty.png", UriKind.Relative));
		private int _newColor;

		public FillColor()
		{
			InitializeComponent();
		}

		private void UserControl_Loaded(object sender, RoutedEventArgs e)
		{
			img.Source = _source;
		}

		private void img_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
		{
			WriteableBitmap bmp = new WriteableBitmap(_source);
			//Start from the clicked point.
			Point clickedPoint = e.GetPosition(img);
			//The position of the clicked point in the pixel array.
			int clickedColorPosition = bmp.PixelWidth * (int)clickedPoint.Y + (int)clickedPoint.X;
			//The color of the clicked point.
			int clickedColor = bmp.Pixels[clickedColorPosition];
			//Get the value of the new color from the sliders.
			this._newColor = ((int)this.alphaSlider.Value << 24) + ((int)this.redSlider.Value << 16) + ((int)this.greenSlider.Value << 8) + (int)this.blueSlider.Value;
			//The clicked point is already in the new color. Terminate the algorithm.
			if (clickedColor == this._newColor)
			{
				return;
			}
			//Paint the clicked point with the new color.
			bmp.Pixels[clickedColorPosition] = this._newColor;
			this.EdgeDetect(clickedPoint, clickedColorPosition, clickedColor, bmp);
			this._source = bmp;
			this.img.Source = bmp;
		}

		/// <summary>
		/// Implement the edge detect algorithm.
		/// </summary>
		/// <param name="clickedPoint">The clicked point.</param>
		/// <param name="clickedColorPosition">The clicked point's postion in the pixel array.</param>
		/// <param name="clickedColor">The clicked point's color.</param>
		/// <param name="bmp">The WriteableBitmap.</param>
		private void EdgeDetect(Point clickedPoint, int clickedColorPosition, int clickedColor, WriteableBitmap bmp)
		{
			//For large images, you may get stack overflow if you use a recursive method. So let's manually walk through the "call stack" instead.
			Stack<ParameterData> stack = new Stack<ParameterData>();
			//Think ParameterData is the parameter you pass to the recursive method. CLR will store all parameters on the call stack. Let's do the same.
			stack.Push(new ParameterData()
			{
				bmp = bmp,
				centerColorPosition = clickedColorPosition,
				centerPoint = clickedPoint
			});
			//The call stack is larger than 0. So more methods must be invoked.
			while (stack.Count > 0)
			{
				//The first step in executing a method is to pop the parameter from the call stack.
				ParameterData pd = stack.Pop();

				//Left
				if (pd.centerPoint.X >= 1)
				{
					Point leftPoint = new Point(pd.centerPoint.X - 1, pd.centerPoint.Y);
					int leftColorPosition = pd.centerColorPosition - 1;
					//Is the left point's color the same as the clicked point's color? If so, paint the left point with the new color. Otherwise, we don't need to continue left any more.
					if (bmp.Pixels[leftColorPosition] == clickedColor)
					{
						bmp.Pixels[leftColorPosition] = this._newColor;
						//Create a new parametar, push it to the call stack, so another method will be "invoked".
						ParameterData pdLeft = new ParameterData()
						{
							bmp = bmp,
							centerColorPosition = leftColorPosition,
							centerPoint = leftPoint
						};
						stack.Push(pdLeft);
					}
				}

				//Right
				if (pd.centerPoint.X <= bmp.PixelWidth - 2)
				{
					Point rightPoint = new Point(pd.centerPoint.X + 1, pd.centerPoint.Y);
					int rightColorPosition = pd.centerColorPosition + 1;
					if (bmp.Pixels[rightColorPosition] == clickedColor)
					{
						bmp.Pixels[rightColorPosition] = this._newColor;
						ParameterData pdRight = new ParameterData()
						{
							bmp = bmp,
							centerColorPosition = rightColorPosition,
							centerPoint = rightPoint
						};
						stack.Push(pdRight);
					}
				}

				//Top
				if (pd.centerPoint.Y >= 1)
				{
					Point topPoint = new Point(pd.centerPoint.X, pd.centerPoint.Y - 1);
					int topColorPosition = pd.centerColorPosition - bmp.PixelWidth;
					if (bmp.Pixels[topColorPosition] == clickedColor)
					{
						bmp.Pixels[topColorPosition] = this._newColor;
						ParameterData pdTop = new ParameterData()
						{
							bmp = bmp,
							centerColorPosition = topColorPosition,
							centerPoint = topPoint
						};
						stack.Push(pdTop);
					}
				}

				//Bottom
				if (pd.centerPoint.Y <= bmp.PixelHeight - 2)
				{
					Point bottomPoint = new Point(pd.centerPoint.X, pd.centerPoint.Y + 1);
					int bottomColorPosition = pd.centerColorPosition + bmp.PixelWidth;
					if (bmp.Pixels[bottomColorPosition] == clickedColor)
					{
						bmp.Pixels[bottomColorPosition] = this._newColor;
						ParameterData pdBottom = new ParameterData()
						{
							bmp = bmp,
							centerColorPosition = bottomColorPosition,
							centerPoint = bottomPoint
						};
						stack.Push(pdBottom);
					}
				}
			}
		}

		private void ColorSliderValueChanged(object sender, System.Windows.RoutedPropertyChangedEventArgs<double> e)
		{
			if (this.fillColorRectangle != null)
			{
				this.fillColorRectangle.Fill = new SolidColorBrush(Color.FromArgb((byte)this.alphaSlider.Value, (byte)this.redSlider.Value, (byte)this.greenSlider.Value, (byte)this.blueSlider.Value));
			}
		}
	}

	/// <summary>
	/// Use this class to simulate the paramater of a recursive method on the CLR call stack.
	/// </summary>
	public class ParameterData
	{
		//The current (center) point.
		public Point centerPoint;
		//The current point's position in the pixel array.
		public int centerColorPosition;
		public WriteableBitmap bmp;
	}
}
