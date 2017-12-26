using System;
using System.Globalization;
using System.Windows;
using System.Windows.Input;
using System.Reactive.Subjects;
using System.Reactive;
using System.Reactive.Linq;
using ReactiveUI;

namespace BubbleBurst.ViewModel
{
    /// <summary>
    /// The ViewModel responsible for supplying data and behavior for the game-over dialog.
    /// </summary>
    public class GameOverViewModel
    {
        readonly BubbleMatrixViewModel _bubbleMatrix;

        public string Title { get; private set; }
        public string Subtitle { get; private set; }

        /// <summary>
        /// Returns the command that exits the application.
        /// </summary>
        public IReactiveCommand QuitCommand { get; private set; }

        /// <summary>
        /// Raised when the game-over dialog should be closed.
        /// </summary>
        readonly Subject<Unit> _requestClose = new Subject<Unit>();
        public IObservable<Unit> RequestClose { get { return _requestClose.AsObservable(); } }

        internal GameOverViewModel(BubbleMatrixViewModel bubbleMatrix)
        {
            if (bubbleMatrix == null)
                throw new ArgumentNullException("bubbleMatrix");

            _bubbleMatrix = bubbleMatrix;

            if (bubbleMatrix.Bubbles.Count == 0)
            {
                this.Title = "CONGRATULATIONS!";
            }
            else
            {
                string theLetterS = bubbleMatrix.Bubbles.Count == 1 ? string.Empty : "S";
                this.Title = string.Format(CultureInfo.CurrentCulture, "{0} BUBBLE{1} LEFT", bubbleMatrix.Bubbles.Count, theLetterS);
            }

            this.Subtitle = "Most bubbles popped at once: " + bubbleMatrix.MostBubblesPoppedAtOnce;

            QuitCommand = new ReactiveCommand();
            QuitCommand.Subscribe(x => Application.Current.Shutdown());
        }

        /// <summary>
        /// Closes the game-over dialog.
        /// </summary>
        public void Close()
        {
            this.RaiseRequestClose();
        }

        /// <summary>
        /// Starts a new round of the BubbleBurst game.
        /// </summary>
        public void StartNewGame()
        {
            _bubbleMatrix.StartNewGame();
        }

        void RaiseRequestClose()
        {
            _requestClose.OnNext(Unit.Default);
            _requestClose.OnCompleted();
        }
    }
}