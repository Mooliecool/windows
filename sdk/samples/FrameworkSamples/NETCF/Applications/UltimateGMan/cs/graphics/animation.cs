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
using System.IO;
using System.Drawing;
using System.Diagnostics;

namespace GraphicsLibrary
{
    /// <summary>
    /// Summary description for Animation.
    /// </summary>
    public class Animation : IDisposable
    {
        /// <summary>
        /// Specifies that no one-shot is playing.
        /// </summary>
        const float InvalidOneShotIndex = -1.0F;

        /// <summary>
        /// Width of an individual animation cell.
        /// </summary>
        public int CellWidth { get { return cellWidthValue; } }
        int cellWidthValue;

        /// <summary>
        /// Height of an individual animation cell.
        /// </summary>
        public int CellHeight { get { return cellHeightValue; } }
        int cellHeightValue;

        /// <summary>
        /// Number of cell rows in the bitmap.
        /// </summary>
        int numberRows;

        /// <summary>
        /// Number of cell columns in the bitmap.
        /// </summary>
        int numberColumns;

        /// <summary>
        /// Bitmap to be used for animating.
        /// </summary>
        public IBitmap Image { get { return bmp; } }
        IBitmap bmp;

        /// <summary>
        /// Index of the animation cell currently being displayed.  This
        /// index is specified within the context of the current cycle
        /// (between the start and end cells), not the absolute cell within
        /// the entire animation image.
        /// </summary>
        public int CurCell
        {
            get
            {
                if (curOneShotCell == InvalidOneShotIndex)
                    return (int)(curCellValue - startCell);

                return (int)(curOneShotCell - oneShotStartCell);

            }

            set
            {
                if (curOneShotCell == InvalidOneShotIndex)
                    curCellValue = (float)value + startCell;
                else
                    curOneShotCell = (float)value + oneShotStartCell;
            }
        }
        float curCellValue;

        /// <summary>
        /// Gets the maximum number of cells in the animation.
        /// </summary>
        public int NumberCells { get { return numberColumns * numberRows; } }

        /// <summary>
        /// Gets/Sets the rate of the animation in cells per second.
        /// </summary>
        public int AnimationRate
        {
            get
            {
                if (curOneShotCell == InvalidOneShotIndex)
                    return (int)cellsPerSecond;

                return (int)oneShotCellsPerSecond;
            }
            set
            {
                if (curOneShotCell == InvalidOneShotIndex)
                    cellsPerSecond = (float)value;
                else
                    oneShotCellsPerSecond = (float)value;
            }
        }
        float cellsPerSecond;

        /// <summary>
        /// Cached rectangle representing the drawing bounds of the current
        /// cell.
        /// </summary>
        internal Rectangle Region { get { return sourceRegion; } }
        Rectangle sourceRegion = new Rectangle(0, 0, 0, 0);

        /// <summary>
        /// Keep track of whether the Bitmap used by this animation was
        /// allocated or not.
        /// </summary>
        bool allocated = false;

        /// <summary>
        /// Specifies whether the animation has been properly initialized or
        /// not.
        /// </summary>
        public bool Init { get { return initialized; } }
        bool initialized = false;

        /// <summary>
        /// Index of the first cell in the animation cycle.
        /// </summary>
        float startCell;

        /// <summary>
        /// Index of the last cell in the animation cycle.
        /// </summary>
        float endCell;

        /// <summary>
        /// Index of the first cell in a one-shot animation cycle.
        /// After a one-shot is complete, it defaults back to the
        /// regular cycle.
        /// </summary>
        float oneShotStartCell;

        /// <summary>
        /// Index of the last cell in a one-shot animation cycle.
        /// After a one-shot is complete, it defaults back to the
        /// regular cycle.
        /// </summary>
        float oneShotEndCell;

        /// <summary>
        /// Rate of one-shot animation in cells per second.
        /// </summary>
        float oneShotCellsPerSecond;

        /// <summary>
        /// Index of cell currently being played on a one-shot
        /// animation.  If set to InvalidOneShotIndex then
        /// no one-shot is playing.
        /// </summary>
        float curOneShotCell = InvalidOneShotIndex;

        /// <summary>
        /// Used to determine if a one-shot has finished.
        /// </summary>
        public bool Done
        {
            get
            {
                return curOneShotCell ==
                    InvalidOneShotIndex;
            }
        }

        /// <summary>
        /// Create an animation from a Bitmap stream.
        /// </summary>
        /// <param name="bmpData">Stream representing source bitmap</param>
        /// <param name="graphics">Graphics object, needed for bitmap loading
        /// </param>
        /// <param name="numberRows">Number of rows of cells in the animation
        /// </param>
        /// <param name="numberColumns">Number of columns of cells in the
        /// animation</param>
        /// <param name="startCell">Index from which to start animating
        /// </param>
        /// <param name="cellWidth">Width of individual animation cells
        /// </param>
        /// <param name="cellHeight">Height of individual animation cells
        /// </param>
        /// <param name="cellsPerSecond">Rate of animation in cells per
        /// second</param>
        public Animation(string fileName, IGraphics graphics, int numberRows,
            int numberColumns, int startCell, int cellWidth, int cellHeight,
            int cellsPerSecond)
        {
            initialized = false;

            // Initialize the cell information
            this.cellWidthValue = cellWidth;
            this.cellHeightValue = cellHeight;
            this.numberRows = numberRows;
            this.numberColumns = numberColumns;

            startCell = 0;
            endCell = numberRows * numberColumns - 1;

            // Load and initialize the Bitmap object
            bmp = graphics.CreateBitmap(fileName, true);
            if (bmp == null)
            {
                initialized = false;
                return;
            }

            allocated = true;

            // Initialize timing information
            this.cellsPerSecond = (float)cellsPerSecond;

            // Initialize the draw region rectangle
            sourceRegion.Width = cellWidth;
            sourceRegion.Height = cellHeight;

            // Validate information for drawing the first cell
            Update(0.0F);

            initialized = true;
        }

        /// <summary>
        /// Create an animation that shares information from another
        /// animation.
        /// </summary>
        /// <param name="animation">Original Animation object</param>
        /// <param name="startCell">Index from which to start animating
        /// </param>
        /// <param name="cellsPerSecond">Rate of animation in cells per
        /// second</param>
        public Animation(Animation animation, int startCell,
            int cellsPerSecond)
        {
            initialized = false;

            // Initialize the cell information
            cellWidthValue = animation.cellWidthValue;
            cellHeightValue = animation.cellHeightValue;
            numberRows = animation.numberRows;
            numberColumns = animation.numberColumns;

            this.startCell = 0;
            endCell = numberRows * numberColumns - 1;

            // Copy the reference to the original bitmap
            bmp = animation.bmp;
            if (bmp == null)
            {
                initialized = false;
                return;
            }

            allocated = false;

            // Initialize timing information
            this.cellsPerSecond = (float)cellsPerSecond;

            // Initialize the draw region rectangle
            sourceRegion.Width = cellWidthValue;
            sourceRegion.Height = cellHeightValue;

            // Validate information for drawing the first cell
            Update(0.0F);

            initialized = true;
        }

        /// <summary>
        /// Set the start and end cells of the current animation cycle.  All
        /// cells will be played sequentially within the cycle.
        /// </summary>
        /// <param name="startCell">Index of starting cell</param>
        /// <param name="endCell">Index of ending cell</param>
        public void SetCycle(int startCell, int endCell, int animationRate)
        {
            curOneShotCell = InvalidOneShotIndex;
            this.startCell = (float)startCell;
            this.endCell = (float)endCell;
            cellsPerSecond = animationRate;
            curCellValue = startCell;
        }

        /// <summary>
        /// Start a one shot animation cycle.  This cycle will interrupt the
        /// current cycle, then resume the current cycle once finished.
        /// </summary>
        /// <param name="startCell">Index of start cell in cycle</param>
        /// <param name="endCell">Index of end cell in cycle</param>
        public void StartOneShot(int startCell, int endCell, int animationRate)
        {
            oneShotStartCell = (float)startCell;
            oneShotEndCell = (float)endCell;
            oneShotCellsPerSecond = (float)animationRate;
            curOneShotCell = oneShotStartCell;
        }

        /// <summary>
        /// Update the animation cell information.
        /// </summary>
        public void Update(float deltaTimeSeconds)
        {
            float deltaFrames = AnimationRate * deltaTimeSeconds;

            // Animations look bad if frames are skipped even though the
            // frame-rate might be too slow so don't allow it
            if (deltaFrames > 1.0F)
                deltaFrames = 1.0F;

            int currentCell;

            if (InvalidOneShotIndex != curOneShotCell)
            {
                curOneShotCell += deltaFrames;
                if (curOneShotCell >= oneShotEndCell + 1.0F)
                {
                    curOneShotCell = InvalidOneShotIndex;
                    currentCell = (int)curCellValue;
                }
                else
                {
                    currentCell = (int)curOneShotCell;
                }
            }
            else
            {
                curCellValue += deltaFrames;
                if (curCellValue >= endCell + 1.0F)
                    curCellValue = startCell;

                currentCell = (int)curCellValue;
            }

            Debug.Assert(numberColumns > 0,
                "Animation.Update: Invalid number of columns specified");

            // Set up the draw region rectangle for the current cell
            sourceRegion.X = (int)((currentCell % numberColumns) * cellWidthValue);
            sourceRegion.Y = (int)((currentCell / numberColumns) * cellHeightValue);
        }

        /// <summary>
        /// Clean up any memory allocated by the bitmap object.
        /// </summary>
        public void Dispose()
        {
            if (!allocated)
                return;

            if (bmp != null)
                bmp.Dispose();
        }
    }
}
