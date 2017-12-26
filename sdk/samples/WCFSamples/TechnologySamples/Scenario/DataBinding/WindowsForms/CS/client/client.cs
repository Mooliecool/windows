//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Windows.Forms;

namespace Microsoft.ServiceModel.Samples
{
	static class client
	{
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main()
		{
			Application.EnableVisualStyles();
			Application.Run(new Form1());
		}
	}
}