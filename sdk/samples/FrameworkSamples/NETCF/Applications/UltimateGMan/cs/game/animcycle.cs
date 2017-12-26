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
    /// Defines the data that makes up an animation cycle.  This is the data
    /// used by world objects to set animation data.
    /// </summary>
    public class AnimationCycle
    {
        /// <summary>
        /// Rate, in frames/second to play the animation.
        /// </summary>
        public int AnimationRate
        {
            get { return animationRateValue; }
            set { animationRateValue = value; }
        }
        private int animationRateValue = 0;

        /// <summary>
        /// First frame in the animation sequence.
        /// </summary>
        public int StartCell
        {
            get { return startCellValue; }
            set { startCellValue = value; }
        }
        private int startCellValue = 0;

        /// <summary>
        /// Last frame in the animation sequence.  This frame is included
        /// in the animation playback.
        /// </summary>
        public int EndCell
        {
            get { return endCellValue; }
            set { endCellValue = value; }
        }
        private int endCellValue = 0;

        /// <summary>
        /// Rate at which to play the animation when being forced to animate.
        /// This is used by the player object when forced to walk at the edge
        /// of the screen.  Frames / Second.
        /// </summary>
        public int ForceAnimationRate
        {
            get { return forceAnimationRateValue; }
            set { forceAnimationRateValue = value; }
        }
        private int forceAnimationRateValue = 0;

        /// <summary>
        /// Movement rate of the owner object (WorldObject) when this
        /// cycle is being played.  Pixels / Second.
        /// </summary>
        public float MoveRate
        {
            get { return moveRateValue; }
            set { moveRateValue = value; }
        }
        private float moveRateValue = 0;

        /// <summary>
        /// Bounding information for the object when this animation is played.
        /// </summary>
        public Bounds Bounds
        {
            get { return boundsValue; }
            set { boundsValue = value; }
        }
        private Bounds boundsValue = null;

        /// <summary>
        /// Number of frames in this animation.
        /// </summary>
        public int NumberCells { get { return endCellValue - startCellValue + 1; } }
    }
}
