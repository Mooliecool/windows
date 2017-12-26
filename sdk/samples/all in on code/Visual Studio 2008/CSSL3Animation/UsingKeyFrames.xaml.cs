/****************************** Module Header ******************************\
* Module Name:  UsingKeyFrames.xaml.cs
* Project:      CSSL3Animation
* Copyright (c) Microsoft Corporation.
* 
* This module shows how to create KeyFrames based animation in Silverilght
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 9/10/2009 03:00 PM Allen Chen Created
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

namespace CSSL3Animation
{
    public partial class UsingKeyFrames : UserControl
    {
        public UsingKeyFrames()
        {
            InitializeComponent();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            this.MyAnimationStoryboard.Begin();
        }
    }
}
