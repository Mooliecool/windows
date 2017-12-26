/********************************** Module Header **********************************\
* Module Name:  MainForm.cs
* Project:      CSHexStringByteArrayConverter
* Copyright (c) Microsoft Corporation.
*
* This sample demonstrates how to convert byte array to hex string and vice 
* versa. For example, 
* 
*    "FF00EE11" <--> { FF, 00, EE, 11 }
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
* EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
* MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***********************************************************************************/

using System;
using System.Windows.Forms;
using System.Globalization;
using System.Collections.Generic;


namespace CSHexStringByteArrayConverter
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Convert hex string into byte array.
        /// </summary>
        private void btnConvertHexStringToByteArray_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrWhiteSpace(tbHexStringInput.Text))
            {
                MessageBox.Show("Please input hex strings that you want to convert!");
                return;
            }
            if (!HexStringByteArrayConverter.VerifyHexString(tbHexStringInput.Text))
            {
                MessageBox.Show("The hex string is not in the right format.");
                return;
            }

            // Convert the hex string to a byte array
            byte[] result = HexStringByteArrayConverter.HexStringToBytes(tbHexStringInput.Text);
            this.cmbByteArrayResult.DataSource = result;
        }

        /// <summary>
        /// Convert bytes array into hex strings.
        /// </summary>
        private void btnConvertByteArrayToHexString_Click(object sender, EventArgs e)
        {
            if (this.cmbByteArrayInput.Items.Count == 0)
            {
                MessageBox.Show("Please input bytes array that you want to convert!");
                return;
            }

            byte[] bytes = new byte[this.cmbByteArrayInput.Items.Count];
            for (int i = 0; i < this.cmbByteArrayInput.Items.Count; i++)
            {
                bytes[i] = (byte)this.cmbByteArrayInput.Items[i];
            }

            tbHexStringResult.Text = HexStringByteArrayConverter.BytesToHexString(bytes);
        }

        /// <summary>
        /// Add a byte into the ComboBox control.
        /// </summary>
        private void btnAddByte_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrWhiteSpace(this.tbByteToAdd.Text))
            {
                MessageBox.Show("Please input the byte.");
                return;
            }

            byte byteToAdd;
            if (!byte.TryParse(this.tbByteToAdd.Text, NumberStyles.AllowHexSpecifier, null,
                out byteToAdd))
            {
                MessageBox.Show("The byte input is not in the right format.");
                return;
            }

            this.cmbByteArrayInput.Items.Add(byteToAdd);
            this.tbByteToAdd.Text = "";
            this.cmbByteArrayInput.SelectedIndex = 0;
        }

        /// <summary>
        /// Clear the ComboBox control.
        /// </summary>
        private void btnClearBytes_Click(object sender, EventArgs e)
        {
            this.cmbByteArrayInput.Items.Clear();
        }

        /// <summary>
        /// Copy the data into Clipboard.
        /// </summary>
        private void btnCopytoClipboard_Click(object sender, EventArgs e)
        {
            if (cmbByteArrayResult.Items.Count != 0)
            {
                // Convert the combobox data to a serializable string collection.
                List<byte> items = new List<byte>();
                foreach (byte b in this.cmbByteArrayResult.Items)
                {
                    items.Add(b);
                }

                IDataObject ido = new DataObject();
                ido.SetData(typeof(ComboBox).FullName, true, items);
                Clipboard.SetDataObject(ido, false);
            }
        }

        /// <summary>
        /// Paste the data that comes from clipboard to Textbox control.
        /// </summary>
        private void btnPasteFromClipboard_Click(object sender, EventArgs e)
        {
            IDataObject ido = Clipboard.GetDataObject();
            string format = typeof(ComboBox).FullName;
            if (ido.GetDataPresent(format))
            {
                List<byte> items = ido.GetData(format) as List<byte>;
                if (items != null)
                {
                    this.cmbByteArrayInput.Items.Clear();
                    foreach (byte b in items)
                    {
                        this.cmbByteArrayInput.Items.Add(b);
                    }
                    this.cmbByteArrayInput.SelectedIndex = 0;
                }
            }
        }
    }
}
