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
using System.Xml;
using System.IO;
using System.Globalization;

namespace SignatureCollector
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
        /// The Commit button Click handler.
        ///   Takes the strokes from the InkCanvas and adds them to the 
        ///   InkData.xml file.  Databinding to the XMLDataProvider will
        ///   update the listview with a new signature item.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void commit_Click(object sender, RoutedEventArgs e)
        {
                StrokeCollection clonedStrokes = myInkCanvas.Strokes.Clone();
                Rect strokesRect = clonedStrokes.GetBounds();
                //Apply a Transform shifts the strokes to the top-left.  The ViewBox in InkDataTemplate will center the strokes.
                clonedStrokes.Transform(new Matrix(1, 0, 0, 1, (0 - strokesRect.X), (0 - strokesRect.Y)), false);

                //Add the ISF from the strokes to the XML document.
                //Databinding in Window1.xaml will update the list view each time you
                //add a signature.
                XmlDataProvider xdp = this.FindResource("InkData") as XmlDataProvider;
                XmlDocument xdoc = xdp.Document;
                XmlElement newInkFile = xdoc.CreateElement("InkFile");
                XmlElement newSig = xdoc.CreateElement("Signature");

                using (MemoryStream base64Ink = new MemoryStream())
                {
                    clonedStrokes.Save(base64Ink);
                    newSig.InnerText = Convert.ToBase64String(base64Ink.ToArray());
                    newInkFile.AppendChild(newSig);
                    xdoc.DocumentElement.AppendChild(newInkFile);
                    //Uncomment this line if you would like to save strokes to the 
                    //resource InkData.xml, which the ListViewItems are databound to.
                    //xdoc.Save("..\\..\\Data\\InkData.xml");

                }

                myInkCanvas.Strokes.Clear();
                //Bring the last item added into view.
                signatureView.ScrollIntoView(signatureView.Items[signatureView.Items.Count - 1]);
        }

    }

    /// <summary>
    /// A Converter class which is used to determine if the InkCanvas contains strokes or not.
    /// </summary>
    public class StrokeCountToBoolConverter : IValueConverter
    {
        /// <summary>
        /// Returns true if InkCanvas.Strokes.Count > 0.
        /// </summary>
        /// <param name="obj"></param>
        /// <param name="type"></param>
        /// <param name="parameter"></param>
        /// <param name="culture"></param>
        /// <returns></returns>
        public object Convert(object obj, Type type, object parameter, CultureInfo culture)
        {
            // If there is no stroke on InkCanvas, return false.  Otherwise, return true.
            if ((int)obj == 0)
            {
                return false;
            }
            else
            {
                return true;
            }
        }

        /// <summary>
        /// Do not convert back.
        /// </summary>
        /// <param name="obj"></param>
        /// <param name="type"></param>
        /// <param name="parameter"></param>
        /// <param name="culture"></param>
        /// <returns></returns>
        public object ConvertBack(object obj, Type type, object parameter, CultureInfo culture)
        {
            return null;
        }
    }
}