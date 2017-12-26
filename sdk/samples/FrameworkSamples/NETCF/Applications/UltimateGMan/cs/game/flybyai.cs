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

namespace GameApp
{
    /// <summary>
    /// Fly by attacking AI implementation.  Defines AI type
    /// kFlyby.  This type swoops in to drop bombs, then swoops back
    /// up to a circling altitude and circles back for another attack.
    /// </summary>
    class FlybyAI : AI
    {
        /// <summary>
        /// Current state of the AI instance
        /// </summary>
        private enum FlybyState
        {
            Circling,
            Attacking,
        }
        private FlybyState flybyState = FlybyState.Attacking;

        /// <summary>
        /// Specifies if an attack has been launched.  This is used to
        /// determine whether the object is circling into or out of an attack.
        /// </summary>
        private bool attackLaunched = false;

        /// <summary>
        /// Circling altitude.  This is actually the height above the player
        /// that the object should fly when circling.
        /// </summary>
        private float circleY = 0.0F;

        /// <summary>
        /// Attacking altitude.  This is actually the height above the player
        /// that the object should fly when attacking.
        /// </summary>
        private float attackY = 0.0F;

        /// <summary>
        /// When resetting a level, this AI type needs to reset some
        /// extra data as well.
        /// </summary>
        public override void Reset()
        {
            base.Reset();
            flybyState = FlybyState.Attacking;
            attackLaunched = false;
        }

        /// <summary>
        /// Initialize this instance of the fly by AI.
        /// </summary>
        /// <param name="wo">WorldObject owning this AI</param>
        /// <param name="lev">Current level</param>
        public override void Init(WorldObject wo, Level lev)
        {
            // Determine random circling and attacking altitudes
            circleY = MaxY * .6F + .8F * MaxY * GameMain.Random();
            attackY = MinY * .8F + .4F * MinY * GameMain.Random();

            // Make sure the object is stopped
            wo.VelocityY = 0.0F;
            wo.VelocityX = 0.0F;

            // Make sure that the circling altitude is at least as high as
            // the attacking altitude.
            if (circleY < attackY)
                circleY = attackY;

            // Start at the circling altitude
            wo.WorldY = circleY;

            // Start with the walking animation
            wo.Walk();
        }

        /// <summary>
        /// Update the AI instance.
        /// </summary>
        /// <param name="wo">WorldObject that owns the AI</param>
        /// <param name="lev">Current level</param>
        public override void Update(WorldObject wo, Level lev)
        {
            // If the player is dead then stop
            if (lev.Player.Dead)
            {
                wo.VelocityX = 0.0F;
                wo.VelocityY = 0.0F;
                wo.Stand();
                return;
            }

            // Update the timers
            CurFireTime += GameMain.SecondsPerFrame;
            CurDuckTime += GameMain.SecondsPerFrame;

            // If attacking then let it finish
            if (wo.State == WorldObject.AnimationState.Attack)
                return;

            // Check the state and update accordingly
            switch (flybyState)
            {
                case FlybyState.Attacking:
                    UpdateFlybyAttacking(lev, wo);
                    break;
                default:
                    UpdateFlybyCircling(lev, wo);
                    break;
            }
        }

        /// <summary>
        /// Update the AI in attacking mode.
        /// </summary>
        /// <param name="lev">Current level</param>
        /// <param name="wo">WorldObject that owns AI</param>
        private void UpdateFlybyAttacking(Level lev, WorldObject wo)
        {
            // If shots are fired then prepare for next attack
            // by chaning the mode to circling
            if (attackLaunched)
            {
                wo.Walk();
                flybyState = FlybyState.Circling;
                attackLaunched = false;
                return;
            }

            // Distance to any incoming shots
            float shotDistX = 0.0F;
            float shotDistY = 0.0F;

            // Check if evasion is required
            if (CheckDuck(wo, lev, ref shotDistX, ref shotDistY))
            {
                flybyState = FlybyState.Circling;
                wo.VelocityY = 0.0F;
                return;
            }

            // Cache some parameters
            Player p = lev.Player;
            float deltaPlayerX = p.WorldX - wo.WorldX;
            float deltaPlayerY = p.WorldY - wo.WorldY;

            // Check if conditions are right for attacking
            if (Math.Abs(deltaPlayerX) <= MinX)
            {
                wo.VelocityY = 0.0F;
                wo.Attack();
                attackLaunched = true;
                return;
            }

            // Move into attack position
            if (deltaPlayerX > 0.0F)
                ApplyPositiveAccelerationX(wo);
            else
                ApplyNegativeAccelerationX(wo);

            if (deltaPlayerY > attackY)
            {
                ApplyPositiveAccelerationY(wo);
            }
            else
            {
                wo.WorldY = lev.Player.WorldY - attackY;
                wo.VelocityY = 0.0F;
            }
        }

        /// <summary>
        /// Update the AI in circling mode
        /// </summary>
        /// <param name="lev">Current level</param>
        /// <param name="wo">WorldObject that owns the AI</param>
        private void UpdateFlybyCircling(Level lev, WorldObject wo)
        {
            // Cache some parameters
            Player p = lev.Player;
            float deltaPlayerX = p.WorldX - wo.WorldX;
            float deltaPlayerY = p.WorldY - wo.WorldY;

            // If far enough from player then switch to attack mode
            if (Math.Abs(deltaPlayerX) >= MaxX)
            {
                wo.VelocityY = 0.0F;
                flybyState = FlybyState.Attacking;
                return;
            }

            // Move into circling position
            if (wo.VelocityX > 0.0F)
                ApplyPositiveAccelerationX(wo);
            else
                ApplyNegativeAccelerationX(wo);

            if (deltaPlayerY < circleY)
            {
                ApplyNegativeAccelerationY(wo);
            }
            else
            {
                wo.WorldY = lev.Player.WorldY - circleY;
                wo.VelocityY = 0.0F;
            }
        }
    }
}
