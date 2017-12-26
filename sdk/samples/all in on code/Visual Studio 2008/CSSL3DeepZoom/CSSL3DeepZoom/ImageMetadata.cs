/****************************** Module Header ******************************\
* Module Name:  ImageMetadata.cs
* Project:      CSSL3DeepZoom
* Copyright (c) Microsoft Corporation.
* 
* This class wraps the data downloaded from Metadata.xml.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 8/27/2009 15:40 Yilun Luo Created
\***************************************************************************/

using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Collections.Generic;

namespace CSSL3DeepZoom
{
	/// <summary>
	/// Corresponds to an entry in Metadata.xml.
	/// </summary>
	public class ImageMetadata
	{
		public string FileName { get; set; }
		public double X { get; set; }
		public double Y { get; set; }
		public double Width { get; set; }
		public double Height { get; set; }
		public int ZOrder { get; set; }
		public string Tag { get; set; }
	}
}
