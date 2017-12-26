using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using BubbleBurst.ViewModel;
using ReactiveUI;
using System.Reactive;
using System.Reactive.Linq;
using System.Diagnostics;

namespace BubbleBurst.View
{
    /// <summary>
    /// The top-level View of the game, which contains a bubble matrix,
    /// game-over dialog, and the context menu.
    /// </summary>
    public partial class BubbleBurstView : UserControl, IViewFor<BubbleBurstViewModel>
    {
        // From XAML: ItemsControl BubbleMatrixView _bubbleMatrixView;
        public BubbleBurstViewModel ViewModel { get; set; }
        object IViewFor.ViewModel { get { return ViewModel; } set { ViewModel = (BubbleBurstViewModel)value; } }

        public BubbleBurstView()
        {
            InitializeComponent();

            SetupPreviewKeyDownHandler();

            ViewModel = base.DataContext as BubbleBurstViewModel;
        }

        /// <summary>
        /// Handle the key combos on keydown.  ex: Ctrl + Z for undo
        /// </summary>
        void SetupPreviewKeyDownHandler()
        {
            var loaded = Observable.FromEventPattern<RoutedEventHandler, RoutedEventArgs>(
                h => this.Loaded += h,
                h => this.Loaded -= h);

            // Take(1) will automatically unsubscribe from the event after loading.
            loaded.Take(1).Subscribe(x =>
            {
                var window = Window.GetWindow(this);
                var keyDown = Observable.FromEventPattern<KeyEventHandler, KeyEventArgs>(
                    h => window.PreviewKeyDown += h,
                    h => window.PreviewKeyDown -= h);

                keyDown.Subscribe(ev => this.HandleWindowPreviewKeyDown(ev.EventArgs));
            });
        }

        void HandleWindowPreviewKeyDown(KeyEventArgs e)
        {
            bool undoKeyPressed =
                Keyboard.Modifiers == ModifierKeys.Control &&
                e.Key == Key.Z;

            if (undoKeyPressed && ViewModel.UndoCommand.CanExecute(null))
            {
                ViewModel.UndoCommand.Execute(null);
                e.Handled = true;
            }
        }
    }
}