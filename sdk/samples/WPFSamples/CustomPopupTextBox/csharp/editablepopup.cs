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
    /// This control is a TextBox that acts like a Popup.  To get a control
    /// to behave like a Popup, you should define the IsOpen, Placement, 
    /// PlacementRectangle, PlacementTarget, HorizontalOffset, and
    /// VerticalOffset properties and then call Popup.CreateRoot to
    /// bind these six properties to their counterparts on the 
    /// internal Popup.
    /// </summary>
    public partial  class EditablePopup : System.Windows.Controls.TextBox
    {
        Popup _parentPopup;

        public EditablePopup()
            : base()
        {
        }

        static EditablePopup()
        {
            //This OverrideMetadata call tells the system that this element 
            //wants to provide a style that is different than its base class.
            //This style is defined in themes\generic.xaml
            DefaultStyleKeyProperty.OverrideMetadata(typeof(EditablePopup), 
                new FrameworkPropertyMetadata(typeof(EditablePopup)));
        }

        #region Properties to implement Popup Behavior

        //Placement
        public static readonly DependencyProperty PlacementProperty =
                    Popup.PlacementProperty.AddOwner(typeof(EditablePopup));

        public PlacementMode Placement
        {
            get { return (PlacementMode)GetValue(PlacementProperty); }
            set { SetValue(PlacementProperty, value); }
        }

        //PlacementTarget
        public static readonly DependencyProperty PlacementTargetProperty =
           Popup.PlacementTargetProperty.AddOwner(typeof(EditablePopup));

        public UIElement PlacementTarget
        {
            get { return (UIElement)GetValue(PlacementTargetProperty); }
            set { SetValue(PlacementTargetProperty, value); }
        }

        //PlacementRectangle
        public static readonly DependencyProperty PlacementRectangleProperty =
                    Popup.PlacementRectangleProperty.AddOwner(typeof(EditablePopup));

        public Rect PlacementRectangle
        {
            get { return (Rect)GetValue(PlacementRectangleProperty); }
            set { SetValue(PlacementRectangleProperty, value); }
        }

        //HorizontalOffset
        public static readonly DependencyProperty HorizontalOffsetProperty =
            Popup.HorizontalOffsetProperty.AddOwner(typeof(EditablePopup));

        public new double HorizontalOffset
        {
            get { return (double)GetValue(HorizontalOffsetProperty); }
            set { SetValue(HorizontalOffsetProperty, value); }
        }

        //VerticalOffset
        public static readonly DependencyProperty VerticalOffsetProperty =
                Popup.VerticalOffsetProperty.AddOwner(typeof(EditablePopup));

        public new double VerticalOffset
        {
            get { return (double)GetValue(VerticalOffsetProperty);  }
            set { SetValue(VerticalOffsetProperty, value); }
        }

        
        public static readonly DependencyProperty IsOpenProperty =
                Popup.IsOpenProperty.AddOwner(
                        typeof(EditablePopup),
                        new FrameworkPropertyMetadata(
                                false,
                                FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
                                new PropertyChangedCallback(OnIsOpenChanged)));

        public bool IsOpen
        {
            get { return (bool)GetValue(IsOpenProperty); }
            set { SetValue(IsOpenProperty, value); }
        }

        private static void OnIsOpenChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            EditablePopup ctrl = (EditablePopup)d;

            if ((bool)e.NewValue)
            {
                if (ctrl._parentPopup == null)
                {
                    ctrl.HookupParentPopup();
                }
            }
        }

        //Create the Popup and attach the CustomControl to it.
        private void HookupParentPopup()
        {

            _parentPopup = new Popup();

            _parentPopup.AllowsTransparency = true;

            Popup.CreateRootPopup(_parentPopup, this);
        }

        #endregion

    }
}
