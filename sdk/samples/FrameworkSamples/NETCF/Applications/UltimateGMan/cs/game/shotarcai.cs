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
    /// Implementation of AI for falling objects.
    /// </summary>
    class ShotArcAI : AI
    {
        /// <summary>
        /// Does nothing but requried.
        /// </summary>
        /// <param name="wo">Not used</param>
        /// <param name="lev">Not used</param>
        public override void Init(WorldObject wo, Level lev)
        {
        }

        /// <summary>
        /// Update the world object by applying a gravitational constant.
        /// </summary>
        /// <param name="wo">WorldObject being processed</param>
        /// <param name="lev">Current level</param>
        public override void Update(WorldObject wo, Level lev)
        {
            wo.VelocityY += GameMain.SecondsPerFrame * lev.Gravity;
            if (wo.WorldY - wo.Bounds.Radius >= lev.MaxWorldY)
                wo.Die();
        }
    }

}
