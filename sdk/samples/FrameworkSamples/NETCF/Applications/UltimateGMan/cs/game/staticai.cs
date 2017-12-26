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
    /// AI instance for static objects.
    /// </summary>
    class StaticAI : AI
    {
        /// <summary>
        /// Initialize the static object.
        /// </summary>
        /// <param name="wo">WorldObject using this AI</param>
        /// <param name="lev">Current level</param>
        public override void Init(WorldObject wo, Level lev)
        {
            wo.VelocityX = 0.0F;
            wo.VelocityY = 0.0F;
            wo.Walk();
        }

        /// <summary>
        /// Does nothing.  Static objects essentially have no AI.
        /// </summary>
        /// <param name="wo">Not used</param>
        /// <param name="lev">Not used</param>
        public override void Update(WorldObject wo, Level lev)
        {
        }
    }
}
