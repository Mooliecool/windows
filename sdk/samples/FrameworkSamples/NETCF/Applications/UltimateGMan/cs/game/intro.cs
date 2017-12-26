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
using System.Data;
using System.Collections;
using System.Diagnostics;
using System.Drawing;
using System.Globalization;
using GraphicsLibrary;

namespace GameApp
{
    /// <summary>
    /// Resposible for loading and displaying a level intro.
    /// For this demo, an intro is a sequence of images.
    /// </summary>
    public class Intro : IDisposable
    {
        /// <summary>
        /// A page is a single image of an intro.
        /// </summary>
        private class Page : IDisposable
        {
            /// <summary>
            /// Specifies if a page is done being displayed
            /// </summary>
            public bool Done { get { return delay <= 0.0F; } }

            /// <summary>
            /// Screen x location of page.
            /// </summary>
            private int x = 0;

            /// <summary>
            /// Screen y location of page.
            /// </summary>
            private int y = 0;

            /// <summary>
            /// Length of time, in seconds, to keep the page displayed.
            /// </summary>
            private float delay = 0.0F;

            /// <summary>
            /// Bitmap representing the page.
            /// </summary>
            private IBitmap bmp = null;

            /// <summary>
            /// File name of page's bitmap.  This is cached because the page
            /// is not loaded until it is needed.
            /// </summary>
            private string fileName = null;

            /// <summary>
            /// Create an instance of a Page based on the definition in the
            /// given DataRow.
            /// </summary>
            /// <param name="dr">DataRow containing Page definition</param>
            /// <param name="graphics">Graphics instance</param>
            public Page(DataRow dr, IGraphics graphics)
            {
                Debug.Assert(dr != null,
                    "Page.Page: Invalid DataRow");

                x = int.Parse((string)dr["X"], CultureInfo.InvariantCulture);
                y = int.Parse((string)dr["Y"], CultureInfo.InvariantCulture);
                delay = float.Parse((string)dr["Delay"], 
                    CultureInfo.InvariantCulture);
                fileName = @"Data\Intro\" + (string)dr["FileName"];
            }

            /// <summary>
            /// Source region for drawing the Page.
            /// Cached to reduce allocations.
            /// </summary>
            private Rectangle src = new Rectangle();

            /// <summary>
            /// Load the image associated with this page.
            /// </summary>
            /// <param name="graphics">Graphics instance</param>
            public void LoadImage(IGraphics graphics)
            {
                bmp = graphics.CreateBitmap(GameMain.GetFullPath(fileName),
                    false);
                Debug.Assert(bmp != null,
                    "Page.LoadImage: Failed to load bitmap");
            }

            /// <summary>
            /// Update the page.
            /// </summary>
            public void Update()
            {
                delay -= GameMain.SecondsPerFrame;
            }

            /// <summary>
            ///  Draw the Page.
            /// </summary>
            /// <param name="graphics">Graphics instance</param>
            public void Draw(IGraphics graphics)
            {
                Debug.Assert(bmp != null,
                    "Page.Draw: Attempt to draw unloaded page");

                src.X = 0;
                src.Y = 0;
                src.Width = bmp.Width;
                src.Height = bmp.Height;

                graphics.DrawBitmap(x, y, src, bmp);
            }

            /// <summary>
            /// Release the resources allocated for the image.
            /// </summary>
            public void Unload()
            {
                if (bmp != null)
                {
                    bmp.Dispose();
                    bmp = null;
                }
            }

            /// <summary>
            /// Clean up the Page's resources.
            /// </summary>
            public void Dispose()
            {
                Unload();
            }
        }

        /// <summary>
        /// List of Pages in the Intro
        /// </summary>
        private ArrayList pages = new ArrayList();

        /// <summary>
        /// Index of page currently being displayed
        /// </summary>
        private int curPage = 0;

        /// <summary>
        /// Specifies if a page load is requested
        /// </summary>
        private bool loadRequested = false;

        /// <summary>
        /// Specifies if the intro is done being displayed.
        /// </summary>
        public bool Done
        {
            get
            {
                return loadRequested && curPage >=
                    pages.Count - 1;
            }
        }

        /// <summary>
        /// Create an instance of Intro given the DataSet that defines it.
        /// </summary>
        /// <param name="ds">DataSet defining the Intro</param>
        /// <param name="graphics">Grahpics instance</param>
        public Intro(DataSet ds, IGraphics graphics)
        {
            // Access the Page information
            DataTable dt = ds.Tables["Page"];
            Debug.Assert(dt != null && dt.Rows != null,
                "Intro.Intro: Invalid DataTable");

            // For each page in the DataSet, create a new page and add
            // it to the list
            foreach (DataRow dr in dt.Rows)
            {
                Page p = new Page(dr, graphics);
                Debug.Assert(p != null,
                    "Intro.Intro: Failed to initialize page");
                pages.Add(p);
            }

            // Load the image of the first page
            ((Page)pages[0]).LoadImage(graphics);
        }

        /// <summary>
        /// Update the intro.
        /// </summary>
        /// <param name="graphics">Graphics instance</param>
        public void Update(IGraphics graphics)
        {
            // If a load was requested on the previous update and there
            // is another page in the list then unload the current page
            // and load the next
            if (loadRequested && curPage < pages.Count - 1)
            {
                ((Page)pages[curPage]).Unload();
                curPage++;
                ((Page)pages[curPage]).LoadImage(graphics);
                loadRequested = false;
            }

            if (((Page)pages[curPage]).Done)
            {
                // If the current page is done, then request a load on the
                // next update.  This is delayed by frame so that the game
                // can have a one frame warning before the page switch occurs.
                loadRequested = true;
            }
            else
            {
                ((Page)pages[curPage]).Update();
            }
        }

        /// <summary>
        /// Draw the current page of the intro.
        /// </summary>
        /// <param name="graphics">Graphics instance</param>
        public void Draw(IGraphics graphics)
        {
            ((Page)pages[curPage]).Draw(graphics);
        }

        /// <summary>
        /// Clean up resources allocated for the intro.
        /// </summary>
        public void Dispose()
        {
            foreach (Page p in pages)
            {
                p.Dispose();
            }
        }
    }
}
