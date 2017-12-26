/****************************** Module Header ******************************\
* Module Name:  MyEllipse.xaml.cs
* Project:      CSSL3Animation
* Copyright (c) Microsoft Corporation.
* 
* This module shows how to write a UserControl that basically wraps EllipseGeometry
* to "expose" its Center property.
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
    public delegate void EllipseCenterChangedEventHandler(DependencyObject sender, DependencyPropertyChangedEventArgs e);
 
    public partial class MyEllipse : UserControl
    {
        public static readonly DependencyProperty EllipseCenterProperty = 
            DependencyProperty.Register("EllipseCenter", typeof(Point), typeof(MyEllipse), new PropertyMetadata(new PropertyChangedCallback((obj,e)=>{MyHandler(obj,e);})));
        EllipseCenterChangedEventHandler _mydelegate;

        public event EllipseCenterChangedEventHandler EllipseCenterChanged
        {
            add 
            {
                if (_mydelegate == null) _mydelegate = value;
                else
                {
                    Delegate.Combine(_mydelegate, value);
                }
            }
            remove
            {
              
                Delegate.Remove(_mydelegate, value); 
            }
        }


        public Point EllipseCenter
        { 
            get { return (Point)GetValue(EllipseCenterProperty); }
            set 
            {
                SetValue(EllipseCenterProperty, value); 
            }
          
        }

        public MyEllipse()
        {
            
            InitializeComponent();
            EllipseCenter= this.MyAnimatedEllipseGeometry.Center;
        }

        /// <summary>
        /// This is a callback method that triggers OnEllipseCenterChanged method of
        /// the MyEllipse object whose EllipseCenter property got changed.
        /// </summary>
        /// <param name="obj"></param>
        /// <param name="e"></param>
        static  void MyHandler(DependencyObject obj,DependencyPropertyChangedEventArgs e)
        {
            
          MyEllipse ellipse=obj as MyEllipse;
            if(ellipse!=null){
            ellipse.OnEllipseCenterChanged(obj,e);
            }
        }

        /// <summary>
        /// This method is triggered by the callback method of EllipseCenterProperty
        /// property. It updates the Center property of the EllipseGeometry object to
        /// update UI,then it fires EllipseCenterChanged event.
        /// </summary>
        /// <param name="obj"></param>
        /// <param name="e"></param>
        void OnEllipseCenterChanged(DependencyObject obj,DependencyPropertyChangedEventArgs e)
        {
            MyEllipse ellipse = obj as MyEllipse;
            if (ellipse != null) {
                ellipse.MyAnimatedEllipseGeometry.Center = ellipse.EllipseCenter;
            }
            if(_mydelegate!=null)
                _mydelegate(obj, e);
        }
    }
}
