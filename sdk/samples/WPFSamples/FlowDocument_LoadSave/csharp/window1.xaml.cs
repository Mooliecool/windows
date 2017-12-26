using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Windows.Forms;
using System.IO;
using System.Windows.Markup;

namespace SDKSample
{

    public partial class Window1 : Window
    {

        public Window1()
        {
            InitializeComponent();
        }

        void LoadFile(Object sender, RoutedEventArgs args)
        {
            FlowDocument content = null;
            
            OpenFileDialog openFile = new OpenFileDialog();
            openFile.Filter = "FlowDocument Files (*.xaml)|*.xaml|All Files (*.*)|*.*";

            if (openFile.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                FileStream xamlFile = openFile.OpenFile() as FileStream;
                if (xamlFile == null) return;
                else
                {   
                    try 
                    { 
                        content = XamlReader.Load(xamlFile) as FlowDocument; 
                        if (content == null) 
                            throw(new XamlParseException("The specified file could not be loaded as a FlowDocument."));
                    }
                    catch (XamlParseException e)
                    {
                        String error = "There was a problem parsing the specified file:\n\n";
                        error += openFile.FileName;
                        error += "\n\nException details:\n\n";
                        error += e.Message;
                        System.Windows.MessageBox.Show(error);
                        return;
                    }
                    catch (Exception e) 
                    {
                        String error = "There was a problem loading the specified file:\n\n";
                        error += openFile.FileName;
                        error += "\n\nException details:\n\n";
                        error += e.Message;
                        System.Windows.MessageBox.Show(error);
                        return;
                    }

                    // At this point, there is a non-null FlowDocument loaded into content.  
                    FlowDocRdr.Document = content;
                }
            }
        }

        void SaveFile(Object sender, RoutedEventArgs args)
        {
            SaveFileDialog saveFile = new SaveFileDialog();
            FileStream xamlFile = null;
            saveFile.Filter = "FlowDocument Files (*.xaml)|*.xaml|All Files (*.*)|*.*";
            if (saveFile.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                try
                {
                    xamlFile = saveFile.OpenFile() as FileStream;
                }
                catch (Exception e)
                {
                    String error = "There was a opening the file:\n\n";
                    error += saveFile.FileName;
                    error += "\n\nException details:\n\n";
                    error += e.Message;
                    System.Windows.MessageBox.Show(error);
                    return;
                }
                if (xamlFile == null) return;
                else
                {
                    XamlWriter.Save(FlowDocRdr.Document, xamlFile);
                    xamlFile.Close();
                }
            }
        }

        void Clear(Object sender, RoutedEventArgs args) { FlowDocRdr.Document = null; }
        void Exit(Object sender, RoutedEventArgs args) { this.Close(); }
    }
}