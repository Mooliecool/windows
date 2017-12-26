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
using System.Drawing;
using System.Globalization;
using System.Data;

namespace GameApp
{
    /// <summary>
    /// Bounding information for collision detection between world objects.
    /// </summary>
    public class Bounds
    {
        /// <summary>
        /// Offset pixels in the X direction of the bounding data.
        /// This value is added to the object's X location.
        /// </summary>
        public float X
        {
            get { return xValue; }
            set { xValue = value; }
        }
        private float xValue = 0.0F;

        /// <summary>
        /// Offset pixels in the Y direction of the bounding data. 
        /// This value is added to the object's Y location.
        /// </summary>
        public float Y
        {
            get { return yValue; }
            set { yValue = value; }
        }
        private float yValue = 0.0F;

        /// <summary>
        /// Radius of the object.  When something passes within this
        /// radius, a collision is determined to have taken place.
        /// </summary>
        public float Radius
        {
            get { return radiusValue; }
            set { radiusValue = value; }
        }
        private float radiusValue = 0.0F;

        /// <summary>
        /// Create bounding information defined by the given DataRow.
        /// </summary>
        /// <param name="dr">DataRow containing bounding data</param>
        public Bounds(DataRow dr)
        {
            xValue = float.Parse((string)dr["BoundsX"], 
                CultureInfo.InvariantCulture);
            yValue = float.Parse((string)dr["BoundsY"], 
                CultureInfo.InvariantCulture);
            radiusValue = float.Parse((string)dr["BoundsRadius"], 
                CultureInfo.InvariantCulture);
        }

        /// <summary>
        /// Create bounding information defined by the given parameters.
        /// </summary>
        /// <param name="x">Pixel offset in X of the bounding data</param>
        /// <param name="y">Pixel offset in Y of the bounding data</param>
        /// <param name="r">Radius of the bounds</param>
        public Bounds(float x, float y, float r)
        {
            this.xValue = x;
            this.yValue = y;
            radiusValue = r;
        }

        /// <summary>
        /// Given the specified world location of the object owning this
        /// Bounds instance, check if a collision occurred with the other 
        /// specified world location and Bounds instance.
        /// </summary>
        /// <param name="worldX">X location of the object owning these
        /// Bounds</param>
        /// <param name="worldY">Y location of the object owning these
        /// Bounds</param>
        /// <param name="otherBounds">Bounds of other object</param>
        /// <param name="otherWorldX">X location of other object</param>
        /// <param name="otherWorldY">Y location of other object</param>
        /// <returns>true if colliding, false otherwise</returns>
        public bool CheckCollision(float worldX, float worldY,
            Bounds otherBounds, float otherWorldX, float otherWorldY)
        {
            // Offset the location of the bounding areas with the
            // world location
            float x1 = worldX + xValue;
            float y1 = worldY + yValue;

            float x2 = otherWorldX + otherBounds.X;
            float y2 = otherWorldY + otherBounds.Y;

            // Determine the distance between the objects
            float deltaX = x1 - x2;
            float deltaY = y1 - y2;

            // The distanced is sqrt(deltaX^2 + deltaY^2) so an optimization
            // is to square both sides, thus removing the sqrt
            float distSquared = deltaX * deltaX + deltaY * deltaY;
            float rSquared = (radiusValue + otherBounds.Radius);
            rSquared *= rSquared;

            // There is a collision of the distance between the objects
            // is less than the sum of the two objects' radii
            if (distSquared < rSquared)
                return true;

            return false;
        }
    }
}
