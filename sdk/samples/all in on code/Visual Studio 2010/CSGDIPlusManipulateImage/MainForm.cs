/****************************** Module Header ******************************\
* Module Name:  MainForm.cs
* Project:	    CSGDIPlusManipulateImage
* Copyright (c) Microsoft Corporation.
* 
* This is the main form of this application. It is used to initialize the UI and 
* handle the events.
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
using System.Windows.Forms;
using System.Drawing.Drawing2D;

namespace CSGDIPlusManipulateImage
{
    public partial class MainForm : Form
    {
        Pen pen = null;

        ImageManipulator imgManipulator = null;

        Point adjustment = Point.Empty;

        public MainForm()
        {
            InitializeComponent();

            // Load a bitmap from a local file.
            Bitmap img = new Bitmap("OneCodeIcon.png");

            // Initialize the bmpEx.
            imgManipulator = new ImageManipulator(img);

            // Add all the InterpolationMode to the combobox.
            for (int i = 0; i <= 7; i++)
            {
                cmbInterpolationMode.Items.Add((InterpolationMode)(i));
            }

            cmbInterpolationMode.SelectedIndex = 0;

        }

        /// <summary>
        /// Handle the click event of the buttons btnRotateLeft, btnRotateRight,
        /// btnFlipVertical and btnFlipHorizontal.
        /// </summary>
        private void btnRotateFlip_Click(object sender, EventArgs e)
        {
            Button rotateFlipButton = sender as Button;

            if (rotateFlipButton == null)
            {
                return;
            }

            RotateFlipType rotateFlipType = RotateFlipType.RotateNoneFlipNone;

            switch (rotateFlipButton.Name)
            {
                case "btnRotateLeft":
                    rotateFlipType = RotateFlipType.Rotate270FlipNone;
                    break;
                case "btnRotateRight":
                    rotateFlipType = RotateFlipType.Rotate90FlipNone;
                    break;
                case "btnFlipVertical":
                    rotateFlipType = RotateFlipType.RotateNoneFlipY;
                    break;
                case "btnFlipHorizontal":
                    rotateFlipType = RotateFlipType.RotateNoneFlipX;
                    break;
            }

            // Rotate or flip the image.
            imgManipulator.RotateFlip(rotateFlipType);

            // Redraw the pnlImage.
            imgManipulator.DrawControl(this.pnlImage, adjustment, pen);
        }

        /// <summary>
        /// Handle the click event of the button btnRotateAngle.
        /// </summary>
        private void btnRotateAngle_Click(object sender, EventArgs e)
        {
            float angle = 0;

            // Verify the input value.
            float.TryParse(tbRotateAngle.Text, out angle);

            if (angle > 0 && angle < 360)
            {
                // Rotate or flip the image.
                imgManipulator.RotateImg(angle);

                // Redraw the pnlImage.
                imgManipulator.DrawControl(this.pnlImage, adjustment, pen);

            }
        }

        /// <summary>
        /// Handle the click event of the buttons btnMoveUp, btnMoveDown,
        /// btnMoveLeft and btnMoveRight.
        /// </summary>
        private void btnMove_Click(object sender, EventArgs e)
        {
            Button moveButton = sender as Button;
            if (moveButton == null)
            {
                return;
            }

            int x = 0;
            int y = 0;

            switch (moveButton.Name)
            {
                case "btnMoveUp":
                    y = -10;
                    break;
                case "btnMoveDown":
                    y = 10;
                    break;
                case "btnMoveLeft":
                    x = -10;
                    break;
                case "btnMoveRight":
                    x = 10;
                    break;
            }
            adjustment = new Point(adjustment.X + x, adjustment.Y + y);

            // Redraw the pnlImage.
            imgManipulator.DrawControl(this.pnlImage, adjustment, pen);

        }

        /// <summary>
        /// Draw the image on the pnlImage when it is painted.
        /// </summary>
        private void pnlImage_Paint(object sender, PaintEventArgs e)
        {

            // Draw the pnlImage for the first time..
            imgManipulator.DrawControl(this.pnlImage, adjustment, pen);

        }

        /// <summary>
        /// Handle the click event of the buttons btnAmplify and btnMicrify.
        /// </summary>
        private void btnAmplify_Click(object sender, EventArgs e)
        {
            Button btnScale = sender as Button;
            if (btnScale.Name == "btnAmplify")
            {
                imgManipulator.Scale(2, 2, (InterpolationMode)cmbInterpolationMode.SelectedItem);
            }
            else
            {
                imgManipulator.Scale(0.5f, 0.5f, (InterpolationMode)cmbInterpolationMode.SelectedItem);
            }
            
            // Redraw the pnlImage.
            imgManipulator.DrawControl(this.pnlImage, adjustment, pen);
        }


        /// <summary>
        /// andle the click event of the buttons btnSkewLeft and btnSkewRight.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnSkew_Click(object sender, EventArgs e)
        {
            Button btnSkew = sender as Button;

            switch(btnSkew.Name)
            {
                case "btnSkewLeft":
                    imgManipulator.Skew(-10);
                    break;
                case "btnSkewRight":
                    imgManipulator.Skew( 10);
                    break;
            }

            // Redraw the pnlImage.
            imgManipulator.DrawControl(this.pnlImage, adjustment, pen);
        }


        /// <summary>
        /// Reset the bmpEx.
        /// </summary>
        private void btnReset_Click(object sender, EventArgs e)
        {

            // Dispose the bmpEx.
            imgManipulator.Dispose();

            // Load a bitmap from a local file.
            Bitmap img = new Bitmap("OneCodeIcon.png");

            // Initialize the bmpEx.
            imgManipulator = new ImageManipulator(img);

            // Redraw the pnlImage.
            imgManipulator.DrawControl(this.pnlImage, adjustment, pen);

        }

        /// <summary>
        /// Handle the CheckedChanged event of the checkbox chkDrawBounds.
        /// </summary>
        private void chkDrawBounds_CheckedChanged(object sender, EventArgs e)
        {

            // If the pen is not null, draw the bounds of the image.
            if (chkDrawBounds.Checked)
            {
                pen = Pens.Blue;
            }
            else
            {
                pen = null;
            }

            // Redraw the pnlImage.
            imgManipulator.DrawControl(this.pnlImage, adjustment, pen);
        }

       
    }
}
