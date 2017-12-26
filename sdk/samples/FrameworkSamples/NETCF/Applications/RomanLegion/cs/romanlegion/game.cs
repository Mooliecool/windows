//=====================================================================
//  File:      Game.cs
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
	public class Game: Form
	{
		// State of the game
		private enum Display
		{
			Splash,
			Won,
			Game
		};
		
		// Where we get the images
		private string baseDirectory = "";

		// Width of Game
		private int gameWidth;

		// Height of Game
		private int gameHeight;
	
		// Game is running ?
		private bool fRunning;
	
		// Mouse/Pen Position
		private int xMouse;
        private int yMouse;
	
		// Is to Repaint the game
		private bool fRepaint;
	
		private Bitmap backgroundBitmap;
		private Graphics offscreenGraphics;
		private Graphics formGraphics;
		private Bitmap[] statusBitmaps;
	
		// The Barbarian launching arrows
		private Barbarian player;

		// The Arrow
		private Arrow currentPlayerArrow;	

		private Arrow currentRomanArrow;	

		// Legion Attacking
	    private Legion attackingLegion;

		// Game Score
		private Score currentScore;
		
		// Last update tick
		private int lastTick;
	
		// Game status
		private Display currentStatus;

		// Game Paused ?
		private bool fPaused = false;
	
		public Game()
		{

			string strAssmPath = System.Reflection.Assembly.GetExecutingAssembly().GetName().CodeBase;
			baseDirectory = strAssmPath.Substring(0, strAssmPath.LastIndexOf(@"\") + 1);

			this.BackColor = Color.Red;

			// Initialize screen size
			this.Size = new Size(
				System.Windows.Forms.Screen.PrimaryScreen.Bounds.Width,
				System.Windows.Forms.Screen.PrimaryScreen.Bounds.Height - 24);


			this.Visible = true;
		
			// Get game client window size
			this.gameWidth = this.ClientSize.Width;
			this.gameHeight = this.ClientSize.Height;
		
			// Window caption
			this.Text = "Roman Legion";
		
			Init();
		
			// Start running
			fRunning = true;
		}

		private void GameMainloop()
		{
			int y;
			Graphics gx;
		
			// get current tick count
			lastTick = Environment.TickCount;
		
			InitGame();

			// Game Running
			while (fRunning)
			{
				// Do game Step
				DoTick();

				// Process messages to this window
				Application.DoEvents();
			}
			
			// Game over, paused or won the level
			gx = this.CreateGraphics();
			
			// Draw the bitmap correspondent to game status
			for (y = 0; y < this.ClientSize.Height; y += 10)
			{
				gx.DrawImage(statusBitmaps[(int)currentStatus], 0, -gameHeight + y);
				Thread.Sleep(10);
			}
					
			gx.Dispose();

		}

		private void DoTick()
		{
			int tickSleep;

			// Let the game components move
			ProcessAll();

			// Redraw the game components
			DrawAll();

			// Game timming (Wait next step)
			tickSleep = lastTick + (1000 / 70) - Environment.TickCount;

			if (tickSleep > 0)
				Thread.Sleep(tickSleep);

			lastTick = Environment.TickCount;

			RefreshAll();
			RestoreAll();
		}

		private void Init()
		{
			// Init game components
			player = new Barbarian(this);
			currentPlayerArrow = new Arrow(this);
			currentRomanArrow = new Arrow(this);
			attackingLegion = new Legion(this);
			currentScore = new Score(this);
			
		
			// Init the mouse x position as the middle of the screen
			xMouse = gameWidth / 2;

			// Load all results bitmaps
			statusBitmaps = new Bitmap[4];

			string Prefix = "PPC";
			
			LoadBmp(Prefix + "Splash.bmp", (int)Display.Splash);
			LoadBmp(Prefix + "Won.bmp", (int)Display.Won);
			LoadBmp(Prefix + "Pause.bmp", (int)Display.Game);
			LoadBmp(Prefix + "Background.bmp", 3);
			backgroundBitmap = new Bitmap(baseDirectory + @"Content\" + Prefix + "Background.bmp");

			// Create offscreen playscren
			
			offscreenGraphics = Graphics.FromImage(backgroundBitmap);
			formGraphics = this.CreateGraphics();
		}

		private void InitGame()
		{
			// Game is running
			fRunning = true;
			
			// If the game was not paused
			if (!fPaused)
			{
				// Disable all arrows
				currentPlayerArrow.Stuck  = true;
				currentRomanArrow.Stuck   = true;
				currentRomanArrow.IsAlive = false;
			}
			// Game paused
			else 
				// Release the game
				fPaused = false;

			fRepaint = true;
		}
		
		private void ProcessAll()
		{
			// Get mouse position
			xMouse = this.PointToClient(Control.MousePosition).X;

			// Calculate the components new position
			player.Process();		
			currentPlayerArrow.Process();
			currentRomanArrow.Process();
			attackingLegion.Process();
		}

		private void DrawAll()
		{
			// Redraw all game components in the new position
			currentRomanArrow.Draw(offscreenGraphics);
			currentPlayerArrow.Draw(offscreenGraphics);
			player.Draw(offscreenGraphics);
			attackingLegion.Draw(offscreenGraphics);
			currentScore.Draw(offscreenGraphics);
		}

		public void Refresh(Rectangle rcDirty)
		{
			// Refresh components last position
			formGraphics.DrawImage(
				backgroundBitmap,
				rcDirty.X,
				rcDirty.Y,
				rcDirty,
				GraphicsUnit.Pixel);
		}

		private void RefreshAll()
		{
			// Refresh the components
			Refresh(player.DirtyArea);
			Refresh(currentPlayerArrow.DirtyArea);
			Refresh(currentRomanArrow.DirtyArea);
			Refresh(attackingLegion.DirtyArea);
			Refresh(currentScore.DirtyArea);

			// Redraw everything
			if (fRepaint)
			{
				formGraphics.DrawImage(backgroundBitmap, 0, 0);
				fRepaint = false;
			}
		}

		private void RestoreAll()
		{
			// Clean where the components were
			player.Restore();
			currentPlayerArrow.Restore();
			currentRomanArrow.Restore();
			attackingLegion.Restore();
			currentScore.Restore();
		}
		
		public void Lost()
		{
			// Game Over 
			// Reset score and level
			currentScore.Value = 0;
			currentScore.Level = 1;

			// Reset the Legion position
			attackingLegion.Reset(currentScore.Level);
			
			// Not running
			fRunning = false;


			// Show splash image
			currentStatus = Display.Splash;
		}

		public void Won()
		{
			// Game won

			// Increase the level
			currentScore.Level++;

			// Reset platoon position
			attackingLegion.Reset(currentScore.Level);

			// Not running
			fRunning = false;

			// Show Won Image
			currentStatus = Display.Won;
		}
	
		// Expose arrow so we can start it from the legion
		public Arrow RomanArrow
		{
			get
			{
				return(currentRomanArrow);
			}
		}

		// Expose the Barbarian to chech game over (hit on Barbarian)
		public Barbarian Barbarian
		{
			get
			{
				return(player);
			}
		}

        public string BaseDirectory
        {
            get
            {
                return(baseDirectory);
            }
        }

		// Expose the Legion to check which roman where killed
		public Legion Legion
		{
			get
			{
				return(attackingLegion);
			}
		}

		// Expose the Score so we can update it when the Romans where killed
		public Score Score
		{
			get
			{
				return(currentScore);
			}
		}

		// Graphics Offline
		public Graphics OffscreenGraphics
		{
			get
			{
				return(offscreenGraphics);
			}
		}

		// Background BMP
		public Bitmap BackBmp
		{
			get
			{
				return(statusBitmaps[3]);
			}
		}

		public Size WorldBounds
		{
			get
			{
				return(new Size(gameWidth, gameHeight));
			}
		}
			
        // Expose the Mouse position to Barbarian get self updated
		public Point MouseLoc
		{
			get
			{
				return(new Point(xMouse, yMouse));
			}
		}
	
		protected override void OnMouseDown(MouseEventArgs e)
		{
			switch(currentStatus)
			{
				case Display.Game:
					if (fRunning)
					{
						player.KeyboardOff();

						// Pause the Game
						if (e.Y < 10)
						{
							fRunning = false;
							fPaused = true;
						}
					}
					// Release the Game
					else
					{
						this.GameMainloop();
					}

					break;

			    // Start the game
				case Display.Splash:
				case Display.Won:
					currentStatus = Display.Game;
					this.GameMainloop();
					break;
			}
		}

		protected override void OnMouseUp(MouseEventArgs e)
		{
			switch(currentStatus)
			{
				case Display.Game:
					if (fRunning)
					{
						// Launch the Arrow
						if (currentPlayerArrow.Stuck == true)
						{
							currentPlayerArrow.Start(
								player.Left + (player.Width / 2),
								WorldBounds.Height-37,
								0,
								-5);
						}
					}
					break;
			}
		}

		protected override void OnMouseMove(MouseEventArgs e)
		{
			// Update mouse position
			xMouse = e.X;
			yMouse = e.Y;
		}

		protected override void OnPaint(PaintEventArgs e)
		{
			Graphics gx;
		
			gx = e.Graphics;
		
			switch(currentStatus)
			{
				case Display.Game:
					gx.DrawImage(statusBitmaps[3], 0, 0);
					fRepaint = true;
					break;

				case Display.Won:
					gx.DrawImage(statusBitmaps[1], 0, 0);
					break;

				case Display.Splash:
					gx.DrawImage(statusBitmaps[0], 0, 0);
					break;
			}
		}

		public void LoadBmp(string bmpName,int i)
		{
			// Try to Load the Bitmap
			try
			{
				statusBitmaps[i] = new Bitmap(baseDirectory + @"Content\" + bmpName);
			}

       		// Any error ? Create a bitmap 
			catch
			{
				statusBitmaps[i] = new Bitmap(Width, Height);
				Graphics.FromImage(
					statusBitmaps[i]).FillRectangle(new SolidBrush(this.BackColor),
						new Rectangle(0, 0, Width - 1, Height - 1));
				Graphics.FromImage(
					statusBitmaps[i]).DrawLine(new Pen(Color.Red), 0, 0, Width - 1, Height - 1);
				Graphics.FromImage(
					statusBitmaps[i]).DrawLine(new Pen(Color.Red), Width - 1, 0, 0, Height - 1);
			}
		}
	
		protected override void OnClosing(CancelEventArgs e)
		{
			// Close Game
			if (fRunning)
			{
				fRunning = false;
				e.Cancel = true;
			}
		}
	
		protected override void OnClosed(EventArgs eventg)
		{
			fRunning = false;
		}
	
		protected override void OnKeyDown(KeyEventArgs e)
		{
			// Enable to use keyboard 
			switch(e.KeyData)
			{
				// Start going left
				case Keys.Left:
					player.GoLeft(true);
					break;
			
				// Start going right
				case Keys.Right:
					player.GoRight(true);
					break;
			}
		}

		protected override void OnKeyUp(KeyEventArgs e)
		{
			// Enable to use keyboard 
			switch(e.KeyData)
			{
				// Stop Going left
				case Keys.Left:
					player.GoLeft(false);
					break;
			
				// Stop going right
				case Keys.Right:
					player.GoRight(false);
					break;

				// Launch Arrow
				case Keys.Space:
					if (!currentPlayerArrow.IsAlive)
						currentPlayerArrow.Start(player.Left + (player.Width / 2),
                                                 WorldBounds.Height - 37, 0, -5);
					break;

				// Pause game
				case Keys.P:
					fRunning = false;
					fPaused = true;
					break;

				// Start game
				case Keys.S:
					currentPlayerArrow.IsAlive = false;
					this.GameMainloop();
					break;
 			}
		}
	}

	// Run the Application
	class MAIN
	{
		public static void Main()
		{
			Application.Run(new Game());
		}
	}
}