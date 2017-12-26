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
using System.Collections;
using System.Diagnostics;
using System.Windows.Forms;
using System.Globalization;
using GraphicsLibrary;
using InputLibrary;

namespace GameApp
{
    /// <summary>
    /// The player object represents the player in the world.  The player is
    /// a special case of a world object.
    /// </summary>
    public class Player : WorldObject
    {
        /// <summary>
        /// Player's current animation cycle.
        /// </summary>
        AnimationCycle curCycle = null;

        /// <summary>
        /// Player's animation state during the previous update frame.
        /// </summary>
        private AnimationState prevState = AnimationState.Stop | AnimationState.Walk;

        /// <summary>
        /// Time since the last shot was fired.
        /// </summary>
        private float shotTime = 0.0F;

        /// <summary>
        /// Specifies if the player shot a misfire during the current frame.
        /// This is only set for one frame.
        /// </summary>
        public bool Misfire { get { return misfireValue; } }
        private bool misfireValue = false;

        /// <summary>
        /// Buttons available to the player.  These are indices into the
        /// button map.
        /// </summary>
        public enum Buttons
        {
            ResetLevel,
            ToggleDebug,
            Quit,
            FireShot
        }

#if SMARTPHONE
        static readonly int[] buttonMapValue = { 1, 2, 3, 6 };
#else
        static readonly int[] buttonMapValue = { 0, 2, 1, 3 };
#endif

        /// <summary>
        /// Specifies if a shot is in the process of being fired, i.e., the
        /// shot bar is loading.
        /// </summary>
        private bool shotStarted = false;

        /// <summary>
        /// Length of time, in seconds, for a shot to be fully loaded.
        /// </summary>
        private float shotChargeTime;

        /// <summary>
        /// Percent chance of a misfire when a shot is fired with an empty
        /// shot bar.
        /// </summary>
        private float shotMisfireMin;

        /// <summary>
        /// Percent chance of a misfire when a shot is fired with a full
        /// shot bar.
        /// </summary>
        private float shotMisfireMax;

        /// <summary>
        /// X velocity of a shot when fired with an empty shot bar.
        /// </summary>
        private float shotVelocityXMin;

        /// <summary>
        /// X velocity of a shot when fired with a full shot bar.
        /// </summary>
        private float shotVelocityXMax;

        /// <summary>
        /// Y velocity of a shot when fired with an empty shot bar.
        /// </summary>
        private float shotVelocityYMin;

        /// <summary>
        /// Y velocity of a shot when fired with a full shot bar.
        /// </summary>
        private float shotVelocityYMax;

        /// <summary>
        /// Minimim range of a misfired shot's x velocity.
        /// </summary>
        private float shotMisfireVelocityXMin;

        /// <summary>
        /// Maximum range of a misfired shot's x velocity.
        /// </summary>
        private float shotMisfireVelocityXMax;

        /// <summary>
        /// Minimim range of a misfired shot's y velocity.
        /// </summary>
        private float shotMisfireVelocityYMin;

        /// <summary>
        /// Maximim range of a misfired shot's y velocity.
        /// </summary>
        private float shotMisfireVelocityYMax;

        /// <summary>
        /// Index of audio to play when the player dies.
        /// </summary>
        public int DeathAudioId { get { return audioDeathId; } }
        private int audioDeathId;

        /// <summary>
        /// Gets the percentage of the shot bar that is currently
        /// charged
        /// </summary>
        public float CurrentShotBarPercent
        {
            get
            {
                float perc = shotTime / this.shotChargeTime;
                return perc >= 1.0F ? 1.0F : perc;
            }
        }

        /// <summary>
        /// Create an instance of the player.
        /// </summary>
        /// <param name="ds">DataSet defining the player</param>
        /// <param name="graphics">Graphics instance</param>
        /// <param name="animList">List of level animations</param>
        public Player(DataSet ds, IGraphics graphics, ArrayList animationList)
        {
            // Yes, this world object is the player
            Player = true;

            // The player is always active and collidable
            Active = true;
            Collidable = true;

            // Allocate space fore all animation cycles
            for (int i = 0; i < (int)AnimationType.Count; i++)
            {
                SetAnimationCycle(i, new AnimationCycle());
                Debug.Assert(GetAnimationCycle(i) != null,
                    "Player.Player: Failed to allocate animation cycle");
            }

            // Start location
            DataTable dt = ds.Tables["StartLocation"];
            Debug.Assert(dt != null && dt.Rows != null,
                "Player.Player: Invalid start location data");

            WorldX = float.Parse((string)(dt.Rows[0]["X"]), 
                CultureInfo.InvariantCulture);
            WorldY = float.Parse((string)(dt.Rows[0]["Y"]), 
                CultureInfo.InvariantCulture);

            // Walk animation
            dt = ds.Tables["Walk"];
            Debug.Assert(dt != null && dt.Rows != null && dt.Rows[0] != null,
                "Player.Player: Walk animation data not valid");

            WalkCycle.AnimationRate = int.Parse((string)(
                dt.Rows[0]["NormalAnimationRate"]), 
                CultureInfo.InvariantCulture);
            WalkCycle.ForceAnimationRate = int.Parse(
                (string)(dt.Rows[0]["ForceAnimationRate"]), 
                CultureInfo.InvariantCulture);
            WalkCycle.MoveRate = float.Parse(
                (string)(dt.Rows[0]["MovementRate"]), 
                CultureInfo.InvariantCulture);
            WalkCycle.StartCell = int.Parse(
                (string)(dt.Rows[0]["StartCell"]), 
                CultureInfo.InvariantCulture);
            WalkCycle.EndCell = int.Parse((string)(dt.Rows[0]["EndCell"]), 
                CultureInfo.InvariantCulture);
            WalkCycle.Bounds = new Bounds(dt.Rows[0]);

            // Attack animation
            dt = ds.Tables["Attack"];
            Debug.Assert(dt != null && dt.Rows != null &&
                dt.Rows[0] != null,
                "Player.Player: Attack animation data not valid");

            AttackCycle.AnimationRate = int.Parse(
                (string)(dt.Rows[0]["NormalAnimationRate"]), 
                CultureInfo.InvariantCulture);
            AttackCycle.ForceAnimationRate = int.Parse(
                (string)(dt.Rows[0]["ForceAnimationRate"]), 
                CultureInfo.InvariantCulture);
            AttackCycle.MoveRate = float.Parse(
                (string)(dt.Rows[0]["MovementRate"]), 
                CultureInfo.InvariantCulture);
            AttackCycle.StartCell = int.Parse(
                (string)(dt.Rows[0]["StartCell"]), 
                CultureInfo.InvariantCulture);
            AttackCycle.EndCell = int.Parse(
                (string)(dt.Rows[0]["EndCell"]), 
                CultureInfo.InvariantCulture);
            AttackCycle.Bounds = new Bounds(dt.Rows[0]);

            // Crawl animation
            dt = ds.Tables["Crawl"];
            Debug.Assert(dt != null && dt.Rows != null && dt.Rows[0] != null,
                "Player.Player: Attack animation data not valid");

            DuckCycle.AnimationRate = int.Parse(
                (string)(dt.Rows[0]["NormalAnimationRate"]), 
                CultureInfo.InvariantCulture);
            DuckCycle.ForceAnimationRate = int.Parse(
                (string)(dt.Rows[0]["ForceAnimationRate"]), 
                CultureInfo.InvariantCulture);
            DuckCycle.MoveRate = float.Parse(
                (string)(dt.Rows[0]["MovementRate"]), 
                CultureInfo.InvariantCulture);
            DuckCycle.StartCell = int.Parse(
                (string)(dt.Rows[0]["StartCell"]), 
                CultureInfo.InvariantCulture);
            DuckCycle.EndCell = int.Parse(
                (string)(dt.Rows[0]["EndCell"]), 
                CultureInfo.InvariantCulture);
            DuckCycle.Bounds = new Bounds(dt.Rows[0]);

            // Death animation
            dt = ds.Tables["Death"];
            Debug.Assert(dt != null && dt.Rows != null &&
                dt.Rows[0] != null,
                "Player.Player: Death animation data not valid");

            DeathCycle.AnimationRate = int.Parse(
                (string)(dt.Rows[0]["AnimationRate"]), 
                CultureInfo.InvariantCulture);
            DeathCycle.StartCell = int.Parse(
                (string)(dt.Rows[0]["StartCell"]), 
                CultureInfo.InvariantCulture);
            DeathCycle.EndCell = int.Parse((string)(dt.Rows[0]["EndCell"]), 
                CultureInfo.InvariantCulture);

            // Shot
            dt = ds.Tables["Shot"];
            Debug.Assert(dt != null && dt.Rows != null && dt.Rows[0] != null,
                "Player.Player: Shot info data not valid");

            ShotInfo[] tempShotInfo = new ShotInfo[1];
            tempShotInfo[0] = new ShotInfo(dt.Rows[0]);
            SetShotInfo(tempShotInfo);
            Debug.Assert(GetShotInfo() != null,
                "Player.Player: Failed to allocate shot info");

            // General
            dt = ds.Tables["General"];
            Debug.Assert(dt != null && dt.Rows != null && dt.Rows[0] != null,
                "Player.Player: General info data not valid");

            DataRow dr = dt.Rows[0];

            shotChargeTime = float.Parse((string)(dr["ShotChargeTime"]), 
                CultureInfo.InvariantCulture);
            shotMisfireMin = float.Parse(
                (string)(dr["ShotMisfireMin"]), 
                CultureInfo.InvariantCulture) / 100.0F;
            shotMisfireMax = float.Parse(
                (string)(dr["ShotMisfireMax"]), 
                CultureInfo.InvariantCulture) / 100.0F;
            shotVelocityXMin = float.Parse(
                (string)(dr["ShotVelocityXMin"]), 
                CultureInfo.InvariantCulture) / 100.0F;
            shotVelocityXMax = float.Parse(
                (string)(dr["ShotVelocityXMax"]), 
                CultureInfo.InvariantCulture) / 100.0F;
            shotVelocityYMin = float.Parse(
                (string)(dr["ShotVelocityYMin"]), 
                CultureInfo.InvariantCulture) / 100.0F;
            shotVelocityYMax = float.Parse(
                (string)(dr["ShotVelocityYMax"]), 
                CultureInfo.InvariantCulture) / 100.0F;
            shotMisfireVelocityXMin = float.Parse(
                (string)(dr["ShotMisfireVelocityXMin"]), 
                CultureInfo.InvariantCulture) / 100.0F;
            shotMisfireVelocityXMax = float.Parse(
                (string)(dr["ShotMisfireVelocityXMax"]), 
                CultureInfo.InvariantCulture) / 100.0F;
            shotMisfireVelocityYMin = float.Parse(
                (string)(dr["ShotMisfireVelocityYMin"]), 
                CultureInfo.InvariantCulture) / 100.0F;
            shotMisfireVelocityYMax = float.Parse(
                (string)(dr["ShotMisfireVelocityYMax"]), 
                CultureInfo.InvariantCulture) / 100.0F;
            audioDeathId = int.Parse((string)(dr["DeathAudioId"]),
                CultureInfo.InvariantCulture);

            // Animation
            int animationId = int.Parse((string)(dr["AnimationId"]), 
                CultureInfo.InvariantCulture);
            Debug.Assert(animationId >= 0 && animationId < animationList.Count,
                "Player.Player: Animation ID out of range");

            Animation = new Animation((Animation)animationList[animationId], 0, 0);
            Debug.Assert(Animation != null && Animation.Init,
                "Player.Player: Failed to load animationation");

            // Start with the walk cycle
            Animation.SetCycle(WalkCycle.StartCell, WalkCycle.EndCell, 0);
            curCycle = WalkCycle;

            // Check if drawing requires transparency
            if (bool.Parse((string)(dt.Rows[0]["Transparency"])))
            {
                DrawOptions = DrawOptions.BlitKeyedTransparency;
            }

            ResetCollidable = Collidable;
            ResetWorldX = WorldX;
            ResetWorldY = WorldY;
        }

        /// <summary>
        /// Maps the player buttons.  Each element in the array is an index
        /// into Input's HardwareKeys list.  The index of each element
        /// corresponds to a Buttons enum value.  For example, to access the
        /// key value for resetting the level, the code would be:
        /// gi.HardwareKeys[buttonMapValue[Buttons.ResetLevel]]
        /// where gi is an instance of Input.
        /// </summary>
        public static int[] ButtonMap()
        { 
            return buttonMapValue;
        }


        /// <summary>
        /// Called when the level is reset.  Resets the player state and
        /// position.
        /// </summary>
        /// <param name="lev">Current level</param>
        public override void Reset(Level lev)
        {
            // Reset the base world object data
            base.Reset(lev);

            // Start with the walk cycle
            Animation.SetCycle(WalkCycle.StartCell, WalkCycle.EndCell, 0);
            curCycle = WalkCycle;
            prevState = AnimationState.Stop | AnimationState.Walk;

            // Reset player specific information
            Active = true;
            Collidable = true;
            shotStarted = false;
            shotTime = 0.0F;
            misfireValue = false;
        }

        /// <summary>
        /// Force the player to walk to the right with the scrolling
        /// world.  This occurs when the player bumps against the left
        /// edge of the screen.
        /// </summary>
        /// <param name="lev">Current level</param>
        /// <param name="src">Animation state that spawned this</param>
        private void ForceRight(Level lev, AnimationState src)
        {
            // If not attacking and we were not being forced right in the 
            // previous update then reset the walk cycle
            if ((State & AnimationState.Attack) == 0 && (State |
                AnimationState.ForceRight) != prevState)
            {
                Animation.SetCycle(curCycle.StartCell, curCycle.EndCell,
                    curCycle.ForceAnimationRate);
                Animation.CurCell = 1;
            }

            // If not previously attacking then face the direction of the
            // source that spawned this walk
            if ((prevState & AnimationState.Attack) == 0)
            {
                if (src == AnimationState.Left)
                    DrawOptions |= DrawOptions.BlitMirrorLeftRight;
                else
                    DrawOptions &= ~DrawOptions.BlitMirrorLeftRight;
            }

            // Update the world position
            WorldX = lev.WorldX + lev.ScreenEdgeBuffer +
                (curCycle.Bounds.Radius);

            // Set the state
            State |= AnimationState.ForceRight;
        }

        /// <summary>
        /// Attempt to walk to the left.
        /// </summary>
        /// <param name="lev">Current world</param>
        private void WalkLeft(Level lev)
        {
            // Calculate the new world location
            float resultX = WorldX - GameMain.SecondsPerFrame *
                (curCycle.MoveRate - lev.ScrollRate);

            // If the world location is off the screen to the left then
            // force the player to walk right
            if (resultX - (curCycle.Bounds.Radius) < lev.WorldX +
                lev.ScreenEdgeBuffer)
            {
                ForceRight(lev, AnimationState.Left);
                return;
            }

            // If not attacking and not previously walking left then reset the
            // animation
            if ((State & AnimationState.Attack) == 0 && (State |
                AnimationState.Left) != prevState)
            {
                Animation.SetCycle(curCycle.StartCell, curCycle.EndCell,
                    curCycle.AnimationRate);
                Animation.CurCell = 1;
                DrawOptions |= DrawOptions.BlitMirrorLeftRight;
            }

            // Set the new world position
            WorldX = resultX;

            // Set the state
            State |= AnimationState.Left;
        }

        /// <summary>
        /// Attempt to walk to the right.
        /// </summary>
        /// <param name="lev">Current level</param>
        private void WalkRight(Level lev)
        {
            // If not attacking and not previously walking to the right then
            // reset the animation
            if ((State & AnimationState.Attack) == 0 && (State |
                AnimationState.Right) != prevState)
            {
                int curCell = 1;
                if (State == AnimationState.ForceRight)
                    curCell = Animation.CurCell;

                Animation.SetCycle(curCycle.StartCell, curCycle.EndCell,
                    curCycle.AnimationRate);

                Animation.CurCell = curCell;

                DrawOptions &= ~DrawOptions.BlitMirrorLeftRight;
            }

            // Update the animation rate and world position
            Animation.AnimationRate = curCycle.AnimationRate;
            WorldX += GameMain.SecondsPerFrame * (curCycle.MoveRate +
                lev.ScrollRate);

            // Validate the new world location
            if (WorldX + (curCycle.Bounds.Radius) > lev.WorldX +
                lev.ViewWidth - lev.ScreenEdgeBuffer)
            {
                WorldX = lev.WorldX + lev.ViewWidth -
                    lev.ScreenEdgeBuffer - (curCycle.Bounds.Radius);
                Animation.AnimationRate = curCycle.ForceAnimationRate;
            }

            // Update the animation state
            State |= AnimationState.Right;
        }

        /// <summary>
        /// Stop walking.
        /// </summary>
        /// <param name="lev">Current level</param>
        private void Stop(Level lev)
        {
            // If the world scrolls past the player then force the player
            // to walk right
            if (WorldX - (curCycle.Bounds.Radius) < lev.WorldX +
                lev.ScreenEdgeBuffer)
            {
                ForceRight(lev, AnimationState.Stop);
                return;
            }

            // If not attacking and not previously stopped then reset
            // the animation cycle
            if ((State & AnimationState.Attack) == 0 && (State |
                AnimationState.Stop) != prevState)
            {
                Animation.SetCycle(curCycle.StartCell, curCycle.EndCell, 0);
            }

            // Set the animation state
            State |= AnimationState.Stop;
        }

        /// <summary>
        /// Update the player.
        /// </summary>
        /// <param name="gi">Input instance</param>
        /// <param name="lev">Current level</param>
        /// <returns>Always false</returns>
        public override bool Update(Input gi, Level lev)
        {
            // Assume there is no misfire this frame
            misfireValue = false;

            // If a shot is charging then update the time
            if (shotStarted)
                shotTime += GameMain.SecondsPerFrame;

            // If the player is not dead then update it
            if ((State & AnimationState.Dead) == 0)
            {
                // clear out the current direction but leave the animation
                // state
                State = prevState & AnimationState.AnimationMask;

                // If attacking then update the shot state
                if ((State & AnimationState.Attack) != 0)
                {
                    // If the shot is not already fired and the current
                    // animation cell is the one where the player shoots
                    // then launch a shot
                    if (!GetShotInfo()[0].Fired &&
                        Animation.CurCell == GetShotInfo()[0].ShootCell)
                    {
                        // Determine the direction of the shot
                        WorldDir dir = WorldDir.Left;
                        if ((DrawOptions & DrawOptions.BlitMirrorLeftRight)
                            == 0)
                            dir = WorldDir.Right;

                        // Velocity multipliers
                        float shotVelXMultiplier;
                        float shotVelYMultiplier;

                        // Determine the chance of a misfire based on the shot
                        // bar charge (min to max)
                        float misfireChance =
                            shotMisfireMax - CurrentShotBarPercent *
                            (shotMisfireMax - shotMisfireMin);

                        if (GameMain.Random() < misfireChance)
                        {
                            // A misfire occurred so set the flag and
                            // update the velocity multipliers accordingly
                            misfireValue = true;
                            shotVelXMultiplier = shotMisfireVelocityXMin +
                                GameMain.Random() *
                                (shotVelocityXMax - shotVelocityXMin);
                            shotVelYMultiplier = shotMisfireVelocityYMin +
                                GameMain.Random() *
                                (shotVelocityYMax - shotVelocityYMin);
                        }
                        else
                        {
                            // No misfire, so the velocities are set
                            // relative to the bar charge percentage
                            shotVelXMultiplier = shotVelocityXMin +
                                CurrentShotBarPercent *
                                (shotVelocityXMax - shotVelocityXMin);
                            shotVelYMultiplier = shotVelocityYMax -
                                CurrentShotBarPercent *
                                (shotVelocityYMax - shotVelocityYMin);
                        }

                        // Fire a shot and reset the charge time
                        shotTime = 0.0F;
                        FireShot(lev, GetShotInfo()[0], dir,
                            shotVelXMultiplier, shotVelYMultiplier);
                    }
                    else if (Animation.Done)
                    {
                        // If the shot animation is done then reset shot info
                        GetShotInfo()[0].Fired = false;
                        State = AnimationState.Walk;
                        curCycle = WalkCycle;
                    }
                }
                else if (gi.KeyPressed((int)Keys.Down))
                {
                    // If pressing down then attempt to crawl.
                    State = AnimationState.Duck;
                    curCycle = DuckCycle;
                }
                else if (gi.KeyPressed((int)Keys.Up))
                {
                    // If pressing up then stand up
                    State = AnimationState.Walk;
                    curCycle = WalkCycle;
                }
                else if (!shotStarted && gi.KeyDown(
                    (int)gi.HardwareKeys[buttonMapValue[
                    (int)Buttons.FireShot]]))
                {
                    // If the fire button is pressed then start charging
                    // the bar
                    shotStarted = true;
                    shotTime = 0.0F;
                }
                else if (shotStarted && gi.KeyReleased(
                    (int)gi.HardwareKeys[buttonMapValue[
                    (int)Buttons.FireShot]]))
                {
                    // If the fire button is released then start the shot
                    // animation
                    curCycle = AttackCycle;
                    State |= AnimationState.Attack;
                    Animation.SetCycle(WalkCycle.StartCell,
                        WalkCycle.EndCell, WalkCycle.AnimationRate);
                    Animation.StartOneShot(AttackCycle.StartCell,
                        AttackCycle.EndCell, AttackCycle.AnimationRate);
                    shotStarted = false;
                }

                Debug.Assert(curCycle != null,
                    "Player.Update: Invalid animation sequence");

                Debug.Assert((State & AnimationState.AnimationMask) != 0,
                    "Player.Update: Invalid animation state determined");

                // Attempt to walk in the direction the user is pressing
                if (gi.KeyDown((int)Keys.Left))
                    WalkLeft(lev);
                else if (gi.KeyDown((int)Keys.Right))
                    WalkRight(lev);
                else
                    Stop(lev);

                Debug.Assert((State & AnimationState.DirectionMask) != 0 &&
                    (State & AnimationState.AnimationMask) != 0,
                    "Player.Update: Invalid movement state determined");
            }

            // Update the animation
            Animation.Update(GameMain.SecondsPerFrame);

            // Set the previous state
            prevState = State;

            // The player always returns false
            return false;
        }
    }
}
