using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;

namespace CSRedirectConsole
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }
        // http://www.java2s.com/Code/CSharp/Development-Class/RedirectConsoleOut.htm

        private void button1_Click(object sender, EventArgs e)
        {
            ProcessStartInfo psi = new ProcessStartInfo("CppConsoleApplication.exe");
            psi.UseShellExecute = false;
            psi.CreateNoWindow = true;
            psi.RedirectStandardOutput = true;

            Process p = Process.Start(psi);
            
            using (StreamReader sr = p.StandardOutput)
            {
                this.label1.Text += sr.ReadLine();
            }
            MessageBox.Show(p.ExitCode.ToString());
        }
    }
}
