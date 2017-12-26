/****************************** Module Header ******************************\
* Module Name:  CoversationControl.cs
* Project:      CSSL3DeepZoom
* Copyright (c) Microsoft Corporation.
* 
* A simple UserControl that displays a conversation.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 8/27/2009 15:40 Yilun Luo Created
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

namespace CSSL3DeepZoom
{
	public partial class ConversationControl : UserControl
	{
		public ConversationControl()
		{
			InitializeComponent();
		}

		public string ConversationText
		{
			get { return this.converstaionText.Text; }
			set { this.converstaionText.Text = value; }
		}
	}
}
