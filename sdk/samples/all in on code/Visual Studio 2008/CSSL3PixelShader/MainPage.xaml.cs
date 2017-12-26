/****************************** Module Header ******************************\
* Module Name:	MainPage.xaml.cs
* Project:		CSSL3PixelShader
* Copyright (c) Microsoft Corporation.
* 
* This example demonstrates how to use new pixel shader feature in 
* Silverlight3. It mainly covers two parts:
* 
* 1. How to use built-in Effect such as DropShadowEffect.
* 2. How to create a custom ShaderEffect and use it in the application.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Using directives
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
using System.Windows.Threading;
using System.Windows.Media.Effects;
#endregion


namespace CSSL3PixelShader
{
    public partial class MainPage : UserControl
    {
        // A timer used to reduce the value of _amplitude gradually.
        private DispatcherTimer _timer = new DispatcherTimer();
        // Init custom effect
        OvalWateryEffect _effect = new OvalWateryEffect(
                    new Uri(@"/CSSL3PixelShader;component/ovalwatery.ps",
                        UriKind.Relative));

        public MainPage()
        {
            InitializeComponent();
            this.ImageWithPixelShader.Effect = _effect;
            // Initialize timer and hook Tick event.
            _timer.Interval = TimeSpan.FromMilliseconds(50);
            _timer.Tick += new EventHandler(_timer_Tick);
        }

        /// <summary>
        /// This event handler reduce the amplitude on each time tick
        /// </summary>
        void _timer_Tick(object sender, EventArgs e)
        {
            if (this._effect.Amplitude > 0.0)
            {
                this._effect.Amplitude -= 0.05; 
            }
            else
            {
                this._timer.Stop();
            }

        }

        /// <summary>
        /// This event handler get the current mouse position, assign it to a 
        /// private field and start the timer to apply new OvalWateryEffect.
        /// </summary>
        private void UserControl_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            // Convert the mouse position from control coordinates to texture coordinates as required by the PixelShader
            this._effect.Center = new Point(e.GetPosition(this.ImageWithPixelShader).X /
            this.ImageWithPixelShader.ActualWidth, e.GetPosition(
            this.ImageWithPixelShader).Y / this.ImageWithPixelShader.ActualHeight);
            this._effect.Amplitude= 0.5;
            _timer.Start();
        }
    }

    /// <summary>
    /// OvalWateryEffect class is a custom ShaderEffect class.
    /// </summary>
    public class OvalWateryEffect : ShaderEffect
    {
        /// <summary>
        /// The following two DependencyProperties are the keys of this custom ShaderEffect.
        /// They create a bridge between managed code and HLSL(High Level Shader Language).
        /// The PixelShaderConstantCallback will be triggered when the propery get changed.
        /// The parameter of the callback represents the register.
        /// For instance, here the 1 in PixelShaderConstantCallback(1) represents C1 of the
        /// following HLSL code. In another word, by changing
        /// the Amplitude property we assign the changed value to the amplitude variable of the
        /// following HLSL code:
        /// 
        /// sampler2D input : register(S0);
        /// float2 center:register(C0);
        /// float amplitude:register(C1);
        /// float4 main(float2 uv : TEXCOORD) : COLOR
        /// {
        /// if(pow((uv.x-center.x),2)+pow((uv.y-center.y),2)<0.15)
        /// {
        /// uv.y = uv.y  + (sin(uv.y*100)*0.1*amplitude);
        /// }
        /// return tex2D( input , uv.xy);
        /// }
        /// </summary>
        public static readonly DependencyProperty AmplitudeProperty = 
            DependencyProperty.Register("Amplitude", typeof(double), typeof(OvalWateryEffect), 
            new PropertyMetadata(0.1, ShaderEffect.PixelShaderConstantCallback(1)));

        public static readonly DependencyProperty CenterProperty = 
            DependencyProperty.Register("Center", typeof(Point), typeof(OvalWateryEffect), 
            new PropertyMetadata(new Point(0.5, 0.5), ShaderEffect.PixelShaderConstantCallback(0)));
        

        public OvalWateryEffect(Uri uri)
        {
            Uri u = uri;
            PixelShader psCustom = new PixelShader();
            psCustom.UriSource = u;
            PixelShader = psCustom;

            base.UpdateShaderValue(CenterProperty);
            base.UpdateShaderValue(AmplitudeProperty);
        }

        public double Amplitude
        {
            get
            {
                return (double)base.GetValue(AmplitudeProperty);
            }
            set
            {
                base.SetValue(AmplitudeProperty, value);
            }
        }

        public Point Center
        {
            get
            {
                return (Point)base.GetValue(CenterProperty);
            }
            set
            {
                base.SetValue(CenterProperty, value);
            }
        }
    }
}
