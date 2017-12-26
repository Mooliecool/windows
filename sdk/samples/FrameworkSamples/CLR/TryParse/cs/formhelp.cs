//-----------------------------------------------------------------------
//  This file is part of the Microsoft .NET SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//-----------------------------------------------------------------------

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace Microsoft.Samples
{

	public partial class FormHelp : System.Windows.Forms.Form
	{
		private System.ComponentModel.IContainer components = null;

		public FormHelp()
		{
			InitializeComponent();
		}

		private void FormHelp_Load(object sender, System.EventArgs e)
		{
			labelDesc.Text = 
				"Parse and TryParse take a string which is expected to be a " + Environment.NewLine +
				"representation " +
			   "for a specific type, and build the given type based " + Environment.NewLine + 
				"on the string value. For example, \"23\" can be parsed into an integer." + Environment.NewLine + Environment.NewLine +
			   "When Parse fails it will throw an exception, indicating that the string" + Environment.NewLine +
				"could not be successfully translated into the given type. Obviously, " + Environment.NewLine +
				"if your string can be parsed, this situation does not arise. But if it" + Environment.NewLine + 
				"cannot, then the exception route is a slow one." + Environment.NewLine + Environment.NewLine +
			   "TryParse takes a slightly different approach. If the string cannot be" + Environment.NewLine +
				"parsed, then false is returned, and the out parameter value becomes " + Environment.NewLine +
				"meaningless. This means that the act of throwing an exception is " + Environment.NewLine + 
				"avoided, and therefore, the slow route is avoided." + Environment.NewLine + Environment.NewLine +
			   "You can see this by attempting to Parse/TryParse strings into a given" + Environment.NewLine +
				"type, and changing what percentage of the parsing should succeed." + Environment.NewLine +
				"When at 100%, you will notice the APIs perform almost identically. " +Environment.NewLine +
			   "However, when you start reducing the success rate, Parse becomes" + Environment.NewLine +
				"markedly worse than TryParse." + Environment.NewLine + Environment.NewLine + "The outcome? We urge you to prefer TryParse over Parse!";

			labelUsagePatternParse.Text = "try {" + Environment.NewLine +
					"    int i = Int32.Parse(someString);" + Environment.NewLine +
					"    // take successful action, on i" + Environment.NewLine +
					"} catch (Exception ex) {" + Environment.NewLine +
					"    // take unsuccessful action" + Environment.NewLine +
					"    // this route is slow" + Environment.NewLine +
					"}";


			labelUsagePatternTryParse.Text = "int i;" + Environment.NewLine +
					"if (Int32.TryParse(someString, i)) {" + Environment.NewLine +
					"    // take successful action" + Environment.NewLine +
					"    // note, this includes processing i" + Environment.NewLine +
					"} else {" + Environment.NewLine +
					"    // take unsuccessful action." + Environment.NewLine +
					"    // Do NOT process outValue" + Environment.NewLine +
					"    // this route is generally as fast" + Environment.NewLine +
					"    // as the success route" + Environment.NewLine +
					"}";
		}

	}
}
