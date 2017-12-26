using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace CustomNavigationWindowChromeSample
{
    public partial class Page3 : Page, IProvideCustomContentState
    {
        public Page3()
        {
            InitializeComponent();
        }

        #region IProvideCustomContentState Members

        public CustomContentState GetContentState()
        {
            return ContentImageCustomContentState.GetContentImageCustomContentState(this, (int)this.ActualWidth, (int)this.ActualHeight);
        }

        #endregion
    }
}