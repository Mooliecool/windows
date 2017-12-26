//=====================================================================
//  File:      BaseObj.cs
//
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
using System.Drawing.Imaging;
using System.Windows.Forms;
using System.Reflection;
using System.Threading;
using System.ComponentModel;

namespace Microsoft.Samples.RomanLegion
{
	public class BaseObj
	{
		// Image index to draw
		private int activeImage = 0;

		// Object's Left
		private int left;
		// Object's Top
		private int top;
	
		// Object's Left before move
		private int leftOld;
		// Object's Top before move
		private int topOld;
	
		// Object's Speed to right 
		private int horizontalVelocity;
		// Object's Speed to down
		private int verticalVelocity;

		// Object's Width 
		private int width;
		// Object's Height
		private int height;
	
		// Max x Position
		private int maxX;
		// Max y Position
		private int maxY;
	
		// Current game
		private Game currentGame;
	
		// is to draw ?
		private bool fActive;

		// Object Bitmap
		private  Bitmap[] bitmapCells;
		
		// Object Attributes (which color range is transparent ?)
		private ImageAttributes[] cellAttributes;

        protected int LeftOld
        {
            get
            {
                return(leftOld);
            }
            set
            {
                leftOld = value;
            }
        }

        protected int TopOld
        {
            get
            {
                return(topOld);
            }
            set
            {
                topOld = value;
            }
        }

        protected int HorizontalVelocity
        {
            get
            {
                return(horizontalVelocity);
            }

            set
            {
                horizontalVelocity = value;
            }
        }

        protected int VerticalVelocity
        {
            get
            {
                return(verticalVelocity);
            }
            set
            {
                verticalVelocity = value;
            }
        }

        protected int MaximumX
        {
            get
            {
                return(maxX);
            }
            set
            {
                maxX = value;
            }
        }

        protected int MaximumY
        {
            get
            {
                return(maxY);
            }
            set
            {
                maxY = value;
            }
        }

        protected Game CurrentGame
        {
            get
            {
                return(currentGame);
            }
            set
            {
                currentGame = value;
            }
        }

        protected Bitmap[] AnimationCells
        {
            get
            {
                return(bitmapCells);
            }
            set
            {
                bitmapCells = value;
            }
        }

        protected ImageAttributes[] CellAttributes
        {
            get
            {
                return(cellAttributes);
            }
            set
            {
                cellAttributes = value;
            }
        }

		public BaseObj(Game game)
		{
			currentGame = game;

			maxX = currentGame.WorldBounds.Width;
			maxY = currentGame.WorldBounds.Height;

			leftOld = 0;
			topOld = 0;
		}

        public void Draw(Graphics gx)
		{
			// if is to draw
			if (fActive)
			{
				gx.DrawImage( 
					bitmapCells[activeImage],
					new Rectangle(left, top, width , height),
					0,
					0,
					width,
					height,
                    GraphicsUnit.Pixel,
					cellAttributes[activeImage]);
			}
		}

		public void Load(string bmpName, int bitmapIndex)
		{
			try
			{
				// Load BMP from file
				bitmapCells[bitmapIndex] = new Bitmap(
                    currentGame.BaseDirectory + @"Content\" + bmpName);
				width=bitmapCells[bitmapIndex].Width;
				height=bitmapCells[bitmapIndex].Height;
			}
			catch
			{
				// Any error reading the BMP, create a dummy one
				bitmapCells[bitmapIndex] = new Bitmap(width, height);
				Graphics.FromImage(bitmapCells[bitmapIndex]).FillEllipse(new SolidBrush(Color.White), new Rectangle(0, 0, width - 1, height - 1));
			}

			// Get the color of a background pixel as the Pixel 0,0 
			Color TranspColor = bitmapCells[bitmapIndex].GetPixel(0, 0);
			
			// Set the Attributes for the Transparent color
			cellAttributes[bitmapIndex] = new ImageAttributes();
			cellAttributes[bitmapIndex].SetColorKey(TranspColor, TranspColor);
		}

		public void Restore()
		{
			// Restore the Game Background
			currentGame.OffscreenGraphics.DrawImage(
				currentGame.BackBmp,
				left,
				top,
				new Rectangle(left, top, width , height),
				GraphicsUnit.Pixel);
		}

		public Rectangle Bounds
		{
			get
			{
				return(new Rectangle(left, top, width, height));
			}
		}

		public Rectangle DirtyArea
		{
			get
			{
				return(Rectangle.Union(new Rectangle(left, top, width, height), new Rectangle(leftOld, topOld, width, height)));
			}
		}

		// Expose object Draw property
		public bool IsAlive
		{
			get
			{
				return(fActive);
			}
			set
			{
				fActive = value;
			}
		}

        public int ActiveImage
        {
            get
            {
                return(activeImage);
            }
            set
            {
                activeImage = value;
            }
        }

        public int Left
        {
            get
            {
                return(left);
            }
            protected set
            {
                left = value;
            }
        }

        public int Top
        {
            get
            {
                return(top);
            }
            protected set
            {
                top = value;
            }
        }

        public int Width
        {
            get
            {
                return(width);
            }
            protected set
            {
                width = value;
            }
        }

        public int Height
        {
            get
            {
                return(height);
            }
            protected set
            {
                height = value;
            }
        }
	}
}
