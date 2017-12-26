========================================================================
    SILVERLIGHT APPLICATION : CSSL3PixelShader Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This example demonstrates how to use new pixel shader feature in Silverlight3.
It mainly covers two parts:

1. How to use built-in Effect such as DropShadowEffect.
2. How to create a custom ShaderEffect and use it in the application.


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

Silverlight 3 Tools for Visual Studio 2008 SP1
http://www.microsoft.com/downloads/details.aspx?familyid=9442b0f2-7465-417a-88f3-5e7b5409e9dd&displaylang=en

Silverilght 3 runtime:
http://silverlight.net/getstarted/

DirectX SDK: (Not required to run the application but you need fxc tool in it
to create the .ps file if you would like to create the .ps file on your own)
http://www.microsoft.com/DOWNLOADS/details.aspx?FamilyID=24a541d6-0486-4453-8641-1eee9e21b282&displaylang=en


/////////////////////////////////////////////////////////////////////////////
Creation:

A. Create a .ps file to be used in custom ShaderEffect. (Skip this step if 
you want to use the existing .ps file in this project directly)

Step1. Create a new .txt file. Open it and paste following HLSL(High Level 
Shader Language) code to it:

sampler2D input : register(S0);
float2 center:register(C0);
float amplitude:register(C1);

float4 main(float2 uv : TEXCOORD) : COLOR
{
	if(pow((uv.x-center.x),2)+pow((uv.y-center.y),2)<0.15)
	{
		uv.y = uv.y  + (sin(uv.y*100)*0.1*amplitude);
	}
	return tex2D( input , uv.xy);
}

Step2. Save the .txt file. Close it. Rename it as ovalwatery.fx.

Step3. Open DirectX SDK Command Prompt and run the following command:

fxc /T ps_2_0 /Fo "<OutputPath>\ovalwatery.ps" "<InputPath>\ovalwatery.fx"

The <InputPath> is the path of the .fx file.
The <OutPutpat> is the path of the .ps file you want to create.

B. Create the Silverlight project.

Step1. Create a Visual C# Silverlight Application project named 
CSSL3PixelShader in Visual Studio 2008 SP1.

C. Add the .ps file to the project.

Step1. Right click the project node in the Solution Explorer window, select 
Add-> Existing Item to add the .ps file (craeted in step3 of A) to the 
project.

D. Add the "Humpback Whale.jpg" file to the project.

Step1. Right click the project node in the Solution Explorer window, select
Add-> Existing Item to add the "Humpback Whale.jpg" (you can find it in this 
project) file to the project.

E. Edit xaml.

Step1. Double click MainPage.xaml in the Solution Explorer window to view the 
xaml code. Replace the <Grid> with the following code:

	<Grid x:Name="LayoutRoot">
		<Grid.RowDefinitions>
			<RowDefinition Height="1*"/>
			<RowDefinition Height="9*"/>
        </Grid.RowDefinitions>
        <TextBlock HorizontalAlignment="Center" Foreground="Red" FontSize="32" 
			Text="Please Click the Image">
            <TextBlock.Effect>
                <DropShadowEffect Color="Black">    
                </DropShadowEffect>
            </TextBlock.Effect>
		</TextBlock>
			<Image Grid.Row="1" Width="640" Height="480"  
				x:Name="ImagePixelShader" Source="Humpback Whale.jpg">
		</Image>
	</Grid>

The above code mainly adds two controls:

A TextBlock used to show how to use the built-in DropShadowEffect.
An Image control used to show how to use custom ShaderEffect.

Step2. Replace <UserControl> tag with the following code:

<UserControl x:Class="CSSL3PixelShader.MainPage"
    xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation" 
    xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
    xmlns:d="http://schemas.microsoft.com/expression/blend/2008" 
    xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006" 
    xmlns:c="clr-namespace:CSSL3PixelShader" 
    MouseLeftButtonDown="UserControl_MouseLeftButtonDown"
    mc:Ignorable="d" d:DesignWidth="640" d:DesignHeight="480">

This step mainly does two things:

Hook the MouseLeftButtonDown event of the UserControl:
MouseLeftButtonDown="UserControl_MouseLeftButtonDown"

Define a new xmlns:
xmlns:c="clr-namespace:CSSL3PixelShader"

F. Edit xaml.cs.

Step1. Double click MainPage.xaml.cs to view the code. Add following code 
after the last "using System.Windows.Shapes;":

using System.Windows.Threading;
using System.Windows.Media.Effects;

Step2. Replace the MainPage class with the following code:

	  public partial class MainPage : UserControl
    {
        // A timer used to reduce the value of _amplitude gradually.
        private DispatcherTimer _timer = new DispatcherTimer();
        // Init custom effect
        OvalWateryEffect _effect = new OvalWateryEffect(
                    new Uri(@"/CSSL3PixelShader;component/ovalwatery.ps",
                        UriKind.Relative));

        public MainPage()
        {
            InitializeComponent();
            this.ImageWithPixelShader.Effect = _effect;
            // Initialize timer and hook Tick event.
            _timer.Interval = TimeSpan.FromMilliseconds(50);
            _timer.Tick += new EventHandler(_timer_Tick);
        }

        /// <summary>
        /// This event handler reduce the amplitude and apply a new 
        /// OvalWateryEffect on each tick.
        /// </summary>
        void _timer_Tick(object sender, EventArgs e)
        {
            if (this._effect.Amplitude > 0.0)
            {
                this._effect.Amplitude -= 0.05; 
            }
            else
            {
                this._timer.Stop();
            }

        }

        /// <summary>
        /// This event handler get the current mouse position, assign it to a 
        /// private field and start the timer to apply new OvalWateryEffect.
        /// </summary>
        private void UserControl_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            // Convert the mouse position from control coordinates to texture coordinates as required by the PixelShader
            this._effect.Center = new Point(e.GetPosition(this.ImageWithPixelShader).X /
            this.ImageWithPixelShader.ActualWidth, e.GetPosition(
            this.ImageWithPixelShader).Y / this.ImageWithPixelShader.ActualHeight);
            this._effect.Amplitude= 0.5;
            _timer.Start();
        }
    }

Step3. Add a new class OvalWateryEffect after the MainPage class:

	 /// <summary>
    /// OvalWateryEffect class is a custom ShaderEffect class.
    /// </summary>
    public class OvalWateryEffect : ShaderEffect
    {
        /// <summary>
        /// The following two DependencyProperties are the keys of this custom ShaderEffect.
        /// They create a bridge between managed code and HLSL(High Level Shader Language).
        /// The PixelShaderConstantCallback will be triggered when the propery get changed.
        /// The parameter of the callback represents the register.
        /// For instance, here the 1 in PixelShaderConstantCallback(1) represents C1 of the
        /// following HLSL code. In another word, by changing
        /// the Amplitude property we assign the changed value to the amplitude variable of the
        /// following HLSL code:
        /// 
        /// sampler2D input : register(S0);
        /// float2 center:register(C0);
        /// float amplitude:register(C1);
        /// float4 main(float2 uv : TEXCOORD) : COLOR
        /// {
        /// if(pow((uv.x-center.x),2)+pow((uv.y-center.y),2)<0.15)
        /// {
        /// uv.y = uv.y  + (sin(uv.y*100)*0.1*amplitude);
        /// }
        /// return tex2D( input , uv.xy);
        /// }
        /// </summary>
        public static readonly DependencyProperty AmplitudeProperty = 
            DependencyProperty.Register("Amplitude", typeof(double), typeof(OvalWateryEffect), 
            new PropertyMetadata(0.1, ShaderEffect.PixelShaderConstantCallback(1)));

        public static readonly DependencyProperty CenterProperty = 
            DependencyProperty.Register("Center", typeof(Point), typeof(OvalWateryEffect), 
            new PropertyMetadata(new Point(0.5, 0.5), ShaderEffect.PixelShaderConstantCallback(0)));
        

        public OvalWateryEffect(Uri uri)
        {
            Uri u = uri;
            PixelShader psCustom = new PixelShader();
            psCustom.UriSource = u;
            PixelShader = psCustom;

            base.UpdateShaderValue(CenterProperty);
            base.UpdateShaderValue(AmplitudeProperty);
        }

        public double Amplitude
        {
            get
            {
                return (double)base.GetValue(AmplitudeProperty);
            }
            set
            {
                base.SetValue(AmplitudeProperty, value);
            }
        }

        public Point Center
        {
            get
            {
                return (Point)base.GetValue(CenterProperty);
            }
            set
            {
                base.SetValue(CenterProperty, value);
            }
        }
    }


/////////////////////////////////////////////////////////////////////////////
References:

Programming Guide for HLSL
http://msdn.microsoft.com/en-us/library/bb509635(VS.85).aspx

Dependency Properties Overview
http://msdn.microsoft.com/en-us/library/cc221408(VS.95).aspx

Pixel Shader Effects
http://msdn.microsoft.com/en-us/library/dd901594(VS.95).aspx


/////////////////////////////////////////////////////////////////////////////
