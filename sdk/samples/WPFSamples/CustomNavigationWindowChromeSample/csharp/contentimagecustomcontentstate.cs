using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;

namespace CustomNavigationWindowChromeSample
{
    /// <summary>
    /// Stores an image in a way that can be stored with a page in navigation history.
    /// This is used to show page content snapshot in the navigation history combobox
    /// on the custom NavigationWindow navigation chrome.
    /// </summary>
    [Serializable]
    public class ContentImageCustomContentState : CustomContentState
    {
        private BitmapSource contentImage;
        public ContentImageCustomContentState(BitmapSource contentImage)
        {
            this.contentImage = contentImage;
        }
        public BitmapSource ContentImage
        {
            get { return this.contentImage; }
        }
        public override void Replay(NavigationService navigationService, NavigationMode mode)
        {
            // Don't replay
        }

        public static ContentImageCustomContentState GetContentImageCustomContentState(Visual visual, int width, int height)
        {
            RenderTargetBitmap contentImage = new RenderTargetBitmap(width, height, 96, 96, new PixelFormat());
            contentImage.Render(visual);
            return new ContentImageCustomContentState(contentImage);
        }
    }
}
