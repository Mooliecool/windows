using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Shapes;
using System.Windows.Media.Imaging;
using System.Windows.Media.Animation;
using System.Windows.Media.Effects;
using System.ComponentModel;
using System.Collections.Generic;
using System.Diagnostics;


namespace PuzzleProject
{


	public partial class PuzzleGrid : Grid
	{
		#region Grid Setup Logic
		
		public PuzzleGrid()
		{
			InitializeComponent();

			// Centralize handling of all clicks in the puzzle grid.
			this.AddHandler(Button.ClickEvent, new RoutedEventHandler(OnPuzzleButtonClick));
		}

		private void SetupThePuzzleGridStructure()
		{
			_puzzleLogic = new PuzzleLogic(_numRows);

			// Define rows and columns in the Grid
			for (int row = 0; row < _numRows; row++)
			{
				RowDefinition r = new RowDefinition();
				r.Height = GridLength.Auto;
				this.RowDefinitions.Add(r);

				ColumnDefinition c = new ColumnDefinition();
				c.Width = GridLength.Auto;
				this.ColumnDefinitions.Add(c);
			}

			Style buttonStyle = (Style)this.Resources["PuzzleButtonStyle"];

			// Now add the buttons in
			int i = 1;
			for (int row = 0; row < _numRows; row++)
			{
				for (int col = 0; col < _numRows; col++)
				{
					// lower right cell is empty
					if (_numRows != 1 && row == _numRows - 1 && col == _numRows - 1)
					{
						continue;
					}
					Button b = new Button();
                    b.FontSize = 24;

                    // Styling comes in only here...
					if (_styling)
					{
						b.Style = buttonStyle;
					}

					b.SetValue(Grid.RowProperty, row);
					b.SetValue(Grid.ColumnProperty, col);
					b.Content = i.ToString();
					i++;
					this.Children.Add(b);
				}
			}
		}

		#endregion

		#region Brush Tiling Logic

		private void PuzzleGridLoaded(object sender, RoutedEventArgs e) 
		{
			if (_styling)
			{
				this.Width = _masterPuzzleSize.Width;
				this.Height = _masterPuzzleSize.Height;

				float edgeSize = 1.0f / _numRows;
				double pieceRowHeight = _masterPuzzleSize.Height / _numRows;
				double pieceColWidth = _masterPuzzleSize.Width / _numRows;

				// Set up viewbox appropriately for each tile.
				foreach (Button b in this.Children)
				{
					Border root = (Border)b.Template.FindName("TheTemplateRoot", b);

					int row = (int)b.GetValue(Grid.RowProperty);
					int col = (int)b.GetValue(Grid.ColumnProperty);

                    VisualBrush vb = new VisualBrush(_elementForPuzzle);
					vb.Viewbox = new Rect(col * edgeSize, row * edgeSize, edgeSize, edgeSize);
					vb.ViewboxUnits = BrushMappingMode.RelativeToBoundingBox;

					root.Background = vb;
					root.Height = pieceRowHeight;
					root.Width = pieceColWidth;
				}
			}
		}

		#endregion

		#region Interaction Logic

		private void OnPuzzleButtonClick(object sender, RoutedEventArgs e)
		{
			Button b = e.Source as Button;
			if (b != null)
			{
				int row = (int)b.GetValue(Grid.RowProperty);
				int col = (int)b.GetValue(Grid.ColumnProperty);

				MoveStatus moveStatus = _puzzleLogic.GetMoveStatus(row, col);

                if (MoveMade != null)
                {
                    MoveMade(this, new HandledEventArgs(moveStatus != MoveStatus.BadMove));
                }

                if (moveStatus != MoveStatus.BadMove)
				{
					if (!_styling || !_shouldAnimateInteractions)
					{
						// Not templated, just move piece
						MovePiece(b, row, col);
					}
					else
					{
						AnimatePiece(b, row, col, moveStatus);
					}
				}

			}
		}


		// Assumed to be a valid move.
		private void AnimatePiece(Button b, int row, int col, MoveStatus moveStatus)
		{
            FrameworkElement root = (FrameworkElement)b.Template.FindName("TheTemplateRoot", b);

			double distance;
            bool isMoveHorizontal;

            Debug.Assert(moveStatus != MoveStatus.BadMove);
            if (moveStatus == MoveStatus.Left || moveStatus == MoveStatus.Right)
			{
                isMoveHorizontal = true;
				distance = (moveStatus == MoveStatus.Left ? -1 : 1) * root.Width;
			}
			else
			{
                isMoveHorizontal = false;
                distance = (moveStatus == MoveStatus.Up ? -1 : 1) * root.Height;
			}

			// pull the animation after it's complete, because we move change Grid cells.
			DoubleAnimation slideAnim = new DoubleAnimation(distance, TimeSpan.FromSeconds(0.3), FillBehavior.Stop);
			slideAnim.CurrentStateInvalidated += delegate(object sender2, EventArgs e2)
			{
				// Anonymous delegate -- invoke when done.
				Clock clock = (Clock)sender2;
				if (clock.CurrentState != ClockState.Active)
				{
					// remove the render transform and really move the piece in the Grid.
					MovePiece(b, row, col);
				}
			};

            DependencyProperty directionProperty = 
                isMoveHorizontal ? TranslateTransform.XProperty : TranslateTransform.YProperty;

            root.RenderTransform.BeginAnimation(directionProperty, slideAnim);

		}


		// Assumed to be a valid move.
		private void MovePiece(Button b, int row, int col)
		{
			PuzzleCell newPosition = _puzzleLogic.MovePiece(row, col);

			b.SetValue(Grid.ColumnProperty, newPosition.Col);
			b.SetValue(Grid.RowProperty, newPosition.Row);

			if (_puzzleLogic.CheckForWin())
			{
				if (PuzzleWon != null)
				{
					PuzzleWon(this, EventArgs.Empty);
				}
			}
        }

        #endregion

        #region Less Consequential Portions

        public void MixUpPuzzle()
		{
			_puzzleLogic.MixUpPuzzle();

			short cellNumber = 1;
			foreach (Button b in this.Children)
			{
				PuzzleCell location = _puzzleLogic.FindCell(cellNumber++);
				b.SetValue(Grid.ColumnProperty, location.Col);
				b.SetValue(Grid.RowProperty, location.Row);
			}
		}

		public void ShowNumbers(bool showNumbers)
		{
			SolidColorBrush nlb = ((SolidColorBrush)this.Resources["NumberLabelBrush"]).Clone();
			nlb.Color = showNumbers ? Colors.Yellow : Colors.Transparent;
            this.Resources["NumberLabelBrush"] = nlb;
		}

		#endregion

		#region Public Properties

		public int NumRows
		{
			get { return _numRows; }
			set
			{
				// Only support setting this once per PuzzleGrid instance.
				if (_numRows != -1)
				{
					throw new InvalidOperationException("NumRows already initialized for this PuzzleGrid instance.");
				}
				else
				{
					_numRows = value;
					CheckToSetup();
				}
			}
		}

		public UIElement ElementToChopUp
		{
			get { return _elementForPuzzle; }
			set
			{
                if (_elementForPuzzle != null)
				{
                    throw new InvalidOperationException("ElementForPuzzle already initialized for this PuzzleGrid instance.");
				}
				else
				{
                    _elementForPuzzle = value;
					CheckToSetup();
				}
			}
		}

		public bool IsApplyingStyle
		{
			get { return _styling; }
			set { _styling = value; }
		}

        public bool ShouldAnimateInteractions
        {
            get { return _shouldAnimateInteractions; }
            set { _shouldAnimateInteractions = value; }
        }

		public Size PuzzleSize
		{
			get { return _masterPuzzleSize; }
			set
			{
				if (!_masterPuzzleSize.IsEmpty)
				{
					throw new InvalidOperationException("SizeForPuzzle already initialized for this PuzzleGrid instance.");
				}
				else
				{
					_masterPuzzleSize = value;
					CheckToSetup();
				}
			}
		}

		public event EventHandler PuzzleWon;
        public event EventHandler<HandledEventArgs> MoveMade;
        
		private void CheckToSetup()
		{
			if (_numRows != -1 && (!_styling || _elementForPuzzle != null) && !_masterPuzzleSize.IsEmpty)
			{
				SetupThePuzzleGridStructure();
			}
		}


		#endregion

		#region Private Data

		private PuzzleLogic _puzzleLogic;
		private Size _masterPuzzleSize = Size.Empty;
		private UIElement _elementForPuzzle;
		private int _numRows = -1;
		private bool _styling = true;
        private bool _shouldAnimateInteractions = false;

		#endregion

	}

}