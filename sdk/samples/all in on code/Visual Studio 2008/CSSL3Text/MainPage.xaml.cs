/****************************** Module Header ******************************\
* Module Name:  MainPage.cs
* Project:      CSSL3Text
* Copyright (c) Microsoft Corporation.
* 
* This example demonstrates how to work with text in Silverlight using C#.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Using directives
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
#endregion


namespace CSSL3Text
{
	public partial class MainPage : UserControl
	{
		public MainPage()
		{
			InitializeComponent();
		}

		private void Page_Loaded(object sender, RoutedEventArgs e)
		{
			this.CreateSimpleTextBlock();
			this.CreateComplexTextBlock();
			this.FormatText();
		}

		/// <summary>
		/// This method creates a simple TextBlock, and adds it to the 
        /// StackPanel simpleTBPlaceHolder.
		/// </summary>
		private void CreateSimpleTextBlock()
		{
			TextBlock simpleTextBlock = new TextBlock() { Text = "Simple TextBlock" };
			this.simpleTBPlaceHolder.Children.Add(simpleTextBlock);
		}

		/// <summary>
		/// This method creates a TextBlock with multiple Runs and 
        /// LineBreaks, and adds it to the StackPanel complexTBPlaceHolder.
		/// </summary>
		private void CreateComplexTextBlock()
		{
			TextBlock complexTextBlock = new TextBlock();
			Run paragraph1 = new Run() { Text = "Paragraph1" };
			LineBreak lineBreak = new LineBreak();
			Run paragraph2 = new Run() { Text = "Paragraph2" };
			complexTextBlock.Inlines.Add(paragraph1);
			complexTextBlock.Inlines.Add(lineBreak);
			complexTextBlock.Inlines.Add(paragraph2);
			this.complexTBPlaceHolder.Children.Add(complexTextBlock);
		}

		/// <summary>
		/// This method creates a TextBlock with advanced format, and adds 
        /// it to the StackPanel customizeFormatPlaceHolder.
		/// </summary>
		private void FormatText()
		{
			TextBlock formatTextBlock = new TextBlock();
			Run paragraph1 = new Run() { Text = "Paragraph1" };
			paragraph1.FontFamily = new FontFamily("Magnetob.ttf#Magneto");
			LineBreak lineBreak = new LineBreak();
			Run paragraph2 = new Run() { Text = "Paragraph2" };
			LinearGradientBrush brush = new LinearGradientBrush();
			brush.GradientStops.Add(new GradientStop() { Color = Colors.Blue, Offset = 0d });
			brush.GradientStops.Add(new GradientStop() { Color = Colors.Red, Offset = 1d });
			paragraph2.Foreground = brush;
			formatTextBlock.Inlines.Add(paragraph1);
			formatTextBlock.Inlines.Add(lineBreak);
			formatTextBlock.Inlines.Add(paragraph2);
			this.customizeFormatPlaceHolder.Children.Add(formatTextBlock);
		}

		/// <summary>
		/// This method selects all text in the TextBox targetTextBox. Note 
        /// you have to focus the TextBox so that the selection will be displayed.
		/// </summary>
		private void selectTextButton_Click(object sender, RoutedEventArgs e)
		{
			this.targetTextBox.SelectAll();
			this.targetTextBox.Focus();
		}
	}
}
