/****************************** Module Header ******************************\
* Module Name:  MainPage.cs
* Project:      CSAzureXbap
* Copyright (c) Microsoft Corporation.
* 
* The main page. All touching events are handled in this class.
* It also invokes a WCF service to obtain the data center list.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.ServiceModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Media.Media3D;
using XbapClient.DataCenterServiceReference;

namespace XbapClient
{
    /// <summary>
    /// Interaction logic for Page1.xaml
    /// </summary>
    public partial class MainPage : Page
    {
        private double _angleBuffer = 0d;
        // Modify the image's size if you want to change the image.
        private static int _imageWidth = 1995;
        private static int _imageHeight = 2051;
        private List<DataCenter> _dataCenters;

        // The following variables are for mouse simulation.
        private bool _isMouseDown;
        private Point _startPoint;

        public MainPage()
        {
            InitializeComponent();
            // You can obtain the current XBAP's URI using BrowserInteropHelper.Source.
            Uri serviceUri = new Uri(BrowserInteropHelper.Source, "../DataCenterService.svc");
            DataCenterServiceClient client = new DataCenterServiceClient(new BasicHttpBinding(), new EndpointAddress(serviceUri));
            client.GetDataCentersCompleted += new EventHandler<GetDataCentersCompletedEventArgs>(client_GetDataCentersCompleted);
            client.GetDataCentersAsync();
            Touch.FrameReported += new TouchFrameEventHandler(Touch_FrameReported);
        }

        void client_GetDataCentersCompleted(object sender, GetDataCentersCompletedEventArgs e)
        {
            this._dataCenters = e.Result.ToList();
        }

        /// <summary>
        /// This is the event handler for the multi-touch ManipulationDelta event.
        /// </summary>
        /// <param name="e">
        /// Provides 2D manipulation information such as scale, translate, and rotate.
        /// The scale and translate information can be used in 3D scenarios as well. But the rotate information can't.
        /// For this sample, however, we do not translate the 3D scene using the translate gesture. 
        /// Instead, we allow the user to rotate the ball follow the finger.
        /// </param>
        private void OnManipulationDelta(object sender, ManipulationDeltaEventArgs e)
        {
            this.scaleTransform.ScaleX *= e.DeltaManipulation.Scale.X;
            this.scaleTransform.ScaleY *= e.DeltaManipulation.Scale.Y;
            this.scaleTransform.ScaleZ *= e.DeltaManipulation.Scale.X;

            this._angleBuffer++;
            // To avoid screen slash and to save a few CPU resource, do not rotate the scene whenever a maniputation event occurs.
            // Only rotate the scene if the angle cumulated enough.
            if (_angleBuffer >= 0)
            {
                Vector delta = e.DeltaManipulation.Translation;
                this.RotateEarth(delta);
            }
            e.Handled = true;
        }

        /// <summary>
        /// Common rotate method that invoked by both manipulation event handler and mouse event handler.
        /// </summary>
        /// <param name="delta">How long has the finger/mouse moved.</param>
        private void RotateEarth(Vector delta)
        {
            if (delta.X != 0 || delta.Y != 0)
            {
                // Convert delta to a 3D vector.
                Vector3D vOriginal = new Vector3D(-delta.X, delta.Y, 0d);
                Vector3D vZ = new Vector3D(0, 0, 1);
                // Find a vector that is perpendicular with the delta vector on the XY surface. This will be the rotation axis.
                Vector3D perpendicular = Vector3D.CrossProduct(vOriginal, vZ);
                RotateTransform3D rotate = new RotateTransform3D();
                // The QuaternionRotation3D allows you to easily specify a rotation axis.
                QuaternionRotation3D quatenion = new QuaternionRotation3D();
                quatenion.Quaternion = new Quaternion(perpendicular, 3);
                rotate.Rotation = quatenion;
                this.transformGroup.Children.Add(rotate);
                this._angleBuffer = 0;
            }
        }

        /// <summary>
        /// TouchUp event handler. Similar to MouseUp event handler
        /// </summary>
        private void OnTouchUp(object sender, TouchEventArgs e)
        {            
            DoHitTest(e.GetTouchPoint(this.viewport).Position);
        }

        /// <summary>
        /// Common hittest method that invoked by both touch event handler and mouse event handler.
        /// </summary>
        /// <param name="point">The touched/clicked point</param>
        private void DoHitTest(Point point)
        {
            if (this._dataCenters != null)
            {
                VisualTreeHelper.HitTest(this.viewport, null, new HitTestResultCallback(target =>
                {
                    RayMeshGeometry3DHitTestResult result = target as RayMeshGeometry3DHitTestResult;
                    if (result != null)
                    {
                        // Calculate the hit point using barycentric coordinates formula:
                        // p = p1 * w1 + p2 * w2 + p3 * w3.
                        // For more information, please refer to http://en.wikipedia.org/wiki/Barycentric_coordinates_%28mathematics%29.
                        Point p1 = result.MeshHit.TextureCoordinates[result.VertexIndex1];
                        Point p2 = result.MeshHit.TextureCoordinates[result.VertexIndex2];
                        Point p3 = result.MeshHit.TextureCoordinates[result.VertexIndex3];
                        double hitX = p1.X * result.VertexWeight1 + p2.X * result.VertexWeight2 + p3.X * result.VertexWeight3;
                        double hitY = p1.Y * result.VertexWeight1 + p2.Y * result.VertexWeight2 + p3.Y * result.VertexWeight3;
                        Point pointHit = new Point(hitX * _imageWidth, hitY * _imageHeight);
                        // If a data center circle is hit, display the information.
                        foreach (DataCenter dc in this._dataCenters)
                        {
                            System.Windows.Rect bound = new System.Windows.Rect(dc.Bound.X, dc.Bound.Y, dc.Bound.Width, dc.Bound.Height);
                            if (bound.Contains(pointHit))
                            {
                                this.InfoTextBox.Text = "You've just touched the " + dc.Name + " data center!";
                                Storyboard sb = this.Resources["sb"] as Storyboard;
                                if (sb != null)
                                {
                                    sb.Begin();
                                }
                                return HitTestResultBehavior.Stop;
                            }
                        }
                    }
                    return HitTestResultBehavior.Continue;
                }), new PointHitTestParameters(point));
            }
        }

        /// <summary>
        /// Handle raw touch events.
        /// In this sample, if you touch the screen with two fingers and release the first finger, 
        /// a line will be drawn to connect the two touch points.
        /// </summary>
        void Touch_FrameReported(object sender, TouchFrameEventArgs e)
        {
            var touchPoints = e.GetTouchPoints(this.viewport);
            if (touchPoints.Count >= 2 && touchPoints[0].Action == TouchAction.Up)
            {
                this.TouchLine.X1 = touchPoints[0].Position.X;
                this.TouchLine.X2 = touchPoints[1].Position.X;
                this.TouchLine.Y1 = touchPoints[0].Position.Y;
                this.TouchLine.Y2 = touchPoints[1].Position.Y;
            }
        }

        // The following are event handlers for mouse simulation.
        private void Grid_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            this._isMouseDown = true;
            this._startPoint = e.GetPosition(this.viewport);
        }

        private void Grid_MouseMove(object sender, MouseEventArgs e)
        {
            if (this._isMouseDown && this.MouseSimulationCheckBox.IsChecked.Value)
            {
                this._angleBuffer++;
                if (_angleBuffer >= 0)
                {
                    Point currentPoint = e.GetPosition(this.viewport);
                    Vector delta = new Vector(currentPoint.X - this._startPoint.X, currentPoint.Y - this._startPoint.Y);
                    RotateEarth(delta);
                }
            }
        }

        private void Grid_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            this._isMouseDown = false;
            if (this.MouseSimulationCheckBox.IsChecked.Value)
            {
                this.DoHitTest(e.GetPosition(this.viewport));
            }
        }

        private void Grid_MouseWheel(object sender, MouseWheelEventArgs e)
        {
            if (this.MouseSimulationCheckBox.IsChecked.Value)
            {
                double delta = e.Delta > 0 ? 1.2 : 0.8;
                this.scaleTransform.ScaleX *= delta;
                this.scaleTransform.ScaleY *= delta;
                this.scaleTransform.ScaleZ *= delta;
            }
        }
    }
}
