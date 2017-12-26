/****************************** Module Header ******************************\
* Module Name:  MainPage.xaml.cs
* Project:      CSSL3MediaElement
* Copyright (c) Microsoft Corporation.
* 
* This example illustrates basic usage of MediaElement.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 10/7/2009 05:00 PM Allen Chen Created
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
using System.Windows.Threading;

namespace CSSL3MediaElement
{
    public partial class MainPage : UserControl
    {
        DispatcherTimer _timer = new DispatcherTimer();

        public MainPage()
        {
            InitializeComponent();
            this.Loaded += new RoutedEventHandler(MainPage_Loaded);
            _timer.Interval = TimeSpan.FromMilliseconds(100);
            _timer.Tick += new EventHandler(_timer_Tick);
        }


        void MainPage_Loaded(object sender, RoutedEventArgs e)
        {
            // Attach events of SilverlightHost to subscribe the 
            // FullScreenChanged event.
            App.Current.Host.Content.FullScreenChanged += new EventHandler(Content_FullScreenChanged);
        }


        void Content_FullScreenChanged(object sender, EventArgs e)
        {
            if (!App.Current.Host.Content.IsFullScreen)
            {
                // When shifting back to normal mode, reset width/height of MediaElement.
                this.MyMediaElement.Width = double.NaN;
                this.MyMediaElement.Height = double.NaN;
                // When shifting back to normal mode, hide FullSreenPanel and show MenuPanel
                this.MenuPanel.Visibility = Visibility.Visible;
                this.FullScreenPanel.Visibility = Visibility.Collapsed;
            }
            else
            {
                // In full-screen mode, set width/height of MediaElement to a big value
                this.MyMediaElement.Width = App.Current.Host.Content.ActualWidth;
                this.MyMediaElement.Height = App.Current.Host.Content.ActualHeight;
                // In full-screen mode, hide MenuPanel and show FullSreenPanel
                this.MenuPanel.Visibility = Visibility.Collapsed;
                this.FullScreenPanel.Visibility = Visibility.Visible;
            }
        }

        void _timer_Tick(object sender, EventArgs e)
        {
            if (this.MyMediaElement.CurrentState ==MediaElementState.Playing)
            {
                // Update Status of controls
                TimeSpan ts = this.MyMediaElement.Position;
                this.CurrentPositionTextBlock.Text = string.Format("{0}:{1}:{2}/", ts.Hours, ts.Minutes, ts.Seconds);
                this.CurrentPositionSlider.Value = ts.TotalMilliseconds;
            }
            else if(this.MyMediaElement.CurrentState==MediaElementState.Stopped)
            {
                // Reset status of controls
                this.CurrentPositionTextBlock.Text = "0:0:0/";
                this.CaptionTextBlock.Text = string.Empty;
                this.CurrentPositionSlider.Value = 0;
            }
        }

        private void PlayButton_Click(object sender, RoutedEventArgs e)
        {
            this.MyMediaElement.Play();
            this._timer.Start();
        }

        private void PauseButton_Click(object sender, RoutedEventArgs e)
        {
            this.MyMediaElement.Pause();
        }

        private void StopButton_Click(object sender, RoutedEventArgs e)
        {
            this.MyMediaElement.Stop();
        }

        private void MyMediaElement_CurrentStateChanged(object sender, RoutedEventArgs e)
        {
            // Show current state of MediaElement. Note only 4 states are catched here.
            // You can add more when creating your own media player.
            switch (this.MyMediaElement.CurrentState) 
            {
                case MediaElementState.Stopped: 
                    {
                        this.StatusTextBlock.Text = "Stopped";
                        break;
                    }
                case MediaElementState.Paused: 
                    {
                        this.StatusTextBlock.Text = "Paused";
                        break;
                    }
                case MediaElementState.Playing:
                    {
                        this.StatusTextBlock.Text = "Playing";
                        break;
                    }
                case MediaElementState.Buffering:
                    {
                        this.StatusTextBlock.Text = "Buffering";
                        break;
                    }
            }
        }

        private void MyMediaElement_MediaOpened(object sender, RoutedEventArgs e)
        {
          TimeSpan ts=  this.MyMediaElement.NaturalDuration.TimeSpan;
          this.DurationTextBlock.Text = string.Format("{0}:{1}:{2}",ts.Hours,ts.Minutes,ts.Seconds);
          // Init CurrentPosition Slider
          this.CurrentPositionSlider.Maximum = ts.TotalMilliseconds;
          this.CurrentPositionSlider.Minimum=0;
          // Init VolumeSlider
          this.VolumeSlider.Value = this.MyMediaElement.Volume;
          // Add a marker by code
          this.MyMediaElement.Markers.Add(new TimelineMarker()
          {
              Text = "This marker is added by code!",
              Time = TimeSpan.FromSeconds(8),
              Type = "MyMarker"
          });
          // Enable progress bar
          this.CurrentPositionSlider.IsEnabled = true;
        }

        private void MyMediaElement_MediaEnded(object sender, RoutedEventArgs e)
        {
            this.MyMediaElement.Stop();
        }

        private void HorizontalThumb_DragStarted(object sender, System.Windows.Controls.Primitives.DragStartedEventArgs e)
        {
            this.MyMediaElement.Pause();
        }

        private void HorizontalThumb_DragCompleted(object sender, System.Windows.Controls.Primitives.DragCompletedEventArgs e)
        {
            this.MyMediaElement.Position = TimeSpan.FromMilliseconds(this.CurrentPositionSlider.Value);
            this.MyMediaElement.Play();
        }

        private void MyMediaElement_MarkerReached(object sender, TimelineMarkerRoutedEventArgs e)
        {
            if (e.Marker.Type == "MyMarker")
            {
                // Show marker text made by code
                this.CaptionTextBlock.FontSize = 22;
                this.CaptionTextBlock.Text = e.Marker.Text;
            }

            else
            {
                // Show marker text made by Expression Blend 3
                // Because the caption is "out of" the video you get more flexibility to customizing it
                this.CaptionTextBlock.FontSize = 12;
                this.CaptionTextBlock.Text = e.Marker.Text;
            }
        }

        private void FullScreenButton_Click(object sender, RoutedEventArgs e)
        {   
            // Shift to full-screen mode
            App.Current.Host.Content.IsFullScreen =!App.Current.Host.Content.IsFullScreen;
        }

        private void VolumeSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            this.MyMediaElement.Volume=this.VolumeSlider.Value;
        }

        private void MyMediaElement_MediaFailed(object sender, ExceptionRoutedEventArgs e)
        {
            this.CurrentPositionSlider.IsEnabled = false;
            MessageBox.Show(e.ErrorException.Message);

        }

     

    }
}
