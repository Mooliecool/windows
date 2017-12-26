/****************************** Module Header ******************************\
* Module Name:  CodeBehindCreation.xaml.cs
* Project:      CSSL3Animation
* Copyright (c) Microsoft Corporation.
* 
* This module shows how to initialize a Storyboard in code behind. The final effect
* is the same as BasicPointAnimation.xaml, which uses XAML to add Storyboard.
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
    public partial class CodeBehindCreation : UserControl
    {
        private PointAnimation _myAnimation=new PointAnimation();
        private Storyboard _myAnimationStoryboard=new Storyboard();

        /// <summary>
        /// In the following constructor, the PointAnimation is added to Storyboard.
        /// They are initialized for animation.
        /// </summary>
        public CodeBehindCreation()
        {
            InitializeComponent();
            _myAnimation.Duration = new Duration(TimeSpan.FromSeconds(2));
            _myAnimation.SetValue(Storyboard.TargetPropertyProperty, new PropertyPath("Center"));
            Storyboard.SetTarget(_myAnimation, MyAnimatedEllipseGeometry);
            _myAnimationStoryboard.Children.Add(_myAnimation);
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
            this._myAnimation.To = targetpoint;
            this._myAnimationStoryboard.Begin();

        }
    }
}
