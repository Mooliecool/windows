using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Navigation;

namespace DPContentStateSample
{
    public partial class DPStatePage : Page
    {
        // Journalable dependency property to remember control focus
        public static readonly DependencyProperty FocusedControlNameProperty;

        static DPStatePage()
        {
            // Register the local property with the journalable dependency property
            DPStatePage.FocusedControlNameProperty = DependencyProperty.Register("FocusedControlName", typeof(string), typeof(DPStatePage), new FrameworkPropertyMetadata(null, FrameworkPropertyMetadataOptions.Journal));
        }

        public DPStatePage()
        {
            InitializeComponent();

            this.PreviewLostKeyboardFocus += DPStatePage_PreviewLostKeyboardFocus;
            this.Loaded += DPStatePage_Loaded;
        }

        void DPStatePage_Loaded(object sender, RoutedEventArgs e)
        {
            // Set focus on last element to have the focus when this page was last browsed to
            if (this.FocusedControlName != null)
            {
                IInputElement element = (IInputElement)LogicalTreeHelper.FindLogicalNode(this, this.FocusedControlName);
                Keyboard.Focus(element);
            }
        }

        void DPStatePage_PreviewLostKeyboardFocus(object sender, KeyboardFocusChangedEventArgs e)
        {
            // Remember the newly focused control, unless it is the hyperlink that is
            // used for navigation.
            IInputElement element = (e.NewFocus != this.navHyperlink ? e.NewFocus : e.OldFocus);

            // Get name (can't object as most aren't serializable, eg TextBox, Hyperlink, which is 
            // a requirement for journaling.
            DependencyProperty nameDP = (element is FrameworkElement ? FrameworkElement.NameProperty : FrameworkContentElement.NameProperty);
            this.FocusedControlName = (string)((DependencyObject)element).GetValue(nameDP);
        }

        // Property to register with the journalable dependency property
        public string FocusedControlName
        {
            get
            {
                return (string)base.GetValue(DPStatePage.FocusedControlNameProperty);
            }
            set
            {
                base.SetValue(DPStatePage.FocusedControlNameProperty, value);
            }
        }
    }
}