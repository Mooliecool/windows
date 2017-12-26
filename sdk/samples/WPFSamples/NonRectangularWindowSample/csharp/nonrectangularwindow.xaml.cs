using System;
using System.Windows;
using System.Windows.Input;

namespace NonRectangularWindowSample {

  public partial class NonRectangularWindow : Window {

    public NonRectangularWindow() {
      InitializeComponent();
    }

    void NonRectangularWindow_MouseLeftButtonDown(object sender, MouseButtonEventArgs e) {
      this.DragMove();
    }

    void closeButtonRectangle_Click(object sender, RoutedEventArgs e) {
      this.Close();
    }
  }
}