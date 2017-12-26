/****************************** Module Header ******************************\
* Module Name:  MyPointAnimation.xaml.cs
* Project:      CSSL3Animation
* Copyright (c) Microsoft Corporation.
* 
* This module shows how to write baisc PointAnimation for an EllipseGeometry
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 9/8/2009 05:00 PM Allen Chen Created
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
    public partial class BasicPointAnimation : UserControl
    {
        public BasicPointAnimation()
        {
            InitializeComponent();
        }
        /// <summary>
        /// Tbe following event handler change the To property of PointAnimation object,
        /// then begin the Storyboard to play the animation. Please note we can change
        /// To property even when the animation is playing.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void StackPanel_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {

            var targetpoint = e.GetPosition(this.MyStackPanel);
            this.MyAnimation.To = targetpoint;
            this.MyAnimationStoryboard.Begin();
                
        }
    }
}
