using System;
using System.Windows;
using System.Windows.Controls;
using System.Speech.Synthesis;

namespace SDKSample
{
    public partial class MyWindow : Window
    {
        SpeechSynthesizer _speechSynthesizer = new SpeechSynthesizer();

        public MyWindow()
        {
            InitializeComponent();
        }

        void VolumeChanged(object sender, RoutedEventArgs args)
        {
            _speechSynthesizer.Volume = (int)((Slider)args.OriginalSource).Value;
        }

        void RateChanged(object sender, RoutedEventArgs args)
        {
            _speechSynthesizer.Rate = (int)((Slider)args.OriginalSource).Value;
        }

        void ButtonEchoOnClick(object sender, RoutedEventArgs args)
        {
            _speechSynthesizer.SpeakAsync(TextToDisplay.Text);
        }

        void ButtonDateOnClick(object sender, RoutedEventArgs args)
        {
            _speechSynthesizer.SpeakAsync("Today is " + DateTime.Now.ToShortDateString());
        }

        void ButtonTimeOnClick(object sender, RoutedEventArgs args)
        {
            _speechSynthesizer.SpeakAsync("The time is " + DateTime.Now.ToShortTimeString());
        }

        void ButtonNameOnClick(object sender, RoutedEventArgs args)
        {
            _speechSynthesizer.SpeakAsync("My name is " + _speechSynthesizer.Voice.Name);
        }
    }
}