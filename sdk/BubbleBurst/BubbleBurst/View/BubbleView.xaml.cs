using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using BubbleBurst.ViewModel;
using ReactiveUI;
using System.Reactive;
using System.Reactive.Linq;

namespace BubbleBurst.View
{
    /// <summary>
    /// Displays a bubble.
    /// </summary>
    public partial class BubbleView : Button, IViewFor<BubbleViewModel>
    {
        public BubbleViewModel ViewModel { get; set; }
        object IViewFor.ViewModel { get { return ViewModel; } set { ViewModel = (BubbleViewModel)value; } }

        public BubbleView()
        {
            InitializeComponent();

            var dataContextChanged = Observable.FromEventPattern<DependencyPropertyChangedEventHandler, DependencyPropertyChangedEventArgs>(
                h => base.DataContextChanged += h,
                h => base.DataContextChanged -= h);

            var mouseEnter = Observable.FromEventPattern<MouseEventHandler, MouseEventArgs>(
                h => base.MouseEnter += h,
                h => base.MouseEnter -= h);

            var mouseLeave = Observable.FromEventPattern<MouseEventHandler, MouseEventArgs>(
                h => base.MouseLeave += h,
                h => base.MouseLeave -= h);

            dataContextChanged.Subscribe(ev => this.HandleDataContextChanged(ev.EventArgs));
            mouseEnter.Subscribe(ev => this.HandleMouseEnter());
            mouseLeave.Subscribe(ev => this.HandleMouseLeave());
        }

        void HandleDataContextChanged(DependencyPropertyChangedEventArgs e)
        {
            ViewModel = e.NewValue as BubbleViewModel;
        }

        void HandleMouseEnter()
        {
            if (ViewModel != null)
            {
                ViewModel.SetFocus(true);
            }
        }

        void HandleMouseLeave()
        {
            if (ViewModel != null)
            {
                ViewModel.SetFocus(false);
            }
        }
    }
}