//=====================================================================
//  File:      Roman.cs
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
	public class Roman : BaseObj
	{
		public Roman(Game game, int initialTop, int row, int col) : base(game)
		{
			// Bmp Size
			Width  = 16;
			Height = 20;
	
			// Load Roman Bmps 
			AnimationCells = new Bitmap[2]; 
			CellAttributes = new ImageAttributes[2];
			Load("RomanA" + row + ".bmp", 0);
			Load("RomanB" + row + ".bmp", 1);

			// Reset Roman Position and turn it alive
			Reset(initialTop, row, col);
		}

		public void Process(int horizontalDelta, int verticalDelta)
		{
			LeftOld = Left;
			TopOld  = Top;

			// Move
			Left += horizontalDelta;
			Top  += verticalDelta;
		}

		public void Reset(int newTop, int row, int col)
		{
			// Turn the Roman on
			this.IsAlive = true;

			// Reset the position
			Left = (Width * 2) * col;
			Top  = newTop + (row * Height); 
		}
	}
}
