//---------------------------------------------------------------------
//  This file is part of the Windows Workflow Foundation SDK Code Samples.
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
using System.Collections.Generic;
using System.Text;
using System.Collections.ObjectModel;
using System.Workflow.ComponentModel;
using System.Workflow.ComponentModel.Design;
using System.Windows.Forms;
using Microsoft.Samples.Workflow.TrackingProfileDesigner.Properties;

namespace Microsoft.Samples.Workflow.TrackingProfileDesigner
{
    /// <summary>
    /// Provides the glyphs for mapping tracking profile data onto individual activity designers
    /// </summary>
    internal class TrackingGlyphProvider : IDesignerGlyphProvider
    {
        //Keep static references to our resource bitmaps
        static Bitmap redPin = Resources.redpin;
        static Bitmap fadedRedPin = Resources.fadedredpin;        

        TrackingProfileManager profileManager;

        internal TrackingGlyphProvider(TrackingProfileManager context)
        {
            profileManager = context;
        }

        #region IDesignerGlyphProvider Members

        /// <summary>
        /// Gets the glyphs for the associated activity designer
        /// </summary>
        /// <param name="activityDesigner"></param>
        /// <returns></returns>
        public ActivityDesignerGlyphCollection GetGlyphs(ActivityDesigner activityDesigner)
        {
            ActivityDesignerGlyphCollection glyphs = new ActivityDesignerGlyphCollection();
            //The glyph position indicates how far down the glyph is drawn
            int glyphPosition = -1;
            string validationError = string.Empty;            
            if (profileManager.IsActivityValid(activityDesigner.Activity, out validationError))
            {
                //Add an error glyph if the selected activity is not configured correctly
                ++glyphPosition;
                glyphs.Add(new ErrorActivityGlyph(validationError));
            }
            if (profileManager.IsTracked(activityDesigner.Activity))
            {
                //Add the glyph for the trackpoint
                glyphs.Add(new TrackedActivityGlyph(++glyphPosition, redPin));
            }
            if (profileManager.IsMatchedByDerivedTrackPoint(activityDesigner.Activity))
            {
                //Add faded derive match glyph
                glyphs.Add(new TrackedActivityGlyph(++glyphPosition, fadedRedPin));
            }
            string annotation = profileManager.GetAnnotation(activityDesigner.Activity);
            if (annotation != null)
            {
                //If an annotation exists, use the tooltip via the description.
                activityDesigner.Activity.Description = annotation;                
            }
            return glyphs;
        }

        #endregion
    }


    /// <summary>
    /// This glyph shows that the activity is tracked
    /// </summary>
    internal sealed class TrackedActivityGlyph : DesignerGlyph
    {
        Bitmap image = null;
        int position = 0;

        internal TrackedActivityGlyph(int glyphPosition, Bitmap icon)
        {
            position = glyphPosition;
            image = icon;
        }

        /// <summary>
        /// Sets the bounds for the tracked activity glyph; the position is used to determine how far down to draw the glyph
        /// </summary>
        /// <param name="designer"></param>
        /// <param name="activated"></param>
        /// <returns></returns>
        public override Rectangle GetBounds(ActivityDesigner designer, bool activated)
        {
            Rectangle imageBounds = new Rectangle();
            if (image != null)
            {
                Size glyphSize = new Size((int)(image.Width * .75), (int)(image.Height * .75));
                imageBounds.Location = new Point(
                        designer.Bounds.Right - glyphSize.Width / 4, 
                        designer.Bounds.Top - glyphSize.Height / 2 + (position * ((int)(.8 * glyphSize.Height))));
                imageBounds.Size = glyphSize;
            }
            return imageBounds;
        }

        protected override void OnPaint(Graphics graphics, bool activated, AmbientTheme ambientTheme, ActivityDesigner designer)
        {
            image.MakeTransparent(Color.FromArgb(255, 255, 255));
            if (image != null)
                graphics.DrawImage(image, GetBounds(designer, activated), new Rectangle(Point.Empty, image.Size), GraphicsUnit.Pixel);
        }
    }

    /// <summary>
    /// This glyph shows that the activity's track point is not correctly configured
    /// </summary>
    internal sealed class ErrorActivityGlyph : DesignerGlyph
    {
        static Bitmap image = Resources.error;
        string errorMessage;        

        internal ErrorActivityGlyph(string errorMessage)
        {
            this.errorMessage = errorMessage;
        }

        public override bool CanBeActivated
        {
            get
            {
                return true;
            }
        }

        /// <summary>
        /// Display an error message when this glyph is clicked
        /// </summary>
        /// <param name="designer"></param>
        protected override void OnActivate(ActivityDesigner designer)
        {
            MessageBox.Show(errorMessage);
        }

        public override Rectangle GetBounds(ActivityDesigner designer, bool activated)
        {
            Rectangle imageBounds = new Rectangle();
            if (image != null)
            {
                imageBounds.Size = image.Size;
                imageBounds.Location = new Point(designer.Bounds.Right - imageBounds.Size.Width / 4, designer.Bounds.Top - imageBounds.Size.Height / 2 );
                
            }
            return imageBounds;                
        }

        protected override void OnPaint(Graphics graphics, bool activated, AmbientTheme ambientTheme, ActivityDesigner designer)
        {
            image.MakeTransparent(Color.FromArgb(255, 255, 255));
            if (image != null)
            {
                graphics.DrawImage(image, GetBounds(designer, activated), new Rectangle(Point.Empty, image.Size), GraphicsUnit.Pixel);
            }            
        }
    }
}
