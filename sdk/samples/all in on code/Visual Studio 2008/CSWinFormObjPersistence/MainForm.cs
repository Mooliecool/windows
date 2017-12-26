/************************************* Module Header **************************************\
* Module Name:  MainForm.cs
* Project:      CSWinFormObjPersistence
* Copyright (c) Microsoft Corporation.
* 
* The Object Persistance sample demonstrates how to persist an object's data between 
* instances, so that the data can be restored the next time the object is instantiated. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 3/25/2009 3:00 PM Zhi-Xin Ye Created
\******************************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;
#endregion


namespace CSWinFormObjPersistence
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void frmObjPersistence_Load(object sender, EventArgs e)
        {
            if (File.Exists("Customer.bin"))
            {
                // If the file exists, restore the data from the file 
                using (Stream s = File.OpenRead("Customer.bin"))
                {
                    BinaryFormatter deserializer = new BinaryFormatter();
                    customer = (Customer)deserializer.Deserialize(s);
                }
            }
            else
            {
                // If the file does not exist,create a new instance of Customer class
                customer = new Customer();
            }

            this.textBox1.Text = customer.Name;
            this.textBox2.Text = customer.Age.ToString();
            this.textBox3.Text = customer.Address;
        }

        Customer customer;

        private void frmObjPersistence_FormClosing(object sender, FormClosingEventArgs e)
        {
            // When closing the form, save the data from the TextBoxes to a file
            customer.Name = this.textBox1.Text;
            customer.Age = Convert.ToInt32(this.textBox2.Text);
            customer.Address = this.textBox3.Text;

            using (Stream s = File.Create("Customer.bin"))
            {
                BinaryFormatter serializer = new BinaryFormatter();
                serializer.Serialize(s, customer);
            }
        }
    }


    // The Serializable attribute tells the compiler that everything 
    // within the class can be persisted to a file.
    [Serializable]
    public class Customer
    {
        private string _name;
        private int _age;
        private string _addr;

        // If you do not want to persist a field, mark it as NonSerialized.
        [NonSerialized()]
        private float _other;

        public string Name
        {
            get { return this._name; }
            set { this._name = value; }
        }

        public int Age
        {
            get { return this._age; }
            set { this._age = value; }
        }

        public string Address
        {
            get { return this._addr; }
            set { this._addr = value; }
        }

        public float Other
        {
            get { return this._other; }
            set { this._other = value; }
        }
    }
}
