//=====================================================================
//  File:      Bubble.cs
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
using System.Windows.Forms;
using System.Drawing;
using System.ComponentModel;

class Window : Form
{
	struct Bubble
	{
		public int x, y;
		public int xOld, yOld;
		public int velocityH, velocityV;
		public int width, height;
		
		static private Random Rand = new Random();
		
		public void Init(Window window)
		{
			Random rnd;
			int bubbleSize;
			int windowWidth, windowHeight;
			
			window.GetClientSize(out windowWidth, out windowHeight);
			
			rnd = Bubble.Rand;
			
			this.x = (rnd.Next() % windowWidth);
			this.y = (rnd.Next() % windowHeight);
			
			this.ResetDx();
			this.ResetDy();
			
			bubbleSize = rnd.Next(30) + 10;
			this.width = bubbleSize;
			this.height = bubbleSize;
		}
				
		void ResetDx()
		{
			this.velocityH = (Bubble.Rand.Next(10) - 5);
			if (this.velocityH == 0)
				this.velocityH = 3;
		}

		void ResetDy()
		{
			this.velocityV = (Bubble.Rand.Next(10) - 5);
			if (this.velocityV == 0)
				this.velocityV = 1;
		}
		
		public void DoTick(Window window)
		{
			int windowWidth, windowHeight;

			window.GetClientSize(out windowWidth, out windowHeight);
			
			this.xOld = this.x;
			this.yOld = this.y;
			
			this.x += this.velocityH;
			this.y += this.velocityV;
			
			if (this.x < 0)
			{
				this.x = 0;
				this.ResetDx();
			}
			
			if (this.x >= windowWidth - this.width)
			{
				this.x = windowWidth - this.width - 1;
				this.ResetDx();
			}

			
			if (this.y < 0)
			{
				this.y = 0;
				this.ResetDy();
			}

			if (this.y >= windowHeight - this.height)
			{
				this.y = windowHeight - this.height - 1;
				this.ResetDy();
			}
		}

		private void FillInnerReflection(Graphics graphics, ref int x, ref int y, ref int width, ref int height)
		{
			x += width / 8;
			y += height / 8;
			
			width /= 2;
			height /= 2;
			
			graphics.FillEllipse(new SolidBrush(Color.White), x, y, width, height);
		}
		
		public void Draw(Graphics graphics)
		{
			int x, y, width, height;

			graphics.DrawEllipse(new Pen(Color.White), this.x, this.y, this.width - 1, this.height - 1);
			
			x = this.x;
			y = this.y;
			width = this.width;
			height = this.height;
			
			FillInnerReflection(graphics, ref x, ref y, ref width, ref height);
		}
		
		public void Erase(Graphics graphics, Brush brush)
		{
			graphics.FillRectangle(brush, this.x, this.y, this.width, this.height);
		}
		
		public Rectangle WholeBounds
		{
			get
			{
				return(Rectangle.Union(new Rectangle(this.x, this.y, this.width + 1, this.height + 1), new Rectangle(this.xOld, this.yOld, this.width + 1, this.height + 1)));
			}
		}
	}
	
	private const int MaxBubbles = 20;
	
	private Bubble[] bubbles = new Bubble[MaxBubbles];
	private Brush backgroundBrush = new SolidBrush(Color.SlateBlue);
	private Pen backgroundPen = new Pen(Color.SlateBlue);
	private Bitmap offscreenBitmap;
	private Graphics offscreenGraphics;
	private int width, height;
	private bool isRunning;
	private Graphics realGraphics;
	
	private int m_tickStart;
	private int m_tickLast;
	private int m_cframe = 0;

	public void Init(int width, int height)
	{
		int bubble;
		
		this.BackColor = Color.SlateBlue;
		this.ClientSize = new Size(width, height);
		this.Text = "Bubbles";
		this.Visible = true;
		this.width = this.ClientSize.Width;
		this.height = this.ClientSize.Height;
		
		for (bubble = 0; bubble < MaxBubbles; bubble++)
			this.bubbles[bubble].Init(this);
		
		this.offscreenBitmap = new Bitmap(this.width, this.height);
		this.offscreenGraphics = Graphics.FromImage(this.offscreenBitmap);
		this.offscreenGraphics.FillRectangle(this.backgroundBrush, 0, 0, this.width, this.height);
		
		this.realGraphics = this.CreateGraphics();
		
		this.isRunning = true;
	}

	public void GetClientSize(out int width, out int height)
	{
		width = this.width;
		height = this.height;
	}
		
	public void DoTick()
	{
		int bubble;
		
		m_cframe++;
		
		EraseAll(this.offscreenGraphics);
		
		for (bubble = 0; bubble < MaxBubbles; bubble++)
			this.bubbles[bubble].DoTick(this);
			
		RedrawAll(this.offscreenGraphics);
		RefreshAll(this.realGraphics);
		
		if ((Environment.TickCount - m_tickLast) / 1000 > 3)
		{
			m_tickLast = Environment.TickCount;
			
			this.SyncTitleBar();
		}
	}

	private void SyncTitleBar()
	{
		int csec;
		int wRefresh;
		
		csec = ((Environment.TickCount - m_tickStart) / 1000);
		if (csec != 0)
			wRefresh = m_cframe / csec;
		else
			wRefresh = 0;
		
		this.Text = "Bubble - " + wRefresh + " frames/sec";
	}

	private void RefreshAll(Graphics graphicsPhys)
	{
		Rectangle rc;
		int bubble;
				
		for (bubble = 0; bubble < MaxBubbles; bubble++)
		{
			rc = this.bubbles[bubble].WholeBounds;
			
			graphicsPhys.DrawImage(this.offscreenBitmap, rc.X, rc.Y, rc, GraphicsUnit.Pixel);
		}
	}
		
	private void RedrawAll(Graphics graphics)
	{
		int bubble;
		
		for (bubble = 0; bubble < MaxBubbles; bubble++)
			this.bubbles[bubble].Draw(graphics);
	}
	
	private void EraseAll(Graphics graphics)
	{
		int bubble;
		
		for (bubble = 0; bubble < MaxBubbles; bubble++)
			this.bubbles[bubble].Erase(graphics, this.backgroundBrush);
	}

	private void Exit()
	{
		this.isRunning = false;
		this.Close();
	}
	
	protected override void OnKeyDown(KeyEventArgs keyArgs)
	{
		this.Exit();
	}
	
	protected override void OnMouseDown(MouseEventArgs mouseArgs)
	{
		this.Exit();
	}

	protected override void OnClosing(CancelEventArgs cancelg)
	{
		this.isRunning = false;
		cancelg.Cancel = true;
	}

	protected override void OnClosed(EventArgs eventg)
	{
		this.isRunning = false;
	}
	
	protected override void OnResize(EventArgs evtg)
	{
		this.SyncTitleBar();
	}
	
	public void RunMe()
	{
		m_cframe = 0;
		m_tickStart = Environment.TickCount;
		m_tickLast = m_tickStart;
		
		while (this.isRunning)
		{
			this.DoTick();
			Application.DoEvents();
		}
	}
}

class App
{
	public static int Main()
	{
		Window window;

		window = new Window();

		window.Init(window.ClientSize.Width, window.ClientSize.Height);
		window.RunMe();

		return (0);
	}
}
