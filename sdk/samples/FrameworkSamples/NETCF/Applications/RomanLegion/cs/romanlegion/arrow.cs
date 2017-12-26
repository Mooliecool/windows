//=====================================================================
//  File:      Arrow.cs
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
	/// <summary>
	/// This Class handdles the Barbarian arrow and the Romans arrows
	/// </summary>
	public class Arrow: BaseObj
	{
		
		private bool fMoving;

		public Arrow(Game game) : base(game)
		{
			Width  = 3;
			Height = 10;
			
			// Position the arrow
			Start(MaximumX / 2, MaximumY - 30, 0, -5);

			// Load bitmap
			AnimationCells = new Bitmap[2];
			CellAttributes = new ImageAttributes[2];
			Load("Arrow.bmp", 0);
			Load("Arrow1.bmp", 1);
		}

		public void Process()
		{
			LeftOld = Left;
			TopOld  = Top;

			// Arrow ready to be launched
			if (!fMoving)
			{
				Left = CurrentGame.Barbarian.Bounds.Left + (CurrentGame.Barbarian.Bounds.Width / 2) - (Width / 2);
				Top  = CurrentGame.Barbarian.Bounds.Top - Height;
			}
			// Arrow moving
			else
			{
				// Move it
				Top += VerticalVelocity;

				// check if any Roman were killed
				if (CurrentGame.Legion.KillRomanFromXY(Left, Top))
				{
					// Stop the arrow
					Stop();
				}
			
				// Arrow reach top of the window
				if (Top < 50)
				{
					//Stop the arrow
					Stop();
				}

				// Arrow reach the bottom of the window
				if (Top > MaximumY)
				{
					this.IsAlive = false;
					this.Stuck   = true;
				}

				// Check if hit the Barbarian
				if (CurrentGame.Barbarian.CollidesWith(this))
				{
					// If the Arrow hit the Barbarian , game over
					CurrentGame.Lost();
				}
			}
		}

		// Can Launch Arrow ?
		public bool Stuck
		{
			get
			{
				return(!fMoving);
			}
			set
			{
				fMoving = !value;
			}
		}

		public void Start(int x, int y, int horizontalVelocity, int verticalVelocity)
		{
			// release the arrow
			fMoving  = true;
			IsAlive  = true;

			// Arrow initial position
			LeftOld = Left;
			TopOld  = Top;
			Left = x - (Width / 2);
			Top  = y;

			this.HorizontalVelocity = horizontalVelocity;

			// Arrow up
			if (verticalVelocity < 0) 
				ActiveImage = 0;
            else // Arrow down
				ActiveImage = 1;

			// Arrow direction
			this.VerticalVelocity = verticalVelocity;
		}

		public void Stop()
		{
			// Disable the arrow
			fMoving = false;
			HorizontalVelocity = 0;
			VerticalVelocity = 0;
		}
	}
}
