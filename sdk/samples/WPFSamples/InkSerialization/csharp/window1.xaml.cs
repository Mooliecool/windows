using Microsoft.Win32;
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Ink;
using System.IO;
using System.Windows.Media;

namespace Ink_Serialization_CS
{
    /// <summary>
    /// Interaction logic for Window1.xaml.
    /// </summary>
    public partial class Window1 : System.Windows.Window
    {
        // Create Guids for each of the properties to be added to the 
        // Strokes collection of the InkCanvas.
        Guid userGuid = new Guid("{ab2503c1-e83c-450c-ae58-3ad2d0819eac}");
        Guid timeStampGuid = new Guid("{a8da70b2-b899-4acd-bb97-1d78e57c24df}");

        public Window1()
        {
            InitializeComponent();
            user.Text = Environment.UserName;
        }

        /// <summary>
        /// Clear the InkCanvas of all stroke data as well as 
        /// resetting the user name field and the last saved
        /// time stamp field.
        /// </summary>
        void Clear(object sender, RoutedEventArgs e)
        {
            inkcanvas.Strokes.Clear();
            user.Text = Environment.UserName;
            timeStamp.Text = "not saved";
        }

        /// <summary>
        /// Call the .Save method on the InkCanvas' Strokes collection
        /// to persist the collection of ink as ISF in either compressed 
        /// or uncompressed format.  In this sample, we also show how to 
        /// serialize arbitrary data in the ISF by creating extended 
        /// properties on the Strokes collection of an InkCanvas.
        /// </summary>
        void Save_Click(object sender, RoutedEventArgs e)
        {
            SaveFileDialog dialog = new SaveFileDialog();
            dialog.DefaultExt = "isf";
            dialog.Filter = "ISF Files (*.isf)|*.isf";
            dialog.ShowDialog();
            try
            {
                if (dialog.FileName != String.Empty)
                {
                    FileStream filestream = new FileStream(dialog.FileName, FileMode.Create);
                    // Set the time stamp and update the UI with it...
                    DateTime theTimeStamp = DateTime.Now;
                    timeStamp.Text = theTimeStamp.ToString();
                    // Add the time stamp and user data as properties of the 
                    // Strokes collection of the InkCanvas for serialization.
                    inkcanvas.Strokes.AddPropertyData(userGuid, user.Text);
                    inkcanvas.Strokes.AddPropertyData(timeStampGuid, theTimeStamp);
                    inkcanvas.Strokes.Save(filestream, (bool)compress.IsChecked);
                    filestream.Close();
                    // Remove the property data since it's been serialized now.
                    inkcanvas.Strokes.RemovePropertyData(userGuid);
                    inkcanvas.Strokes.RemovePropertyData(timeStampGuid);
                }
            }
            catch (Exception exc)
            {
                MessageBox.Show(exc.Message);
            }
        }

        /// <summary>
        /// Deserialize ISF previously saved from this or any other 
        /// application to a temporary stroke collection.  Check the 
        /// stroke collection's extended properties for data related to
        /// this application in particular (i.e., user and time stamp)
        /// and update the relevant UI fields if found.  Finally, set this
        /// application's inkcanvas.Strokes collection to the temporary
        /// stroke collection.
        /// </summary>
        void Load_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog dialog = new OpenFileDialog();
            dialog.DefaultExt = "isf";
            dialog.Filter = "ISF Files (*.isf)|*.isf";
            dialog.ShowDialog();
            try
            {
                if (dialog.FileName != String.Empty)
                {
                    FileStream filestream = new FileStream(dialog.FileName, FileMode.Open);
                    StrokeCollection strokes = new StrokeCollection(filestream);
                    filestream.Close();
                    // Check for our property data serialized with the Strokes collection.
                    if (strokes.ContainsPropertyData(userGuid))
                    {
                        user.Text = strokes.GetPropertyData(userGuid).ToString();
                        strokes.RemovePropertyData(userGuid);
                    }
                    if (strokes.ContainsPropertyData(timeStampGuid))
                    {
                        timeStamp.Text = strokes.GetPropertyData(timeStampGuid).ToString();
                        strokes.RemovePropertyData(timeStampGuid);
                    }
                    inkcanvas.Strokes = strokes;
                }
            }
            catch (Exception exc)
            {
                MessageBox.Show(exc.Message);
            }
        }
    }
}