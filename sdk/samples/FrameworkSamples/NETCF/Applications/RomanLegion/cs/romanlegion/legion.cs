//=====================================================================
//  File:      Legion.cs
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
	public class Legion
	{
		// The Game
		private Game currentGame;

		// Which Roman's image is to draw
		private int idxDirection=0;

		// Legion Maximum Top position
		private const int StartTop = 50;
	
		// Column number 
		private const int Columns = 6;
		// Row number 
		private const int Rows = 6;

		// Left speed
		private int horizontalSpeed = 1;
		// Down speed
		private int verticalSpeed = 0;
	
		// Total Romans number in the legion
		private const int CountOfRomans = Columns * Rows;

		// Array of Romans
		private Roman[] romans;

		// Bounding Box for the Legion
		Rectangle boundingBox;

		public Legion(Game game)
		{
			int row;
			int col;

			// Roman adding
			Roman newRoman;
			
			currentGame = game;

			// Create Romans
			romans = new Roman[CountOfRomans];
			for (row = 0; row < Rows; row++)
			{
				for (col = 0; col < Columns; col++)
				{
					newRoman = new Roman(currentGame, StartTop, row, col);
					romans[(row * Columns) + col] = newRoman;
				}		
			}
		}

		public void Process()
		{
			verticalSpeed = 0;

			// Check walls

			// If reach the right wall
			if ((boundingBox.Left + horizontalSpeed) < 0)
			{
				horizontalSpeed = Math.Abs(horizontalSpeed);
				verticalSpeed = 10;
			}

			// If reach the left wall
			if ((boundingBox.Right + horizontalSpeed) >= currentGame.WorldBounds.Width)
			{
				horizontalSpeed = -Math.Abs(horizontalSpeed);
				verticalSpeed = 10;
			}

			// If the Legion reach the Barbarian
			if (boundingBox.Bottom >= currentGame.Barbarian.Bounds.Top)
				currentGame.Lost();

			// Process all Romans alive
			int iLastRoman = -1;
			for (int iRoman = 0; iRoman < CountOfRomans; iRoman++)
			{
				if (romans[iRoman].IsAlive)
				{
					romans[iRoman].Process(horizontalSpeed, verticalSpeed);
					iLastRoman = iRoman;
				}
			}
			
			// The last Roman launch the Arrow
			if (!currentGame.RomanArrow.IsAlive && (iLastRoman >= 0))
			{
				currentGame.RomanArrow.Start(
					romans[iLastRoman].Left + (romans[iLastRoman].Width / 2),
					romans[iLastRoman].Top + romans[iLastRoman].Height,
					0,
					5);
			}
		}

		public void Draw(Graphics gx)
		{
			// Try to draw all Romans
			for (int iRoman = 0; iRoman < CountOfRomans; iRoman++)
			{
				romans[iRoman].ActiveImage = idxDirection;
				romans[iRoman].Draw(gx);
			}

			// Switch the Romans Bitmap
			if (idxDirection == 0)
			{
				idxDirection = 1;
			}
			else
			{
				idxDirection = 0;
			}
		}

		public Rectangle DirtyArea
		{
			// Return the Bounding Box as the Dirty area
			get
			{
				Rectangle rc;

				rc = boundingBox;

				if (horizontalSpeed > 0)
					rc.Width+=horizontalSpeed;
				else
				{
					rc.Width -= horizontalSpeed;
					rc.X += horizontalSpeed;
				}

				return(rc);
			}
		}

		public void Restore()
		{
			// All Romans killed ?
			bool fAllDead = true;

			// Is the fFirstAlive
			bool fFirstAlive = true;

			// all Romans
			for (int iRoman = 0; iRoman < CountOfRomans; iRoman++)
			{
				// If Roman not dead
				if (romans[iRoman].IsAlive)
				{
					// If is the fFirstAlive Roman init the Bound box as him
					if (fFirstAlive)
					{
						boundingBox = romans[iRoman].DirtyArea;
						fFirstAlive = false;
					}
					// Add the Roman to the Bound Box
					else
						boundingBox=Rectangle.Union(boundingBox,romans[iRoman].DirtyArea);

					fAllDead=false;

					// Restore the backgroud of the Roman
					romans[iRoman].Restore();
				}
			}

			// If no Romans alive, the level is finished
			if (fAllDead)
				currentGame.Won();	
		}

		public void Reset(int level)
		{
			// increase the Legion speed
			horizontalSpeed = level;
			
			// Reset All Legion
			boundingBox = romans[0].DirtyArea;
			for (int iRoman = 0; iRoman < CountOfRomans; iRoman++)
			{
				romans[iRoman].Reset(StartTop, (int)iRoman / Columns, iRoman % Columns);
				boundingBox=Rectangle.Union(boundingBox, romans[iRoman].DirtyArea);
			}
		}

		public bool KillRomanFromXY(int x, int y)
		{
			bool fHit = false;
		
			// check all Romans
			for (int iRoman = 0; iRoman < CountOfRomans; iRoman++)
			{
				// If the Roman is Alive
				if (romans[iRoman].IsAlive)
				{
					// If tha Arrow hit the Roman
					if (romans[iRoman].Bounds.Contains(x,y)) 
					{
						fHit = true;

						// Disable the Roman
						romans[iRoman].IsAlive = false;

						// Increase the Score
						currentGame.Score.Value += 10 * (6 - (iRoman / Columns));
						break;
					}
				}
			}

			return(fHit);
		}
	}
}
