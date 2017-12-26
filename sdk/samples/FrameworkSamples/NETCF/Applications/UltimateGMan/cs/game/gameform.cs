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
using System.Collections;
using System.Windows.Forms;
using System.Data;
using System.Diagnostics;
using System.Globalization;

namespace GameApp
{

    class SizeNotSupportedException : Exception {}

    /// <summary>
    /// Game form.
    /// </summary>
    public class GameForm : System.Windows.Forms.Form
    {

        GameMain Game = null;

        /// <summary>
        /// When true assures that the window size does not change from
        /// 240 x 320
        /// </summary>
        bool sizeLocked = false;

        public GameForm()
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();
#if !DESKTOP
            this.WindowState = FormWindowState.Maximized;
#endif
        }
        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose(bool disposing)
        {
            base.Dispose(disposing);
        }
        #region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            // 
            // GameForm
            // 
            this.ClientSize = new System.Drawing.Size(240, 320);
            this.MinimizeBox = false;
            this.Name = "GameForm";
            this.Load += new System.EventHandler(this.GameForLoad);

        }
        #endregion

        /// <summary>
        /// The main entry point for the application.
        /// </summary>

        static void Main()
        {
            Application.Run(new GameForm());
        }

        protected override void OnPaint(PaintEventArgs e)
        {
        }

        protected override void OnPaintBackground(PaintEventArgs e)
        {
        }

        protected override void OnResize(System.EventArgs e)
        {
            VerifySize();   
        }

        // Since the graphics in this app are only intended for display at
        // one size we need to make sure our window size is supported
        protected void VerifySize()
        {
            if(sizeLocked && (Size.Width != 240 || Size.Height != 320))
                throw new SizeNotSupportedException();
        }

        /// <summary>
        /// Create an instance of the game and run it.  When it finishes,
        /// close the form.
        /// </summary>
        private void GameForLoad(object sender, System.EventArgs e)
        {
            try
            {
                // Check for events
                Application.DoEvents();
                Show();


                // Create and run an instance of the game
                Game = new GameMain(this);
                sizeLocked = true;
                VerifySize();
                Game.Run();
            }
            catch (SizeNotSupportedException)
            {
                sizeLocked = false;
                MessageBox.Show("This sample does not support running in " +
                    "landscape mode or at resolutions other than 240 x 320");
            }
            catch (Exception ex)
            {
                MessageBox.Show(string.Format(CultureInfo.InvariantCulture,
                    "Fatal exception {0}\n",
                    ex.Message));
            }
            finally
            {
                // Clean up game resources
                if (Game != null)
                    Game.Dispose();

                Application.Exit();
            }
        }

        /// <summary>
        /// Make everything is shut down properly when closing the form.
        /// </summary>
        protected override void OnClosed(EventArgs e)
        {
            Game.Stop();
            base.OnClosed(e);
        }

        /// <summary>
        /// Close the form if the top right corner is clicked (PPC only)
        /// </summary>
        protected override void OnMouseDown(MouseEventArgs e)
        {
#if !(SMARTPHONE || MAINSTONE || DESKTOP) // PPC
            Rectangle rect = new Rectangle(this.Width - 45, 0, 45, 40);
            if (rect.Contains(e.X, e.Y)) {
                this.Close();
            }
#endif
            base.OnMouseDown(e);
        }
        
    }
}
