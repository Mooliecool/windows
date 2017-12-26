========================================================================
    SILVERLIGHT APPLICATION : VBSL3Animation Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This example illustrates how to play animation in Silverlight. Since animation
is a big topic, we only covers PointAnimation in this sample. For other animation
classes, the code should be similar.


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

Silverlight 3 Tools for Visual Studio 2008 SP1
http://www.microsoft.com/downloads/details.aspx?familyid=9442b0f2-7465-417a-88f3-5e7b5409e9dd&displaylang=en

Silverilght 3 runtime:
http://silverlight.net/getstarted/


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. MainPage.xaml:

It is the container of the following UserControls:
BasicPointAnimation.xaml
AnimateDependencyProperty.xaml
Easing.xaml
CodeBehindCreation.xaml
UsingKeyFrames.xaml

2. BasicPointAnimation.xaml:

It shows how to write baisc PointAnimation for an EllipseGeometry. In the
MouseLeftButtonDown event of MyStackPanel, current mouse position is got and the
To property of PointAnimation object is updated. After that, call Begin() method
of the StoryBoard object to play animation.

3. MyEllipse.xaml:

This UserControl is used to wrap a EllipseGeometry. EllipseGeometry doesn't
expose any event for its Center change. So to get a notification after Center
change EllipseCenterProperty is added. A callback method is hooked so that whenever
the EllipseCenterProperty is changed we can synchronize the changed value with the
Center property of EllipseGeometry object, in which way to update UI.

4. AnimateDependencyProperty.xaml

This UserControl uses MyEllipse UserControl. The TargetProperty of the StoryBoard
in it is the EllipseCenterProperty of MyEllipse object. During the animation, the
EllipseCenterProperty will be changed and the callback method MyHandler() will be 
called, which in turn calls OnEllipseCenterChanged() method to trigger the 
EllipseCenterChanged event. The EllipseCenterChanged event is public so we can hook
event handler in AnimateDependencyProperty UserControl to get a notification for 
EllipseCenterProperty's change. In this sample, a Line object is updated in this
event handler to simulate an animation effect.

5. Easing.xaml

This UserControl demonstrates how to use built-in Ease classes and how to write a
custom Ease class. By inheriting EasingFunctionBase and override its EaseInCore()
method we can write our own Ease class.

6. CodeBehindCreation.xaml

This UserControl shows how to initialize a Storyboard in code behind. The final effect
is the same as BasicPointAnimation.xaml, which uses XAML to add Storyboard.

7. UsingKeyFrames.xaml

This UserControl shows how to create KeyFrames based animation in Silverlight.
In the sample SplineDoubleKeyFrame is used to specify each KeyFrame.


/////////////////////////////////////////////////////////////////////////////
References:

Animation
http://msdn.microsoft.com/en-us/library/cc189090(VS.95).aspx


/////////////////////////////////////////////////////////////////////////////