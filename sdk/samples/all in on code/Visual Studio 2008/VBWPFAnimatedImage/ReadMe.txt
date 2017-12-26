================================================================================
       WPF APPLICATION : VBWPFAnimatedImage Project Overview
       
                       AnimatedImage Sample
                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The sample demonstrates how to display a series of photos just like a digital
picuture frame with a "Wipe" effect.
   

/////////////////////////////////////////////////////////////////////////////
Code Logic:


1. Add two Image controls named myImage1 and myImage2 on a Window. The myImage1
lies on top of the myImage2.

2. Set the OpacityMask of the myImage1 to a LinearGradientBrush. Add two 
GradientStop in the LinearGradientBrush.

3. Add two Storyboards in the resource dictionary of the Window. One storyboard
is named VisibleToInvisible. It animates the two GradientStop above to hide the
myImage1. The other storyboard is named InvisibleToVisible. It animates the
two GradientStop to show the myImage1.

4. Create a collection of type List<BitmapImage> and add images to be shown in 
the collection when the Window is loaded.

5. Subscribe the Completed event of the two storyboard.

6. In the Completed event handler of the VisibleToInvisible storyboard, change
the Source of the myImage1 to the next image to be shown. Get the 
InvisibleToVisible storyboard from the resource dictionary and start it.

7. In the Completed event handler of the InvisibleToVisible storyboard, change
the Source of the myImage2 to the next image to be shown. Get the
VisibleToInVisible storyboard from the resource dictionary and start it.



/////////////////////////////////////////////////////////////////////////////
References:


   

/////////////////////////////////////////////////////////////////////////////
Demo

Step1. Build the sample project in Visual Studio 2008.

Step2. Press F5 to start without Debugging. You will see images shown the 
window is switching.
/////////////////////////////////////////////////////////////////////////////