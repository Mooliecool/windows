using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace CommandLinkTestApp
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void OnFormLoad(object sender, EventArgs e)
        {
        }

        private void OnCallOperation(object sender, EventArgs e)
        {
            if (cmbOp.SelectedItem == null) { return; }

            txtRetVal.Text = "";

            switch (cmbOp.SelectedItem.ToString())
            {
                case "Set Command Text":
                    commandLink1.Text = txtParam.Text;
                    txtParam.Text = "";
                    break;
                case "Set Note Text":
                    commandLink1.NoteText = txtParam.Text;
                    txtParam.Text = "";
                    break;
                case "Get Note Text":
                    txtRetVal.Text = commandLink1.NoteText;
                    if (txtRetVal.Text == String.Empty)
                    {
                        txtRetVal.Text = "(empty)";
                    }
                    break;
                case "Set Shield Icon":
                    commandLink1.ShieldIcon = (txtParam.Text == "1") || (txtParam.Text.ToUpper() == "TRUE");
                    break;
                default:
                    break;
            }
        }

        private void OnClick(object sender, EventArgs e)
        {
            MessageBox.Show("Command Link has been clicked");
        }
    }
}