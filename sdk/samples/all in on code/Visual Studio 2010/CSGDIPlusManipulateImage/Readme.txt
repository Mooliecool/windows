================================================================================
	   Windows APPLICATION: CSGDIPlusManipulateImage Overview                        
===============================================================================
/////////////////////////////////////////////////////////////////////////////
Summary:
The sample demonstrates how to to resize, scale, rotate, flip and skew the image using
GDI+.

////////////////////////////////////////////////////////////////////////////////
Demo:

Step1. Build this project in VS2010. 

Step2. Run CSGDIPlusManipulateImage.exe. 

Step3. Press the button "Micrify", you will see that the image size changes to the half of the 
       original value.

Step4. Press the button "Reset". 

Step5. Press the button "Amplify", you will see that the image size changes to the double of the 
       original value.

Step6. Press the button "Reset". 

Step7. Press the button "RotateLeft", you will see that the image was rotated to the left by  
       90 degree. 

Step8. Press the button "Reset". 

Step9. Press the button "RotateRight", you will see that the image was rotated to the right by  
       90 degree. 

Step10. Press the button "Reset". 

Step11. Press the button "FlipHorizontal", you will see that the image was flipped by 
        horizontal.

Step12. Press the button "Reset". 

Step13. Press the button "FlipVertical", you will see that the image was flipped by 
        vertical.

Step14. Press the button "Reset". 

Step15. Type a value between 0 and 360 in the Rotate Angle textbox, and then Press the 
        button "OK", you will see that the image was rotated by the specified degree.

		You can rotate the image continually without Reset.

Step16. Press the button "Reset". 

Step17. Press the button "SkewLeft", you will see that the top of the image was skewed
        to the left. 

		Press the button "SkewRight", you will see that the top of the image was skewed
        to the right. 

		You can skew the image continually without Reset.

		NOTE: Since the "Skew" is not a linear transform, you have to reset the image 
		      after other operations in this application.
		      

Step16. Press the button "Reset". 

Step17. Press the buttons "MoveUp", "MoveLeft", "MoveDown" and "Moveright", you will
        see that the image was moved up, left, down and right.

/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Design a class ImageManipulator which is used to manipulate an image using GDI+. 
   It supplies methods to resize, scale, rotate and flip the image.

   1.1 Resize
       Use the following constructor of the Bitmap class to get a resized image from
	   the original image.
  
       public Bitmap(Image original, Size newSize);
  
   1.2 Scale
       Initializes a new instance of the Matrix class with the specified elements. 
       This Matrix is used in Transform. When draw the original image to the new bitmap,
       the Transform will take effect.

       The Matrix is like 
       | xFactor      0 |
       | 0      yFactor |
       | 0            0 |
       which means that the image will be scaled by xFactor in Width and yFactor in 
	   Height.

   1.3 Rotate and Flip
      
	   1.3.1 The Image clsss supplies a method RotateFlip(RotateFlipType rotateFlipType) 
	       to rotate and flip the image. But this method could only rotate 90, 180 or
		   270 degree.

       1.3.2 Use Transform. The steps are
           1.3.2.1 Calcualte the necessary size based on the ContentSize and RotatedAngle.
           1.3.2.2 Move the image center to the point (0,0) of the new bitmap.
           1.3.2.3 Rotate the image to a specified angle.
           1.3.2.4 Move the rotated image center to the center of the new bitmap.

   1.4 Skew
       You can skew an image by specifying destination points for the upper-left, 
	   upper-right, and lower-left corners of the original image. The three destination
	   points determine an affine transformation that maps the original rectangular 
	   image to a parallelogram.

2. Design the UI that contains many buttons to manipulate the image.

/////////////////////////////////////////////////////////////////////////////
References:

Image Class
http://msdn.microsoft.com/en-us/library/system.drawing.image.aspx

Bitmap Class
http://msdn.microsoft.com/en-us/library/system.drawing.bitmap.aspx

Graphics Class
http://msdn.microsoft.com/en-us/library/system.drawing.graphics.aspx

InterpolationMode Enumeration
http://msdn.microsoft.com/en-us/library/system.drawing.drawing2d.interpolationmode.aspx

Matrix Class
http://msdn.microsoft.com/en-us/library/system.drawing.drawing2d.matrix.aspx

/////////////////////////////////////////////////////////////////////////////
