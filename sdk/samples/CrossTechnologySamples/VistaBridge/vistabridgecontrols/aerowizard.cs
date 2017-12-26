using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.ComponentModel;
using System.Windows.Interop;
using System.Windows.Input;
using System.Collections.Generic;
using Microsoft.SDK.Samples.VistaBridge.Interop;

namespace Microsoft.SDK.Samples.VistaBridge.Controls
{
    /// <summary>
    /// ========================================
    /// WinFX Custom Control
    /// ========================================
    ///
    /// Follow steps 1a or 1b and then 2 to use this 
    /// custom control in a XAML file.
    ///
    /// Step 1a) Using this custom control in a XAML file 
    /// that exists in the current project.
    /// Add this XmlNamespace attribute to the root element 
    /// of the markup file where it is to be used:
    ///
    ///     xmlns:MyNamespace="clr-namespace:AERO"
    ///
    ///
    /// Step 1b) Using this custom control in a XAML file 
    /// that exists in a different project.
    /// Add this XmlNamespace attribute to the root element 
    /// of the markup file where it is to be used:
    ///
    ///     xmlns:MyNamespace="clr-namespace:AERO;assembly=AERO"
    ///
    /// You will also need to add a project reference from the project 
    /// where the XAML file lives
    /// to this project and Rebuild to avoid compilation errors:
    ///
    ///     Right click on the target project in the Solution Explorer and
    ///     "Add Reference"->"Projects"->[Browse to and select this project]
    ///
    ///
    /// Step 2)
    /// Go ahead and use your control in the XAML file. 
    /// Note that Intellisense in the
    /// XML editor does not currently work on custom controls 
    /// and their child elements.
    ///
    ///     <MyNamespace:Wizard/>
    ///
    /// </summary>
    public class AeroWizard : Window, INotifyPropertyChanged
    {
        static AeroWizard()
        {
            // This OverrideMetadata call tells the system that this 
            // element wants to provide a style that is different 
            // than its base class.
            // This style is defined in themes\generic.xaml
            DefaultStyleKeyProperty.OverrideMetadata(typeof(AeroWizard), new FrameworkPropertyMetadata(typeof(AeroWizard)));
        }

        public new string Title
        {
            get { return (string)GetValue(TitleProperty); }
            set { SetValue(TitleProperty, value); }
        }

        // Using a DependencyProperty as the backing 
        // store for Title.  This enables animation, styling, binding, etc...
        public static new readonly DependencyProperty TitleProperty =
            DependencyProperty.Register(
            "Title", 
            typeof(string), 
            typeof(AeroWizard), 
            new UIPropertyMetadata(null));

        public string MainInstruction
        {
            get { return (string)GetValue(MainInstructionProperty); }
            set { SetValue(MainInstructionProperty, value); }
        }

        // Using a DependencyProperty as the backing 
        // store for MainInstruction.  This enables animation, 
        // styling, binding, etc...
        public static readonly DependencyProperty MainInstructionProperty =
            DependencyProperty.Register(
                "MainInstruction", 
                typeof(string), 
                typeof(AeroWizard), 
                new UIPropertyMetadata("Main Instruction"));

        public ImageSource Glyph
        {
            get { return (ImageSource)GetValue(GlyphProperty); }
            set { SetValue(GlyphProperty, value); }
        }

        // Using a DependencyProperty as the backing store for 
        // Glyph.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty GlyphProperty =
            DependencyProperty.Register(
                "Glyph", 
                typeof(ImageSource), 
                typeof(AeroWizard), 
                new UIPropertyMetadata(null));

        IntPtr handle;

        public Frame ContentArea
        {
            get { return (Frame)GetValue(ContentAreaProperty); }
            set { SetValue(ContentAreaProperty, value); }
        }

        // Using a DependencyProperty as the backing store
        // for ContentArea.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty ContentAreaProperty =
            DependencyProperty.Register(
                "ContentArea", 
                typeof(Frame), 
                typeof(AeroWizard), 
                new UIPropertyMetadata(null));


        public AeroWizard()
        {
            this.Loaded += new RoutedEventHandler(Window1_Loaded);
            CommandBinding cb = new CommandBinding(NavigationCommands.BrowseBack);

            cb.CanExecute += cb_CanExecute;
            cb.Executed += cb_Executed;

            this.CommandBindings.Add(cb);
            ContentArea = new Frame();
            ContentArea.NavigationUIVisibility = System.Windows.Navigation.NavigationUIVisibility.Hidden;
        }

        void cb_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            if (this.ContentArea.CanGoBack)
                this.ContentArea.GoBack();
        }

        void cb_CanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = true;
        }

        private void LoadPages()
        {
            this.ContentArea.Content = GetPages(this)[0];
        }

        public static List<Page> GetPages(DependencyObject obj)
        {
            return (List<Page>)obj.GetValue(PagesProperty);
        }

        public static void SetPages(DependencyObject obj, List<Page> value)
        {
            obj.SetValue(PagesProperty, value);
        }

        // Using a DependencyProperty as the backing store for Pages.  
        // This enables animation, styling, binding, etc...
        public static readonly DependencyProperty PagesProperty =
            DependencyProperty.RegisterAttached(
                "Pages", 
                typeof(List<Page>), 
                typeof(Window), 
                new UIPropertyMetadata(new List<Page>()));


        void OnClick(object sender, RoutedEventArgs e)
        {
            NextPage();
        }

        public void NextPage()
        {
            List<Page> pages = GetPages(this);

            int index = pages.IndexOf((Page)this.ContentArea.Content);

            if (index + 1 < pages.Count)
            {
                this.ContentArea.Navigate(pages[index + 1]);
            }
        }

        void Window1_Loaded(object sender, RoutedEventArgs e)
        {
            LoadPages();

            ActivateGlass();
        }

        void ActivateGlass()
        {
            PresentationSource p = PresentationSource.FromVisual(this);
            HwndSource s_hwndSource = p as HwndSource;

            if (s_hwndSource != null)
            {
                s_hwndSource.CompositionTarget.BackgroundColor = Color.FromArgb(0, 0, 0, 0);
                handle = s_hwndSource.Handle;
            }


            SafeNativeMethods.RECT r = new SafeNativeMethods.RECT();

            UnsafeNativeMethods.GetClientRect(handle, ref r);

            IntPtr hrgn = UnsafeNativeMethods.CreateRectRgn(0, 0, 1, 1);

            SafeNativeMethods.DWM_BLURBEHIND bb = new SafeNativeMethods.DWM_BLURBEHIND();
            bb.dwFlags = SafeNativeMethods.DWM_BB_ENABLE | SafeNativeMethods.DWM_BB_BLURREGION;
            bb.fEnable = true;
            bb.hRgnBlur = hrgn;

            UnsafeNativeMethods.DwmEnableBlurBehindWindow(handle, ref bb);

            SafeNativeMethods.MARGINS mar = new SafeNativeMethods.MARGINS();

            mar.cyTopHeight = 37;

            UnsafeNativeMethods.DwmExtendFrameIntoClientArea(handle, ref mar);

            //Need to make the Window size dirty.
            this.WindowState = WindowState.Minimized;
            this.WindowState = WindowState.Normal;
        }


        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler PropertyChanged;

        #endregion
    }
}
