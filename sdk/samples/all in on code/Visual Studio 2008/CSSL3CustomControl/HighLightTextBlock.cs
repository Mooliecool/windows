/****************************** Module Header ******************************\
* Module Name:  HighLightTextBlock.cs
* Project:      CSSL3CustomControl
* Copyright (c) Microsoft Corporation.
* 
* Implementation of custom control HighLightTextBlock.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 8/18/2009 8:06 PM Mog Liang Created
\***************************************************************************/

using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace CSSL3CustomControl
{
    [TemplateVisualState(Name = "HighLight", GroupName = "HightLightStates")]
    [TemplateVisualState(Name = "NonHighLight", GroupName = "HightLightStates")]
    public class HighLightTextBlock : Control
    {
        // In order to use templatebinding on 'Text' property,
        // it must be dependency property.
        public string Text
        {
            get { return (string)GetValue(TextProperty); }
            set { SetValue(TextProperty, value); }
        }

        public static readonly DependencyProperty TextProperty =
            DependencyProperty.Register("Text", typeof(string), typeof(HighLightTextBlock), null);

		//If this property is set to true, the text will automatically be dehilighted after a specific time. Use the LightTime to control the highlight period.
		public bool AutoDehighlight { get; set; }

		//Use this property to (de)highlight the text.
		public static readonly DependencyProperty IsHighlightedProperty =
			DependencyProperty.Register("DependencyProperty", typeof(Nullable<bool>), typeof(HighLightTextBlock), new PropertyMetadata(false, new PropertyChangedCallback(IsHighlightedChanged)));

		public Nullable<bool> IsHighlighted
		{
			get { return (Nullable<bool>)this.GetValue(IsHighlightedProperty); }
			set { this.SetValue(IsHighlightedProperty, value); }
		}

		private static void IsHighlightedChanged(DependencyObject sender, DependencyPropertyChangedEventArgs e)
		{
			if (((Nullable<bool>)e.NewValue).Value)
			{
				((HighLightTextBlock)sender).Highlight();
			}
			else
			{
				((HighLightTextBlock)sender).Dehighlight();
			}
			((HighLightTextBlock)sender).IsHighlighted = (Nullable<bool>)e.NewValue;
		}

        // Expose LightTime property for highlight time.
        public TimeSpan LightTime
        {
            set
            {
                if (_timer == null)
                    InitTimer();
                _timer.Interval = value;
            }
            get
            {
                if (_timer == null)
                    InitTimer();
                return _timer.Interval;
            }
        }
        public override void OnApplyTemplate()
        {
            base.OnApplyTemplate();
        }

        DispatcherTimer _timer;
        public HighLightTextBlock()
        {
            // Register custom control's default style
            DefaultStyleKey = typeof(HighLightTextBlock);
            InitTimer();
        }

        // Initialize timer
        void InitTimer()
        {
            _timer = new DispatcherTimer();
            _timer.Interval = TimeSpan.FromSeconds(1);
            _timer.Tick += delegate
            {
                // When time out, change HighLightStates to 'NonHighLight'
				Dehighlight();
            };
        }

		private void Dehighlight()
		{
			VisualStateManager.GoToState(this, "NonHighLight", true);
			_timer.Stop();
		}

        // User call this method to highlight text.
        public void Highlight()
        {
            // Change control state to 'HightLight'
            VisualStateManager.GoToState(this, "HighLight", true);
			if (this.AutoDehighlight)
			{
				// Start timer
				_timer.Start();
			}
        }
    }
}
