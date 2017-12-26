========================================================================
    SILVERLIGHT APPLICATION : VBSL3FullScreen Project Overview
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

Step1. Create a Visual Basic Silverlight Application project named VBSL3FullScreen
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

<UserControl x:Class="VBSL3FullScreen.MainPage"
    xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation" 
    xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
    xmlns:d="http://schemas.microsoft.com/expression/blend/2008" xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006" 
    mc:Ignorable="d" d:DesignWidth="640" d:DesignHeight="480" KeyDown="UserControl_KeyDown">
      
C. Edit MainPage.xaml.vb.

Step1. Replace MainPage class with the following code.

   Partial Public Class MainPage
    Inherits UserControl
    Public Sub New()
        InitializeComponent()

        ' Attach the Loaded event to hook up events on load stage.    
        AddHandler Loaded, AddressOf MainPage_Loaded

    End Sub

    Private Sub MainPage_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)
        ' Attach events of SilverlightHost to subscribe the 
        ' FullScreenChanged and Resized event.
        AddHandler App.Current.Host.Content.FullScreenChanged, AddressOf Content_FullScreenChanged
        AddHandler App.Current.Host.Content.Resized, AddressOf Content_Resized
    End Sub

    Private Sub Content_Resized(ByVal sender As Object, ByVal e As EventArgs)
        ' When content get resized, refresh TextBlockShowSize control to
        ' show the size of the Silverlight plug-in.           
        RefreshTextBlockShowSize()
    End Sub

    Private Sub Content_FullScreenChanged(ByVal sender As Object, ByVal e As EventArgs)
        ' When full screen mode changed, refresh TextBlockShowSize 
        ' control to show the size of the Silverlight plug-in.
        RefreshTextBlockShowSize()
    End Sub
    Private Sub RefreshTextBlockShowSize()
        ' Show the size of the Silverlight plug-in on TextBlockShowSize 
        ' control.
        Me.TextBlockShowSize.Text = String.Format("{0}*{1}", App.Current.Host.Content.ActualWidth, App.Current.Host.Content.ActualHeight)
    End Sub
    Private Sub UserControl_KeyDown(ByVal sender As Object, ByVal e As KeyEventArgs)
        ' Show the input key on TextBlockShowKeyboardInput control.
        Me.TextBlockShowKeyboardInput.Text = e.Key.ToString()
    End Sub

    Private Sub Button_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        ' Switch to full screen mode or embeded mode.
        App.Current.Host.Content.IsFullScreen = Not App.Current.Host.Content.IsFullScreen
    End Sub
End Class
    
    
/////////////////////////////////////////////////////////////////////////////
References:

Full-Screen Support
http://msdn.microsoft.com/en-us/library/cc189023(VS.95).aspx


/////////////////////////////////////////////////////////////////////////////
