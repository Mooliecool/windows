//=====================================================================
//  File:      Score.cs
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
using System.Windows.Forms;
using System.Reflection;
using System.Threading;
using System.ComponentModel;

namespace Microsoft.Samples.RomanLegion
{
	public class Score : BaseObj
	{
		// Score Font
		Font drawFont;
		SolidBrush drawBrush;
		int currentScore;
		int currentLevel;

		public Score(Game game) : base(game)
		{
			// Score Font
			drawFont = new Font("Arial", 16, System.Drawing.FontStyle.Bold);

			// Font Color
			drawBrush = new SolidBrush(Color.BlueViolet);

			// Create rectangle for drawing.
			Left = 0;
			Top  = 0;
				
			LeftOld = Left;
			TopOld  = Top;
			currentScore = 0;
			currentLevel = 1;
			IsAlive = true;
			Width  = 240;
			Height = 50;
		}

		public void Process()
		{
		}

		new public void Draw(Graphics gx)
		{
			// Write the Score
			string Score = "Score: " + currentScore + " Level: " + currentLevel;
			gx.DrawString(Score, drawFont, drawBrush, 5.0F, 10.0F);
		}

		// Expose the Score
		public int Value
		{
			get
			{
				return(currentScore);
			}
			set
			{
				currentScore = value;
			}
		}
		// Expose the Level
		public int Level
		{
			get
			{
				return(currentLevel);
			}
			set
			{
				currentLevel = value;
			}
		}
	}
}
