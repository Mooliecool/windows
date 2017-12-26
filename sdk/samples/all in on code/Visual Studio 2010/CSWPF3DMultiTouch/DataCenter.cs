/****************************** Module Header ******************************\
* Module Name:  DataCenter.cs
* Project:      CSWpf3DMultiTouch
* Copyright (c) Microsoft Corporation.
* 
* The data source object.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System.Windows;

namespace CSWPF3DMultiTouch
{
	public class DataCenter
	{
		public string Name { get; set; }
		public Rect Bound { get; set; }
	}
}
