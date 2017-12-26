using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace CSWinFormSplashScreen
{
    public partial class SplashScreen2 : Form
    {
        public SplashScreen2()
        {
            InitializeComponent();
            Splash2Setting();
        }

        private void Splash2Setting()
        {
            this.FormBorderStyle = FormBorderStyle.None;
            this.BackgroundImage = Properties.Resources.SplashImage;
            this.StartPosition = FormStartPosition.CenterScreen;
        }
    }
}
