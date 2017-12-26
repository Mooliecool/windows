//<snippet100>
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace WmpAxLib
{
    public partial class WmpAxControl : UserControl
    {
        public WmpAxControl()
        {   
            InitializeComponent();
        }

        public void Play(string url)
        {
            this.axWindowsMediaPlayer1.URL = url;
        }
    }
}
//</snippet100>