using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace MyCompany.CSVSToolbox
{
    // Set the display name and custom bitmap to use for this item.
    // The build action for the bitmap must be "Embedded Resource".
    [DisplayName("ToolboxMemberDemo")]
    [Description("Custom toolbox item from package LoadToolboxMembers.")]
    [ToolboxItem(true)]
    [ToolboxBitmap(typeof(MyControl), "MyControl.bmp")]
    public partial class MyControl : UserControl
    {
        public MyControl()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            OpenFileDialog opg = new OpenFileDialog();
            opg.Multiselect = false;
            if (opg.ShowDialog() == DialogResult.OK)
                this.textBox1.Text = opg.FileName;
        }
    }
}
