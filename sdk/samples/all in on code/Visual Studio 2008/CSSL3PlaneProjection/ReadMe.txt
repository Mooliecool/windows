========================================================================
    SILVERLIGHT APPLICATION : CSSL3PlaneProjection Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This example illustrates how to use the new perspective 3D feature of 
Silverlight 3. PlaneProjection is the commonly  used object to achieve 
perspective 3D effect. In this sample, you'll see how to control perspective 
projection by adjusting the properties of PlaneProjection object. In addition,
you'll see the control's functionality remains after projection. You can 
confirm this by clicking a date of the Calendar control.


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

Silverlight 3 Tools for Visual Studio 2008 SP1
http://www.microsoft.com/downloads/details.aspx?familyid=9442b0f2-7465-417a-88f3-5e7b5409e9dd&displaylang=en

Silverilght 3 runtime:
http://silverlight.net/getstarted/


/////////////////////////////////////////////////////////////////////////////
Creation:

A. Create the Silverlight project

Step1. Create a Visual C# Silverlight Application project named CSSL3PlaneProjection
in Visual Studio 2008 SP1.

B. Edit MainPage.xaml

Step1. Double click MainPage.xaml in the Solution Explorer window to view the 
xaml code. Drag a Calendar control from toolbox and drop it in the <Grid>.

Step2. Replace the entire <Grid> with the following code:

  <Grid x:Name="LayoutRoot" Background="AliceBlue">
      <Grid.RowDefinitions>
            <RowDefinition Height="2*"></RowDefinition>
            <RowDefinition Height="4*"></RowDefinition>
            <RowDefinition Height="4*"></RowDefinition>
      </Grid.RowDefinitions>
        <StackPanel Grid.Row="0">
            <TextBlock x:Name="TextBlockPlaneProjectionDetails"></TextBlock>
            <TextBlock x:Name="TextBlockShowDate" VerticalAlignment="Center" HorizontalAlignment="Center" FontSize="24"/>
        </StackPanel>
      <controls:Calendar x:Name="CalendarPerspective3D" Grid.Row="1" SelectedDatesChanged="Calendar_SelectedDatesChanged">
      	<controls:Calendar.Projection>
      		<PlaneProjection x:Name="PlaneProjection" CenterOfRotationX="0" CenterOfRotationY="0"/>
            </controls:Calendar.Projection></controls:Calendar>
        <Grid  Grid.Row="2">
            <Grid.ColumnDefinitions>
                <ColumnDefinition></ColumnDefinition>
                <ColumnDefinition></ColumnDefinition>
            </Grid.ColumnDefinitions>
            <StackPanel Grid.Column="0">
                <TextBlock Height="17" Margin="0" Width="60" Text="RotationX" TextWrapping="Wrap"/>
                <Slider x:Name="SliderRotationX" Minimum="-360" Maximum="360" Width="200" Margin="0" ValueChanged="SliderRotation_ValueChanged"/>
                <TextBlock Height="17" Margin="0" Width="60" Text="RotationY" TextWrapping="Wrap"/>
                <Slider x:Name="SliderRotationY" Minimum="-360" Maximum="360" Width="200" Margin="0" ValueChanged="SliderRotation_ValueChanged"/>
                <TextBlock Height="17" Margin="0" Width="60" Text="RotationZ" TextWrapping="Wrap"/>
                <Slider x:Name="SliderRotationZ" Minimum="-360" Maximum="360" Width="200" Margin="0" ValueChanged="SliderRotation_ValueChanged"/>
                <Button Content="Reset" Width="200" Height="25" Click="ButtonResetRotation_Click"></Button>
            </StackPanel>
            <StackPanel Grid.Column="1">
                <TextBlock Height="17" Margin="0" Width="110" Text="CenterOfRotationX" TextWrapping="Wrap"/>
                <Slider x:Name="SliderCenterX" Minimum="-50" Maximum="50" Width="200" Margin="0" ValueChanged="SliderCenterOfRotation_ValueChanged"/>
                <TextBlock Height="17" Margin="0" Width="110" Text="CenterOfRotationY" TextWrapping="Wrap"/>
                <Slider x:Name="SliderCenterY" Minimum="-50" Maximum="50" Width="200" Margin="0" ValueChanged="SliderCenterOfRotation_ValueChanged"/>
                <TextBlock Height="17" Margin="0" Width="110" Text="CenterOfRotationZ" TextWrapping="Wrap"/>
                <Slider x:Name="SliderCenterZ" Minimum="-50" Maximum="50" Width="200" Margin="0" ValueChanged="SliderCenterOfRotation_ValueChanged"/>
                <Button Content="Reset" Width="200" Height="25" Click="ButtonResetCenterOfRotation_Click"></Button>
            </StackPanel>
        </Grid>          
  </Grid>
    
The above code mainly adds these controls:
A Calendar control that the projection act on.
Three Slider controls that control the rotation related properties of PlaneProjection.
Three Slider controls that control the rotation center related properties of PlaneProjection.
A TextBlock control (TextBlockPlaneProjectionDetails) that shows the current rotation and
rotation center related values of PlaneProjection.
A TextBlock control (TextBlockShowDate) that shows the selected date of Calendar control.

C. Edit MainPage.xaml.cs

Step1. Replace MainPage class with the following code:

    public partial class MainPage : UserControl
    {

        public MainPage()
        {
            InitializeComponent();
        }


        private void Calendar_SelectedDatesChanged(object sender, SelectionChangedEventArgs e)
        {
            // Get the reference of the sender Calendar, then show the selected date on TextBlockShowDate control.
            Calendar calendar = (Calendar)sender;
            this.TextBlockShowDate.Text = calendar.SelectedDate.Value.ToShortDateString();
        }

        private void SliderRotation_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            // Change the rotation center position of PlaneProjection and rotate Calendar.
            RefreshPlaneProjection(this.PlaneProjection, this.SliderRotationX.Value, this.SliderRotationY.Value, this.SliderRotationZ.Value,
                double.NaN, double.NaN, double.NaN);
            // Refresh TextBlockPlaneProjectionDetails control to show the current value of 
            // RotationX, RotationY, RotationZ, CenterOfRotationX, CenterOfRotationY, CenterOfRotationZ 
            // of PlaneProjection.
            RefreshTextBlockPlaneProjectionDetails();
        }

        private void SliderCenterOfRotation_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            // Change the rotation center position of PlaneProjection and rotate Calendar.
            RefreshPlaneProjection(this.PlaneProjection, double.NaN, double.NaN, double.NaN,
                this.SliderCenterX.Value, this.SliderCenterY.Value, this.SliderCenterZ.Value);
            // Refresh TextBlockPlaneProjectionDetails control to show the current value of 
            // RotationX, RotationY, RotationZ, CenterOfRotationX, CenterOfRotationY, CenterOfRotationZ 
            // of PlaneProjection.
            RefreshTextBlockPlaneProjectionDetails();
        }

        private void RefreshPlaneProjection(PlaneProjection planeProjection, double rotationX, double rotationY, double rotationZ, double centerOfRotationX, double centerOfRotationY, double centerOfRotationZ)
        {
            // Change rotation and rotation center related properties of PlaneProjection.
            if (!double.IsNaN(rotationX))
                planeProjection.RotationX = rotationX;
            if (!double.IsNaN(rotationY))
                planeProjection.RotationY = rotationY;
            if (!double.IsNaN(rotationZ))
                planeProjection.RotationZ = rotationZ;
            if (!double.IsNaN(centerOfRotationX))
                planeProjection.CenterOfRotationX = centerOfRotationX;
            if (!double.IsNaN(centerOfRotationY))
                planeProjection.CenterOfRotationY = centerOfRotationY;
            if (!double.IsNaN(centerOfRotationZ))
                planeProjection.CenterOfRotationZ = centerOfRotationZ;
        }

        private void RefreshTextBlockPlaneProjectionDetails()
        {
            this.TextBlockPlaneProjectionDetails.Text = string.Format(@"RotationX:{0}, RotationY:{1}, RotationZ:{2}
CenterOfRotationX:{3}, CenterOfRotationY:{4}, CenterOfRotationZ:{5}",
            this.PlaneProjection.RotationX, this.PlaneProjection.RotationY, this.PlaneProjection.RotationZ,
            this.PlaneProjection.CenterOfRotationX, this.PlaneProjection.CenterOfRotationY, this.PlaneProjection.CenterOfRotationZ);
        }

        private void ButtonResetRotation_Click(object sender, RoutedEventArgs e)
        {
            // Reset the value of roation related Sliders.
            this.SliderRotationX.Value = 0;
            this.SliderRotationY.Value = 0;
            this.SliderRotationZ.Value = 0;
        }

        private void ButtonResetCenterOfRotation_Click(object sender, RoutedEventArgs e)
        {
            // Reset the value of roation center related Sliders.
            this.SliderCenterX.Value = 0;
            this.SliderCenterY.Value = 0;
            this.SliderCenterZ.Value = 0;
        }
    }


/////////////////////////////////////////////////////////////////////////////
References:

3-D Effects (Perspective Transforms)
http://msdn.microsoft.com/en-us/library/dd470131(VS.95).aspx


/////////////////////////////////////////////////////////////////////////////