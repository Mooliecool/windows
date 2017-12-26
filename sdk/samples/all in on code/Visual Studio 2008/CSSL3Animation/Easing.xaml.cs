/****************************** Module Header ******************************\
* Module Name:  Easing.xaml.cs
* Project:      CSSL3Animation
* Copyright (c) Microsoft Corporation.
* 
* This module shows how to use EasingFunction for PointAnimation. In addition,
* it demonstrates how to write a custom Ease class.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 9/9/2009 05:00 PM Allen Chen Created
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
    public partial class Easing : UserControl
    {
        public Easing()
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
        private void MyStackPanel_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            var targetpoint = e.GetPosition(this.MyStackPanel);
            this.MyAnimation.To = targetpoint;
            this.MyAnimationStoryboard.Begin();
        }

        private void MyEaseRadioButton_Click(object sender, RoutedEventArgs e)
        {
            this.MyAnimation.EasingFunction = this.Resources["MyEase"] as IEasingFunction;
        }

        private void BackEaseRadioButton_Click(object sender, RoutedEventArgs e)
        {
            this.MyAnimation.EasingFunction = this.Resources["BackEase"] as IEasingFunction;
        }
    }

    /// <summary>
    /// A custom Ease class 
    /// </summary>
    public class MyEase : EasingFunctionBase
    {
        protected override double EaseInCore(double normalizedTime)
        {
            return normalizedTime/5;
        }

    }
}
