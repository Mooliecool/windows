using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;


namespace InkCanvasGestureSample
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>

    public partial class Window1 : System.Windows.Window
    {

        public Window1()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Prepare the InkCanvas for InkAndGesture (if gesture recognition is available).
        /// </summary>
        void OnLoaded(object sender, RoutedEventArgs e)
        {
            //  After the window is loaded and laid out but before the user can interact with the application
            // it is made sure that the user has gesture recognizers installed. If not, gesturing is not enabled 
            // on the InkCanvas and a message to remedy the situation is displayed.
            //
            //  This check is absolutely necessary - if not done, a gesture on the InkAndGesture EditingMode
            // InkCanvas will throw an exception. Please refer to the SDK for more information regarding 
            // InkCanvas gestures.

            if (myInkCanvas.IsGestureRecognizerAvailable)
            {
                myInkCanvas.EditingMode = InkCanvasEditingMode.InkAndGesture;
                myInkCanvas.Gesture += new InkCanvasGestureEventHandler(InkCanvas_Gesture);

                //  The gestures in which the application is interested are enabled here.
                myInkCanvas.SetEnabledGestures(new ApplicationGesture[] { ApplicationGesture.ScratchOut, 
                                                                                     ApplicationGesture.Up, 
                                                                                     ApplicationGesture.Down, 
                                                                                     ApplicationGesture.Left, 
                                                                                     ApplicationGesture.Right });
            }
            else
            {
                myInkCanvas.EditingMode = InkCanvasEditingMode.None;
                myInkTextBox.Text = String.Empty;
                myInkLabel.Content = String.Empty;
                myNoGestureRecoTextBlock.Visibility = Visibility.Visible;
            }
        }

        /// <summary>
        ///  This is the InkCanvas gesture event handler. Here certain gestures are received 
        /// and acted upon accordingly.
        /// </summary>

        void InkCanvas_Gesture(object sender, InkCanvasGestureEventArgs e)
        {
            GestureRecognitionResult topResult = e.GetGestureRecognitionResults()[0];
            
            if (topResult.RecognitionConfidence == RecognitionConfidence.Strong)
            {
                ApplicationGesture gesture = topResult.ApplicationGesture;

                switch (gesture)
                {
                    case ApplicationGesture.ScratchOut:

                        StrokeCollection strokesToRemove = myInkCanvas.Strokes.HitTest(e.Strokes.GetBounds(), 10);
                        if (strokesToRemove.Count > 0)
                            myInkCanvas.Strokes.Remove(strokesToRemove);
                        break;

                    case ApplicationGesture.Right:

                        myScrollViewer.ScrollToHorizontalOffset(myScrollViewer.HorizontalOffset + 30);
                        break;

                    case ApplicationGesture.Left:

                        myScrollViewer.ScrollToHorizontalOffset(myScrollViewer.HorizontalOffset - 30);
                        break;

                    case ApplicationGesture.Up:

                        myScrollViewer.ScrollToVerticalOffset(myScrollViewer.VerticalOffset - 30);
                        break;

                    case ApplicationGesture.Down:

                        myScrollViewer.ScrollToVerticalOffset(myScrollViewer.VerticalOffset + 30);
                        break;
                }
            }
        }
    }
}