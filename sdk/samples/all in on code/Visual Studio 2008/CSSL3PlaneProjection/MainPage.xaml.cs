/****************************** Module Header ******************************\
* Module Name:  MainPage.xaml.cs
* Project:      CSSL3PlaneProjection
* Copyright (c) Microsoft Corporation.
* 
* This example illustrates how to use the new perspective 3D feature of Silverlight 3. 
* PlaneProjection is the commonly  used object to achieve perspective 3D effect.
* 
* Perspective in the graphic arts, such as drawing, is an approximate representation,
* on a flat surface (such as paper), of an image as it is perceived by the eye. 
*
* Now in Silverlight 3 you can use PlaneProjection to achieve the amazing effect!
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 7/15/2009 01:00 PM Allen Chen Created
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

namespace CSSL3PlaneProjection
{
    public partial class MainPage : UserControl
    {

        public MainPage()
        {
            InitializeComponent();
        }


        private void Calendar_SelectedDatesChanged(object sender, SelectionChangedEventArgs e)
        {
            // Get the reference of the sender Calendar, then show the selected date on TextBlockShowDate control.
            Calendar calendar = (Calendar)sender;
            this.TextBlockShowDate.Text = calendar.SelectedDate.Value.ToShortDateString();
        }

        private void SliderRotation_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            // Change the rotation center position of PlaneProjection and rotate Calendar.
            RefreshPlaneProjection(this.PlaneProjection, this.SliderRotationX.Value, this.SliderRotationY.Value, this.SliderRotationZ.Value,
                double.NaN, double.NaN, double.NaN);
            // Refresh TextBlockPlaneProjectionDetails control to show the current value of 
            // RotationX, RotationY, RotationZ, CenterOfRotationX, CenterOfRotationY, CenterOfRotationZ 
            // of PlaneProjection.
            RefreshTextBlockPlaneProjectionDetails();
        }

        private void SliderCenterOfRotation_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            // Change the rotation center position of PlaneProjection and rotate Calendar.
            RefreshPlaneProjection(this.PlaneProjection, double.NaN, double.NaN, double.NaN,
                this.SliderCenterX.Value, this.SliderCenterY.Value, this.SliderCenterZ.Value);
            // Refresh TextBlockPlaneProjectionDetails control to show the current value of 
            // RotationX, RotationY, RotationZ, CenterOfRotationX, CenterOfRotationY, CenterOfRotationZ 
            // of PlaneProjection.
            RefreshTextBlockPlaneProjectionDetails();
        }

        private void RefreshPlaneProjection(PlaneProjection planeProjection, double rotationX, double rotationY, double rotationZ, double centerOfRotationX, double centerOfRotationY, double centerOfRotationZ)
        {
            // Change rotation and rotation center related properties of PlaneProjection.
            if (!double.IsNaN(rotationX))
                planeProjection.RotationX = rotationX;
            if (!double.IsNaN(rotationY))
                planeProjection.RotationY = rotationY;
            if (!double.IsNaN(rotationZ))
                planeProjection.RotationZ = rotationZ;
            if (!double.IsNaN(centerOfRotationX))
                planeProjection.CenterOfRotationX = centerOfRotationX;
            if (!double.IsNaN(centerOfRotationY))
                planeProjection.CenterOfRotationY = centerOfRotationY;
            if (!double.IsNaN(centerOfRotationZ))
                planeProjection.CenterOfRotationZ = centerOfRotationZ;
        }

        private void RefreshTextBlockPlaneProjectionDetails()
        {
            this.TextBlockPlaneProjectionDetails.Text = string.Format(@"RotationX:{0}, RotationY:{1}, RotationZ:{2}
CenterOfRotationX:{3}, CenterOfRotationY:{4}, CenterOfRotationZ:{5}",
            this.PlaneProjection.RotationX, this.PlaneProjection.RotationY, this.PlaneProjection.RotationZ,
            this.PlaneProjection.CenterOfRotationX, this.PlaneProjection.CenterOfRotationY, this.PlaneProjection.CenterOfRotationZ);
        }

        private void ButtonResetRotation_Click(object sender, RoutedEventArgs e)
        {
            // Reset the value of roation related Sliders.
            this.SliderRotationX.Value = 0;
            this.SliderRotationY.Value = 0;
            this.SliderRotationZ.Value = 0;
        }

        private void ButtonResetCenterOfRotation_Click(object sender, RoutedEventArgs e)
        {
            // Reset the value of roation center related Sliders.
            this.SliderCenterX.Value = 0;
            this.SliderCenterY.Value = 0;
            this.SliderCenterZ.Value = 0;
        }
    }
}
