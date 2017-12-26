using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;


namespace CustomPopupTextBox
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>

    public partial class Window1 : System.Windows.Window
    {
        EditablePopup berlinDesc;
        EditablePopup budapestDesc;
        EditablePopup dresdenDesc;
        EditablePopup pragueDesc;

        public Window1()
        {
            InitializeComponent();
            this.LocationChanged += new EventHandler(Window1_LocationChanged);
           
            CreateDescriptions();

        }

        void Window1_LocationChanged(object sender, EventArgs e)
        {
            if ((bool)showDesc.IsChecked)
            {
                TogglePopups(false);
                TogglePopups(true);

                
            }
        }

        //Create an EditablePopup for each image.
        void CreateDescriptions()
        {
            berlinDesc = new EditablePopup();
            berlinDesc.Text = "Title:\tBerlin, Germany\n" +
                                "Description:\n\tBrandenburg Gate.";
            berlinDesc.PlacementTarget = berlinPic;

            budapestDesc = new EditablePopup();
            budapestDesc.Text = "Title:\tBudapest, Hungary\n" +
                                "Description:\n\tHungarian Parliament Building and the Danube River";
            budapestDesc.PlacementTarget = budapestPic;

            dresdenDesc = new EditablePopup();
            dresdenDesc.Text = "Title:\tDresden, Germany\n" +
                                "Description:\n\tZwinger Palace";
            dresdenDesc.PlacementTarget = dresdenPic;

            pragueDesc = new EditablePopup();
            pragueDesc.Text = "Title:\tPrague, Czech Republic\n" +
                                "Description:\n\tView from the Prague Castle.";
            pragueDesc.PlacementTarget = praguePic;


        }

        void TogglePopups(bool showPopup)
        {
            berlinDesc.IsOpen = showPopup;
            budapestDesc.IsOpen = showPopup;
            dresdenDesc.IsOpen = showPopup;
            pragueDesc.IsOpen = showPopup;

        }

        // Hide the EditablePopup controls.
        void showDesc_Unchecked(object sender, RoutedEventArgs e)
        {
            TogglePopups(false);
        }

        // Show the EditablePopup controls.
        void showDesc_Checked(object sender, RoutedEventArgs e)
        {
            TogglePopups(true);
        }

    }
}