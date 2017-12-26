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
using System.Diagnostics;
using System.Globalization;

namespace GameApp
{
    /// <summary>
    /// Abstract class from which all AI types are derived.
    /// </summary>
    public abstract class AI
    {
        /// <summary>
        /// Called when the level is reset, this method resets the instance
        /// of the AI type.
        /// </summary>
        public virtual void Reset()
        {
            CurFireTime = 0.0F;
            CurDuckTime = 0.0F;
        }

        /// <summary>
        /// Time since last shot was fired.
        /// </summary>
        protected float CurFireTime
        {
            get { return curFireTimeValue; }
            set { curFireTimeValue = value; }
        }
        float curFireTimeValue = 0.0F;

        /// <summary>
        /// Time since last duck check.
        /// </summary>
        protected float CurDuckTime
        {
            get { return curDuckTimeValue; }
            set { curDuckTimeValue = value; }
        }
        float curDuckTimeValue = 0.0F;

        /// <summary>
        /// Minimim amount of time between shots.
        /// </summary>
        protected float FireRateMin
        {
            get { return fireRateMinValue; }
            set { fireRateMinValue = value; }
        }
        float fireRateMinValue = 0.0F;

        /// <summary>
        /// Maximum amount of time between shots if firing back-to-back.
        /// </summary>
        protected float FireRateMax
        {
            get { return fireRateMaxValue; }
            set { fireRateMaxValue = value; }
        }
        float fireRateMaxValue = 0.0F;

        /// <summary>
        /// Time interval do wait since the last shot.  This is a random value
        /// between the range of fireRateMin and fireRateMax.
        /// </summary>
        protected float CurFireRate
        {
            get { return curFireRateValue; }
            set { curFireRateValue = value; }
        }
        float curFireRateValue = 0.0F;

        /// <summary>
        /// Chance of successfully ducking when a shot is incoming.
        /// </summary>
        protected float DuckChance
        {
            get { return duckChanceValue; }
            set { duckChanceValue = value; }
        }
        float duckChanceValue = 0.0F;

        /// <summary>
        /// Interval between checks to see if a duck should be triggered.
        /// </summary>
        protected float DuckRate
        {
            get { return duckRateValue; }
            set { duckRateValue = value; }
        }
        float duckRateValue = 0.0F;

        /// <summary>
        /// Minimum X distance.  The meaning of this value varies per type.
        /// </summary>
        protected float MinX
        {
            get { return minXValue; }
            set { minXValue = value; }
        }
        float minXValue = 0.0F;

        /// <summary>
        /// Maximum X distance.  The meaning of this value varies per type.
        /// </summary>
        protected float MaxX
        {
            get { return maxXValue; }
            set { maxXValue = value; }
        }
        float maxXValue = 0.0F;

        /// <summary>
        /// Minimum Y distance.  The meaning of this value varies per type.
        /// </summary>
        protected float MinY
        {
            get { return minYValue; }
            set { minYValue = value; }
        }
        float minYValue = 0.0F;

        /// <summary>
        /// Maximum Y distance.  The meaning of this value varies per type.
        /// </summary>
        protected float MaxY
        {
            get { return maxYValue; }
            set { maxYValue = value; }
        }
        float maxYValue = 0.0F;

        /// <summary>
        /// Maximum X Velocity in pixels/second.
        /// </summary>
        protected float VelocityX
        {
            get { return velocityXValue; }
            set { velocityXValue = value; }
        }
        float velocityXValue = 0.0F;

        /// <summary>
        /// Maximum Y Velocity in pixels/second.
        /// </summary>
        protected float VelocityY
        {
            get { return velocityYValue; }
            set { velocityYValue = value; }
        }
        float velocityYValue = 0.0F;

        /// <summary>
        /// Maximum X acceleration.
        /// </summary>
        protected float AccelerateX
        {
            get { return accelerateXValue; }
            set { accelerateXValue = value; }
        }
        float accelerateXValue = 0.0F;

        /// <summary>
        /// Maximum Y acceleration.
        /// </summary>
        protected float AccelerateY
        {
            get { return accelerateYValue; }
            set { accelerateYValue = value; }
        }
        float accelerateYValue = 0.0F;

        /// <summary>
        /// AI types derived from this base.  For each enumerated type there
        /// must be an equivalent implementation class.
        /// </summary>
        public enum AIType
        {
            None = -1,
            Static = 0,
            DuckAndFire,
            Flyby,
            ShotArc,
            Aggressive,
            Count
        }

        /// <summary>
        /// Initialize the AI class instance.
        /// </summary>
        /// <param name="wo">WorldObject controlled by the AI instance</param>
        /// <param name="lev">Currently active level</param>
        public abstract void Init(WorldObject wo, Level lev);

        /// <summary>
        /// Update the AI class instance.
        /// </summary>
        /// <param name="wo">WorldObject controlled by the AI instance</param>
        /// <param name="lev">Currently active level</param>
        public abstract void Update(WorldObject wo, Level lev);

        /// <summary>
        /// AI type that the derived class implements.
        /// </summary>
        public AIType Type { get { return typeValue; } }
        AIType typeValue = AIType.None;

        /// <summary>
        /// Overloaded Load method which creates an instance of the
        /// specified type.
        /// </summary>
        /// <param name="type">AI type to be initialized</param>
        public void Load(AIType type)
        {
            Debug.Assert(type == AIType.ShotArc,
                "Only shots support dynamic AI allocation");

            this.typeValue = type;
        }

        /// <summary>
        /// Overloaded Load method which creates a copy of the specified
        /// AI instance.
        /// </summary>
        /// <param name="ai">AI instance which is to be copied</param>
        public void Load(AI ai)
        {
            typeValue = ai.typeValue;

            FireRateMin = ai.FireRateMin;
            FireRateMax = ai.FireRateMax;
            DuckChance = ai.DuckChance;
            DuckRate = ai.DuckRate;

            MaxX = ai.MaxX;
            MinX = ai.MinX;
            MaxY = ai.MaxY;
            MinY = ai.MinY;
            VelocityX = ai.VelocityX;
            VelocityY = ai.VelocityY;
            AccelerateX = ai.AccelerateX;
            AccelerateY = ai.AccelerateY;
        }

        /// <summary>
        /// Overloaded Load method which loads an AI instance from
        /// the specified DataRow.
        /// </summary>
        /// <param name="dr">DataRow from AI DataTable</param>
        public void Load(DataRow dr)
        {
            AIType aiType = (AIType)int.Parse((string)dr["Type"], 
                CultureInfo.InvariantCulture);
            Debug.Assert(aiType >= 0 && aiType < AIType.Count,
                "AI.AI: Invalid AI Type");

            typeValue = aiType;
            MinX = float.Parse((string)dr["MinX"], 
                CultureInfo.InvariantCulture);
            MaxX = float.Parse((string)dr["MaxX"], 
                CultureInfo.InvariantCulture);
            MinY = float.Parse((string)dr["MinY"], 
                CultureInfo.InvariantCulture);
            MaxY = float.Parse((string)dr["MaxY"], 
                CultureInfo.InvariantCulture);
            VelocityX = float.Parse((string)dr["VelocityX"], 
                CultureInfo.InvariantCulture);
            VelocityY = float.Parse((string)dr["VelocityY"], 
                CultureInfo.InvariantCulture);
            AccelerateX = float.Parse((string)dr["AccelerationX"], 
                CultureInfo.InvariantCulture);
            AccelerateY = float.Parse((string)dr["AccelerationY"], 
                CultureInfo.InvariantCulture);

            FireRateMin = float.Parse((string)dr["FireRateMin"], 
                CultureInfo.InvariantCulture);
            FireRateMax = float.Parse((string)dr["FireRateMax"], 
                CultureInfo.InvariantCulture);

            DuckChance = float.Parse((string)dr["DuckChance"], 
                CultureInfo.InvariantCulture);
            DuckRate = float.Parse((string)dr["DuckRate"], 
                CultureInfo.InvariantCulture);

            Debug.Assert(aiType > AIType.None && aiType < AIType.Count,
                "AI.AI: Invalid AI type");
        }

        /// <summary>
        /// Apply a positive x acceleration to the world object and
        /// turn in the direction of the acceleration.
        /// </summary>
        /// <param name="wo">WorldObject to be accelerated</param>
        protected void ApplyPositiveAccelerationX(WorldObject wo)
        {
            wo.VelocityX += GameMain.SecondsPerFrame * AccelerateX;
            if (wo.VelocityX > VelocityX)
                wo.VelocityX = VelocityX;

            wo.FlipX();
        }

        /// <summary>
        /// Apply a negative x acceleration to the world object and
        /// turn in the direction of the acceleration.
        /// </summary>
        /// <param name="wo">WorldObject to be accelerated</param>
        protected void ApplyNegativeAccelerationX(WorldObject wo)
        {
            wo.VelocityX -= GameMain.SecondsPerFrame * AccelerateX;
            if (wo.VelocityX < -VelocityX)
                wo.VelocityX = -VelocityX;

            wo.UnflipX();
        }

        /// <summary>
        /// Apply a positive y acceleration to the world object and
        /// turn in the direction of the acceleration.
        /// </summary>
        /// <param name="wo">WorldObject to be accelerated</param>
        protected void ApplyPositiveAccelerationY(WorldObject wo)
        {
            wo.VelocityY += GameMain.SecondsPerFrame * AccelerateY;
            if (wo.VelocityY > VelocityY)
                wo.VelocityY = VelocityY;
        }

        /// <summary>
        /// Apply a negative y acceleration to the world object and
        /// turn in the direction of the acceleration.
        /// </summary>
        /// <param name="wo">WorldObject to be accelerated</param>
        protected void ApplyNegativeAccelerationY(WorldObject wo)
        {
            wo.VelocityY -= GameMain.SecondsPerFrame * AccelerateY;
            if (wo.VelocityY < -VelocityY)
                wo.VelocityY = -VelocityY;
        }

        /// <summary>
        /// Determine if the world object should duck, assuming there is a
        /// need to.
        /// </summary>
        /// <param name="lev">Current level</param>
        /// <param name="wo">WorldObject to check</param>
        /// <returns>true if the object should duck, false otherwise</returns>
        protected bool ShouldDuck(Level lev, WorldObject wo)
        {
            return ((CurDuckTime >= DuckRate &&
                GameMain.Random() < DuckChance) ||
                Math.Abs(wo.WorldX - lev.Player.WorldX) > MaxX);
        }

        /// <summary>
        /// Determine if a shot is posing a threat to the WorldObject.
        /// </summary>
        /// <param name="lev">Current level</param>
        /// <param name="wo">WorldObject to be checked</param>
        /// <param name="shotDistX">Distance to shot in x if detected</param>
        /// <param name="shotDistY">Distance to shot in y if detected</param>
        /// <returns>true if a shot is coming, false otherwise</returns>
        protected bool IsShotComing(Level lev, WorldObject wo,
            ref float shotDistX, ref float shotDistY)
        {
            foreach (WorldObject w in lev.WorldObjects)
            {
                if (w.Dynamic)
                {
                    if ((w.VelocityX > 0.0F && (w.WorldX < wo.WorldX +
                        (wo.Width >> 1))) ||
                        (w.VelocityX < 0.0F &&
                        (wo.WorldX - (wo.Width >> 1) < w.WorldX)))
                    {
                        if (Math.Abs(wo.WorldX - w.WorldX) < MinX)
                        {
                            shotDistX = wo.WorldX - w.WorldX;
                            shotDistY = wo.WorldY - w.WorldY;
                            return true;
                        }
                    }
                }
            }

            return false;
        }

        /// <summary>
        /// Check if the world object should duck.  This determined by
        /// whether a
        /// shot is coming, the time since the last shot, and the chance of
        /// ducking.
        /// </summary>
        /// <param name="wo">WorldObject to be checked</param>
        /// <param name="lev">Current level</param>
        /// <param name="shotDistX">X distance to shot if detected</param>
        /// <param name="shotDistY">Y distance to shot if detected</param>
        /// <returns>true if ducked, false otherwise</returns>
        protected bool CheckDuck(WorldObject wo, Level lev,
            ref float shotDistX, ref float shotDistY)
        {
            bool bShotComing = IsShotComing(lev, wo, ref shotDistX,
                ref shotDistY);

            if (!bShotComing && wo.State == WorldObject.AnimationState.Duck)
                wo.Stand();

            if (bShotComing && ShouldDuck(lev, wo))
            {
                CurDuckTime = 0;
                wo.Duck();
                return true;
            }
            else if (CurDuckTime >= DuckRate)
            {
                CurDuckTime = 0;
            }

            return false;
        }
    }
}
