================================================================================
       WINDOWS FORMS APPLICATION : CSWinFormGraphics Project Overview
       
                        Graphics Sample
                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The Graphics sample demonstrates the fundamentals of GDI+ programming. 

GDI+ allows you to create graphics, draw text, and manipulate graphical images as objects. 
GDI+ is designed to offer performance as well as ease of use. 
You can use GDI+ to render graphical images on Windows Forms and controls. 
GDI+ has fully replaced GDI, and is now the only way to render graphics programmatically 
in Windows Forms applications.

In this sample, there're 5 examples:

1. Draw A Line.
   Demonstrates how to draw a solid/dash/dot line.
2. Draw A Curve.
   Demonstrates how to draw a curve, and the difference between antialiasing rendering mode
   and no antialiasing rendering mode.
3. Draw An Arrow.
   Demonstrates how to draw an arrow.
4. Draw A Vertical String.
   Demonstrates how to draw a vertical string.
5. Draw A Ellipse With Gradient Brush.
   Demonstrates how to draw a shape with gradient effect.
   

/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Example 1: "Draw A Line".

   1). Use Graphics.DrawLine method with a normal pen to draw a normal line;
   2). Use Graphics.DrawLine method with a dash pen to draw a dashed line;
   3). Use Graphics.DrawLine method with a dot pen to draw a dotted line;

2. Example 2: "Draw A Curve".

   1). Create a collection of points for the curve;
   2). Draw a curve without antialiasing rendering mode.
   3). Draw a curve with antialiasing rendering mode.
   
3. Example 3: "Draw An Arrow".
   
   1). Create a pen with EndCap property set to ArrowAnchor;
   2). Use the pen created at step #1 to draw the line;
   
4. Example 4: "Draw A Vertical String".
  
   1). Create a StringFormat object with FormatFlags property set to 
       StringFormatFlags.DirectionVertical;
   2). Pass the StringFormat object create at step #1 to the 
       Graphics.DrawString method to draw vertical string.
   
5. Example 5: "Draw A Ellipse With Gradient Brush".

   1). Create a LinearGradientBrush object;
   2). Pass the LinearGradientBrush object create at step #1 to the 
       Graphics.FillEllipse method to draw a ellipse with gradient color effect.


/////////////////////////////////////////////////////////////////////////////
References:

1. GDI+ Graphics
   http://msdn.microsoft.com/en-us/library/aa984108(VS.71).aspx
   
2. Windows Forms General FAQ.
   http://social.msdn.microsoft.com/Forums/en-US/winforms/thread/77a66f05-804e-4d58-8214-0c32d8f43191
   

/////////////////////////////////////////////////////////////////////////////
