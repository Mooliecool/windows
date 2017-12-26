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
using System.IO;
using System.Reflection;
using System.Windows.Forms;
using System.Diagnostics;
using System.Drawing;
using System.Threading;
using System.Globalization;
using GraphicsLibrary;
using InputLibrary;
using Timer;

namespace GameApp
{
    /// <summary>
    /// Encapsulates loading and runnig of the game.  This class contains the
    /// game loop and initialization and is the top-level entry point to the
    /// game.
    /// </summary>
    public class GameMain : IDisposable
    {
        /// <summary>
        /// Minimum seconds that one frame can take i.e., the fastest
        /// framerate.
        /// </summary>
        private const float MinSecondsPerFrame = 1.0F / 50.0F;

        /// <summary>
        /// Maximum seconds that one frame can take i.e., the slowest
        /// framerate.
        /// </summary>
        private const float MaxSecondsPerFrame = 1.0F / 10.0F;

        /// <summary>
        /// The initial estimate of the framerate. This value will only be
        /// used until enough frames have been rendered that we can make
        /// reasonable estimates of the true framerate.
        /// </summary>
        private const float InitialSecondsPerFrame = 1.0F / 25.0F;

        /// <summary>
        /// Total length of time to count down after loading level,
        /// before level is playable.
        /// </summary>
        private const float TotalCountDownTime = 3.5F;

        /// <summary>
        /// Minimum length of time to display the splash screen.
        /// </summary>
        private const float TotalSplashTime = 2.0F;

        /// <summary>
        /// Current frame time in seconds.  This is provided as a static
        /// method accessible throughout the game because the rate can be
        /// variable.
        /// </summary>
        public static float SecondsPerFrame
        { get { return currentSecondsPerFrame; } }

        /// <summary>
        /// Current frame time in seconds.
        /// </summary>
        private static float currentSecondsPerFrame =
            InitialSecondsPerFrame;

        /// <summary>
        /// Specifies if the game is done.  When done, the game exits.
        /// </summary>
        private bool done = false;

        /// <summary>
        /// Graphics instance used by the game.
        /// </summary>
        private IGraphics graphics = null;

        /// <summary>
        /// Input instance used by the game.
        /// </summary>
        private Input gi = null;

        /// <summary>
        /// Stopwatch used by the game for timing the frames.
        /// </summary>
        private Timer.Stopwatch sw = null;

        /// <summary>
        /// Level that is currently loaded in the game.
        /// </summary>
        private Level level = null;

        /// <summary>
        /// User interface that is currently loaded in the game.
        /// </summary>
        private UserInterface ui = null;

        /// <summary>
        /// Intro data displayed before the current level.  This is only valid
        /// once loaded and until the level starts.
        /// </summary>
        private Intro intro = null;

        /// <summary>
        /// Defines the current update method.  This is determined by which
        /// state the game is in.
        /// </summary>
        private UpdateDelegate update = null;
        private delegate void UpdateDelegate();

        /// <summary>
        /// This enum is set by update delegates when the game needs to switch
        /// to a different update mode.  This is not done from within the
        /// update methods because problems can occur when delegate is
        /// modified from within a call to that delegate.
        /// </summary>
        private enum ModeSwitch
        {
            UpdateCountdown,
            UpdateLevel,
            UpdateIntro,
            None
        }
        private ModeSwitch mode = ModeSwitch.None;

        /// <summary>
        /// Splash screen image.  This is only valid while the splash
        /// screen is displayed at the start of the game.
        /// </summary>
        private IBitmap splash = null;

        /// <summary>
        /// Shared instance of the game's random number generator.
        /// </summary>
        private static Random rnd = null;

        /// <summary>
        /// Number of frames since the last auto-update check.
        /// </summary>
        public static Int64 CurrentFrame { get { return numFrames; } }
        private static Int64 numFrames = 0;

        /// <summary>
        /// The number of seconds which have passed for 'numFrames' 
        /// number of frames to have been rendered
        /// </summary>
        private float secondsElapsedForCurrentFrames;

        /// <summary>
        /// Specifies if the intro has finished loading.
        /// </summary>
        private bool introLoaded = false;

        /// <summary>
        /// Specifies if the level has finished loading.
        /// </summary>
        private bool levelLoaded = false;

        /// <summary>
        /// Total time left to countdown before starting the game.
        /// </summary>
        private float countDown = TotalCountDownTime;

        /// <summary>
        /// Rectangle cached for drawing routines to reduce memory
        /// allocations.
        /// </summary>
        Rectangle src = new Rectangle();

        /// <summary>
        /// Specifies if the countdown to start the level should begin.
        /// </summary>
        private bool startCountDown = false;

        /// <summary>
        /// Initializes the game libraries.
        /// </summary>
        /// <param name="owner">Control (Form) that owns the game</param>
        public GameMain(Control owner)
        {
            // Create a Graphics instance
#if USE_GDI
            graphics = new GdiGraphics(owner);
#else
            graphics = new DirectXGraphics(owner);
#endif
            Debug.Assert(graphics != null,
                "GameMain.GameMain: Failed to initialize Graphics object");

            // Create a Input instance
            gi = new Input(owner);
            Debug.Assert(gi != null,
                "GameMain.GameMain: Failed to initialize Input object");

            // Register the hardware buttons
            gi.RegisterAllHardwareKeys();

            // Initialize the random number generator
            rnd = new Random();

            // Create a stopwatch instance for timing the frames
            sw = new Timer.Stopwatch();
            Debug.Assert(sw != null,
                "GameMain.Run: Failed to initialize StopWatch");

        }

        /// <summary>
        /// Get a random float from 0-1.
        /// </summary>
        /// <returns>Random float from 0-1</returns>
        public static float Random()
        {
            return (float)rnd.NextDouble();
        }

        /// <summary>
        /// Get a random number in the specified range.
        /// </summary>
        /// <param name="min">Minimum number to return</param>
        /// <param name="max">Maximum number to return</param>
        /// <returns>Random int in range</returns>
        public static int Random(int min, int max)
        {
            return rnd.Next(min, max);
        }

        /// <summary>
        /// Get the full path to the specified file by prepending it
        /// with the directory of the executable.
        /// </summary>
        /// <param name="fileName">Name of file</param>
        /// <returns>Full path of the file</returns>
        public static string GetFullPath(string fileName)
        {
            Debug.Assert(fileName != null && fileName.Length > 0,
                "GameMain.GetFullPath: Invalid string");

            Assembly asm = Assembly.GetExecutingAssembly();
            string str = asm.GetName().CodeBase;
            string fullName = Path.GetDirectoryName(str);

            // the full name can be a URI (eg file://...) but some of the
            // loader functions can't parse that type of path. Hence we get
            // a path that starts with a drive letter.
            Uri uri = new Uri(Path.Combine(fullName, fileName));
            return uri.LocalPath;
        }

        /// <summary>
        /// Reset the current level.
        /// </summary>
        private void Reset()
        {
            // Reset the game
            level.ResetAll();

            // Clear any latent key presses
            gi.ClearKeyPresses();

            // Do one update of the level so it can be drawn
            level.Update(gi);

            startCountDown = false;
            update = new UpdateDelegate(UpdateCountdown);
            countDown = TotalCountDownTime;
            numFrames = 0;
            secondsElapsedForCurrentFrames = 0;

        }

        /// <summary>
        /// Start the game.  This method loads the game resources and
        /// runs the main game loop.
        /// </summary>
        public void Run()
        {

            // Load and validate the splash screen
            splash = graphics.CreateBitmap(GetFullPath(@"Data\Splash\splash.bmp"), false);
            Debug.Assert(splash != null,
                "GameMain.Run: Failed to initialized splash screen");
            Debug.Assert(splash.Width <= graphics.ScreenWidth &&
                splash.Height <= graphics.ScreenHeight,
                "GameMain.Run: Splash screen has invalid dimensions");

            // Load the game ui now because it has font information that is
            // needed for drawing the 'Loading...' tag
            DataSet dsUI = new DataSet();
            
            Debug.Assert(dsUI != null,
                "GameMain.LoadLevel: Failed to initialize UI DataSet");

            dsUI.Locale = CultureInfo.InvariantCulture;

            // Load the ui xml file
            dsUI.ReadXml(GetFullPath(@"Data\UI\ui.xml"));

            // Load the resources specified in the xml file
            ui = new UserInterface(dsUI, graphics, level);
            Debug.Assert(ui != null,
                "GameMain.LoadLevel: Failed to initialize UI");
            
            // Set the current update method as the splash screen updater
            update = new UpdateDelegate(UpdateSplash);

            // Loop until the game is done
            while (!done)
            {
                // Switch the update delegate if a switch was requested.
                switch (mode)
                {
                    case ModeSwitch.UpdateLevel:
                        gi.ClearKeyPresses();
                        update = new UpdateDelegate(UpdateLevel);
                        numFrames = 0;
                        secondsElapsedForCurrentFrames = 0;
                        break;
                    case ModeSwitch.UpdateCountdown:
                        intro.Dispose();
                        intro = null;
                        level.Update(gi);
                        update = new UpdateDelegate(UpdateCountdown);
                        break;
                    case ModeSwitch.UpdateIntro:
                        LoadLevel();
                        update = new UpdateDelegate(UpdateIntro);
                        splash.Dispose();
                        splash = null;
                        break;
                }

                mode = ModeSwitch.None;

                // Store the tick at which this frame started
                Int64 startTick = sw.CurrentTick();

                // Check if the user pressed the exit key
                if (gi.KeyPressed((int)gi.HardwareKeys[Player.ButtonMap()[
                    (int)Player.Buttons.Quit]]))
                {
                    done = true;
                }
                else if (levelLoaded && (level.Done || gi.KeyPressed(
                    (int)gi.HardwareKeys[Player.ButtonMap()[
                    (int)Player.Buttons.ResetLevel]])))
                {
                    Reset();
                    Application.DoEvents();
                    continue;
                }

                // Update the game
                update();

                // Check for pending events from the OS
                Application.DoEvents();

                // Lock the framerate...
                Int64 deltaMS = sw.DeltaTimeMilliseconds(sw.CurrentTick(), startTick);
                Int64 minMS = (Int64)(1000.0F * MinSecondsPerFrame);
                Int64 maxMS = (Int64)(1000.0F * MaxSecondsPerFrame);

                // Check if the frame time was fast enough
                if (deltaMS <= minMS)
                {
                    // Loop until the frame time is met
                    do
                    {
                        if (gi.KeyPressed((int)gi.HardwareKeys[
                            Player.ButtonMap()[(int)Player.Buttons.Quit]]))
                        {
                            done = true;
                            break;
                        }

                        // Thread.Sleep(0);
                        Application.DoEvents();
                        deltaMS = sw.DeltaTimeMilliseconds(sw.CurrentTick(),
                            startTick);

                    } while (deltaMS < minMS);
                }

                // Increment the number of frames
                numFrames++;
                // Increment the overall time for these frames
                if (deltaMS < maxMS)
                    secondsElapsedForCurrentFrames += deltaMS / 1000.0F;
                else
                    secondsElapsedForCurrentFrames += maxMS / 1000.0F;

                // Make sure enough time has elapsed since the last check
                if (level != null && secondsElapsedForCurrentFrames >
                    level.FrameRateCheckRate)
                {
                    currentSecondsPerFrame =
                        secondsElapsedForCurrentFrames / numFrames;
                    numFrames = 0;
                    secondsElapsedForCurrentFrames = 0;
                }
            }
        }

        /// <summary>
        /// Breaks the run loop and causes the game to exit
        /// </summary>
        public void Stop()
        {
            done = true;
        }

        /// <summary>
        /// Load the introduction to the level.
        /// </summary>
        public void LoadIntro()
        {
            DataSet dsIntro = new DataSet();

            Debug.Assert(dsIntro != null,
                "GameMain.LoadIntro: Failed to initialize intro DataSet");

            dsIntro.Locale = CultureInfo.InvariantCulture;

            // Load the intro xml file
            dsIntro.ReadXml(GetFullPath(@"Data\Intro\intro.xml"));

            // Load the intr resources specified in the xml file
            intro = new Intro(dsIntro, graphics);
            Debug.Assert(intro != null,
                "GameMain.LoadIntro: Failed to initialize Intro");

            // The intro is loaded at this point
            introLoaded = true;
        }

        /// <summary>
        /// Load the current level.  For this demo there is only one.
        /// </summary>
        public void LoadLevel()
        {
            DataSet dsLevel = new DataSet();
            Debug.Assert(dsLevel != null,
                "GameMain.LoadLevel: Failed to initialize level DataSet");

            dsLevel.Locale = CultureInfo.InvariantCulture;

            // Load the level xml file
            dsLevel.ReadXml(GetFullPath(@"Data\Level\level.xml"));

            // Load the level resources specified in the xml file
            level = new Level(dsLevel, graphics);
            Debug.Assert(level != null,
                "GameMain.LoadLevel: Failed to initialize level");

            // The level is loaded at this point
            levelLoaded = true;
        }

        /// <summary>
        /// Update the level.  This method is called during level gameplay.
        /// </summary>
        public void UpdateLevel()
        {
            // Update
            level.Update(gi);
            ui.Update(level);

            // Draw
            level.Draw();
            ui.Draw(graphics);

            // Flip the back buffer
            graphics.Flip();
        }

        /// <summary>
        /// Update the count down that is displayed before gameplay starts.
        /// </summary>
        public void UpdateCountdown()
        {
            if (startCountDown)
            {
                // Decrement the count
                countDown -= currentSecondsPerFrame;

                // Check if the count down is done
                if (countDown <= 0.0F)
                {
                    // Start updating the level
                    mode = ModeSwitch.UpdateLevel;
                    return;
                }
            }

            // Update UI
            ui.Update(level);

            // Draw
            level.Draw();
            ui.Draw(graphics);

            // Display the time left until the game starts. If the time is 
            // less than 1/2 second then flash
            if (startCountDown && (countDown > 0.5F ||
                (countDown <= 0.5F && (numFrames % 2 == 0))))
            {
                // Display "Start" for the last 1/2 second
                graphics.DrawText(graphics.ScreenWidth >> 1, 
                    graphics.ScreenHeight >> 1,
                    countDown > 0.5F ? string.Format(CultureInfo.InvariantCulture,
                    "{0}",
                    (int)(countDown + .49F)) : "Start",
                    Color.White, ui.Font, FontDrawOptions.DrawTextCenter);
            }

            if (!startCountDown)
            {
                int fireId = Player.ButtonMap()[(int)Player.Buttons.FireShot]
                    + 1;
                int quitId = Player.ButtonMap()[(int)Player.Buttons.Quit] + 1;
                int resetId = Player.ButtonMap()[
                    (int)Player.Buttons.ResetLevel] + 1;
                int x = graphics.ScreenWidth >> 1;
                int y = (graphics.ScreenHeight >> 1) - 60;

                graphics.DrawText(x, y, "<- -> to move player", Color.Yellow,
                    ui.Font, FontDrawOptions.DrawTextCenter);

#if (SMARTPHONE || MAINSTONE || DESKTOP)

                y += 20;
                graphics.DrawText(x, y, string.Format(
                    CultureInfo.InvariantCulture,
                    "Button {0} to fire",
                    fireId), Color.Yellow, ui.Font,
                    FontDrawOptions.DrawTextCenter);

                y += 20;
                graphics.DrawText(x, y, string.Format(
                    CultureInfo.InvariantCulture,
                    "Button {0} to Quit",
                    quitId), Color.Yellow, ui.Font,
                    FontDrawOptions.DrawTextCenter);

                y += 20;
                graphics.DrawText(x, y, string.Format(
                    CultureInfo.InvariantCulture,
                    "Button {0} to Reset",
                    resetId), Color.Yellow, ui.Font,
                    FontDrawOptions.DrawTextCenter);

#else // PPC

                y += 20;
                graphics.DrawText(x, y, 
                    "Action button to fire", 
                    Color.Yellow, ui.Font,
                    FontDrawOptions.DrawTextCenter);

                y += 20;
                graphics.DrawText(x, y, 
                    "Click the upper right", 
                    Color.Yellow, ui.Font,
                    FontDrawOptions.DrawTextCenter);

                y += 20;
                graphics.DrawText(x, y, 
                    "corner GMan to quit", 
                    Color.Yellow, ui.Font,
                    FontDrawOptions.DrawTextCenter);

#endif

                y += 20;
                graphics.DrawText(x, y, "Press any key to start", Color.Red,
                    ui.Font, FontDrawOptions.DrawTextCenter);

                if (gi.AnyKeyPressed())
                    startCountDown = true;
            }

            // Flip the back buffer
            graphics.Flip();
        }

        /// <summary>
        /// Update the intro that leads into the current level.
        /// </summary>
        public void UpdateIntro()
        {
            // If the level has finished loading and the intro is done
            // playing then start the count down
            if (levelLoaded && intro.Done)
            {
                mode = ModeSwitch.UpdateCountdown;
                return;
            }

            // Update the intro
            intro.Update(graphics);

            // Draw the intro
            intro.Draw(graphics);

            // Flip the back buffers
            graphics.Flip();
        }

        /// <summary>
        /// Update the splash screen.
        /// </summary>
        public void UpdateSplash()
        {

            // Calculate the upper-left corner of the splash screen image
            int x = (graphics.ScreenWidth - splash.Width) >> 1;
            int y = (graphics.ScreenHeight - splash.Height) >> 1;

            // The source region is the entire image
            src.X = 0;
            src.Y = 0;
            src.Width = splash.Width;
            src.Height = splash.Height;

            // Draw the splash screen
            graphics.DrawBitmap(x, y, src, splash);

            // Flip the back buffers
            graphics.Flip();

            // Store the tick at which this frame started
            Timer.Stopwatch sw = new Timer.Stopwatch();
            Int64 startTick = sw.CurrentTick();

            // Draw the splash screen
            graphics.DrawBitmap(x, y, src, splash);

            // Draw a "Loading" message as this might take a while
            graphics.DrawText(graphics.ScreenWidth >> 1, 
                graphics.ScreenHeight - 50,
                "Loading...",
                Color.White, ui.Font, FontDrawOptions.DrawTextCenter);

            // Flip the back buffers
            graphics.Flip();

            if (!introLoaded) LoadIntro();

            // delay if the splash screen hasn't been up long enough
            while (sw.DeltaTimeMilliseconds(sw.CurrentTick(), startTick)
                < TotalSplashTime * 1000.0F)
                Thread.Sleep(0);

            mode = ModeSwitch.UpdateIntro;
        }

        /// <summary>
        /// Clean up the game's resources.
        /// </summary>
        public void Dispose()
        {
            if (intro != null)
                intro.Dispose();

            if (ui != null)
                ui.Dispose();

            if (level != null)
                level.Dispose();

            if (graphics != null)
                graphics.Dispose();
        }
    }
}
