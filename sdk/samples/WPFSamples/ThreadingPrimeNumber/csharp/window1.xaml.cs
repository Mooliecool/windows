//<SnippetThreadingPrimeNumberCodeBehind>
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Threading;
using System.Threading;

namespace SDKSamples
{
    public partial class Window1 : Window
    {
        public delegate void NextPrimeDelegate();
        
        //Current number to check 
        private long num = 3;   

        private bool continueCalculating = false;

        public Window1() : base()
        {
            InitializeComponent();
        }

        private void StartOrStop(object sender, EventArgs e)
        {
            if (continueCalculating)
            {
                continueCalculating = false;
                startStopButton.Content = "Resume";
            }
            else
            {
                continueCalculating = true;
                startStopButton.Content = "Stop";
                startStopButton.Dispatcher.BeginInvoke(
                    DispatcherPriority.Normal,
                    new NextPrimeDelegate(CheckNextNumber));
            }
        }

        public void CheckNextNumber()
        {
            // Reset flag.
            NotAPrime = false;

            for (long i = 3; i <= Math.Sqrt(num); i++)
            {
                if (num % i == 0)
                {
                    // Set not-a-prime flag to true.
                    NotAPrime = true;
                    break;
                }
            }

            // If a prime number.
            if (!NotAPrime)
            {
                bigPrime.Text = num.ToString();
            }

            num += 2;
            if (continueCalculating)
            {
                startStopButton.Dispatcher.BeginInvoke(
                    System.Windows.Threading.DispatcherPriority.SystemIdle, 
                    new NextPrimeDelegate(this.CheckNextNumber));
            }
        }
        
        private bool NotAPrime = false;
    }
}
//</SnippetThreadingPrimeNumberCodeBehind>