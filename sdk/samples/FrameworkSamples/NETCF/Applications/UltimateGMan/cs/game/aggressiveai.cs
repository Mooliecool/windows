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
    /// Aggressive ground-based AI implementation.  
    /// Defines AI type Aggressive.
    /// </summary>
    public class AggressiveAI : AI
    {
        /// <summary>
        /// Initialize this instance of AI.
        /// </summary>
        /// <param name="wo">WorldObject controlled by this AI instance
        /// </param>
        /// <param name="lev">Currently active level</param>
        public override void Init(WorldObject wo, Level lev)
        {
            wo.Stand();
            CurFireRate = (FireRateMin +
                GameMain.Random() * (FireRateMax - FireRateMin));
        }

        /// <summary>
        /// Update this AI instance.
        /// </summary>
        /// <param name="wo">WorldObject controlled by this AI instance
        /// </param>
        /// <param name="lev">Currently active level</param>
        public override void Update(WorldObject wo, Level lev)
        {
            // If the player is dead then do not update and stop motion
            if (lev.Player.Dead)
            {
                wo.Stand();
                wo.VelocityX = 0.0F;
                wo.VelocityY = 0.0F;
                return;
            }

            // Update the timers
            CurFireTime += GameMain.SecondsPerFrame;
            CurDuckTime += GameMain.SecondsPerFrame;

            // If attacking then let it finish
            if (wo.State == WorldObject.AnimationState.Attack)
                return;

            // Distance to any incoming shots
            float shotDistX = 0.0F;
            float shotDistY = 0.0F;

            // Cache some parameters
            Player p = lev.Player;
            float deltaPlayerX = p.WorldX - wo.WorldX;
            float deltaPlayerY = p.WorldY - wo.WorldY;

            // Check if the world object should evade a shot
            if (!CheckDuck(wo, lev, ref shotDistX, ref shotDistY))
            {
                if (wo.State != WorldObject.AnimationState.Duck &&
                    Math.Abs(deltaPlayerX) <= MaxX &&
                    CurFireTime >= CurFireRate)
                {
                    // If the player is within range, the world object is not
                    // ducking, and it is time to fire then shoot at the
                    // player
                    CurFireTime = 0;
                    CurFireRate = (FireRateMin + GameMain.Random() *
                        (FireRateMax - FireRateMin));
                    wo.Attack();
                    wo.VelocityX = 0.0F;
                }
                else if (wo.State != WorldObject.AnimationState.Duck)
                {
                    // Walk toward the player if not overlapping
                    // Overlapping can only occur when collisions are off
                    if (deltaPlayerX > 5.0F)
                    {
                        wo.Walk();
                        wo.VelocityX = VelocityX;
                        wo.FlipX();
                    }
                    else if (deltaPlayerX < -5.0F)
                    {
                        wo.Walk();
                        wo.VelocityX = -VelocityX;
                        wo.UnflipX();
                    }
                    else
                    {
                        wo.Stand();
                        wo.VelocityX = 0.0F;
                    }
                }
            }
            else
            {
                // Stop all movement if ducking
                wo.VelocityX = 0.0F;
            }
        }
    }
}
