/****************************** Module Header ******************************\
* Module Name:  ImageManipulator.cs
* Project:	    CSGDIPlusManipulateImage
* Copyright (c) Microsoft Corporation.
* 
* This class BitmapEx is used to manipulate an image using GDI+. It supplies 
* methods to resize, scale, rotate and flip the image. The rotate method can 
* support any degree. This class also supplies a method to draw the image on
* an object that inherits System.Windows.Forms.Control.
* 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

namespace CSGDIPlusManipulateImage
{
    public class ImageManipulator : IDisposable
    {

        // Specify whether the object is disposed.
        bool disposed = false;

        /// <summary>
        /// The image to handle.
        /// </summary>
        public Bitmap Image { get; private set; }

        /// <summary>
        /// The real size of the image. If only rotate or flip the image, the real
        /// size will not change. 
        /// This property is used to calculate the necessary size while rotate the
        /// image.
        /// </summary>
        public Size ContentSize { get; private set; }

        /// <summary>
        /// The rotated angle. Its value is between 0 and 360.
        /// </summary>
        public float RotatedAngle { get; private set; }

        /// <summary>
        /// The skewed unit. Its value may be less than 0.
        /// </summary>
        public int SkewedUnit { get; private set; }



        public ImageManipulator(Bitmap img)
        {
            if (img == null)
            {
                throw new ArgumentNullException("Image could not be null");
            }

            // Initialize the properties.
            this.Image = img;
            this.ContentSize = img.Size;
            RotatedAngle = 0;
        }

        /// <summary>
        /// Resize the image with a specified size. 
        /// This method will create a new Bitmap from the original image with the
        /// specified size, then set this.Image to the new bitmap.
        /// </summary>
        public void ResizeImage(Size size)
        {
            Bitmap newImg = null;
            try
            {

                // Create a new bitmap from the original image with the specified size.
                newImg = new Bitmap(Image, size);

                // Dispose the original image.
                this.Image.Dispose();

                // Set this.Image to the new bitmap.
                this.Image = newImg;

                this.ContentSize = this.Image.Size;
            }
            catch
            {

                // If there is any exception, dispose the new bitmap.
                if (newImg != null)
                {
                    newImg.Dispose();
                }

                throw;
            }
        }

        /// <summary>
        /// Scale the image with the specified factors.
        /// The steps are
        /// 1 Calculate the new size.
        /// 2 Initializes a new instance of the Matrix class with the specified 
        ///   elements. This Matrix is used in Transform. 
        /// 3 Draw the original image to the new bitmap, then the Transform will
        ///   take effect.
        /// </summary>
        public void Scale(float xFactor, float yFactor, InterpolationMode mode)
        {
            Bitmap newImg = null;
            Graphics g = null;
            Matrix mtrx = null;

            try
            {
                // Create a new bitmap with the specified size.
                newImg = new Bitmap(Convert.ToInt32(this.Image.Size.Width * xFactor),
                    Convert.ToInt32(this.Image.Size.Height * yFactor));

                // Create a new Graphics from the bitmap.
                g = Graphics.FromImage(newImg);

                // Set the Interpolation Mode. 
                g.InterpolationMode = mode;

                // Initializes a new instance of the Matrix class with the specified 
                // elements. This Matrix is used in Transform. 
                mtrx = new Matrix(xFactor, 0, 0, yFactor, 0, 0);

                // Multiplies the world transformation of this Graphics and specified the Matrix.
                g.MultiplyTransform(mtrx, MatrixOrder.Append);

                // Draw the original image to the new bitmap.
                g.DrawImage(this.Image, 0, 0);

                // Dispose the original image.
                this.Image.Dispose();

                // Set this.Image to the new bitmap.
                this.Image = newImg;

                this.ContentSize = this.Image.Size;
            }
            catch
            {

                // If there is any exception, dispose the new bitmap.
                if (newImg != null)
                {
                    newImg.Dispose();
                }

                throw;
            }
            finally
            {

                // Dispose the Graphics and Matrix.
                if (g != null)
                {
                    g.Dispose();
                }

                if (mtrx != null)
                {
                    mtrx.Dispose();
                }
            }
        }

        /// <summary>
        /// Rotates, flips, or rotates and flips the image.
        /// </summary>
        /// <param name="type">
        /// A System.Drawing.RotateFlipType member that specifies the type of rotation
        //  and flip to apply to the image.
        /// </param>
        public void RotateFlip(RotateFlipType type)
        {
            // Rotates, flips, or rotates and flips the image.
            this.Image.RotateFlip(type);

            // Calculate the rotated angle. 
            switch (type)
            {
                // Rotate180FlipXY just means RotateNoneFlipNone;
                case RotateFlipType.RotateNoneFlipNone:
                    break;

                // Rotate270FlipXY just means Rotate90FlipNone;
                case RotateFlipType.Rotate90FlipNone:
                    this.RotatedAngle += 90;
                    break;

                // RotateNoneFlipXY just means Rotate180FlipNone;
                case RotateFlipType.Rotate180FlipNone:
                    this.RotatedAngle += 180;
                    break;

                // Rotate90FlipXY just means Rotate270FlipNone;
                case RotateFlipType.Rotate270FlipNone:
                    this.RotatedAngle += 270;
                    break;

                // Rotate180FlipY just means RotateNoneFlipX;
                case RotateFlipType.RotateNoneFlipX:
                    this.RotatedAngle = 180 - this.RotatedAngle;
                    break;

                // Rotate270FlipY just means Rotate90FlipX;
                case RotateFlipType.Rotate90FlipX:
                    this.RotatedAngle = 90 - this.RotatedAngle;
                    break;

                // Rotate180FlipX just means RotateNoneFlipY;
                case RotateFlipType.RotateNoneFlipY:
                    this.RotatedAngle = 360 - this.RotatedAngle;
                    break;

                // Rotate270FlipX just means Rotate90FlipY;
                case RotateFlipType.Rotate90FlipY:
                    this.RotatedAngle = 270 - this.RotatedAngle;
                    break;

                default:
                    break;
            }

            // The value of RotatedAngle is between 0 and 360.
            if (RotatedAngle >= 360)
            {
                RotatedAngle -= 360;
            }
            if (RotatedAngle < 0)
            {
                RotatedAngle += 360;
            }
        }

        /// <summary>
        /// Rotate the image to any degree.
        /// The steps are
        /// 1 Calculate the necessary size based on the ContentSize and RotatedAngle.
        /// 2 Move the image center to the point (0,0) of the new bitmap.
        /// 3 Rotate the image to a specified angle.
        /// 4 Move the rotated image center to the center of the new bitmap.
        /// </summary>
        public void RotateImg(float angle)
        {
            RectangleF necessaryRectangle = RectangleF.Empty;

            // Calculate the necessary size while rotate the image based on the 
            // ContentSize and RotatedAngle.
            using (GraphicsPath path = new GraphicsPath())
            {
                path.AddRectangle(
                    new RectangleF(0f, 0f, ContentSize.Width, ContentSize.Height));
                using (Matrix mtrx = new Matrix())
                {
                    float totalAngle = angle + this.RotatedAngle;
                    while (totalAngle >= 360)
                    {
                        totalAngle -= 360;
                    }
                    this.RotatedAngle = totalAngle;
                    mtrx.Rotate(RotatedAngle);

                    //  the necessary Rectangle
                    necessaryRectangle = path.GetBounds(mtrx);
                }
            }

            Bitmap newImg = null;
            Graphics g = null;

            try
            {

                // Create a new bitmap with the specified size.
                newImg = new Bitmap(Convert.ToInt32(necessaryRectangle.Width),
                 Convert.ToInt32(necessaryRectangle.Height));

                // Create a new Graphics from the bitmap.
                g = Graphics.FromImage(newImg);

                // Move the image center to the point (0,0) of the new bitmap.
                g.TranslateTransform(-this.Image.Width / 2, -this.Image.Height / 2);

                // Rotate the image to a specified angle.
                g.RotateTransform(angle, MatrixOrder.Append);

                // Move the rotated image center to the center of the new bitmap.
                g.TranslateTransform(newImg.Width / 2, newImg.Height / 2,
                    MatrixOrder.Append);

                g.InterpolationMode = InterpolationMode.HighQualityBicubic;

                g.DrawImage(this.Image, 0, 0);

                // Dispose the original image.
                this.Image.Dispose();

                // Set this.Image to the new bitmap.
                this.Image = newImg;

            }
            catch
            {

                // If there is any exception, dispose the new bitmap.
                if (newImg != null)
                {
                    newImg.Dispose();
                }

                throw;
            }
            finally
            {

                // Dispose the Graphics
                if (g != null)
                {
                    g.Dispose();
                }

            }
        }

        /// <summary>
        /// You can skew an image by specifying 
        /// destination points for the upper-left, upper-right, and lower-left corners
        /// of the original image. The three destination points determine an affine 
        /// transformation that maps the original rectangular image to a parallelogram.
        /// 
        /// For example, the orignal image rectangle is {[0,0], [100,0], [100,50],[50, 0]},
        /// and unit is -10, the result is {[-10,0], [90,0], [100,50],[50, 0]}. Since the 
        /// X value of these points may be less than 0, so we have to move the image to 
        /// make sure that all the values are not less than 0.
        /// </summary>
        /// <param name="unit">
        /// The unit to skew. 
        /// </param>
        public void Skew(int unit)
        {
            RectangleF necessaryRectangle = RectangleF.Empty;
            int totalUnit = 0;

            // Calculate the necessary size while rotate the image based on the 
            // ContentSize and SkewUnit.
            using (GraphicsPath path = new GraphicsPath())
            {
                Point[] newPoints = null;

                totalUnit = SkewedUnit + unit;

                newPoints = new Point[] 
                {

                    new Point(totalUnit, 0),
                    new Point(totalUnit+this.ContentSize.Width, 0),
                    new Point(this.ContentSize.Width, this.ContentSize.Height),
                    new Point(0, this.ContentSize.Height),
                };
                path.AddLines(newPoints);
                necessaryRectangle = path.GetBounds();
            }


            Bitmap newImg = null;
            Graphics g = null;

            try
            {

                // Create a new bitmap with the specified size.
                newImg = new Bitmap(Convert.ToInt32(necessaryRectangle.Width),
                 Convert.ToInt32(necessaryRectangle.Height));

                // Create a new Graphics from the bitmap.
                g = Graphics.FromImage(newImg);

                // Move the image.
                if (totalUnit <= 0 && SkewedUnit <=0)
                {
                    g.TranslateTransform(-unit, 0, MatrixOrder.Append);
                }

                g.InterpolationMode = InterpolationMode.HighQualityBilinear;

                Point[] destinationPoints = new Point[] 
                {
                    new Point(unit, 0),
                    new Point(unit+this.Image.Width, 0),
                    new Point(0, this.Image.Height),
                };
                
                g.DrawImage(this.Image, destinationPoints);

                // Dispose the original image.
                this.Image.Dispose();

                // Set this.Image to the new bitmap.
                this.Image = newImg;
           
                SkewedUnit = totalUnit;             
            }
            catch
            {

                // If there is any exception, dispose the new bitmap.
                if (newImg != null)
                {
                    newImg.Dispose();
                }

                throw;
            }
            finally
            {

                // Dispose the Graphics
                if (g != null)
                {
                    g.Dispose();
                }

            }

        }



        /// <summary>
        /// Draw the image on a control.
        /// </summary>
        public void DrawControl(Control control, Point adjust, Pen penToDrawBounds)
        {

            //  Creates the System.Drawing.Graphics for the control.
            Graphics graphicsForPanel = control.CreateGraphics();

            // Clears the entire drawing surface and fills it with the specified 
            // background color.
            graphicsForPanel.Clear(System.Drawing.SystemColors.ControlDark);

            // Draw the image on the center of the control.
            Point p = new Point((control.Width - this.Image.Size.Width) / 2,
                (control.Height - this.Image.Size.Height) / 2);

            // Adjust the position.
            graphicsForPanel.TranslateTransform(adjust.X, adjust.Y);

            graphicsForPanel.DrawImage(this.Image, p);

            // Draw the bounds if the penToDrawBounds is not null.
            if (penToDrawBounds != null)
            {

                var unit = GraphicsUnit.Pixel;
                var rec = this.Image.GetBounds(ref unit);

                graphicsForPanel.DrawRectangle(penToDrawBounds, rec.X + p.X, rec.Y + p.Y, rec.Width, rec.Height);
                graphicsForPanel.DrawLine(penToDrawBounds,
                    rec.X + p.X, rec.Y + p.Y,
                    rec.X + p.X + rec.Width, rec.Y + p.Y + rec.Height);

                graphicsForPanel.DrawLine(penToDrawBounds,
                   rec.X + p.X + rec.Width, rec.Y + p.Y,
                   rec.X + p.X, rec.Y + p.Y + rec.Height);
            }
        }

        public void Dispose()
        {
            this.Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            // Protect from being called multiple times.
            if (disposed)
            {
                return;
            }

            if (disposing)
            {
                // Clean up all managed resources.
                if (this.Image != null)
                {
                    this.Image.Dispose();
                }

            }
            disposed = true;
        }
    }
}
