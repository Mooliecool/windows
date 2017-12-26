//-----------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
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
//
/*=====================================================================
  File:      Tester.cs

  Summary:   A simple DLL that exposes a function or two to be tested
             by the invoke sample

=====================================================================*/


using System;
using System.Windows.Forms;
using System.ComponentModel;

// Some test methods to try from the Invoke sample
namespace Microsoft.Samples
{
	public class SomeType
	{
		public static double AddTwoDoubles(double num1, double num2)
		{
			return num1 + num2;
		}

		public DialogResult ShowMessage(String message)
		{
			DialogResult DR_Ret = DialogResult.Cancel; // Assume action was cancelled, by default

			try
			{
				DR_Ret = MessageBox.Show(message);
			}
			catch (InvalidOperationException e)
			{
				MessageBox.Show("ERROR: " + e.Message, System.Diagnostics.Process.GetCurrentProcess().MainModule.ModuleName, System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Error);
			}
			return DR_Ret;
		} // CodeFunction

		public DialogResult ShowMessage(String message, String caption)
		{
			DialogResult DR_Ret = DialogResult.Cancel; // Assume action was cancelled, by default

			try
			{
				DR_Ret = MessageBox.Show(message, caption);
			}
			catch (InvalidOperationException e)
			{
				MessageBox.Show("ERROR: " + e.Message, System.Diagnostics.Process.GetCurrentProcess().MainModule.ModuleName, System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Error);
			}
			return DR_Ret;
		} // CodeFunction

		public DialogResult ShowMessage(String message, String caption, int style)
		{
			DialogResult DR_Ret = DialogResult.Cancel; // Assume action was cancelled, by default

			try
			{
				DR_Ret = MessageBox.Show(message, caption, (MessageBoxButtons)style);
			}
			catch (InvalidEnumArgumentException e)
			{
				MessageBox.Show("ERROR: " + e.Message, System.Diagnostics.Process.GetCurrentProcess().MainModule.ModuleName, System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Error);
			}
			catch (InvalidOperationException e)
			{
				MessageBox.Show("ERROR: " + e.Message, System.Diagnostics.Process.GetCurrentProcess().MainModule.ModuleName, System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Error);
			}
			return DR_Ret;
		} // CodeFunction
	} // class
} // namespace

