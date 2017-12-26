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
using System.Drawing;
using System.Collections;
using System.Diagnostics;
using System.Globalization;
using GraphicsLibrary;
using InputLibrary;

namespace GameApp
{
    /// <summary>
    /// Defines a level in the game and all of its contents.
    /// </summary>
    public class Level : IDisposable
    {
        /// <summary>
        /// Gets the list of AI definitions for the level.
        /// </summary>
        public ArrayList AIList { get { return ai; } }
        private ArrayList ai = new ArrayList();

        /// <summary>
        /// Gets the list of world objects in the level.
        /// </summary>
        public ArrayList WorldObjects { get { return worldObjectsValue; } }
        private ArrayList worldObjectsValue = new ArrayList();

        /// <summary>
        /// List of layers in the level.
        /// </summary>
        private ArrayList layers = new ArrayList();

        /// <summary>
        /// List of images used by layers in the level.
        /// </summary>
        private ArrayList images = new ArrayList();

        /// <summary>
        /// Gets the list of animation definitions used in the level.
        /// </summary>
        public ArrayList AnimationList { get { return animations; } }
        private ArrayList animations = new ArrayList();

        /// <summary>
        /// Graphics instance used by the game.
        /// </summary>
        private IGraphics graphics = null;

        /// <summary>
        /// Width of the screen portion into which the level is drawn.
        /// </summary>
        public int ViewWidth { get { return graphics.ScreenWidth; } }

        /// <summary>
        /// Gets the buffer area on the left and right screen edges where the
        /// player is not allowed to travel.
        /// </summary>
        public float ScreenEdgeBuffer { get { return screenEdgeBufferValue; } }
        private float screenEdgeBufferValue = 0;

        /// <summary>
        /// Gets the player from the world object list.
        /// </summary>
        public Player Player { get { return (Player)worldObjectsValue[0]; } }

        /// <summary>
        /// Gets the foreground layer.  The foreground is actually the closer
        /// of the backgrounds but is still behind the player and world
        /// objects.
        /// </summary>
        public Layer Foreground { get { return ((Layer)layers[1]); } }

        /// <summary>
        /// Gets the background layer.
        /// </summary>
        public Layer Background { get { return ((Layer)layers[0]); } }

        /// <summary>
        /// Rate at which the layer of the world that the player is on is
        /// scrolling (pixels/second).
        /// </summary>
        public float ScrollRate { get { return Foreground.ScrollRate; } }

        /// <summary>
        /// Maximum Y world location. 
        /// </summary>
        public float MaxWorldY
        {
            get
            {
                return (Foreground.ScreenY - Background.ScreenY) +
                    Foreground.Height;
            }
        }

        /// <summary>
        /// X location of the screen in the world.
        /// </summary>
        public float WorldX { get { return Foreground.WorldX; } }

        /// <summary>
        /// Y location of the screen in the world.
        /// </summary>
        public float WorldY { get { return 0.0F; } }

        /// <summary>
        /// X screen location of the world
        /// </summary>
        public float DrawX { get { return 0.0F; } }

        /// <summary>
        /// Y screen location of the world.
        /// </summary>
        public float DrawY { get { return (float)Background.ScreenY; } }

        /// <summary>
        /// Rate of gravity in pixels/second.
        /// </summary>
        public float Gravity { get { return gravityValue; } }
        private float gravityValue;

        /// <summary>
        /// Rate, in seconds, to check the framerate.
        /// </summary>
        public float FrameRateCheckRate
        {
            get { return FrameRateCheckRateValue; }
        }

        private float FrameRateCheckRateValue = 1.0F;

        /// <summary>
        /// Gets if the level is done and waiting for a reset.
        /// </summary>
        public bool Done { get { return Player.Dead && Player.Animation.Done; } }

        /// <summary>
        /// Initialize the level.
        /// </summary>
        /// <param name="ds">DataSet containing level data</param>
        /// <param name="graphics">Valid Graphics object</param>
        public Level(DataSet ds, IGraphics graphics)
        {
            // Store graphics
            Debug.Assert(graphics != null,
                "Level.Level: Invalid Graphics object");

            this.graphics = graphics;

            // Validate the DataSet
            Debug.Assert(ds != null && ds.Tables != null,
                "Level.Level: Invalid DataSet");

            // General
            DataTable dt = ds.Tables["General"];
            DataRow drGen = dt.Rows[0];

            screenEdgeBufferValue = float.Parse(
                (string)(drGen["ScreenEdgeBuffer"]), 
                CultureInfo.InvariantCulture);
            FrameRateCheckRateValue = float.Parse(
                (string)(drGen["FrameRateCheckRate"]), 
                CultureInfo.InvariantCulture);
            gravityValue = float.Parse((string)(drGen["Gravity"]), 
                CultureInfo.InvariantCulture);

            // Images
            Debug.Assert(ds.Tables["Image"] != null &&
                ds.Tables["Image"].Rows != null,
                "Level.Level: No images specified in level data");

            dt = ds.Tables["Image"];
            foreach (DataRow dr in dt.Rows)
            {
                IBitmap bmp = graphics.CreateBitmap(GameMain.GetFullPath(
                    @"Data\Level\" + (string)dr["FileName"]),
                    bool.Parse((string)dr["Transparency"]));
                Debug.Assert(bmp != null,
                    string.Format(CultureInfo.InvariantCulture,
                    "Failed to initialize bitmap {0}",
                    @"Data\Level\" + (string)dr["FileName"]));

                images.Add(bmp);
            }

            // Layers
            dt = ds.Tables["Layer"];
            foreach (DataRow dr in dt.Rows)
            {
                layers.Add(new Layer(dr, images));
            }

            Debug.Assert(layers.Count >= 1,
                "Level does not contain 2 or more layers");

            ds = null;

            // AI
            DataSet dsAI = new DataSet();
            Debug.Assert(dsAI != null && dsAI.Tables != null,
                "Level.Level: Failed to initialize AI DataSet");

            dsAI.Locale = CultureInfo.InvariantCulture;
            dsAI.ReadXml(GameMain.GetFullPath(@"Data\AI\ai.xml"));
            dt = dsAI.Tables["Definition"];
            Debug.Assert(dt != null && dt.Rows != null,
                "Level.Level: Failed to load AI DataTable");

            foreach (DataRow dr in dt.Rows)
            {
                AI ai = AIHandler.Create(dr);
                Debug.Assert(ai != null,
                    "Level.Level: Failed to initialize AI");
                this.ai.Add(ai);
            }
            dsAI = null;

            DataSet dsAnimations = new DataSet();
            Debug.Assert(dsAnimations != null && dsAnimations.Tables != null,
                "Level.Level: Failed to initialize animation DataSet");

            dsAnimations.Locale = CultureInfo.InvariantCulture;

            // Animations
            dsAnimations.ReadXml(GameMain.GetFullPath(@"Data\Animations\animations.xml"));
            dt = dsAnimations.Tables["Definition"];
            Debug.Assert(dt != null && dt.Rows != null,
                "Level.Level: Failed to load animation DataTable");

            foreach (DataRow dr in dt.Rows)
            {
                int numberRows = int.Parse((string)dr["Rows"], 
                    CultureInfo.InvariantCulture);
                int numberColumns = int.Parse((string)dr["Columns"], 
                    CultureInfo.InvariantCulture);
                int cellWidth = int.Parse((string)dr["CellWidth"], 
                    CultureInfo.InvariantCulture);
                int cellHeight = int.Parse((string)dr["CellHeight"], 
                    CultureInfo.InvariantCulture);

                Animation animation = new Animation(GameMain.GetFullPath(
                    @"Data\Animations\" + (string)dr["FileName"]),
                    graphics, numberRows, numberColumns, 0, cellWidth, 
                    cellHeight, 0);
                Debug.Assert(animation != null && animation.Init,
                    "Level.Level: Failed to load animation");

                animations.Add(animation);
            }
            dsAnimations = null;

            // Player
            DataSet dsPlayer = new DataSet();
            Debug.Assert(dsPlayer != null,
                "Level.Level: Failed to initialize player DataSet");

            dsPlayer.Locale = CultureInfo.InvariantCulture;
            dsPlayer.ReadXml(GameMain.GetFullPath(@"Data\Player\player.xml"));
            Player p = new Player(dsPlayer, graphics, animations);
            Debug.Assert(p != null,
                "Level.Level: Failed to initialize player");
            worldObjectsValue.Add(p);
            dsPlayer = null;

            // World Objects
            DataSet dsWorldObjects = new DataSet();
            Debug.Assert(dsWorldObjects != null && dsWorldObjects.Tables !=
                null,
                "Level.Level: Failed to initialize world object DataSet");

            dsWorldObjects.Locale = CultureInfo.InvariantCulture;
            dsWorldObjects.ReadXml(GameMain.GetFullPath(@"Data\WorldObjects\worldobjects.xml"));
            dt = dsWorldObjects.Tables["Instance"];
            if (dt != null)
            {
                foreach (DataRow dr in dt.Rows)
                {
                    WorldObject wo = new WorldObject(dr, this);
                    Debug.Assert(wo != null,
                        "Level.Level: Failed to initialize world object");

                    worldObjectsValue.Add(wo);
                }
            }
        }

        /// <summary>
        /// Update the level.
        /// </summary>
        /// <param name="gi">Input instance</param>
        public void Update(Input gi)
        {
            // Assume that the level is not stopped
            bool bStopAll = false;
            foreach (Layer l in layers)
            {
                l.Update(graphics);

                // If any layer gets to the end of the world then stop
                if (l.ScrollRate == 0.0F)
                    bStopAll = true;
            }

            // Update world objects in the level
            for (int i = 0; i < worldObjectsValue.Count; i++)
            {
                // If Update returns true then the object is dead
                if (((WorldObject)worldObjectsValue[i]).Update(gi, this))
                {
                    worldObjectsValue.RemoveAt(i);
                    i--;
                }
            }

            // Check collisions between world objects
            CheckCollisions();

            // Check if any objects have been killed by collision
            for (int i = 0; i < worldObjectsValue.Count; i++)
            {
                if (((WorldObject)worldObjectsValue[i]).RemoveMe)
                {
                    worldObjectsValue.RemoveAt(i);
                    i--;
                }
            }

            // If the level is done or the player is dead then stop
            if (bStopAll || Player.Dead)
            {
                foreach (Layer l in layers)
                {
                    l.ScrollRate = 0.0F;

                    // If the player is dead then make sure he did not
                    // fall out of the level
                    if (Player.Dead)
                        l.MoveForPlayer(Player);
                }
            }
        }

        /// <summary>
        /// Check for collisions between world objects.  Each world object
        /// has a set of bounds that are defined by the animation that is
        /// currently playing.
        /// </summary>
        protected void CheckCollisions()
        {
            // Cycle through each world object
            for (int i = 0; i < worldObjectsValue.Count; i++)
            {
                // Cache the object
                WorldObject wo1 = (WorldObject)worldObjectsValue[i];

                // Make sure the object can be checked
                if (!wo1.Active || !wo1.Collidable ||
                    wo1.DrawX(this) > DrawX + ViewWidth)
                    continue;

                Debug.Assert(wo1.Bounds != null,
                    "Level.Update: Invalid bounds on world object 1");

                // Check every subsequent object in the list for collisions.
                // All previous objects will have been checked already.
                for (int j = i + 1; j < worldObjectsValue.Count; j++)
                {
                    // Cache the object
                    WorldObject wo2 = (WorldObject)worldObjectsValue[j];
                    Debug.Assert(wo2 != null &&
                        ((wo2.Active && wo2.Collidable) ?
                        wo2.Bounds != null : true),
                        "Level.Update: Invalid bounds on world object 2");

                    if (wo2.DrawX(this) > DrawX + ViewWidth)
                        continue;

                    if (wo2.Active && wo2.Collidable &&
                        !(wo1.Dynamic && wo2.Dynamic) &&
                        wo1.Bounds.CheckCollision(wo1.WorldX, wo1.WorldY,
                        wo2.Bounds, wo2.WorldX, wo2.WorldY))
                    {
                        // If the collided and are neither one owns the
                        // other then they both die here
                        if ((wo1.Parent != wo2 && wo2.Parent != wo1) ||
                            (wo1.Parent == Player && wo1.VelocityY > 0.0F) ||
                            (wo2.Parent == Player && wo2.VelocityY > 0.0F))
                        {
                            wo1.Die();
                            wo2.Die();
                        }
                    }
                }
            }
        }

        /// <summary>
        /// Reset the level for replaying.
        /// </summary>
        public void ResetAll()
        {
            // Reset each layer
            foreach (Layer l in layers)
            {
                l.Reset();
            }

            // Remove any dynamic objects from the level
            for (int i = 0; i < worldObjectsValue.Count; i++)
            {
                if (((WorldObject)worldObjectsValue[i]).Dynamic)
                {
                    worldObjectsValue.RemoveAt(i);
                    i--;
                }
            }

            // Reset all world objects
            foreach (WorldObject wo in worldObjectsValue)
            {
                wo.Reset(this);
            }
        }

        /// <summary>
        /// Draw the level and all of its objects.
        /// </summary>
        public void Draw()
        {
            // Draw the layers
            for (int i = 0; i < 2; i++)
            {
                ((Layer)layers[i]).Draw(graphics, images);
            }

            // Draw the world objects
            foreach (WorldObject wo in worldObjectsValue)
            {
                wo.Draw(graphics, this);
            }

            // Draw any further layers in the foreground of the player
            for (int i = 2; i < layers.Count; i++)
            {
                ((Layer)layers[i]).Draw(graphics, images);
            }
        }

        /// <summary>
        /// Free any resources allocated by the level
        /// </summary>
        public void Dispose()
        {
            foreach (IBitmap bmp in images)
            {
                bmp.Dispose();
            }

            foreach (Animation animation in animations)
            {
                animation.Dispose();
            }
        }
    }
}
