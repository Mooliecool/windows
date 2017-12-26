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
using System.Globalization;

namespace GameApp
{
    /// <summary>
    /// Describes generic information about shots fired from world objects.
    /// </summary>
    public class ShotInfo
    {
        /// <summary>
        /// Index int level audio list that defines this shot.
        /// </summary>
        public int AudioId { get { return audioIdValue; } }
        private int audioIdValue = 0;

        /// <summary>
        /// Index into level animation list that defines this shot.
        /// </summary>
        public int AnimationId { get { return animationIdValue; } }
        private int animationIdValue = 0;

        /// <summary>
        /// Animation cell where shot should be initiated.
        /// </summary>
        public int ShootCell { get { return shootCellValue; } }
        private int shootCellValue = 0;

        /// <summary>
        /// X Offset from world object position, at which the shot
        /// should initiate.
        /// </summary>
        public float XOffset { get { return xOffsetValue; } }
        private float xOffsetValue = 0.0F;

        /// <summary>
        /// Y Offset from world object position, at which the shot
        /// should initiate.
        /// </summary>
        public float YOffset { get { return yOffsetValue; } }
        private float yOffsetValue = 0.0F;

        /// <summary>
        /// Initial X velocity of the shot.
        /// </summary>
        public float VelocityX { get { return velocityXValue; } }
        private float velocityXValue = 0.0F;

        /// <summary>
        /// Initial Y velocity of the shot.
        /// </summary>
        public float VelocityY { get { return velocityYValue; } }
        private float velocityYValue = 0.0F;

        /// <summary>
        /// Animation rate of the shot in cells / second.
        /// </summary>
        public int AnimationRate { get { return animationRateValue; } }
        private int animationRateValue = 0;

        /// <summary>
        /// Radius of the shot.  Used for collision detection.
        /// </summary>
        public float Radius { get { return radiusValue; } }
        private float radiusValue = 0.0F;

        /// <summary>
        /// Specifies if the image is drawn with transparency.
        /// </summary>
        public bool UseTransparency { get { return useTransparencyValue; } }
        private bool useTransparencyValue = false;

        /// <summary>
        /// Specifies if the object should have gravity applied to it.
        /// </summary>
        public bool UseGravity { get { return useGravityValue; } }
        private bool useGravityValue = false;

        /// <summary>
        /// Specifies if a shot has been fired during the current animation
        /// cycle.
        /// </summary>
        public bool Fired
        {
            get { return firedValue; }
            set { firedValue = value; }
        }
        private bool firedValue = false;

        /// <summary>
        /// Initialize a shot with the data supplied by the DataRow.  This
        /// overload is used to load player shot information.
        /// </summary>
        /// <param name="dr">DataRow containing shot information</param>
        public ShotInfo(DataRow dr)
        {
            audioIdValue = int.Parse((string)(dr["AudioId"]), 
                CultureInfo.InvariantCulture);
            animationIdValue = int.Parse((string)(dr["AnimationId"]), 
                CultureInfo.InvariantCulture);
            shootCellValue = int.Parse((string)(dr["ShootCell"]), 
                CultureInfo.InvariantCulture);
            xOffsetValue = float.Parse((string)(dr["XOffset"]), 
                CultureInfo.InvariantCulture);
            yOffsetValue = float.Parse((string)(dr["YOffset"]), 
                CultureInfo.InvariantCulture);
            velocityXValue = float.Parse((string)(dr["VelocityX"]), 
                CultureInfo.InvariantCulture);
            velocityYValue = float.Parse((string)(dr["VelocityY"]), 
                CultureInfo.InvariantCulture);
            animationRateValue = int.Parse((string)(dr["AnimationRate"]), 
                CultureInfo.InvariantCulture);
            radiusValue = float.Parse((string)(dr["Radius"]), 
                CultureInfo.InvariantCulture);
            useTransparencyValue = bool.Parse((string)(dr["Transparency"]));
            useGravityValue = bool.Parse((string)(dr["ApplyGravity"]));
        }

        /// <summary>
        /// Initialize a shot with the data supplied by the DataRow. The shot
        /// index specifies which data in the DataRow to access.
        /// </summary>
        /// <param name="dr">DataRow containing shot information</param>
        /// <param name="shotIndex">Index of shot information since world
        /// objects can have up to 2</param>
        public ShotInfo(DataRow dr, int shotIndex)
        {
            audioIdValue = int.Parse(
                (string)(dr[string.Format(CultureInfo.InvariantCulture,
                "Shot{0}AudioId", shotIndex)]), 
                CultureInfo.InvariantCulture);
            animationIdValue = int.Parse(
                (string)(dr[string.Format(CultureInfo.InvariantCulture,
                "Shot{0}AnimationId", shotIndex)]), 
                CultureInfo.InvariantCulture);
            shootCellValue = int.Parse(
                (string)(dr[string.Format(CultureInfo.InvariantCulture,
                "Shot{0}Frame", shotIndex)]), 
                CultureInfo.InvariantCulture);
            xOffsetValue = float.Parse(
                (string)(dr[string.Format(CultureInfo.InvariantCulture,
                "Shot{0}XOffset", shotIndex)]), 
                CultureInfo.InvariantCulture);
            yOffsetValue = float.Parse(
                (string)(dr[string.Format(CultureInfo.InvariantCulture,
                "Shot{0}YOffset", shotIndex)]), 
                CultureInfo.InvariantCulture);
            animationRateValue = int.Parse(
                (string)(dr[string.Format(CultureInfo.InvariantCulture,
                "Shot{0}AnimationRate", shotIndex)]), 
                CultureInfo.InvariantCulture);
            radiusValue = float.Parse(
                (string)(dr[string.Format(CultureInfo.InvariantCulture,
                "Shot{0}Radius", shotIndex)]), 
                CultureInfo.InvariantCulture);
            useTransparencyValue = bool.Parse(
                (string)(dr[string.Format(CultureInfo.InvariantCulture,
                "Shot{0}Transparency", shotIndex)]));
            velocityXValue = float.Parse(
                (string)(dr[string.Format(CultureInfo.InvariantCulture,
                "Shot{0}VelocityX", shotIndex)]), 
                CultureInfo.InvariantCulture);
            velocityYValue = float.Parse(
                (string)(dr[string.Format(CultureInfo.InvariantCulture,
                "Shot{0}VelocityY", shotIndex)]), 
                CultureInfo.InvariantCulture);
            useGravityValue = bool.Parse(
                (string)(dr[string.Format(CultureInfo.InvariantCulture,
                "Shot{0}ApplyGravity", shotIndex)]));
        }

        /// <summary>
        /// Called when the level is reset, this method resets the shot.
        /// </summary>
        public void Reset()
        {
            firedValue = false;
        }
    }
}
