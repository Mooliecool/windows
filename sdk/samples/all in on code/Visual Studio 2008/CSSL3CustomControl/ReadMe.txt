========================================================================
    SILVERLIGHT APPLICATION : CSSL3CustomControl Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This example demonstrates how to Create custom control HighLightTextBlock in
silverlight 3. HighLightTextBlock enhanced TextBlock, providing HighLight() 
method. By calling HighLight(), text appearance would be changed to attract
user’s attention.

/////////////////////////////////////////////////////////////////////////////
Prerequisites:

Silverlight 3 Tools for Visual Studio 2008 SP1
http://www.microsoft.com/downloads/details.aspx?familyid=9442b0f2-7465-417a-88f3-5e7b5409e9dd&displaylang=en

Silverilght 3 runtime:
http://silverlight.net/getstarted/

/////////////////////////////////////////////////////////////////////////////
Project Relation:

CSSL3CustomControl - XAMLSL3StyleControlTemplate

CSSL3CustomControl demonstrates how to create a Custom Control.
XAMLSL3StyleControlTemplate demonstrates how to customize the style
and template of both built-in Controls and Custom Controls.

/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. How to create custom control
    1. Create class derived from System.Windows.Controls.Control.
    2. In constructor, set DefaultStyleKey to typeof custom control.
    3. Create ResourceDictionary file named generic.xaml and place at Themes folder.
    4. in generic.xaml, create default template for custom control
    
2. How to manipulate controls which are in custom control's template
    1. In controltemplate, set Name to target control.
    2. In custom control class, we could use GetTemplateChild("[controlname]")
    method to get reference to target control.

3. How to use VisualState in custom control?
    1. Define VisualStateGroup and VisualState in xaml
    
            <VisualStateManager.VisualStateGroups>
                <VisualStateGroup x:Name="HightLightStates">
                    <VisualState x:Name="HighLight">
                        <Storyboard>
                            //animate the control
                        </Storyboard>
                    </VisualState>
                    <VisualState x:Name="NonHighLight"/>
                </VisualStateGroup>
            </VisualStateManager.VisualStateGroups>
            
    2. Change VisualState in custom control code

            VisualStateManager.GoToState(this, "[visualstate name]", true);

4. How to use Timer in silverlight?
    In silverlight, you could use System.Windows.Threading.DispatcherTimer.
    
    private void TestDispatcherTimer(Panel counterPanel)
    {
        DispatcherTimer timer = new DispatcherTimer();
        int counter = 0;

        counterPanel.MouseLeftButtonDown += 
            delegate(object s, MouseButtonEventArgs args) {
                if (timer.IsEnabled) timer.Stop(); else timer.Start(); 
            };

        timer.Tick += 
            delegate(object s, EventArgs args) {
                counterPanel.Children.Clear();
                counterPanel.Children.Add( new TextBlock { 
                    Text = counter++.ToString() });
            };

        timer.Interval = new TimeSpan(0, 0, 1); // one second
        timer.Start();
    }
    You could get detail about DispatcherTimer at
    http://msdn.microsoft.com/en-us/library/system.windows.threading.dispatchertimer(VS.95).aspx
  

/////////////////////////////////////////////////////////////////////////////
References:

Creating a New Control by Creating a ControlTemplate
http://msdn.microsoft.com/en-us/library/cc278064(VS.95).aspx


/////////////////////////////////////////////////////////////////////////////
