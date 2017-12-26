using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Windows.Controls.Primitives;

namespace PopupPlacement
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>

    public partial class Window1 : System.Windows.Window
    {
        // The Rect for PlacementRectangle.
        Rect placementRect = new Rect(50, 150, 60, 90);

        public Window1()
        {
            InitializeComponent();

            popup1.CustomPopupPlacementCallback =
                                new CustomPopupPlacementCallback(placePopup);
        }


        // Provide to possible places for the Popup when Placement
        // is set to Custom.
        public CustomPopupPlacement[] placePopup(Size popupSize,
                                                 Size targetSize,
                                                 Point offset)
        {
            CustomPopupPlacement placement1 =
                new CustomPopupPlacement(new Point(-50, 100), PopupPrimaryAxis.Vertical);

            CustomPopupPlacement placement2 =
                new CustomPopupPlacement(new Point(10, 20), PopupPrimaryAxis.Horizontal);
            
            CustomPopupPlacement[] ttplaces =
                    new CustomPopupPlacement[] { placement1, placement2};
            return ttplaces;
        }

        // Set PlacementRectangle and show a Rectangle with the same
        // dimensions.
        void showPlacementRectangle(object sender, RoutedEventArgs e)
        {
            placementRectArea.Visibility = Visibility.Visible;
            popup1.PlacementRectangle = placementRect;
        }

        // Clear PlacementRectangle and hide the Rectangle
        void hidePlacementRectangle(object sender, RoutedEventArgs e)
        {
            placementRectArea.Visibility = Visibility.Hidden;
            popup1.PlacementRectangle = Rect.Empty;
        }

        // Set the Placement property of the Popup.
        void setPlacement(object sender, RoutedEventArgs e)
        {
            RadioButton placementChoice = e.Source as RadioButton;

            if (placementChoice == null)
            {
                return;
            }

            switch (placementChoice.Name)
            {
                case "placementAbsolute":
                    popup1.Placement = PlacementMode.Absolute;
                    break;
                case "placementAbsolutePoint":
                    popup1.Placement = PlacementMode.AbsolutePoint;
                    break;
                case "placementBottom":
                    popup1.Placement = PlacementMode.Bottom;
                    break;
                case "placementCenter":
                    popup1.Placement = PlacementMode.Center;
                    break;
                case "placementCustom":
                    popup1.Placement = PlacementMode.Custom;
                    break;
                case "placementLeft":
                    popup1.Placement = PlacementMode.Left;
                    break;
                case "placementMouse":
                    popup1.Placement = PlacementMode.Mouse;
                    break;
                case "placementMousePoint":
                    popup1.Placement = PlacementMode.MousePoint;
                    break;
                case "placementRelative":
                    popup1.Placement = PlacementMode.Relative;
                    break;
                case "placementRelativePoint":
                    popup1.Placement = PlacementMode.RelativePoint;
                    break;
                case "placementRight":
                    popup1.Placement = PlacementMode.Right;
                    break;
                case "placementTop":
                    popup1.Placement = PlacementMode.Top;
                    break;
            }
        }

        // Reset the offsets of the Popup.
        void resetOffsets_Click(object sender, RoutedEventArgs e)
        {
            popup1.HorizontalOffset = 0;
            popup1.VerticalOffset = 0;  
        }

    }
}