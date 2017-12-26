using System;
using System.Collections.Generic;
using System.Linq;
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

namespace Company.VSPackageWPFToolWindow
{
    /// <summary>
    /// Interaction logic for WPFControl.xaml
    /// </summary>
    public partial class WPFControl : UserControl
    {
        public WPFControl()
        {
            InitializeComponent();
        }

        public TreeView WPFTreeView
        {
            get
            {
                return this.treeView;
            }
        }
    }
}
