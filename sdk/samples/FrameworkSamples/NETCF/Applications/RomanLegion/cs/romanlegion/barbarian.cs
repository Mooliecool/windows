//=====================================================================
//  File:      Barbarian.cs
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
	public class Barbarian : BaseObj
	{
		private const int KeyboardMove = 5;

		private bool fMoveLeft;
		private bool fMoveRight;
		private bool fKeyboard;

		public Barbarian(Game game) : base(game)
		{
			//Barbarian Always Alive
			IsAlive = true;
		
			// Bmp Size
			Width = 14;
			Height = 10;
	
			// Start Position
			Left = MaximumX / 2;
			Top  = CurrentGame.WorldBounds.Height - Height - 20;
			LeftOld = Left;
			TopOld  = Top;

			// Load Bmp
			AnimationCells = new Bitmap[1];
			CellAttributes = new ImageAttributes[1];
			Load("Barbarian.bmp", 0);
		}

		public void Process()
		{
			Point ptMouse;
			LeftOld = Left;

			// If input is from Keyboard
			if (fKeyboard)
			{
				if (fMoveLeft)
					Left -= KeyboardMove;
				else if (fMoveRight)
					Left += KeyboardMove;
			}
			// If the mouse move
			else
			{
				ptMouse = CurrentGame.MouseLoc;
				Left = ptMouse.X - (Width / 2);
			}

			// Limit the Barbarian position to the window
			if (Left < 0)
				Left = 0;
			else if ((Left + Width) >= MaximumX)
				Left = MaximumX - Width;
		}
	
		// Set the Barbarian to move left
		public void GoLeft(bool move)
		{
            fKeyboard = true;
            fMoveLeft = move;
		}
	
		// set the Barbarian to move right
		public void GoRight(bool move)
		{
            fKeyboard = true;
            fMoveRight = move;
		}

		public void KeyboardOff()
		{
            fKeyboard = false;
		}
			
		public bool CollidesWith(Arrow arrow)
		{
			bool bCollide = false;

			// Check if the Arrow hits the Barbarian 
			if (this.Bounds.IntersectsWith(arrow.Bounds))
				bCollide = true;
			
            return(bCollide);
		}
	}
}
