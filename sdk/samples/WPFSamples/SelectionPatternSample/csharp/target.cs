using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace SelectionPatternTarget
{
    public partial class Target : Form
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Target());
        }

        /// <summary>
        /// The target application for the SelectionPattern Sample.
        /// </summary>
        public Target()
        {
            InitializeComponent();
        }
    }
}