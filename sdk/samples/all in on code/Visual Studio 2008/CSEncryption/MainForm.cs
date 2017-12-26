using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace CSEncryption
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void btnSymmetricAlgorithmDemo_Click(object sender, EventArgs e)
        {
            SymmetricAlgorithmForm symmetricAlgorithmForm = new SymmetricAlgorithmForm();
            symmetricAlgorithmForm.ShowDialog();
        }

        private void btnAsymmetricAlgorithm_Click(object sender, EventArgs e)
        {
            AsymmetricAlgorithmForm asymmetricAlgorithmform = new AsymmetricAlgorithmForm();
            asymmetricAlgorithmform.ShowDialog();
        }
    }
}