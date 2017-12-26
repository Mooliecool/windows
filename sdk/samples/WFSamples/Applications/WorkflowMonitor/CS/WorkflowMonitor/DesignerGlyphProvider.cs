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
using System.Workflow.ComponentModel.Design;
using Microsoft.Samples.Workflow.WorkflowMonitor.Properties;

namespace Microsoft.Samples.Workflow.WorkflowMonitor
{
    //Custom glyphprovider used to draw the monitor glyphs on the designer surface
    internal class WorkflowMonitorDesignerGlyphProvider : IDesignerGlyphProvider
    {
        private Dictionary<string, ActivityStatusInfo> activityStatusList;

        internal WorkflowMonitorDesignerGlyphProvider(Dictionary<string, ActivityStatusInfo> activityStatusList)
        {
            this.activityStatusList = activityStatusList;
        }

        ActivityDesignerGlyphCollection IDesignerGlyphProvider.GetGlyphs(ActivityDesigner activityDesigner)
        {
            ActivityDesignerGlyphCollection glyphList = new ActivityDesignerGlyphCollection();

            //Walk all of the activities and use the 'CompletedGlyph' for all activities that are not 'closed'
            foreach (ActivityStatusInfo activityStatus in activityStatusList.Values)
            {
                if (activityStatus.Name == activityDesigner.Activity.QualifiedName)
                {
                    if (activityStatus.Status == "Closed")
                        glyphList.Add(new CompletedGlyph());
                    else
                        glyphList.Add(new ExecutingGlyph());
                }
            }

            return glyphList;
        }
    }

    //Define a glyph to show an activity is executing, i.e. not 'closed'
    internal sealed class ExecutingGlyph : DesignerGlyph
    {
        internal ExecutingGlyph()
        {
        }

        public override Rectangle GetBounds(ActivityDesigner designer, bool activated)
        {
            Rectangle imageBounds = Rectangle.Empty;
            Image image = Resources.Executing;
            if (image != null)
            {
                Size glyphSize = WorkflowTheme.CurrentTheme.AmbientTheme.GlyphSize;
                imageBounds.Location = new Point(designer.Bounds.Right - glyphSize.Width / 2, designer.Bounds.Top - glyphSize.Height / 2);
                imageBounds.Size = glyphSize;
            }
            return imageBounds;
        }

        protected override void OnPaint(Graphics graphics, bool activated, AmbientTheme ambientTheme, ActivityDesigner designer)
        {
            Bitmap bitmap = Resources.Executing;
            bitmap.MakeTransparent(Color.FromArgb(0, 255, 255));
            if (bitmap != null)
                graphics.DrawImage(bitmap, GetBounds(designer, activated), new Rectangle(Point.Empty, bitmap.Size), GraphicsUnit.Pixel);
        }
    }

    //Define a glyph to show an activity is 'closed'
    internal sealed class CompletedGlyph : DesignerGlyph
    {
        internal CompletedGlyph()
        {
        }

        public override Rectangle GetBounds(ActivityDesigner designer, bool activated)
        {
            Rectangle imageBounds = Rectangle.Empty;
            Image image = Resources.complete;
            if (image != null)
            {
                Size glyphSize = WorkflowTheme.CurrentTheme.AmbientTheme.GlyphSize;
                imageBounds.Location = new Point(designer.Bounds.Right - glyphSize.Width / 2, designer.Bounds.Top - glyphSize.Height / 2);
                imageBounds.Size = glyphSize;
            }
            return imageBounds;
        }

        protected override void OnPaint(Graphics graphics, bool activated, AmbientTheme ambientTheme, ActivityDesigner designer)
        {
            Bitmap bitmap = Resources.complete;
            bitmap.MakeTransparent(Color.FromArgb(0, 255, 255));
            if (bitmap != null)
                graphics.DrawImage(bitmap, GetBounds(designer, activated), new Rectangle(Point.Empty, bitmap.Size), GraphicsUnit.Pixel);
        }
    }
}
