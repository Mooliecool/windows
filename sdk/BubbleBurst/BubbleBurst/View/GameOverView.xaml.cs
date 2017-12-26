using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media.Animation;
using BubbleBurst.ViewModel;
using ReactiveUI;

namespace BubbleBurst.View
{
    /// <summary>
    /// The modal dialog shown once a game has ended.
    /// </summary>
    public partial class GameOverView : UserControl, IViewFor<GameOverViewModel>
    {
        public GameOverViewModel ViewModel { get; set; }
        object IViewFor.ViewModel { get { return ViewModel; } set { ViewModel = (GameOverViewModel)value; } }

        readonly Storyboard _outroStoryboard;

        public GameOverView()
        {
            InitializeComponent();

            _outroStoryboard = _contentBorder.Resources["OutroStoryboard"] as Storyboard;

            base.DataContextChanged += this.HandleDataContextChanged;
        }

        void HandleDataContextChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            ViewModel = base.DataContext as GameOverViewModel;
        }

        void HandlePlayAgainHyperlinkClick(object sender, RoutedEventArgs e)
        {
            ViewModel.StartNewGame();
            _outroStoryboard.Begin(this);
        }

        void HandleOutroCompleted(object sender, EventArgs e)
        {
            ViewModel.Close();
        }
    }
}