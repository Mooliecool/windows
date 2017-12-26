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
using System.Diagnostics;
using System.ComponentModel;


namespace PuzzleProject
{
	public partial class Puzzle : Window
	{
        private void PuzzleSourceChanged(object sender, SelectionChangedEventArgs e)
		{
			ComboBox cb = (ComboBox) sender;
			ComboBoxItem item = (ComboBoxItem) cb.SelectedValue;
			string tag = (string) item.Tag;

			_stylingPuzzle = true;

            if (_masterVideoElement != null)
            {
                _masterVideoElement.Stop();
                _masterVideoElement = null;
            }

			switch (tag)
			{
				case "Untemplated":
					_stylingPuzzle = false;
					break;

				case "StaticBitmap":
					{
                        Image masterImage = (Image)this.Resources["MasterImage"];
                        _elementToChopUp = masterImage;
                        BitmapSource bitmap = (BitmapSource)masterImage.Source;
                        _puzzleSize = new Size(bitmap.PixelWidth / 1.5, bitmap.PixelHeight / 1.5);
                    }
					break;

				case "StaticVectorContent":
					{
						_elementToChopUp = new PuzzleProject.VectorContent();
						_puzzleSize = new Size(500, 500);
					}
					break;

                case "VideoContent":
                    {
                        _elementToChopUp = PrepareVideoElement(out _puzzleSize);
                    }
                    break;

				case "AnimateVectorContent":
					{
						VectorContent vc = new VectorContent();
                        
                        // Must explicitly start storyboards that only appear in VisualBrushes
                        vc.BeginStoryboard(vc.myStoryboard);  
                        
						_elementToChopUp = vc;
						_puzzleSize = new Size(500, 500);
					}

					break;

                case "Document":
                    {
                        FrameworkElement elt = new PuzzleProject.MyDocument();
                        _elementToChopUp = elt;
                        _puzzleSize = new Size(elt.Width, elt.Height);
                    }
                    break;

                case "FormsContent":
                    {
                        FrameworkElement elt = new PuzzleProject.FormContent();
                        _elementToChopUp = elt;
                        _puzzleSize = new Size(700, 500);
                    }
                    break;

				case "SpinningCube":
					{
                        VectorContent vc = new PuzzleProject.VectorContent();
                        vc.BeginStoryboard(vc.myStoryboard);  // Must explicitly start storyboards that only appear in VisualBrushes
                        Image masterImage = (Image)this.Resources["TableImage"];

                        SpinningCube sc = new SpinningCube();

                        sc.CubeMaterial.Brush = new ImageBrush(masterImage.Source); 
                        sc.CubeMaterial2.Brush = new VisualBrush(vc);

                        Storyboard myStoryboard = (Storyboard)sc.Resources["myStoryboard"];
                        sc.BeginStoryboard(myStoryboard);  // Must explicitly start storyboards that only appear in VisualBrushes
                        _elementToChopUp = sc;
						_puzzleSize = new Size(500, 500);
					}

					break;

				default:
                    Debug.Assert(false, "Unexpected Puzzle Source");
					break;
			}

			NewPuzzleGrid();

		}

        
        private UIElement PrepareVideoElement(out Size resultingSize)
        {
            _masterVideoElement = (MediaElement)this.Resources["MasterVideo"];
            _masterVideoElement.UnloadedBehavior = MediaState.Manual;


            resultingSize = new Size(_masterVideoElement.Width, _masterVideoElement.Height);

            _masterVideoElement.Play();

            return _masterVideoElement;
        }

        private void OnMoveMade(object sender, HandledEventArgs e)
        {
            // Blur or unblur based on whether the move was a valid one.
            BlurBitmapEffect blur = (BlurBitmapEffect)ControlPanel.BitmapEffect;
            if (blur != null)
            {
                if (e.Handled)
                {
                    if (blur.Radius >= 2.0)
                    {
                        blur.Radius -= 2.0;
                    }
                    StatusLabel.Content = "";
                }
                else
                {
                    blur.Radius += 2.0;
                    StatusLabel.Content = "Bad Move!";
                }
            }
        }


        private void NewPuzzleGrid()
        {
            if (_puzzleGrid != null)
            {
                PuzzleHostingPanel.Children.Remove(_puzzleGrid);
            }
            _puzzleGrid = new PuzzleGrid();
            _puzzleGrid.PuzzleWon += delegate(object sender, EventArgs e)
            {
                StatusLabel.Content = "Got it!!!";
            };

            _puzzleGrid.MoveMade += new EventHandler<HandledEventArgs>(OnMoveMade);

            _puzzleGrid.IsApplyingStyle = _stylingPuzzle;
            _puzzleGrid.NumRows = _numRows;

            _puzzleGrid.ElementToChopUp = _elementToChopUp;
            _puzzleGrid.PuzzleSize = _puzzleSize;

            _puzzleGrid.ShowNumbers(ChkShowNumbers.IsChecked.Value);

            _puzzleGrid.ShouldAnimateInteractions = ChkShowAnimations.IsChecked.Value;

            PuzzleHostingPanel.Children.Add(_puzzleGrid);
            StatusLabel.Content = "New " + _numRows + "x" + _numRows + " game";
        }
        
        #region Less Consequential Event Handlers

        private void OnMixUp(object sender, RoutedEventArgs e)
		{
			_puzzleGrid.MixUpPuzzle();
		}

		private void OnResetPuzzle(object sender, RoutedEventArgs e)
		{
            NewPuzzleGrid();
		}

		private void OnShowHideNumbers(object sender, RoutedEventArgs e)
		{
			CheckBox cb = (CheckBox)sender;
			_puzzleGrid.ShowNumbers(cb.IsChecked.Value);
		}

        private void OnChkAnimation(object sender, RoutedEventArgs e)
        {
            CheckBox cb = (CheckBox)sender;
            _puzzleGrid.ShouldAnimateInteractions = cb.IsChecked.Value;
        }
        private void PuzzleDimensionsChanged(object sender, SelectionChangedEventArgs e)
        {
            ComboBox cb = (ComboBox)sender;
            ComboBoxItem item = (ComboBoxItem)cb.SelectedValue;
            string tag = (string)item.Tag;
            _numRows = Int32.Parse(tag);
            NewPuzzleGrid();
        }

        #endregion

        #region Private Data and Boilerplate

        public Puzzle()
        {
            InitializeComponent();
        }
        
        private PuzzleGrid _puzzleGrid;
        private UIElement _elementToChopUp;
        private Size _puzzleSize;
        private bool _stylingPuzzle;
        private int _numRows;

        private MediaElement _masterVideoElement = null;

        #endregion
    }
}