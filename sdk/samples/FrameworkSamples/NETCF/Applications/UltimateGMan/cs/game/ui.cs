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
using System.Collections;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Globalization;
using GraphicsLibrary;

namespace GameApp
{
    /// <summary>
    /// Implements the user interface for the gameplay portion of the game.
    /// </summary>
    public class UserInterface : IDisposable
    {
        /// <summary>
        /// A UI panel is a UI image on the screen.
        /// </summary>
        private class UIPanel : IDisposable
        {
            /// <summary>
            /// Bitmap associated with this panel
            /// </summary>
            private IBitmap bmp = null;

            /// <summary>
            /// Screen x coordinate at which to display this panel
            /// </summary>
            private int x = 0;

            /// <summary>
            /// Screen y coordinate at which to display this panel
            /// </summary>
            private int y = 0;

            /// <summary>
            /// Rectangle uses to define the panel source region for
            /// drawing.
            /// </summary>
            private Rectangle src = new Rectangle();

            /// <summary>
            /// Initialize a panel with the data contained in the DataRow.
            /// </summary>
            /// <param name="dr">DataRow that defines the panel</param>
            /// <param name="graphics">Graphics instance</param>
            public UIPanel(DataRow dr, IGraphics graphics)
            {
                Debug.Assert(dr != null,
                    "UIPanel.UIPanel: Invalid DataTable");

                x = int.Parse((string)dr["X"], CultureInfo.InvariantCulture);
                y = int.Parse((string)dr["Y"], CultureInfo.InvariantCulture);

                string fullName = GameMain.GetFullPath(
                    @"Data\UI\" + (string)dr["FileName"]);

                bmp = graphics.CreateBitmap(fullName, false);
                Debug.Assert(bmp != null,
                    "UIPanel.UIPanel: Failed to initialize UI panel bitmap");
            }

            /// <summary>
            /// Update the UI panel.  For the demo, this does nothing.
            /// </summary>
            public void Update()
            {
            }

            /// <summary>
            /// Draw the panel.
            /// </summary>
            /// <param name="graphics">Graphics instance</param>
            public void Draw(IGraphics graphics)
            {
                src.X = 0;
                src.Y = 0;
                src.Width = bmp.Width;
                src.Height = bmp.Height;

                graphics.DrawBitmap(x, y, src, bmp);
            }

            /// <summary>
            /// Free any resources allocated for the panel.
            /// </summary>
            public void Dispose()
            {
                if (bmp != null)
                    bmp.Dispose();
            }
        }

        /// <summary>
        /// List of panels that make the UI
        /// </summary>
        private ArrayList panels = new ArrayList();

        /// <summary>
        /// Gets the Font used for UI in the game.
        /// </summary>
        public IFont Font { get { return fontValue; } }
        private IFont fontValue = null;

        /// <summary>
        /// X screen coordinate of misfire text
        /// </summary>
        int misfireX;

        /// <summary>
        /// Y screen coordinate of misfire text
        /// </summary>
        int misfireY;

        /// <summary>
        /// Misfire text which is displayed when the player misfires.
        /// </summary>
        string misfireText = "";

        /// <summary>
        /// Length of time left for misfire to be displayed.
        /// </summary>
        private float misfireTime = 0.0F;

        /// <summary>
        /// Fill area of the player's shot bar.
        /// </summary>
        private Rectangle shotBarGreen = new Rectangle(0, 0, 0, 0);

        /// <summary>
        /// Outline of the player's shot bar.
        /// </summary>
        private Rectangle shotBarOutline = new Rectangle(0, 0, 0, 0);

        /// <summary>
        /// Initialize UI from the specified DataSet.
        /// </summary>
        /// <param name="ds">DataSet containing UI definition</param>
        /// <param name="graphics">Graphics instance</param>
        /// <param name="lev">Current level</param>
        public UserInterface(DataSet ds, IGraphics graphics, Level lev)
        {
            DataTable dt = ds.Tables["General"];
            Debug.Assert(dt != null && dt.Rows != null,
                "UI.UI: Invalid General DataTable");


            string fontName = (string)dt.Rows[0]["Font"];
            fontValue = graphics.CreateFont(GameMain.GetFullPath(fontName));
            Debug.Assert(fontValue != null,
                "UI.UI: Failed to initialize UI font");

            shotBarOutline.X = int.Parse((string)dt.Rows[0]["ShotBarX"], 
                CultureInfo.InvariantCulture);
            shotBarOutline.Y = int.Parse((string)dt.Rows[0]["ShotBarY"], 
                CultureInfo.InvariantCulture);
            shotBarOutline.Width = int.Parse(
                (string)dt.Rows[0]["ShotBarWidth"], 
                CultureInfo.InvariantCulture);
            shotBarOutline.Height = int.Parse(
                (string)dt.Rows[0]["ShotBarHeight"], 
                CultureInfo.InvariantCulture);

            misfireX = int.Parse((string)dt.Rows[0]["MisFireX"], 
                CultureInfo.InvariantCulture);
            misfireY = int.Parse((string)dt.Rows[0]["MisFireY"], 
                CultureInfo.InvariantCulture);

            dt = ds.Tables["Panel"];
            Debug.Assert(dt != null && dt.Rows != null,
                "UI.UI: Invalid Panel DataTable");

            foreach (DataRow dr in dt.Rows)
            {
                UIPanel p = new UIPanel(dr, graphics);
                Debug.Assert(p != null,
                    "UI.UI: Failed to initialize UIPanel");

                panels.Add(p);
            }

            shotBarGreen.X = shotBarOutline.X + 1;
            shotBarGreen.Y = shotBarOutline.Y + 1;
            shotBarGreen.Height = shotBarOutline.Height - 2;
        }

        /// <summary>
        /// Upate the game UI.
        /// </summary>
        /// <param name="lev">Current level</param>
        public void Update(Level lev)
        {
            foreach (UIPanel p in panels)
            {
                p.Update();
            }

            if (lev.Player.Misfire)
            {
                misfireTime = 0.5F;
                misfireText = "Misfire";
            }
            else if (misfireTime <= 0.0F)
            {
                misfireText = "";
            }

            misfireTime -= GameMain.SecondsPerFrame;

            shotBarGreen.Width =
                (int)((float)(shotBarOutline.Width - 2) *
                lev.Player.CurrentShotBarPercent);
        }

        /// <summary>
        /// Draw the UI to the back buffer.
        /// </summary>
        /// <param name="graphics">Graphics instance</param>
        public void Draw(IGraphics graphics)
        {
            foreach (UIPanel p in panels)
            {
                p.Draw(graphics);
            }

            graphics.DrawFilledRect(shotBarGreen, Color.Green);

            if (misfireText.Length > 0)
                graphics.DrawText(misfireX, misfireY, misfireText,
                    Color.Red, fontValue, FontDrawOptions.DrawTextCenter);
        }

        /// <summary>
        /// Free any resources allocated for UI.
        /// </summary>
        public void Dispose()
        {
            foreach (UIPanel p in panels)
            {
                p.Dispose();
            }
        }
    }
}
