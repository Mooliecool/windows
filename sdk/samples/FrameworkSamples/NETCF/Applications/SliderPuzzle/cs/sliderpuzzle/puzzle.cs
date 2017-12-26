//=====================================================================
//  File:      Puzzle.cs
//
//  
//  ---------------------------------------------------------------------
//   Copyright (C) Microsoft Corporation.  All rights reserved.
//
//  This source code is intended only as a supplement to Microsoft
// Development Tools and/or on-line documentation.  See these other
//  materials for detailed information regarding Microsoft code samples.
//
//  THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//=====================================================================

using System;

namespace Microsoft.Samples.CompactFramework
{
    public class SliderPuzzle
    {
        // Default board size:
        public static readonly ushort DefaultDimension = 4;

        // Maximum number of reverse steps NewGame will take to randomize
        // the board configuration starting from the solution:
        public static readonly ushort MaxEntropy = 1000;

        // Rank and file linear dimensions:
        private ushort DimensionValue;

        // Each board square holds a tile id:
        //  1..(DimensionValue**2 - 1) or 0 == empty
        private int[][] Squares;

        // Table holding the current square index for each tile:
        private SquareIndex[] Tiles;

        // Number of rows and columns on board, Board is always square:
        public ushort Dimension
        {
            get { return DimensionValue; }
            
            set
            {
                if ( value < 2 )
                    throw new ArgumentOutOfRangeException("new value");

                if ( DimensionValue != value )
                {
                    DimensionValue = value;
                    Squares   = new int[DimensionValue][];
                    Tiles     = new SquareIndex[NumberOfSquares];

                    for ( int i = 0; i < DimensionValue; ++i )
                        Squares[i] = new int[DimensionValue];    

                    Reset();
                }
            }
        }

        // Number of squares on board:
        public ushort NumberOfSquares
        {
            get { return (ushort)(Dimension * Dimension); }
        }

        // Board location of the empty square:
        private SquareIndex EmptySquare
        {
            get { return Tiles[0]; }
        }

        // Board square index:
        private class SquareIndex
        {
            private int RowValue;
            private int ColValue;

            /// <summary>
            /// Construct a square index given row and column values.
            /// </summary>

            public SquareIndex( int row, int col )
            {
                RowValue = row;
                ColValue = col;
            }

            /// <summary>
            /// Row coordinate accessor.
            /// </summary>

            public int Row
            {
                get { return RowValue; }
                set { RowValue = value; }
            }

            /// <summary>
            /// Column coordinate accessor.
            /// </summary>

            public int Col
            {
                get { return ColValue; }
                set { ColValue = value; }
            }

            /// <summary>
            /// Calculate movement required to get from square a to square b.
            /// </summary>

            public static MoveVector Subtract( SquareIndex a, SquareIndex b )
            {
                return new MoveVector(a.RowValue - b.RowValue, 
                                a.ColValue - b.ColValue);
            }

            /// <summary>
            /// Apply movement to a square index to get a new square index.
            /// </summary>

            public SquareIndex Plus( MoveVector move )
            {
                return new
		    SquareIndex(this.RowValue + move.DeltaRow,
			            this.ColValue + move.DeltaCol );
            }
        }

        public enum Move {
            None  = -1,
            Up    =  0,
            Down  =  1,
            Left  =  2,
            Right =  3
        }

        private class MoveVector
        {
            private int DeltaRowValue;	// row displacement
            private int DeltaColValue;	// column displacement

            /// <summary>
            /// Construct a movement given row and column displacements.
            /// </summary>

            public MoveVector( int rowDelta, int columnDelta )
            {
                DeltaRowValue = rowDelta;
                DeltaColValue = columnDelta;
            }

            /// <summary>
            /// Row displacement accessor.
            /// </summary>

            public int DeltaRow
            {
                get { return DeltaRowValue; }
            }

            /// <summary>
            /// Column displacement accessor.
            /// </summary>

            public int DeltaCol
            {
                get { return DeltaColValue; }
            }

            /// <summary>
            /// Two moves are equal if their displacements are identical.
            /// </summary>

            public static bool operator ==( MoveVector a, MoveVector b )
            {
                return (a.DeltaRowValue == b.DeltaRowValue && 
                        a.DeltaColValue == b.DeltaColValue);
            }
            /// <summary>
            /// Two moves are equal if their displacements are identical.
            /// </summary>

            public static bool operator !=( MoveVector a, MoveVector b )
            {
                return !(a == b);
            }

            public override bool Equals(object o)
            {
                return (this == (MoveVector)o);
            }

            public override int GetHashCode()
            {
                return this.DeltaRowValue + this.DeltaColValue;
            }

            /// <summary>
            /// Produce vector sum of two moves.
            /// </summary>

            public static MoveVector operator +( MoveVector a, MoveVector b )
            {
                return new MoveVector(a.DeltaRowValue + b.DeltaRowValue, 
                                a.DeltaColValue + b.DeltaColValue);
            }

            /// <summary>
            /// Produce vector sum of two moves.
            /// </summary>

            public static MoveVector Add( MoveVector a, MoveVector b )
            {
                return new MoveVector(a.DeltaRowValue + b.DeltaRowValue, 
                                a.DeltaColValue + b.DeltaColValue);
            }

            /// <summary>
            /// Produce vector difference of two moves.
            /// </summary>

            public static MoveVector operator -( MoveVector a, MoveVector b )
            {
                return new MoveVector(a.DeltaRowValue - b.DeltaRowValue, 
                                a.DeltaColValue - b.DeltaColValue);
            }

            /// <summary>
            /// Produce vector difference of two moves.
            /// </summary>

            public static MoveVector Subtract( MoveVector a, MoveVector b )
            {
                return new MoveVector(a.DeltaRowValue - b.DeltaRowValue, 
                                a.DeltaColValue - b.DeltaColValue);
            }

            /// <summary>
            /// Two moves are inverse if their displacements sum to zero.
            /// </summary>

            public bool IsInverse( MoveVector other )
            {
                return (this + other) == MoveVector.None;
            }

            /// <summary>
            /// Predefined moves.
            /// </summary>

            public static readonly MoveVector None  = new MoveVector( 0,  0);
            public static readonly MoveVector Up    = new MoveVector(-1,  0);
            public static readonly MoveVector Down  = new MoveVector( 1,  0);
            public static readonly MoveVector Left  = new MoveVector( 0, -1);
            public static readonly MoveVector Right = new MoveVector( 0,  1);
            
            /// <summary>
            /// All possible moves that are orthogonal single steps. That is,
	    /// the list of moves that take a single step either vertically
	    /// or horizontally, but not both at once:
            /// </summary>

            public static readonly MoveVector[] OrthogonalSteps = new MoveVector[] { MoveVector.Up, MoveVector.Down, MoveVector.Left, MoveVector.Right };
        }

        /// <summary>
        /// Construct a puzzle and place tiles in the solved configuration.
        /// </summary>

        public SliderPuzzle( ushort dimension )
        {
            Dimension = dimension;
        }

        /// <summary>
        /// Is specified square on the board.
        /// </summary>

        private bool SquareIsOnTheBoard( SquareIndex selectedSquare )
        {
            if ( selectedSquare.Row >= 0 && selectedSquare.Row < Dimension
              && selectedSquare.Col >= 0 && selectedSquare.Col < Dimension )
                return true;
            
            return false;
        }

        /// <summary>
        /// Access a board square.
        /// </summary>

        private int this[ SquareIndex selectedSquare ]
        {
            get
            {
                return Squares[selectedSquare.Row][selectedSquare.Col];
            }
            set
            {
                Squares[selectedSquare.Row][selectedSquare.Col] = value;
                Tiles[value] = selectedSquare;
            }
        }

        /// <summary>
        /// Access board by row and column.
        /// </summary>

        public int this[ int row, int col ]
        {
            get
            {
                SquareIndex selectedSquare = new SquareIndex(row, col);
                return this[selectedSquare];
            }
            set 
            {
                SquareIndex selectedSquare = new SquareIndex(row, col);
                this[selectedSquare] = value;
            }
        }

        /// <summary>
        /// Serial board accessor: left-to-right, then top-to-bottom
        /// </summary>

        public int this[ int index ]
        {
            get
            {
                SquareIndex selectedSquare =
                    new SquareIndex(index / Dimension, index % Dimension);

                return this[selectedSquare];
            }
            set
            {
                SquareIndex selectedSquare =
                    new SquareIndex(index / Dimension, index % Dimension);

                this[selectedSquare] = value;
            }
        }

        /// <summary>
        /// Exchange contents of two squares.
        /// </summary>

        private void ExchangeTiles( SquareIndex squareA, SquareIndex squareB )
        {
            int tileId = this[squareA];
            this[squareA] = this[squareB];
            this[squareB] = tileId;
        }

        /// <summary>
        /// Place tiles on board in the "solved" configuration.
        /// </summary>

        public void Reset()
        {
            // Place each tile on its sequentially correct square
            // assuming left-to-right, top-to-bottom order:
            for ( int index = 0; index < NumberOfSquares; ++index )
                this[index] = index;    // tileId is square index
        }

        /// <summary>
        /// Place tiles on board such that resulting configuration is solvable
        /// and that the number of moves required to reach the solution is
        /// proportional to the specified "entropy" value:
        /// </summary>

        public void NewGame( int entropy )
        {
            if ( entropy <= 0 || entropy > MaxEntropy )
                throw new ArgumentOutOfRangeException("entropy");

            // Place tiles in solved positions:
            Reset();

            // Randomize the tile positions by "moving" the empty square
            // about the board without retracing steps:

            Random randGen = new Random();
            
            MoveVector lastMove = MoveVector.None;
            MoveVector[] possibleMoves = new MoveVector[MoveVector.OrthogonalSteps.Length];
                
            for ( int e = entropy; e != 0; --e )
            {
                // Find all possible moves from current square that are single
		// orthogonal steps that lead to on-board squares and that do
		// not retrace the last move:

                int maxPossibleMoves = 0;

                for ( int stepIdx = 0; stepIdx < possibleMoves.Length; ++stepIdx )
                {
                    MoveVector possibleMove = MoveVector.OrthogonalSteps[stepIdx];
                    
                    if ( !SquareIsOnTheBoard(EmptySquare.Plus(possibleMove))
                        || possibleMove.IsInverse(lastMove) )
                        continue;

                    possibleMoves[maxPossibleMoves++] = possibleMove;
                }

                // Select a possible move at random and move the empty square:
                MoveVector randomMove =
		    possibleMoves[randGen.Next(0, maxPossibleMoves)];

                ExchangeTiles(EmptySquare.Plus(randomMove), EmptySquare);
                
                lastMove = randomMove;
            }
        }

        public bool CanMove( Move move )
        {
            bool fCanMove = false;

            SquareIndex selectedSquare = this.EmptySquare.Plus(MoveVector.OrthogonalSteps[(int)move]);
            int tileId = this[selectedSquare];

            if (SquareIsOnTheBoard(selectedSquare))
            {
                // Can't move the "empty" tile:
                if (tileId != 0)
                {
                    // Find specified tile:
                    SquareIndex currentSquare = Tiles[tileId];

                    // Calculate required movement from current square
                    // to the empty square:
                    MoveVector movement = SquareIndex.Subtract(EmptySquare, currentSquare);

                    // See if movement is an orthogonal step:
                    foreach ( MoveVector orthogonalStep in MoveVector.OrthogonalSteps )
                        if ( movement == orthogonalStep )
                        {
                            fCanMove = true;
                            break;
                        }
                }
            }

            return(fCanMove);
        }

        public void DoMove( Move move )
        {
            SquareIndex selectedSquare = this.EmptySquare.Plus(MoveVector.OrthogonalSteps[(int)move]);

            MoveTile(this[selectedSquare]);
        }

        /// <summary>
        /// Move the specified tile from its current square to the empty square;
        /// returns true if the tile moves, false if tile can't be moved.
        /// </summary>

        public bool MoveTile( int tileId )
        {
            // Can't move the "empty" tile:
            if ( tileId == 0 )
                return false;

            // Find specified tile:
            SquareIndex selectedSquare = Tiles[tileId];

            // Calculate required movement from current square
            // to the empty square:
            MoveVector movement = SquareIndex.Subtract(EmptySquare, selectedSquare);

            // See if movement is an orthogonal step:
            foreach ( MoveVector orthogonalStep in MoveVector.OrthogonalSteps )
                if ( movement == orthogonalStep )
                {
                    ExchangeTiles(selectedSquare, EmptySquare);
                    return true;
                }

            // Can't move the tile from the specified square
            // to the empty square in a single orthogonal step:
            return false;
        }

        /// <summary>
        /// Returns true if all tiles are in their reset or "solved" positions.
        /// </summary>

        public bool IsSolved()
        {
            for ( int index = 0; index < NumberOfSquares; ++index )
                if ( index != this[index] )
                    return false;	// Fail if tileId does not equal index
            
            return true;
        }
    }
}
