using System;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;

using Microsoft.Win32;

namespace SafeFileUploadPartialTrustSample {
    public partial class HomePage : Page, IProvideCustomContentState {
        public HomePage() {
            InitializeComponent();
        }

        void uploadButton_Click(object sender, RoutedEventArgs e) {

            // Configure OpenFileDialog to open images
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Filter = "Image Files(*.BMP;*.JPG;*.GIF)|*.BMP;*.JPG;*.GIF|All files (*.*)|*.*";

            // Open file, if user clicked "Open" button on OpenFileDialog
            if (dlg.ShowDialog() == true) {

                // If existing image, put into back navigation history
                if (this.viewImage.Source != null) {
                    ImageCustomContentState iccs = new ImageCustomContentState(this.viewImage.Source, (string)this.nameLabel.Content);
                    this.NavigationService.AddBackEntry(iccs);
                }

                // Show new image
                using (Stream stream = dlg.OpenFile()) {
                    BitmapDecoder bitmapDecoder = BitmapDecoder.Create(stream, BitmapCreateOptions.PreservePixelFormat, BitmapCacheOption.OnLoad);
                    this.viewImage.Source = bitmapDecoder.Frames[0];
                    this.nameLabel.Content = dlg.SafeFileName;
                }
            }
        }

        #region IProvideCustomContentState Members

        public CustomContentState GetContentState() {
            // Add to history that is in the opposite direction to which we are navigating
            return new ImageCustomContentState(this.viewImage.Source, (string)this.nameLabel.Content);
        }

        #endregion
    }
}