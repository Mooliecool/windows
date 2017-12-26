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
using System.Drawing;
using System.Diagnostics;
using System.Globalization;
using GraphicsLibrary;

namespace GameApp
{
    /// <summary>
    /// A Layer represents a scrolling background or foreground for the level.
    /// </summary>
    public class Layer
    {
        /// <summary>
        /// Rate that the layer scrolls in pixels / second
        /// </summary>
        public float ScrollRate
        {
            get { return scrollPerFrame; }
            set { scrollPerFrame = value; }
        }
        private float scrollPerFrame = 0.0F;

        /// <summary>
        /// Width of the widest image in the layer
        /// </summary>
        private int imageWidth = 0;

        /// <summary>
        /// Height of the highest image in the layer
        /// </summary>
        public int Height { get { return imageHeight; } }
        private int imageHeight = 0;

        /// <summary>
        /// Current x location within the world.  This is the number of pixels
        /// that the layer has scrolled.
        /// </summary>
        public float WorldX { get { return worldXValue; } }
        private float worldXValue = 0.0F;

        /// <summary>
        /// Screen Y location at which to draw the layer.
        /// </summary>
        public int ScreenY { get { return screenYValue; } }
        private int screenYValue = 0;

        /// <summary>
        /// Defines a panel to be drawn to the screen.  Panels represent
        /// the visible portion of the images that make up a layer.
        /// </summary>
        private class PanelInfo
        {
            public Rectangle drawRegion = new Rectangle(0, 0, 0, 0);
            public int bmpIndex = 0;
            public int xOffset = 0;
        }
        private PanelInfo[] panels = new PanelInfo[2];

        /// <summary>
        /// Number of panels needed to draw the image since the screen may
        /// span more than one image.
        /// </summary>
        private int numPanels = 0;

        /// <summary>
        /// Screen X location at which to draw the layer.
        /// </summary>
        private int screenX = 0;

        /// <summary>
        /// Draw options that specify how to draw the images associated with
        /// this layer.
        /// </summary>
        private DrawOptions drawOptions = 0;

        /// <summary>
        /// Sequence of image indices that make up this layer.
        /// </summary>
        private byte[] sequence = null;

        /// <summary>
        /// Initial scroll rate stored to be re-used when the level is reset.
        /// </summary>
        private float resetScrollPerFrame;

        /// <summary>
        /// Create a layer instance as defined by DataRow.
        /// </summary>
        /// <param name="dr">DataRow defining the layer</param>
        /// <param name="imageList">List of images used in the level</param>
        public Layer(DataRow dr, ArrayList imageList)
        {
            Debug.Assert(imageList.Count > 0,
                "Layer.Layer: Image list must be loaded before Layer");

            Debug.Assert(dr != null,
                "Layer.Layer: DataRow invalid");

            // Initialize the panel data
            for (int i = 0; i < panels.Length; i++)
            {
                panels[i] = new PanelInfo();
                Debug.Assert(panels[i] != null,
                    "Layer.Layer: Failed to initialize PanelInfo");
            }

            // Read in the general information
            screenX = int.Parse((string)dr["X"], 
                CultureInfo.InvariantCulture);
            screenYValue = int.Parse((string)dr["Y"], 
                CultureInfo.InvariantCulture);
            scrollPerFrame = float.Parse((string)dr["ScrollRate"], 
                CultureInfo.InvariantCulture);
            resetScrollPerFrame = scrollPerFrame;

            // Read in the image sequence.  Each element in the list is
            // an index into imageList representing an image in the background
            ReadSequence((string)dr["ScrollIndices"]);

            // Set the width and height of the layer
            imageWidth = 0;
            imageHeight = 0;

            // Cycle through the sequence to determine some layer
            // parameters and set the images' source keys
            foreach (byte b in sequence)
            {
                IBitmap bmp = (IBitmap)imageList[sequence[b]];
                Debug.Assert(bmp != null,
                    string.Format(CultureInfo.InvariantCulture,
                    "Layer.Layer: Bitmap {0} is invalid", b));

                if (bmp.Width > imageWidth)
                    imageWidth = bmp.Width;

                if (bmp.Height > imageHeight)
                    imageHeight = bmp.Height;
            }



            // Verify that the sequence is valid
            VerifySequence(imageList);
        }

        /// <summary>
        /// Called when the level is reset.
        /// </summary>
        public void Reset()
        {
            scrollPerFrame = resetScrollPerFrame;
            worldXValue = 0.0F;
        }

        /// <summary>
        /// Scroll the layer to keep the player on the screen.  This is
        /// used if the
        /// player dies and the animation moves off screen
        /// </summary>
        /// <param name="p">Player instance</param>
        public void MoveForPlayer(Player p)
        {
            if (p.WorldX - worldXValue - (p.Width >> 1) < 0.0F)
            {
                worldXValue -= 1.0F;
                if (worldXValue < 0.0F)
                    worldXValue = 0.0F;
            }
        }

        /// <summary>
        /// Update the layer.  This method calculates which images are visible
        /// given the location of the layer and fills in the appropriate
        /// panel information.
        /// </summary>
        /// <param name="graphics">Graphics instance</param>
        public void Update(IGraphics graphics)
        {
            // Update the world location
            worldXValue += GameMain.SecondsPerFrame * scrollPerFrame;
            int drawX = (int)(worldXValue);

            Debug.Assert(imageWidth > 0,
                "Layer.Update: Invalid image width");

            // Determine in which image the layer starts
            int curImage = drawX / imageWidth;
            Debug.Assert(curImage >= 0 && curImage < sequence.Length,
                "Layer.Update: Current image evaluated out of range");

            // Determine the starting pixel in the image
            drawX -= curImage * imageWidth;
            Debug.Assert(drawX >= 0,
                "Layer.Update: Draw X evaluated to negative number");

            // If the layer has scrolled to the end then stop
            if ((curImage >= sequence.Length - 1) &&
                (drawX > imageWidth - graphics.ScreenWidth))
            {
                scrollPerFrame = 0.0F;
                drawX = imageWidth - graphics.ScreenWidth;
                worldXValue = (float)(drawX + (curImage * imageWidth));
            }

            // If the current panel does not encompass the width of the screen
            // then two panels need to be drawn
            if (drawX + graphics.ScreenWidth > imageWidth)
            {
                numPanels = 2;

                // Draw the panel on the left
                panels[0].drawRegion.X = drawX;
                panels[0].drawRegion.Y = 0;
                panels[0].drawRegion.Height = imageHeight;
                panels[0].drawRegion.Width = imageWidth - drawX;
                panels[0].bmpIndex = sequence[curImage];
                panels[0].xOffset = 0;

                // Fill the rest of the screen with the next image
                panels[1].drawRegion.X = 0;
                panels[1].drawRegion.Y = 0;
                panels[1].drawRegion.Height = imageHeight;
                panels[1].drawRegion.Width =
                    graphics.ScreenWidth - panels[0].drawRegion.Width;
                panels[1].bmpIndex = sequence[curImage + 1];
                panels[1].xOffset = panels[0].drawRegion.Width;
            }
            else
            {
                numPanels = 1;

                // The current panel will fill the whole screen
                panels[0].drawRegion.X = drawX;
                panels[0].drawRegion.Width = graphics.ScreenWidth;
                panels[0].drawRegion.Y = 0;
                panels[0].drawRegion.Height = imageHeight;
                panels[0].bmpIndex = sequence[curImage];
                panels[0].xOffset = 0;
            }
        }

        /// <summary>
        /// Draw the current layer.
        /// </summary>
        /// <param name="graphics">Graphics instance</param>
        /// <param name="imageList">List of level images</param>
        public void Draw(IGraphics graphics, ArrayList imageList)
        {
            // Set the layer draw options
            graphics.SetDrawOptions(drawOptions);

            // Draw each panel in the layer (as set by Update)
            for (int i = 0; i < numPanels; i++)
            {
                graphics.DrawBitmap(screenX + panels[i].xOffset, screenYValue,
                    panels[i].drawRegion,
                    ((IBitmap)imageList[panels[i].bmpIndex]));
            }

            // Clear the draw options
            graphics.ClearDrawOptions(drawOptions);
        }

        /// <summary>
        /// Parse a string that defines a sequence of bytes.
        /// The format of the string should read "a,b,c,d" where
        /// a,b,c, and d are entries in the sequence.
        /// </summary>
        /// <param name="seqText">Sequence text</param>
        private void ReadSequence(string seqText)
        {
            Debug.Assert(seqText.Length > 0,
                "Layer.ReadSequence: No text specified");

            string[] txtIndex = seqText.Split(',');

            Debug.Assert(txtIndex.Length > 0,
                "Layer.ReadSequence: Invaid sequence text");

            sequence = new byte[txtIndex.Length];
            for (int i = 0; i < txtIndex.Length; i++)
            {
                sequence[i] = byte.Parse(txtIndex[i], NumberStyles.Integer, 
                    CultureInfo.InvariantCulture);
            }
        }

        /// <summary>
        /// Validates a sequence by raising an assert if the sequence has
        /// an index that is out of range or if all images in the sequence
        /// are not of equal width and height.
        /// </summary>
        /// <param name="imageList">List of images in the level</param>
        private void VerifySequence(ArrayList imageList)
        {
            for (byte i = 1; i < sequence.Length; i++)
            {
                Debug.Assert(sequence[i] >= 0 &&
                    sequence[i] < imageList.Count &&
                    imageList[sequence[i]] != null,
                    "Image index in background is out of range");

                Debug.Assert(imageWidth == ((IBitmap)imageList[
                    sequence[0]]).Width &&
                    imageHeight ==
                    ((IBitmap)imageList[sequence[0]]).Height,
                    "Background images in the same layer must be the same" +
                    " dimensions");
            }
        }
    }
}
