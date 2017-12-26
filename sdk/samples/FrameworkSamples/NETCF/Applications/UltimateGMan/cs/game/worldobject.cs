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
using System.Collections;
using System.Diagnostics;
using System.Data;
using System.Globalization;
using GraphicsLibrary;
using InputLibrary;

namespace GameApp
{
    /// <summary>
    /// Defines an object within the game world. World objects range
    /// from the player and enemies, to static objects like mailboxes.
    /// </summary>
    public class WorldObject
    {
        /// <summary>
        /// Stores the original collidable flag.
        /// </summary>
        protected bool ResetCollidable
        {
            get { return resetCollidableValue; }
            set { resetCollidableValue = value; }
        }
        bool resetCollidableValue;

        /// <summary>
        /// Stores the original world x location.
        /// </summary>
        protected float ResetWorldX
        {
            get { return resetWorldXValue; }
            set { resetWorldXValue = value; }
        }
        float resetWorldXValue;

        /// <summary>
        /// Stores the original world y location.
        /// </summary>
        protected float ResetWorldY
        {
            get { return resetWorldYValue; }
            set { resetWorldYValue = value; }
        }
        float resetWorldYValue;

        /// <summary>
        /// Defines the world object's state.
        /// </summary>
        public enum AnimationState
        {
            None = 0x0000,
            Stop = 0x0001,
            Right = 0x0002,
            Left = 0x0004,
            ForceRight = 0x0008,
            Dead = 0x0010,
            DirectionMask = 0x00ff,

            Walk = 0x0100,
            Duck = 0x0200,
            Attack = 0x0400,
            Static = 0x1000,
            AnimationMask = 0xff00
        }

        /// <summary>
        /// Current stat of the world object.
        /// </summary>
        public AnimationState State 
        {
            get { return curState; }
            set { curState = value; }
        }
        AnimationState curState = AnimationState.None;

        /// <summary>
        /// Get the shot info for this object.
        /// </summary>
        protected ShotInfo[] GetShotInfo()
        {
            return shotInfoValue;
        }

        /// <summary>
        /// Set the shot info for this object
        /// </summary>
        /// <param name="shotInfo"></param>
        protected void SetShotInfo(ShotInfo[] shotInfo)
        {
            shotInfoValue = shotInfo;
        }

        ShotInfo[] shotInfoValue = null;

        /// <summary>
        /// Bounds used by static objects - never changes.
        /// </summary>
        Bounds staticBounds = null;

        /// <summary>
        /// Defines acknowledged directions within the world.
        /// </summary>
        protected enum WorldDir
        {
            Left,
            Right,
            Up,
            Down
        }

        /// <summary>
        /// Types of animations the object can be playing.
        /// </summary>
        public enum AnimationType
        {
            Walk = 0,
            Attack,
            Duck,
            Death,
            Count
        }

        /// <summary>
        /// Owner of this object.  This is used to differentiate shots
        /// fired from an object from shots fired by other objects.
        /// </summary>
        public WorldObject Parent { get { return parentValue; } }
        WorldObject parentValue = null;

        /// <summary>
        /// Specifies if the world object is dead.
        /// </summary>
        public bool Dead
        {
            get
            {
                return (AnimationState.Dead & curState) ==
                    AnimationState.Dead;
            }
        }

        /// <summary>
        /// Get the world object's current bounds.  The bounds will vary
        /// depending on the current animation state.
        /// </summary>
        public Bounds Bounds
        {
            get
            {
                if ((curState & AnimationState.Static) != 0)
                    return staticBounds;

                if ((curState & AnimationState.Attack) != 0)
                    return AttackCycle.Bounds;

                if ((curState & AnimationState.Duck) != 0)
                    return DuckCycle.Bounds;

                return WalkCycle.Bounds;
            }
        }

        /// <summary>
        /// List of animation cycles available to the world object.
        /// </summary>
        protected AnimationCycle GetAnimationCycle(int index)
        {
           return animationCycles[index];
        }

        /// <summary>
        /// Sets one of the animation cycles available to a world object
        /// </summary>
        /// <param name="index">The index of the animation cycle</param>
        /// <param name="cycle">The animation cycle to store</param>
        protected void SetAnimationCycle(int index, AnimationCycle cycle)
        {
            animationCycles[index] = cycle;
        }

        AnimationCycle[] animationCycles =
            new AnimationCycle[(int)AnimationType.Count];

        /// <summary>
        /// Gets the walk animation cycle.
        /// </summary>
        protected AnimationCycle WalkCycle
        {
            get { return GetAnimationCycle((int)AnimationType.Walk); }
        }

        /// <summary>
        /// Gets the attack animation cycle.
        /// </summary>
        protected AnimationCycle AttackCycle
        {
            get { return GetAnimationCycle((int)AnimationType.Attack); }
        }

        /// <summary>
        /// Gets the duck animation cycle.
        /// </summary>
        protected AnimationCycle DuckCycle
        {
            get { return GetAnimationCycle((int)AnimationType.Duck); }
        }

        /// <summary>
        /// Gets the death animation cycle.
        /// </summary>
        protected AnimationCycle DeathCycle
        {
            get { return GetAnimationCycle((int)AnimationType.Death); }
        }

        /// <summary>
        /// Specifies if this world object is the player.
        /// </summary>
        protected bool Player 
        { 
            get { return playerValue; }
            set { playerValue = value; }
        }
        bool playerValue = false;

        /// <summary>
        /// Activation distance of this object. The object will not become
        /// active, meaning that it will not be processed, until the player
        /// comes within this distance of the object.
        /// </summary>
        float activateDistance = 0.0F;

        /// <summary>
        /// AI instance that controls this object.
        /// </summary>
        AI AILogic = null;


        /// <summary>
        /// Specifies if this object is active.  The object is not processed
        /// or drawn unless it is active.
        /// </summary>
        public bool Active 
        {
            get { return activeValue; }
            set { activeValue = value; }
        }
        bool activeValue = false;

        /// <summary>
        /// Specifies if this object is dynamic.  Dynamic objects are created
        /// programmatically and not defined in the level data file.
        /// </summary>
        public bool Dynamic { get { return dynamicValue; } }
        bool dynamicValue = false;

        /// <summary>
        /// Specifies if the world object should be removed from the level's
        /// list of objects and disposed.
        /// </summary>
        public bool RemoveMe { get { return removeMeValue; } }
        bool removeMeValue = false;

        /// <summary>
        /// Specifies if this object is collidable.  If not then it will be
        /// ignored when performing collision checks with other objects.
        /// </summary>
        public bool Collidable 
        {
            get { return collidableValue; }
            set { collidableValue = value; }
        }
        bool collidableValue = false;

        /// <summary>
        /// X location within the world.  Do not confuse this with the screen
        /// location of the object.
        /// </summary>
        public float WorldX
        {
            get { return worldXValue; }
            set { worldXValue = value; }
        }
        float worldXValue = 0.0F;

        /// <summary>
        /// Y location within the world.  Do not confuse this with the screen
        /// location of the object.
        /// </summary>
        public float WorldY
        {
            get { return worldYValue; }
            set { worldYValue = value; }
        }
        float worldYValue = 0.0F;

        /// <summary>
        /// Velocity in the x direction, at which the object is traveling
        /// through the world.
        /// </summary>
        public float VelocityX
        {
            get { return velocityXValue; }
            set { velocityXValue = value; }
        }
        float velocityXValue = 0.0F;

        /// <summary>
        /// Velocity in the y direction, at which the object is traveling
        /// through the world.
        /// </summary>
        public float VelocityY
        {
            get { return velocityYValue; }
            set { velocityYValue = value; }
        }
        float velocityYValue = 0.0F;

        /// <summary>
        /// Draw width of this object.
        /// </summary>
        public int Width { get { return (animationValue.CellWidth); } }

        /// <summary>
        /// Draw options for this object.  These options are set before drawing
        /// this object.
        /// </summary>
        protected DrawOptions DrawOptions
        {
            get { return drawOptionsValue; }
            set { drawOptionsValue = value; }
        }
        DrawOptions drawOptionsValue = 0;

        /// <summary>
        /// Animation instance which represents this object.
        /// </summary>
        public Animation Animation 
        {
            get { return animationValue; }
            set { animationValue = value; }
        }
        Animation animationValue = null;

        /// <summary>
        /// Create an empty instance of a world object.  This overload is
        /// provided for creating a player object.
        /// </summary>
        protected WorldObject()
        {
        }

        /// <summary>
        /// Initialize a world object with the data specified in the given
        /// DataRow.
        /// </summary>
        /// <param name="dr">DataRow defining the world object</param>
        /// <param name="lev">Current level</param>
        public WorldObject(DataRow dr, Level lev)
        {
            Debug.Assert(lev.AnimationList != null && lev.AnimationList.Count > 0,
                "WorldObject.WorldObject: No animations loaded");

            Debug.Assert(lev.AIList != null && lev.AIList.Count > 0,
                "WorldObject.WorldObject: No AI loaded");

            for (int i = 0; i < (int)AnimationType.Count; i++)
            {
                animationCycles[i] = new AnimationCycle();
                Debug.Assert(animationCycles[i] != null,
                    "WorldObject.WorldObject: Failed to allocate " +
                    "animation cycle");
            }

            // Walk
            WalkCycle.AnimationRate = int.Parse((string)dr["WalkAnimationRate"], 
            CultureInfo.InvariantCulture);
            WalkCycle.StartCell = int.Parse((string)dr["WalkStartCell"], 
                CultureInfo.InvariantCulture);
            WalkCycle.EndCell = int.Parse((string)dr["WalkEndCell"], 
                CultureInfo.InvariantCulture);
            float x = float.Parse((string)dr["WalkBoundsX"], 
                CultureInfo.InvariantCulture);
            float y = float.Parse((string)dr["WalkBoundsY"], 
                CultureInfo.InvariantCulture);
            float r = float.Parse((string)dr["WalkBoundsRadius"], 
                CultureInfo.InvariantCulture);
            WalkCycle.Bounds = new Bounds(x, y, r);

            // Attack
            AttackCycle.AnimationRate = int.Parse(
                (string)dr["AttackAnimationRate"], 
                CultureInfo.InvariantCulture);
            AttackCycle.StartCell = int.Parse((string)dr["AttackStartCell"], 
                CultureInfo.InvariantCulture);
            AttackCycle.EndCell = int.Parse((string)dr["AttackEndCell"], 
                CultureInfo.InvariantCulture);
            x = float.Parse((string)dr["AttackBoundsX"], 
                CultureInfo.InvariantCulture);
            y = float.Parse((string)dr["AttackBoundsY"], 
                CultureInfo.InvariantCulture);
            r = float.Parse((string)dr["AttackBoundsRadius"], 
                CultureInfo.InvariantCulture);
            AttackCycle.Bounds = new Bounds(x, y, r);

            // Duck
            DuckCycle.AnimationRate = int.Parse(
                (string)dr["DuckAnimationRate"], 
                CultureInfo.InvariantCulture);
            DuckCycle.StartCell = int.Parse((string)dr["DuckStartCell"], 
                CultureInfo.InvariantCulture);
            DuckCycle.EndCell = int.Parse((string)dr["DuckEndCell"], 
                CultureInfo.InvariantCulture);
            x = float.Parse((string)dr["DuckBoundsX"], 
                CultureInfo.InvariantCulture);
            y = float.Parse((string)dr["DuckBoundsY"], 
                CultureInfo.InvariantCulture);
            r = float.Parse((string)dr["DuckBoundsRadius"], 
                CultureInfo.InvariantCulture);
            DuckCycle.Bounds = new Bounds(x, y, r);

            // Death
            DeathCycle.AnimationRate = int.Parse(
                (string)dr["DeathAnimationRate"], 
                CultureInfo.InvariantCulture);
            DeathCycle.StartCell = int.Parse((string)dr["DeathStartCell"], 
                CultureInfo.InvariantCulture);
            DeathCycle.EndCell = int.Parse((string)dr["DeathEndCell"], 
                CultureInfo.InvariantCulture);

            // General info
            worldXValue = float.Parse((string)dr["X"], 
            CultureInfo.InvariantCulture);
            worldYValue = float.Parse((string)dr["Y"], 
                CultureInfo.InvariantCulture);

            activateDistance = float.Parse((string)dr["ActivateDistance"], 
                CultureInfo.InvariantCulture);
            collidableValue = bool.Parse((string)dr["Collidable"]);

            // Animation
            int animationId = int.Parse((string)dr["AnimationId"], 
                CultureInfo.InvariantCulture);
            Debug.Assert(animationId >= 0 && 
                animationId < lev.AnimationList.Count,
                "WorldObject.WorldObject: Animatin ID out of range");

            animationValue = new Animation(
                (Animation)lev.AnimationList[animationId], 0, 0);
            Debug.Assert(animationValue != null && animationValue.Init,
                "WorldObject.WorldObject: Failed to initialize animation");

            // AI
            int aiId = int.Parse((string)dr["AIId"], 
                CultureInfo.InvariantCulture);
            if (aiId >= 0)
            {
                Debug.Assert(aiId < lev.AIList.Count,
                    "WorldObject.WorldObject: AI Id out of range");
                AILogic = AIHandler.Create((AI)lev.AIList[aiId]);
                AILogic.Init(this, lev);
            }

            // Draw options
            if (bool.Parse((string)dr["Transparency"]))
            {
                DrawOptions = DrawOptions.BlitKeyedTransparency;
            }

            // Shot, if one exists
            int numShots = 0;
            if (!dr.IsNull("Shot2AnimationId"))
                numShots = 2;
            else if (!dr.IsNull("Shot1AnimationId"))
                numShots = 1;

            if (numShots > 0)
            {
                shotInfoValue = new ShotInfo[numShots];

                for (int i = 0; i < shotInfoValue.Length; i++)
                {
                    shotInfoValue[i] = new ShotInfo(dr, i + 1);
                    ShotInfo shot = shotInfoValue[i];
                    Debug.Assert(shot != null,
                        "WorldObject.WorldObject: Failed to allocate " +
                        "shot info");
                }
            }

            ResetCollidable = collidableValue;
            ResetWorldX = worldXValue;
            ResetWorldY = worldYValue;
        }

        /// <summary>
        /// Initialize a world object based on the given animation. This
        /// method is provided for creating dynamic objects that are not
        /// defined in the level data file.
        /// </summary>
        /// <param name="animation">Source animation</param>
        /// <param name="animRate">Animation rate</param>
        public WorldObject(Animation animation, int animationRate)
        {
            dynamicValue = true;
            activeValue = true;
            collidableValue = true;
            curState = AnimationState.Static;

            this.animationValue = new Animation(animation, 0, animationRate);
        }

        /// <summary>
        /// Called when the level is reset, this method resets the world
        /// object.
        /// </summary>
        /// <param name="lev">Current level</param>
        virtual public void Reset(Level lev)
        {
            if (AILogic != null)
            {
                AILogic.Reset();
                AILogic.Init(this, lev);
            }

            if (animationValue != null)
                animationValue.SetCycle(0, animationValue.NumberCells - 1, 0);

            activeValue = false;
            collidableValue = ResetCollidable;
            worldXValue = ResetWorldX;
            worldYValue = ResetWorldY;
            velocityXValue = 0.0F;
            velocityYValue = 0.0F;

            curState = AnimationState.None;

            if (GetShotInfo() != null)
            {
                foreach (ShotInfo si in GetShotInfo())
                {
                    si.Reset();
                }
            }

            DrawOptions &= ~DrawOptions.BlitMirrorLeftRight;
        }

        /// <summary>
        /// Kill this object.  If a death animation exists then it is
        /// initiated.
        /// </summary>
        public void Die()
        {
            collidableValue = false;

            if (dynamicValue)
            {
                activeValue = false;
                removeMeValue = true;
            }
            else
            {
                animationValue.SetCycle(DeathCycle.EndCell, DeathCycle.EndCell, 0);
                animationValue.StartOneShot(DeathCycle.StartCell,
                    DeathCycle.EndCell, DeathCycle.AnimationRate);
                curState |= AnimationState.Dead;
                collidableValue = false;
            }
        }

        /// <summary>
        /// Set or clear the transparency data for this object.
        /// </summary>
        /// <param name="set">Set transparency if true, otherwise clear
        /// it</param>
        public void SetTransparency(bool set)
        {
            if (set)
            {
                DrawOptions |= DrawOptions.BlitKeyedTransparency;
            }
            else
            {
                DrawOptions &= ~DrawOptions.BlitKeyedTransparency;
            }
        }

        /// <summary>
        /// Update this world object.
        /// </summary>
        /// <param name="gi">Input instance</param>
        /// <param name="lev">Current level</param>
        /// <returns>true if the object should be removed, false otherwise
        /// </returns>
        virtual public bool Update(Input gi, Level lev)
        {
            // If the object is not active then check the activation distance
            if (!activeValue)
            {
                if (Math.Abs(lev.Player.WorldX - worldXValue) <=
                    activateDistance)
                    activeValue = true;
                else
                    return false;
            }

            // If the object is in a death animation then check if it has
            // finished
            if ((curState & AnimationState.Dead) != 0)
            {
                if (animationValue.Done)
                {
                    activeValue = false;
                }

                animationValue.Update(GameMain.SecondsPerFrame);

                return false;
            }

            // If the object is attacking then check the shot data
            if ((curState & AnimationState.Attack) != 0)
            {
                // If the object has a shot...
                if (GetShotInfo() != null)
                {
                    // Check all shots...
                    foreach (ShotInfo shot in GetShotInfo())
                    {
                        // If the shot is not already fired and the animation
                        // is in the proper shooting cell then fire it
                        if (!shot.Fired && animationValue.CurCell == shot.ShootCell)
                        {
                            WorldDir dir = WorldDir.Left;
                            if ((DrawOptions &
                                DrawOptions.BlitMirrorLeftRight) != 0)
                                dir = WorldDir.Right;

                            FireShot(lev, shot, dir, 1.0F, 1.0F);
                        }

                        // If the animation is done the reset it
                        if (animationValue.Done)
                        {
                            shot.Fired = false;
                            Stand();
                        }
                    }
                }
            }

            // If the object has AI then update it
            if (AILogic != null)
                AILogic.Update(this, lev);

            // Update the object's world position
            worldXValue += GameMain.SecondsPerFrame * velocityXValue;
            worldYValue += GameMain.SecondsPerFrame * velocityYValue;

            // If the object is dynamic then check if it is off-screen
            if (dynamicValue)
            {
                if (worldXValue > lev.Player.WorldX + 1.5F * lev.ViewWidth ||
                    worldXValue < lev.WorldX - 1.5F * lev.ViewWidth)
                {
                    return true;
                }
            }

            // Update the animation
            animationValue.Update(GameMain.SecondsPerFrame);

            // Do not remove this object
            return false;
        }

        /// <summary>
        /// Returns the object's draw location relative to the screen.
        /// </summary>
        /// <param name="lev">Current level</param>
        /// <returns>X coordinate of the draw</returns>
        public float DrawX(Level lev)
        {
            return (int)lev.DrawX + (int)worldXValue -
                (int)lev.WorldX - (animationValue.CellWidth >> 1);
        }

        /// <summary>
        /// Returns the object's draw location relative to the screen.
        /// </summary>
        /// <param name="lev">Current level</param>
        /// <returns>Y coordinate of the draw</returns>
        public float DrawY(Level lev)
        {
            return (int)lev.DrawY + (int)worldYValue -
                (int)lev.WorldY - (animationValue.CellHeight >> 1);
        }

        /// <summary>
        /// Draw the world object on the back buffer.
        /// </summary>
        /// <param name="graphics">Graphics instance</param>
        /// <param name="lev">Current level</param>
        public void Draw(IGraphics graphics, Level lev)
        {
            if (!activeValue)
                return;

            graphics.SetDrawOptions(DrawOptions);

            int drawX = (int)lev.DrawX + (int)worldXValue -
                (int)lev.WorldX - (animationValue.CellWidth >> 1);
            int drawY = (int)lev.DrawY + (int)worldYValue -
                (int)lev.WorldY - (animationValue.CellHeight >> 1);

            graphics.DrawAnimation(drawX, drawY, animationValue);

            graphics.ClearDrawOptions(DrawOptions);
        }

        /// <summary>
        /// Start the object walk animation.
        /// </summary>
        public void Walk()
        {
            if (curState == AnimationState.Walk)
                return;

            animationValue.SetCycle(WalkCycle.StartCell, WalkCycle.EndCell,
                WalkCycle.AnimationRate);

            curState = AnimationState.Walk;
        }

        /// <summary>
        /// Start the object duck animation.
        /// </summary>
        public void Duck()
        {
            if (curState == AnimationState.Duck)
                return;

            animationValue.SetCycle(DuckCycle.EndCell, DuckCycle.EndCell, 0);
            animationValue.StartOneShot(DuckCycle.StartCell, DuckCycle.EndCell,
                DuckCycle.AnimationRate);

            curState = AnimationState.Duck;
        }

        /// <summary>
        /// Start the object stand animation (stationary walk).
        /// </summary>
        public void Stand()
        {
            if (curState == AnimationState.Stop)
                return;

            animationValue.SetCycle(WalkCycle.StartCell, WalkCycle.EndCell, 0);

            curState = AnimationState.Stop;
        }

        /// <summary>
        /// Fire a shot from the object's current location.
        /// </summary>
        /// <param name="lev">Current level</param>
        /// <param name="shot">Information regarding shot to be fired</param>
        /// <param name="dir">Direction to fire shot</param>
        /// <param name="xMultiplier">Velocity x multiplier</param>
        /// <param name="yMultiplier">Velocity y multiplier</param>
        protected void FireShot(Level lev, ShotInfo shot, WorldDir dir,
            float xMultiplier, float yMultiplier)
        {
            shot.Fired = true;

            WorldObject wo = new WorldObject(
                (Animation)lev.AnimationList[shot.AnimationId], shot.AnimationRate);
            Debug.Assert(wo != null,
                "Player.Update: Failed to create player bullet");

            float velX = shot.VelocityX * xMultiplier;
            float offsetX = shot.XOffset;
            if (dir == WorldDir.Left)
            {
                velX = -velX;
                offsetX = -offsetX;
            }

            wo.VelocityX = velX;
            wo.VelocityY = shot.VelocityY * yMultiplier;
            wo.WorldX = worldXValue + offsetX;
            wo.WorldY = worldYValue + shot.YOffset;
            wo.SetTransparency(shot.UseTransparency);
            wo.staticBounds = new Bounds(0.0F, 0.0F, shot.Radius);
            wo.parentValue = this;

            if (shot.UseGravity)
            {
                wo.AILogic = AIHandler.Create(AI.AIType.ShotArc);
            }

            lev.WorldObjects.Add(wo);
        }

        /// <summary>
        /// Start the object attack animation (stationary walk).
        /// </summary>
        public void Attack()
        {
            if (curState == AnimationState.Attack)
                return;

            animationValue.SetCycle(WalkCycle.StartCell, WalkCycle.EndCell, 0);
            animationValue.StartOneShot(AttackCycle.StartCell, AttackCycle.EndCell,
                AttackCycle.AnimationRate);

            curState = AnimationState.Attack;
        }

        /// <summary>
        /// Flip the object draw about the x axis.
        /// </summary>
        public void FlipX()
        {
            DrawOptions |= DrawOptions.BlitMirrorLeftRight;
        }

        /// <summary>
        /// UnFlip the object draw about the x axis.
        /// </summary>
        public void UnflipX()
        {
            DrawOptions &= ~DrawOptions.BlitMirrorLeftRight;
        }
    }
}
