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
    /// Non-aggressive ground-based AI implementation.  Defines AI type
    /// kDuckAndFire.
    /// </summary>
    class DuckAndFireAI : AI
    {
        /// <summary>
        /// Initialize this instance of AI.
        /// </summary>
        /// <param name="wo">WorldObject controlled by this AI instance
        /// </param>
        /// <param name="lev">Currently active level</param>
        public override void Init(WorldObject wo, Level lev)
        {
            CurFireRate = (FireRateMin + GameMain.Random() *
                (FireRateMax - FireRateMin));
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
                return;
            }

            // Turn to face the player
            if (wo.WorldX - lev.Player.WorldX < 0)
                wo.FlipX();
            else
                wo.UnflipX();

            // Update the timers
            CurFireTime += GameMain.SecondsPerFrame;
            CurDuckTime += GameMain.SecondsPerFrame;

            // If attacking then let it finish
            if (wo.State == WorldObject.AnimationState.Attack)
                return;

            // Distance to any incoming shots
            float shotDistX = 0.0F;
            float shotDistY = 0.0F;

            if (!CheckDuck(wo, lev, ref shotDistX, ref shotDistY))
            {
                // If not ducking a shot then check if an attack
                // should occur
                if (wo.State != WorldObject.AnimationState.Duck &&
                    Math.Abs(wo.WorldX - lev.Player.WorldX) <= MaxX &&
                    CurFireTime >= CurFireRate)
                {
                    CurFireTime = 0;
                    CurFireRate = (FireRateMin + GameMain.Random() *
                        (FireRateMax - FireRateMin));
                    wo.Attack();
                }
                else if (wo.State != WorldObject.AnimationState.Duck)
                {
                    wo.Stand();
                }
            }
        }
    }
}
