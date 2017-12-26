using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace InkCanvasCustomRenderingSample
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>

    public partial class App : System.Windows.Application
    {
        //  Rather than render ink in this sample, pieces (strokes from the stylus) of the image are
        // rendered on the InkCanvas. ImageSource is the place from where the information to do this
        // is drawn.
        public static ImageSource myImageSource;

        public App()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Static constructor to instance our static ImageSource
        /// </summary>
        static App()
        {
            System.Reflection.Assembly thisExe = System.Reflection.Assembly.GetExecutingAssembly();
            System.IO.Stream stream = thisExe.GetManifestResourceStream("InkCanvasCustomRenderingSample.Winter Leaves.jpg");

            JpegBitmapDecoder codec = new JpegBitmapDecoder(stream, BitmapCreateOptions.None, BitmapCacheOption.Default);
            myImageSource = codec.Frames[0];
            myImageSource.Freeze();
        }
    }
}