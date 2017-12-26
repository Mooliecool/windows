//---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
//
//  Copyright (C) Microsoft Corporation.  All rights reserved.
//
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
//
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------

using System;
using System.Drawing;
using System.Windows.Forms;
using Microsoft.WindowsCE.Forms;
using System.Globalization;

namespace Microsoft.Samples
{
    public class RotatedText : Form
    {
        const int CONTROL_HEIGHT = 22;

        Button drawButton;
        NumericUpDown spokesNumericUpDown;
        FontwhirlControl whirlControl;

        public RotatedText()
        {
            // Suspend control layout for better performance and to suspend
            // automatic scaling until we've set the AutoScaleDimensions and
            // AutoScaleMode
            this.SuspendLayout();

            // note: controls use the Dock property to automatically
            // reposition the controls when a device's screen orientation
            // changes
            this.whirlControl = new FontwhirlControl();
            this.whirlControl.BackColor = Color.LawnGreen;
            this.whirlControl.Dock = DockStyle.Fill;
            this.whirlControl.Parent = this;
            this.whirlControl.Text = "abc ABC 123";

            Panel controlsPanel = new Panel();
            controlsPanel.Height = CONTROL_HEIGHT;
            controlsPanel.Dock = DockStyle.Bottom;
            controlsPanel.Parent = this;

            this.drawButton = new Button();
            this.drawButton.Text = "Draw";
            this.drawButton.Parent = controlsPanel;

            this.drawButton.Width = 60;
            this.drawButton.Dock = DockStyle.Right;
            this.drawButton.Click += new EventHandler(this.DrawButton_Click);

            this.spokesNumericUpDown = new NumericUpDown();
            this.spokesNumericUpDown.Parent  = controlsPanel;
            this.spokesNumericUpDown.Width   = 50;
            this.spokesNumericUpDown.Maximum = 50;
            this.spokesNumericUpDown.Minimum = 1;
            this.spokesNumericUpDown.Value   = 10;
            this.spokesNumericUpDown.Dock    = DockStyle.Left;

            Label label = new Label();
            label.Parent = controlsPanel;
            label.Width = 115;
            label.Text = "Number of Spokes:";
            label.Dock = DockStyle.Left;

            // put (ok) button on form to close it when done with this sample
            this.MinimizeBox = false;

            // dpi settings for the screen the form was initially designed on
            this.AutoScaleDimensions = new SizeF(96f, 96f);

            // automatically scale controls based on the DPI of the screen
            this.AutoScaleMode = AutoScaleMode.Dpi;

            // show current dpi of screen
            this.Text = string.Format(CultureInfo.InvariantCulture, 
                      "RotatedText({0}dpi)",
                      new object[] { this.CurrentAutoScaleDimensions.Height });

            // Now that we've set the AutoScaleMode, call ResumeLayout() to
            // do the actual scaling
            this.ResumeLayout();

            // draw default image
            DrawButton_Click(null, EventArgs.Empty);
        }

        /// <summary>
        ///     When someone clicks the Draw button
        ///        a) set the number of spokes from the numericUpDown control
        ///        b) instruct whirl control to render the rotated text
        /// </summary>
        void DrawButton_Click(object o, EventArgs e)
        {
            this.whirlControl.Spokes = (int)this.spokesNumericUpDown.Value;
            this.whirlControl.DrawText();
        }

        static void Main()
        {
            Application.Run(new RotatedText());
        }
    }


    /// <summary>
    ///     Control to draw rotated text.  By rendering the rotated text in a
    ///     separate control we can take advantage of docking and anchoring
    ///     to resize the control and make the application respond to screen
    ///     orientation changes.
    /// </summary>
    public class FontwhirlControl : Control
    {
        const int POINTSPERINCH = 96;   // initial dpi value used when designed
        int spokesValue = 10;
        Bitmap offScreenBitmap = null;

        public FontwhirlControl()
        {
        }

        /// <summary>
        ///   property specifying number of text spokes to draw
        /// </summary>
        public int Spokes
        {
            get
            {
                return this.spokesValue;
            }
            set
            {
                this.spokesValue = value;
            }
        }

        /// <summary>
        ///     when the Resize() event fires, make a new off screen bitmap
        ///     to draw into.
        /// </summary>
        void ResizeDrawingBuffer()
        {
            Bitmap newBitmap = new Bitmap(this.ClientSize.Width,
                                          this.ClientSize.Height);
            using (Graphics g = Graphics.FromImage(newBitmap))
                g.Clear(this.BackColor);

            // copy the old offScreenBitmap into the new buffer
            if(this.offScreenBitmap != null)
            {
                using (Graphics g = Graphics.FromImage(newBitmap))
                    g.DrawImage(this.offScreenBitmap, 0, 0);

                // Graphics objects are not garbage collected and need to be
                // explicitly disposed
                this.offScreenBitmap.Dispose();
            }

            this.offScreenBitmap = newBitmap;
        }

        /// <summary>
        ///     Create the rotated font using a LOGFONT structure.  Important
        ///     notes:
        ///         a) rotation angle is in 1/10's of a degree
        ///         b) orientation and escapement values are the same on
        ///            mobile devices.
        /// </summary>
        Font CreateRotatedFont(int angle, Graphics g)
        {
            LogFont lf = new LogFont();

            // scale a 12 point font for current screen DPI
            lf.Height = (int)(-16f * g.DpiY / POINTSPERINCH);
            lf.Width  = 0;

            // rotation angle in tenths of degrees
            lf.Escapement = angle * 10;

            // Orientation == Escapement for mobile device OS
            lf.Orientation = lf.Escapement;
            lf.Weight = 0;
            lf.Italic = 0;
            lf.Underline = 0;
            lf.StrikeOut = 0;
            lf.CharSet = LogFontCharSet.Default;
            lf.OutPrecision = LogFontPrecision.Default;
            lf.ClipPrecision = LogFontClipPrecision.Default;
            lf.Quality = LogFontQuality.ClearType;
            lf.PitchAndFamily = LogFontPitchAndFamily.Default;
            lf.FaceName = "Tahoma";

            return Font.FromLogFont(lf);
        }


        /// <summary>
        ///     Draw rotated text spokes into an off screen bitmap which will
        ///     be drawn to the screen via the OnPaint() event.
        /// </summary>
        public void DrawText()
        {
            using (Graphics g = Graphics.FromImage(this.offScreenBitmap))
            {
                g.Clear(this.BackColor);

                float fontAngle;
                Font rotatedFont;
                for(int spoke = 0; spoke < this.spokesValue; spoke++)
                {
                    // calculate the rotation angle and create a rotated font
                    fontAngle = (float)spoke * (360f / (float)this.spokesValue);
                    rotatedFont = CreateRotatedFont((int)fontAngle, g);

                    // Draw the rotated text starting from the center of the
                    // off screen bitmap (essentially, the client area of the
                    // custom control).
                    g.DrawString(this.Text,
                                 rotatedFont,
                                 new SolidBrush(this.ForeColor),
                                 this.offScreenBitmap.Width / 2,
                                 this.offScreenBitmap.Height / 2,
                                 new StringFormat(StringFormatFlags.NoWrap |
                                                  StringFormatFlags.NoClip));

                    // Graphics objects are not garbage collected and need to be
                    // explicitly disposed
                    rotatedFont.Dispose();
                }
            }

            // cause the control to re-paint itself
            this.Invalidate();
        }

        // override this to do nothing.  Allowing the background to paint would
        // cause the control to flicker when it repaints.  And, it's not
        // necessary since we draw the whole off screen bitmap during a paint
        // event.
        protected override void OnPaintBackground(PaintEventArgs e)
        {
        }

        /// <summary>
        ///     Draw the off screen bitmap that contains the rotated text spokes
        /// </summary>
        protected override void OnPaint(PaintEventArgs e)
        {
            if(this.offScreenBitmap == null)
                return;

            e.Graphics.DrawImage(this.offScreenBitmap, 0, 0);
        }

        /// <summary>
        ///     Resize event fires when screen orientation changes.  When this
        ///     happens, resize the off screen bitmap and redraw the rotated
        ///     text spokes.
        /// </summary>
        protected override void OnResize(EventArgs e)
        {
            if(this.ClientSize.Width <= 0 || this.ClientSize.Height <= 0)
                return;

            ResizeDrawingBuffer();
            DrawText();
        }
    }
}
