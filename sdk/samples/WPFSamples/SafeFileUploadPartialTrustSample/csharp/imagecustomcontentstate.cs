using System;
using System.IO;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;

namespace SafeFileUploadPartialTrustSample
{

    [Serializable]
    public class ImageCustomContentState : CustomContentState
    {

        ImageSource imageSource;
        string filename;

        public ImageCustomContentState(ImageSource imageSource, string filename)
        {
            this.imageSource = imageSource;
            this.filename = filename;
        }

        public override string JournalEntryName
        {
            get { return this.filename; }
        }

        public override void Replay(NavigationService navigationService, NavigationMode mode)
        {
            HomePage homePage = (HomePage)navigationService.Content;
            homePage.viewImage.Source = this.imageSource;
            homePage.nameLabel.Content = this.filename;
        }
    }
}
