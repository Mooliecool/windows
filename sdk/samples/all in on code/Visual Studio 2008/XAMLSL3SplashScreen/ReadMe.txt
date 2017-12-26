========================================================================
    SILVERLIGHT APPLICATION : XAMLSL3SplashScreen Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This example demonstrates how to customize the splash screen for Silverlight
application.


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

Silverlight 3 Tools for Visual Studio 2008 SP1
http://www.microsoft.com/downloads/details.aspx?familyid=9442b0f2-7465-417a-88f3-5e7b5409e9dd&displaylang=en

Silverilght 3 runtime:
http://silverlight.net/getstarted/


/////////////////////////////////////////////////////////////////////////////
Creation:

A. Create a new ASP.NET Web Application project.

Step1. Create a new ASP.NET Web Application project in Visual Studio 2008 
(This sample is a C# project. However, it doesn't matter if you create a 
VB.NET project). Name it as "XAMLSL3SplashScreen".

B. Add a .xaml file and a .js file to the created ASP.NET Web Application 
project.

Step1. Add a new .xaml file to the project. Name it as SplashScreen.xaml. 
Then paste the following code to it:

<StackPanel Width="392" xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        x:Name="parentCanvas"
     
      HorizontalAlignment="Center">
 	<StackPanel HorizontalAlignment="Center" Margin="8,8,0,229" Width="392">
  		<Canvas x:Name="AnimationCanvas" Height="132" Margin="8,0,41,0">
        <Canvas.Triggers>

          <EventTrigger RoutedEvent="Canvas.Loaded">
            <EventTrigger.Actions>
              <BeginStoryboard>
                <Storyboard AutoReverse="True" x:Name="LoadingStoryboard" RepeatBehavior="Forever">
                  <DoubleAnimationUsingKeyFrames BeginTime="00:00:00" Storyboard.TargetName="AnimationCanvas" Storyboard.TargetProperty="(Panel.Background).(GradientBrush.GradientStops)[0].(GradientStop.Offset)">
                    <EasingDoubleKeyFrame KeyTime="00:00:00" Value="0.144"/>
                    <EasingDoubleKeyFrame KeyTime="00:00:00.5000000" Value="0.296"/>
                  </DoubleAnimationUsingKeyFrames>
                </Storyboard>
              </BeginStoryboard>

            </EventTrigger.Actions>
          </EventTrigger>
        </Canvas.Triggers>
        <Canvas.Background>
          <LinearGradientBrush EndPoint="0.587,1.407" StartPoint="0.583,-0.519">
            <GradientStop Color="Black" Offset="0.144"/>
            <GradientStop x:Name="uxGradientStop" Color="White" Offset="0.86"/>
          </LinearGradientBrush>
        </Canvas.Background>
  			<TextBlock Height="81" Width="303" Canvas.Left="32" Canvas.Top="8" Text="Loading" TextWrapping="Wrap" FontSize="64" FontWeight="Bold">
  				<TextBlock.Foreground>
  					<LinearGradientBrush EndPoint="0.978,0.543" StartPoint="-0.005,0.542">
  						<GradientStop x:Name="uxGradientStop1" Color="Orange"/>
  						<GradientStop x:Name="uxGradientStop2" Color="White"/> 					
  					</LinearGradientBrush>
  				</TextBlock.Foreground>
  			</TextBlock>
  			<TextBlock x:Name="uxStatus" Height="18" Width="161" Canvas.Left="70" Canvas.Top="96" TextWrapping="Wrap" TextAlignment="Center"/>
  		</Canvas>
  	</StackPanel>
</StackPanel>

The above XAML code mainly adds three controls:

1. A Canvas that triggers a storyboard on its Loaded event. It demonstrates 
how to play animation on the splash screen.

2. A TextBlock control renders "Loading" text. GradientStop objects of its 
Foreground brush are given names so that we can access them in JavaScript 
functions.

3. Another TextBlock intended to render download progress. Its name is 
specified so that we can access it in JavaScript functions.

Step2. Add a new .js file to the project. Name it as "XAMLSplashScreen.js". 
Then paste following code to it:

function onSourceDownloadProgressChanged(sender, eventArgs) {
    sender.findName("uxStatus").Text = Math.round((eventArgs.progress * 1000)) / 10 + "%";
    sender.findName("uxGradientStop1").Offset = eventArgs.progress;
    sender.findName("uxGradientStop2").Offset = eventArgs.progress;
}

In the above function, we get the progress from eventArgs, then find the 
TextBlock and GradientStop objects and update their properties.

C. Add .xap to the project.

Step1. Add a new Folder named "ClientBin" to the project.

Step2. Add a .xap file under the "ClientBin" folder. The .xap file is created 
by Silverlight project. You can find it in the output folder of any 
Silverlight projects. By default it is under "Bin\Debug". If you cannot find 
it, please check the output path setting of the Silverlight project. You can 
right click the project node in the Solution Explorer window, click 
"Properties", select "Build" tab and set it in the Output section. Please 
note that the .xap file should be large enough or else it's possible that the 
splash screen cannot be seen by users. In this sample the .xap file is only 
1250 KB so you possibly cannot see the effect clearly. If you want to test 
the effect of splash screen, you can add large files as resources in the 
Silverlight project. Please remember to set files' Build Action to "Resource" 
or "Embeded Resource" so that they will be included in the generated .xap 
file.

D. Edit Default.aspx file.

Step1. Replace the existing <html> tag and it's children elements with the 
following code:

<html xmlns="http://www.w3.org/1999/xhtml" >
<head id="Head1" runat="server">
    <title></title>
    <style type="text/css">
    html, body {
	    height: 100%;
	    overflow: auto;
    }
    body {
	    padding: 0;
	    margin: 0;
    }
    #silverlightControlHost {
	    height: 100%;
	    text-align:center;
    }
    </style>
    <script type="text/javascript" src="Silverlight.js"></script>
    <script type="text/javascript">
        function onSilverlightError(sender, args) {
            var appSource = "";
            if (sender != null && sender != 0) {
              appSource = sender.getHost().Source;
            }
            
            var errorType = args.ErrorType;
            var iErrorCode = args.ErrorCode;

            if (errorType == "ImageError" || errorType == "MediaError") {
              return;
            }

            var errMsg = "Unhandled Error in Silverlight Application " +  appSource + "\n" ;

            errMsg += "Code: "+ iErrorCode + "    \n";
            errMsg += "Category: " + errorType + "       \n";
            errMsg += "Message: " + args.ErrorMessage + "     \n";

            if (errorType == "ParserError") {
                errMsg += "File: " + args.xamlFile + "     \n";
                errMsg += "Line: " + args.lineNumber + "     \n";
                errMsg += "Position: " + args.charPosition + "     \n";
            }
            else if (errorType == "RuntimeError") {           
                if (args.lineNumber != 0) {
                    errMsg += "Line: " + args.lineNumber + "     \n";
                    errMsg += "Position: " +  args.charPosition + "     \n";
                }
                errMsg += "MethodName: " + args.methodName + "     \n";
            }

            throw new Error(errMsg);
        }
    </script>
    
    <script type="text/javascript" src="XAMLSplashScreen.js"></script>
</head>
<body>
    <form id="form1" runat="server" style="height:100%">
    <div id="silverlightControlHost">
    <h2>
    XAMLSplashScreen
    </h2>
        <object data="data:application/x-silverlight-2," type="application/x-silverlight-2" width="100%" height="100%">
		  <param name="source" value="ClientBin/SplashScreenLoaded.xap"/>
		  <param name="onError" value="onSilverlightError" />
		  <param name="background" value="white" />
		  <param name="minRuntimeVersion" value="3.0.40624.0" />
		  <param name="autoUpgrade" value="true" />
		  
		  
<param name="splashscreensource" value="SplashScreen.xaml"/>
<param name="onSourceDownloadProgressChanged" value="onSourceDownloadProgressChanged" />


		  <a href="http://go.microsoft.com/fwlink/?LinkID=149156&v=3.0.40624.0" style="text-decoration:none">
 			  <img src="http://go.microsoft.com/fwlink/?LinkId=108181" alt="Get Microsoft Silverlight" style="border-style:none"/>
		  </a>
	    </object>

   
	
	    <iframe id="_sl_historyFrame" style="visibility:hidden;height:0px;width:0px;border:0px"></iframe></div>
    </form>
</body>
</html>

The above code is the same as auto-generated test aspx page for Silverlight 
projects except for two parts:

1. A <script> tag is used to reference the JavaScript file:
 <script type="text/javascript" src="XAMLSplashScreen.js"></script>
 
2. Two <param>s are used to specify the splash screen source and the event 
handler of onSourceDownloadProgressChanged:

<param name="splashscreensource" value="SplashScreen.xaml"/>
<param name="onSourceDownloadProgressChanged" value="onSourceDownloadProgressChanged" />

To use your xap file, you also need to change the value of:
<param name="source" value="ClientBin/SplashScreenLoaded.xap"/>


/////////////////////////////////////////////////////////////////////////////
Known Issue:

If the source URL does not end in ".xap", Silverlight cannot know if it is a 
request to a Silverlight 1.0 app (XAML+JS) or a XAP before download. So the 
default splash screen will be used even if you've specified a custom splash 
screen source.


/////////////////////////////////////////////////////////////////////////////
References:

How to: Define a Simple Silverlight Splash Screen
http://msdn.microsoft.com/en-us/library/cc903962(VS.95).aspx


/////////////////////////////////////////////////////////////////////////////