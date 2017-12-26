========================================================================
    SILVERLIGHT APPLICATION : CSSL3FullScreen Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This example demonstrates how to use the full screen feature in Silverlight 3.
In addition, it demonstrates the keyboard limiation in full screen mode, how
to get the size of Silverilght plug-in in full screen mode and how to subscribe
FullScreenChanged event.


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

Silverlight 3 Tools for Visual Studio 2008 SP1
http://www.microsoft.com/downloads/details.aspx?familyid=9442b0f2-7465-417a-88f3-5e7b5409e9dd&displaylang=en

Silverilght 3 runtime:
http://silverlight.net/getstarted/


/////////////////////////////////////////////////////////////////////////////
Creation:

A. Create the Silverlight project

Step1. Create a Visual C# Silverlight Application project named CSSL3FullScreen
in Visual Studio 2008 SP1.

B. Edit MainPage.xaml

Step1. Double click MainPage.xaml in the Solution Explorer window to view the 
xaml code. Replace <Grid> with the following code:

  <Grid x:Name="LayoutRoot" Background="AliceBlue">
        <Grid.RowDefinitions>
            <RowDefinition Height="2.3*"></RowDefinition>
            <RowDefinition Height="1*"></RowDefinition>
            <RowDefinition Height="3*"></RowDefinition>
            <RowDefinition Height="2*"></RowDefinition>
        </Grid.RowDefinitions>
        <TextBlock>
            Please click the button below to switch to full-screen mode or embeded mode.
           <LineBreak/>
            Please test the keyboard in both full-screen mode and embeded mode. 
           <LineBreak/>
            All supported keys will be printed on the screen. In full-screen mode, the only
            supported keys are:
           <LineBreak/>
            UP ARROW, DOWN ARROW, LEFT ARROW, RIGHT ARROW, SPACEBAR, TAB, PAGE UP, PAGE DOWN, HOME, END, ENTER
        </TextBlock>

        <Button IsTabStop="False" Grid.Row="1" Width="180" Height="30" Content="Full Screen Switch" Click="Button_Click"></Button>
        <TextBlock x:Name="TextBlockShowKeyboardInput" Grid.Row="2" FontSize="36"></TextBlock>
        <TextBlock x:Name="TextBlockShowSize" Grid.Row="3" FontSize="20"></TextBlock>
    </Grid>

The above code mainly adds four controls:
A TextBlock control to instruct you how to test the project.
A Button control usded to swith to full screen mode or embeded mode.
TextBlockShowKeyboardInput control (TextBlock) to show the keyboard input.
TextBlockShowSize control (TextBlock) to show the size of Silverlight plug-in.

Step2. Add KeyDown event for the <UserControl> to catch keyboard input, like below:

<UserControl x:Class="CSSL3FullScreen.MainPage"
    xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation" 
    xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
    xmlns:d="http://schemas.microsoft.com/expression/blend/2008" xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006" 
    mc:Ignorable="d" d:DesignWidth="640" d:DesignHeight="480"   KeyDown="UserControl_KeyDown">
    
C. Edit MainPage.xaml.cs.

Step1. Replace MainPage class with the following code.

    public partial class MainPage : UserControl
    {
        public MainPage()
        {
            InitializeComponent();

            // Attach the Loaded event to hook up events on load stage.    
            this.Loaded += new RoutedEventHandler(MainPage_Loaded);

        }

        void MainPage_Loaded(object sender, RoutedEventArgs e)
        {
            // Attach events of SilverlightHost to subscribe the 
            // FullScreenChanged and Resized event.
            App.Current.Host.Content.FullScreenChanged += new EventHandler(Content_FullScreenChanged);
            App.Current.Host.Content.Resized += new EventHandler(Content_Resized);
        }

        void Content_Resized(object sender, EventArgs e)
        {
            // When content get resized, refresh TextBlockShowSize control to
            // show the size of the Silverlight plug-in.           
            RefreshTextBlockShowSize();
        }

        void Content_FullScreenChanged(object sender, EventArgs e)
        { 
            // When full screen mode changed, refresh TextBlockShowSize 
            // control to show the size of the Silverlight plug-in.
            RefreshTextBlockShowSize();
        }
        private void RefreshTextBlockShowSize()
        {
            // Show the size of the Silverlight plug-in on TextBlockShowSize 
            // control.
            this.TextBlockShowSize.Text = string.Format("{0}*{1}",
                    App.Current.Host.Content.ActualWidth,
                    App.Current.Host.Content.ActualHeight);
        }
        private void UserControl_KeyDown(object sender, KeyEventArgs e)
        {
            // Show the input key on TextBlockShowKeyboardInput control.
            this.TextBlockShowKeyboardInput.Text = e.Key.ToString();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            // Switch to full screen mode or embeded mode.
            App.Current.Host.Content.IsFullScreen =
                !App.Current.Host.Content.IsFullScreen;
        }
    }
    
    
/////////////////////////////////////////////////////////////////////////////
References:

Full-Screen Support
http://msdn.microsoft.com/en-us/library/cc189023(VS.95).aspx


/////////////////////////////////////////////////////////////////////////////
