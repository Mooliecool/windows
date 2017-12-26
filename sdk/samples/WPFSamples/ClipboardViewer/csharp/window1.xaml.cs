using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.IO;
using System.Text;
using System.Threading;

using Microsoft.Win32;


namespace ClipboardViewer
{
    // This class implements callbacks for the application UI.
    public partial class Window1 : Window
    {
        public Window1()
        {
            InitializeComponent();
        }

        // This is called when UI is laid out, rendered and ready for interaction.
        private void WindowLoaded(Object sender, RoutedEventArgs args)
        {
            // Check the current Clipboard data format status
            RefreshClipboardDataFormat();

            // Set the text copy information on RichTextBox
            SetTextOnRichTextBox("Please type clipboard copy data here!");
        }

        private void ClearClipboard(Object sender, RoutedEventArgs args)
        {
            // Clear Clipboard
            Clipboard.Clear();

            // Update the current Clipboard data format status after clear Clipboard
            RefreshClipboardDataFormat();
        }

        private void RefreshClipboardDataFormatStatus(Object sender, RoutedEventArgs args)
        {
            // Refresh the all clipboard data format into the information panel
            RefreshClipboardDataFormat();
        }

        private void DumpAllClipboardContents(Object sender, RoutedEventArgs args)
        {
            // Dump all Clipboard contents on the Clipboard information panel
            DumpAllClipboardContentsInternal();
        }

        private void CopyToClipboard(Object sender, RoutedEventArgs args)
        {
            DataObject dataObject = new DataObject();

            // Copy data from RichTextBox/File content into DataObject
            if ((bool)rbCopyDataFromRichTextBox.IsChecked)
            {
                CopyDataFromRichTextBox(dataObject);
            }
            else
            {
                CopyDataFromFile(dataObject);
            }

            // Copy DataObject on the system Clipboard
            if (dataObject.GetFormats().Length > 0)
            {
                if ((bool)cbFlushOnCopy.IsChecked)
                {
                    // Copy data to the system clipboard with flush
                    Clipboard.SetDataObject(dataObject, true/*copy*/);
                }
                else
                {
                    // Copy data to the system clipboard without flush
                    Clipboard.SetDataObject(dataObject, false/*copy*/);
                }
            }

            // Dump the copied data contents on the information panel
            DumpAllClipboardContentsInternal();
        }

        private void PasteFromClipboard(Object sender, RoutedEventArgs args)
        {
            IDataObject dataObject = GetDataObjectFromClipboard();

            if (dataObject == null)
            {
                MessageBox.Show("Couldn't get DataObject from system clipboard!\n", "Clipboard Error");                
                return;
            }

            bool autoConvert = (bool)cbAutoConvertibleData.IsChecked;
            string pasteDataFormat = lbPasteDataFormat.SelectedItems[0] as string;

            if ((bool)rbPasteDataToRichTextBox.IsChecked)
            {
                // Paste data from Clipboard to RichTextBox
                PasteClipboardDataToRichTextBox(pasteDataFormat, dataObject, autoConvert);
            }
            else
            {
                // Paste data from Clipboard to File
                PasteClipboardDataToFile(pasteDataFormat, dataObject, autoConvert);
            }
        }

        private void ClearRichTextBox(Object sender, RoutedEventArgs args)
        {
            SetTextOnRichTextBox("");
        }

        // Update the data format status and describe all available data formats
        private void RefreshClipboardDataFormat()
        {
            clipboardInfo.Clear();

            lbPasteDataFormat.Items.Clear();

            IDataObject dataObject = GetDataObjectFromClipboard();

            if (dataObject == null)
            {
                clipboardInfo.Text = "Can't access clipboard now! Please click Refresh button again.";
            }
            else
            {
                // Check the data format whether it is on Clipboard or not
                cbAudio.IsChecked = dataObject.GetDataPresent(DataFormats.WaveAudio);
                cbFileDropList.IsChecked = dataObject.GetDataPresent(DataFormats.FileDrop);
                cbImage.IsChecked = dataObject.GetDataPresent(DataFormats.Bitmap);
                cbText.IsChecked = dataObject.GetDataPresent(DataFormats.Text);
                cbRtf.IsChecked = dataObject.GetDataPresent(DataFormats.Rtf);
                cbXaml.IsChecked = dataObject.GetDataPresent(DataFormats.Xaml);

                // Update the data format into the information panel
                UpdateAvailableDataFormats(dataObject);
            }
        }

        // Update and describe all available data formats
        private void UpdateAvailableDataFormats(IDataObject dataObject)
        {
            clipboardInfo.AppendText("Clipboard DataObject Type: ");
            clipboardInfo.AppendText(dataObject.GetType().ToString());

            clipboardInfo.AppendText("\n\n****************************************************\n\n");

            string[] formats = dataObject.GetFormats();

            clipboardInfo.AppendText("The following data formats are present in the data object obtained from the clipboard:\n");

            if (formats.Length > 0)
            {
                bool nativeData;

                foreach (string format in formats)
                {
                    if (dataObject.GetDataPresent(format, false))
                    {
                        nativeData = true;
                        clipboardInfo.AppendText("\t- " + format + " (native)\n");
                    }
                    else
                    {
                        nativeData = false;
                        clipboardInfo.AppendText("\t- " + format + " (autoconvertable)\n");
                    }

                    if (nativeData)
                    {
                        lbPasteDataFormat.Items.Add(format);
                    }
                    else if ((bool)cbAutoConvertibleData.IsChecked)
                    {
                        lbPasteDataFormat.Items.Add(format);
                    }
                }

                lbPasteDataFormat.SelectedIndex = 0;
            }
            else
            {
                clipboardInfo.AppendText("\t- no data formats are present\n");
            }
        }

        // Dump all available data contents
        private void DumpAllClipboardContentsInternal()
        {
            clipboardInfo.Clear();

            lbPasteDataFormat.Items.Clear();

            IDataObject dataObject = GetDataObjectFromClipboard();

            if (dataObject == null)
            {
                clipboardInfo.Text = clipboardInfo.Text = "Can't access clipboard now! \n\nPlease click Dump All Clipboard Contents button again.";
            }
            else
            {
                // Update the availabe data formats 
                UpdateAvailableDataFormats(dataObject);

                // Update the all available data contents
                string[] formats = dataObject.GetFormats();
                foreach (string format in formats)
                {
                    clipboardInfo.AppendText("\n\n****************************************************\n");
                    clipboardInfo.AppendText(format + " data:\n");
                    clipboardInfo.AppendText("****************************************************\n\n");

                    object data = GetDataFromDataObject(dataObject, format, true /*autoConvert*/);

                    clipboardInfo.AppendText(data != null ? data.ToString() : "null");
                }
            }
        }

        // Set the selected data format's data from RichTextBox's content 
        // into DataObject for copying data on the system clipboard
        private void CopyDataFromRichTextBox(IDataObject dataObject)
        {
            if ((bool)cbCopyTextDataFormat.IsChecked)
            {
                string textData = GetTextStringFromRichTextBox(DataFormats.Text);
                if (textData != string.Empty)
                {
                    dataObject.SetData(DataFormats.Text, textData);
                }
            }

            if ((bool)cbCopyXamlDataFormat.IsChecked)
            {
                string textData = GetTextStringFromRichTextBox(DataFormats.Xaml);
                if (textData != string.Empty)
                {
                    dataObject.SetData(DataFormats.Xaml, textData);
                }
            }

            if ((bool)cbCopyRtfDataFormat.IsChecked)
            {
                string textData = GetTextStringFromRichTextBox(DataFormats.Rtf);
                if (textData != string.Empty)
                {
                    dataObject.SetData(DataFormats.Rtf, textData);
                }
            }

            // Finally, consider a custom, application defined format.
            // We use an arbitrary encoding here, for demonstration purposes.
            if ((bool)cbCustomSampleDataFormat.IsChecked)
            {
                Stream customStream = new MemoryStream();

                string textData = "This is Custom Sample Data Start\n\n" +
                    GetTextStringFromRichTextBox(DataFormats.Text) +
                    "\nCustom Sample Data End.";

                byte[] bytesUnicode = Encoding.Unicode.GetBytes(textData);
                byte[] bytes = Encoding.Convert(Encoding.Unicode, Encoding.UTF8, bytesUnicode);

                if (bytes.Length > 0)
                {
                    customStream.Write(bytes, 0, bytes.Length);
                    dataObject.SetData("CustomSample", customStream);
                }
            }
        }

        // Set the selected data format's data from the file content 
        // into DataObject for copying data on the system clipboard
        private void CopyDataFromFile(IDataObject dataObject)
        {
            string fileName = null;

            OpenFileDialog dialog = new OpenFileDialog();
            dialog.CheckFileExists = true;

            if ((bool)cbCopyTextDataFormat.IsChecked)
            {
                dialog.Filter = "Plain Text (*.txt)|*.txt";
                dialog.ShowDialog();
                fileName = dialog.FileName;

                if (fileName == null || fileName == string.Empty)
                {
                    return;
                }

                Encoding fileEncoding = Encoding.Default;
                string textData = GetTextStringFromFile(fileName, fileEncoding);
                if (textData != null && textData != string.Empty)
                {
                    dataObject.SetData(DataFormats.Text, textData);
                }
            }

            if ((bool)cbCopyRtfDataFormat.IsChecked)
            {
                fileName = null;
                dialog.Filter = "RTF Documents (*.rtf)|*.rtf";
                dialog.ShowDialog();
                fileName = dialog.FileName;

                if (fileName == null || fileName == string.Empty)
                {
                    return;
                }

                Encoding fileEncoding = Encoding.ASCII;
                string textData = GetTextStringFromFile(fileName, fileEncoding);
                if (textData != null && textData != string.Empty)
                {
                    dataObject.SetData(DataFormats.Rtf, textData);
                }
            }

            if ((bool)cbCopyXamlDataFormat.IsChecked)
            {
                fileName = null;
                dialog.Filter = "XAML Flow Documents (*.xaml)|*.xaml";
                dialog.ShowDialog();
                fileName = dialog.FileName;

                if (fileName == null || fileName == string.Empty)
                {
                    return;
                }

                Encoding fileEncoding = Encoding.UTF8;
                string textData = GetTextStringFromFile(fileName, fileEncoding);
                if (textData != null && textData != string.Empty)
                {
                    dataObject.SetData(DataFormats.Xaml, textData);
                }
            }

            // Finally, consider a custom, application defined format.
            // We use an arbitrary encoding here, for demonstartion purposes.
            if ((bool)cbCustomSampleDataFormat.IsChecked)
            {
                fileName = null;
                dialog.Filter = "All Files (*.*)|*.*";
                dialog.ShowDialog();
                fileName = dialog.FileName;

                if (fileName == null || fileName == string.Empty)
                {
                    return;
                }

                Encoding fileEncoding = Encoding.UTF8;
                string textData = GetTextStringFromFile(fileName, fileEncoding);
                if (textData != null && textData != string.Empty)
                {
                    byte[] bytesUnicode = Encoding.Unicode.GetBytes(textData);
                    byte[] bytes = Encoding.Convert(Encoding.Unicode, Encoding.UTF8, bytesUnicode);

                    if (bytes.Length > 0)
                    {
                        MemoryStream customStream = new MemoryStream();
                        customStream.Write(bytes, 0, bytes.Length);
                        dataObject.SetData("CustomSample", customStream); ;
                    }
                }
            }
        }

        // Paste a selected paste data format's data to RichTextBox
        private void PasteClipboardDataToRichTextBox(string dataFormat, IDataObject dataObject, bool autoConvert)
        {
            if (dataObject != null && dataObject.GetFormats().Length > 0)
            {
                bool pasted = false;

                if (dataFormat == DataFormats.Xaml)
                {
                    string xamlData = dataObject.GetData(DataFormats.Xaml) as string;
                    if (xamlData != null && xamlData != string.Empty)
                    {
                        pasted = PasteTextDataToRichTextBox(DataFormats.Xaml, xamlData);
                    }
                }
                else if (dataFormat == DataFormats.Rtf)
                {
                    string rtfData = dataObject.GetData(DataFormats.Rtf) as string; ;
                    if (rtfData != null && rtfData != string.Empty)
                    {
                        pasted = PasteTextDataToRichTextBox(DataFormats.Rtf, rtfData);
                    }
                }
                else if (dataFormat == DataFormats.UnicodeText 
                         || dataFormat == DataFormats.Text
                         || dataFormat == DataFormats.StringFormat)
                {
                    string textData = dataObject.GetData(dataFormat) as string; ;
                    if (textData != string.Empty)
                    {
                        SetTextOnRichTextBox(textData);
                        pasted = true;
                    }
                } 
                else if (dataFormat == "CustomSample")
                {
                    // Paste the application defined custom data format's data to RichTextBox content
                    Stream customStream = dataObject.GetData(dataFormat, autoConvert) as Stream;
                    if (customStream.Length > 0)
                    {
                        TextRange textRange = new TextRange(richTextBox.Document.ContentStart, richTextBox.Document.ContentEnd);
                        textRange.Load(customStream, DataFormats.Text);
                        pasted = true;
                    }
                }

                if (!pasted)
                {
                    MessageBox.Show("Can't paste the selected data format into RichTextBox!\n\nPlease click Refresh button to update the current clipboard format Or select File RadioButton to paste data.",
                        "Paste Data Format Error",
                        MessageBoxButton.OK,
                        MessageBoxImage.Exclamation);
                }
            }
        }

        // Paste a selected paste data format's data to the fileSSS
        private void PasteClipboardDataToFile(string dataFormat, IDataObject dataObject, bool autoConvert)
        {
            string fileName;

            SaveFileDialog dialog;

            dialog = new SaveFileDialog();
            dialog.CheckFileExists = false;

            if (dataFormat == DataFormats.Text
                || dataFormat == DataFormats.UnicodeText
                || dataFormat == DataFormats.StringFormat)
            {
                dialog.Filter = "Plain Text (*.txt)|*.txt | All Files (*.*)|*.*";
            }
            else if (dataFormat == DataFormats.Xaml)
            {
                dialog.Filter = "XAML Flow Documents (*.xaml)|*.xaml | All Files (*.*)|*.*";
            }
            else if (dataFormat == DataFormats.Rtf)
            {
                dialog.Filter = "RTF Documents (*.rtf)|*.rtf | All Files (*.*)|*.*";
            }
            else
            {
                dialog.Filter = "All Files (*.*)|*.*";
            }

            if (!(bool)dialog.ShowDialog())
            {
                return;
            }

            fileName = dialog.FileName;

            if (dataFormat == DataFormats.Xaml)
            {
                string xamlData = dataObject.GetData(DataFormats.Xaml) as string;
                if (xamlData != null && xamlData != string.Empty)
                {
                    PasteTextDataToFile(dataFormat, xamlData, fileName, Encoding.UTF8);
                }
            }
            else if (dataFormat == DataFormats.Rtf)
            {
                string rtfData = dataObject.GetData(DataFormats.Rtf) as string;
                if (rtfData != null && rtfData != string.Empty)
                {
                    PasteTextDataToFile(dataFormat, rtfData, fileName, Encoding.ASCII);
                }
            }
            else if (dataFormat == DataFormats.UnicodeText
                     || dataFormat == DataFormats.Text
                     || dataFormat == DataFormats.StringFormat)
            {
                string textData = dataObject.GetData(dataFormat, autoConvert) as string; ;
                if (textData != null && textData != string.Empty)
                {
                    PasteTextDataToFile(dataFormat, textData, fileName, dataFormat == DataFormats.Text ? Encoding.Default : Encoding.Unicode);
                }
            }
            else
            {
                // Paste the CustomSample data or others to the file
                //Stream customStream = dataObject.GetData(dataFormat, autoConvert) as Stream;
                Stream customStream = GetDataFromDataObject(dataObject, dataFormat, autoConvert) as Stream;
                if (customStream != null && customStream.Length > 0)
                {
                    PasteStreamDataToFile(customStream, fileName);
                }
            }

            return;
        }

        // Get DataObject from the system clipboard
        private IDataObject GetDataObjectFromClipboard()
        {
            IDataObject dataObject;

            try
            {
                dataObject = Clipboard.GetDataObject();
            }
            catch (System.Runtime.InteropServices.COMException)
            {
                // Clipboard.GetDataObject can be failed by opening the system clipboard 
                // from other or processing clipboard operation like as setting data on clipboard
                dataObject = null;
            }

            return dataObject;
        }

        private object GetDataFromDataObject(IDataObject dataObject, string dataFormat, bool autoConvert)
        {
            object data = null;

            try
            {
                data = dataObject.GetData(dataFormat, autoConvert);
            }
            catch (System.Runtime.InteropServices.COMException)
            {
                // Fail to get the data by the invalid value like tymed(DV_E_TYMED) 
                // or others(Aspect, Formatetc).
                // It depends on application's IDataObject::GetData implementation.
                clipboardInfo.AppendText("Fail to get data!!! ***COMException***");
            }
            catch (OutOfMemoryException)
            {
                // Fail by the out of memory from getting data on Clipboard. 
                // Occurs with the low memory.
                clipboardInfo.AppendText("Fail to get data!!! ***OutOfMemoryException***");
            }

            return data;
        }

        // Set the plain text on RichTextBox
        private void SetTextOnRichTextBox(string text)
        {
            FlowDocument document = richTextBox.Document;
            TextRange textRange = new TextRange(document.ContentStart, document.ContentEnd);

            textRange.Text = text;

            richTextBox.Focus();
            richTextBox.SelectAll();
        }

        // Get text data string from RichTextBox content which encoded as UTF8
        private string GetTextStringFromRichTextBox(string dataFormat)
        {
            FlowDocument document = richTextBox.Document;
            TextRange textRange = new TextRange(document.ContentStart, document.ContentEnd);

            if (dataFormat == DataFormats.Text)
            {
                return textRange.Text;
            }
            else
            {
                Stream contentStream = new MemoryStream();
                textRange.Save(contentStream, dataFormat);

                if (contentStream.Length > 0)
                {
                    byte[] bytes = new byte[contentStream.Length];

                    contentStream.Position = 0;
                    contentStream.Read(bytes, 0, bytes.Length);

                    Encoding utf8Encoding = Encoding.UTF8;

                    return utf8Encoding.GetString(bytes);
                }
            }

            return null;
        }

        // Get text data string from file content with the proper encoding
        private string GetTextStringFromFile(string fileName, Encoding fileEncoding)
        {
            string textString = null;

            FileStream fileStream = null;

            try
            {
                fileStream = new FileStream(fileName, FileMode.Open, FileAccess.Read);
            }
            catch (System.IO.IOException)
            {
                MessageBox.Show("File is not acessible.\n", "File Open Error");
                return null;
            }

            fileStream.Position = 0;

            byte[] bytes = new byte[fileStream.Length];

            fileStream.Read(bytes, 0, bytes.Length);

            if (bytes.Length > 0)
            {
                textString = fileEncoding.GetString(bytes);
            }

            fileStream.Close();

            return textString;
        }

        // Paste text data on RichTextBox as UTF8 encoding
        private bool PasteTextDataToRichTextBox(string dataFormat, string textData)
        {
            bool pasted = false;

            FlowDocument document = richTextBox.Document;
            TextRange textRange = new TextRange(document.ContentStart, document.ContentEnd);

            Stream stream = new MemoryStream();

            byte[] bytesUnicode = Encoding.Unicode.GetBytes(textData);
            byte[] bytes = Encoding.Convert(Encoding.Unicode, Encoding.UTF8, bytesUnicode);

            if (bytes.Length > 0 && textRange.CanLoad(dataFormat))
            {
                stream.Write(bytes, 0, bytes.Length);
                textRange.Load(stream, dataFormat);
                pasted = true;
            }

            return pasted;
        }

        // Paste text data to the file with the file encoding
        private void PasteTextDataToFile(string dataFormat, string textData, string fileName, Encoding fileEncoding)
        {
            FileStream fileWriteStream = null;

            try
            {
                fileWriteStream = new FileStream(fileName, FileMode.OpenOrCreate, FileAccess.Write);
            }
            catch (System.IO.IOException)
            {
                MessageBox.Show("File is not acessible.\n", "File Write Error");
                return;
            }

            fileWriteStream.SetLength(0);

            byte[] bytesUnicode = Encoding.Unicode.GetBytes(textData);
            byte[] bytes = Encoding.Convert(Encoding.Unicode, fileEncoding, bytesUnicode);

            if (bytes.Length > 0)
            {
                fileWriteStream.Write(bytes, 0, bytes.Length);
            }

            fileWriteStream.Close();
        }

        // Paste stream data to the file
        private void PasteStreamDataToFile(Stream stream, string fileName)
        {
            FileStream fileWriteStream = null;

            try
            {
                fileWriteStream = new FileStream(fileName, FileMode.OpenOrCreate, FileAccess.Write);
            }
            catch (System.IO.IOException)
            {
                MessageBox.Show("File is not acessible.\n", "File Write Error");
            }

            fileWriteStream.SetLength(0);

            byte[] bytes = new byte[stream.Length];

            stream.Position = 0;
            stream.Read(bytes, 0, bytes.Length);

            if (bytes.Length > 0)
            {
                fileWriteStream.Write(bytes, 0, bytes.Length);
            }

            fileWriteStream.Close();
        }

    }  // End class Window1
}  // End namespace ClipboardViewer