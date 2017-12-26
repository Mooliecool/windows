using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace SDKSample
{
    public partial class Window1 : Window
    {
        public Window1()
        {
            InitializeComponent();
        }

        void WindowLoaded(Object sender, RoutedEventArgs args)
        {
            CheckStatus(null, null);
        }

        void GetClipboard(Object sender, RoutedEventArgs args)
        {
            _dataObject = Clipboard.GetDataObject();
            CheckCurrentDataObject();
        }

        void CheckStatus(Object sender, RoutedEventArgs args)
        {
            cbContainsAudio.IsChecked = Clipboard.ContainsAudio() ? true : false;
            cbContainsFileDropList.IsChecked = Clipboard.ContainsFileDropList() ? true : false;
            cbContainsImage.IsChecked = Clipboard.ContainsImage() ? true : false;
            cbContainsText.IsChecked = Clipboard.ContainsText() ? true : false;
        }

        void ClearClipboard(Object sender, RoutedEventArgs args)
        {
            Clipboard.Clear();
            CheckStatus(null, null);
            GetClipboard(null, null);
        }

        void CheckCurrentDataObject()
        {
            CheckStatus(null, null);
            
            tbInfo.Clear();

            if (_dataObject == null)
            {
                tbInfo.Text = "DataObject is null.";
            }
            else
            {
                tbInfo.AppendText("Clipboard DataObject Type: ");
                tbInfo.AppendText(_dataObject.GetType().ToString());

                tbInfo.AppendText("\n\n****************************************************\n\n");

                string[] formats = _dataObject.GetFormats();

                tbInfo.AppendText("The following data formats are present in the data object obtained from the clipboard:\n");
                
                if (formats.Length > 0)
                {
                    foreach (string format in formats)
                    {
                        if (_dataObject.GetDataPresent(format, false)) tbInfo.AppendText("\t- " + format + " (native)\n");
                        else tbInfo.AppendText("\t- " + format + " (autoconvertable)\n");
                    }
                }
                else tbInfo.AppendText("\t- no data formats are present\n");

                foreach (string format in formats)
                {
                    tbInfo.AppendText("\n\n****************************************************\n");
                    tbInfo.AppendText(format + " data:\n");
                    tbInfo.AppendText("****************************************************\n\n");
                    tbInfo.AppendText(_dataObject.GetData(format, true).ToString());
                }
                

            }
        }

        IDataObject _dataObject = null;

    }  // End class Window1

}  // End namespace ClipboardSpy_Sample