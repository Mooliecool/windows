/****************************** Module Header ******************************\
* Module Name:  AnimateDependencyProperty.xaml.cs
* Project:      CSSL3Animation
* Copyright (c) Microsoft Corporation.
* 
* This module shows how to catch custom event of MyEllipse object and in the
* relevant event handler, it creates animation effect for the Line object by
* syncronizing its endpoints with the latest mouse click point and current position
* of MyEllipse object.
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
    public partial class AnimateDependencyProperty : UserControl
    {
      
        Point _currenttargetpoint;
        public AnimateDependencyProperty()
        {
            InitializeComponent();

        }

        private void MyStackPanel_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            _currenttargetpoint = e.GetPosition(this.MyStackPanel);
            this.MyAnimation.To = _currenttargetpoint;
            this.MyAnimationStoryboard.Begin();

        }

        /// <summary>
        /// The following method syncronize MyLine's endpoints with the latest mouse
        /// click point and current position of MyEllipse object. Therefore, an animation
        /// is created for MyLine.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void MyAnimatedEllipseGeometry_EllipseCenterChanged(DependencyObject sender, DependencyPropertyChangedEventArgs e)
        {
            this.MyLine.Visibility = Visibility.Visible;
            this.MyLine.X1 = this.MyAnimatedEllipseGeometry.EllipseCenter.X;
            this.MyLine.Y1 = this.MyAnimatedEllipseGeometry.EllipseCenter.Y;
            this.MyLine.X2 = this._currenttargetpoint.X;
            this.MyLine.Y2 = this._currenttargetpoint.Y;
            
        }

    }
}
