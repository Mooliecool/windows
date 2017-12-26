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
using System.Globalization;
using System.Security;
using System.Security.Permissions;
using System.Diagnostics;
using System.Runtime.InteropServices;

[assembly: CLSCompliant(true)]
[assembly: ComVisible(false)]
[assembly: FileIOPermissionAttribute(SecurityAction.RequestMinimum)]
namespace Microsoft.Samples
{
	class ConsoleDemo
	{
		string loginName;
		int mainCursorLeft;
		int mainCursorTop;
		SecureString password;

		[STAThread]
		static void Main(string[] args)
		{
			ConsoleDemo cd = new ConsoleDemo();
			cd.Run();
		}

		void Run() {
			// set the buffersize to a width that allows us to move the screen...
			// ShowPoint: SetWindowSize, SetBufferSize
			Console.SetWindowSize(45, 16);
			Console.SetBufferSize(Console.WindowWidth * 2, Console.WindowHeight);

			// ShowPoint: Change Title, Change CursorSize
			Console.Title = "Fanatical Health Entry System";
			Console.CursorSize = 100;

			while (true)
			{
				PaintMainScreen();

				// ShowPoint: get a key, asit's pressed
				switch (Console.ReadKey().KeyChar)
				{
					case '1':
						if (LogInOut())
						{
							Console.Clear();
						} else
						{
							Console.CursorVisible = false;
							Console.Clear();
							Console.SetCursorPosition(2, 7);
							Console.ForegroundColor = ConsoleColor.White;
							Console.Write("Login unsuccessful: canceling application");
							System.Threading.Thread.Sleep(2500);
							return;
						}
						break;
					case '2':
						if (CheckLoggedIn())
						{
							PaintPatientScreen();
						}
						break;
					case '3':
						if (CheckLoggedIn())
						{
							OpenDoctorFile();
						}
						break;
					case '4':
						if (CheckLoggedIn())
						{
							OpenNurseFile();
						}
						break;
					case '5':
						return;
					default:
						Console.Clear();
						break;
				}
			}
		}

		bool CheckLoggedIn()
		{
			if (loginName == null)
			{
				// ShowPoint: Clear the screen
				Console.Clear();
				PaintBorder();
				Console.CursorVisible = false;
				Console.SetCursorPosition(5, 5);
				Console.ForegroundColor = ConsoleColor.Cyan;
				Console.Write("You must login first!!!");
				// ShowPoint: beep!
				Console.Beep();
				Stopwatch sw = new Stopwatch();
				sw.Start();
				while (sw.ElapsedMilliseconds < 2500) {}
				return false;
			} else
			{
				return true;
			}
		}

		void PaintMainScreen()
		{
			// ShowPoint: Make the cursor invisible
			Console.CursorVisible = false;
			Console.Clear();
			PaintBorder();
			Console.SetCursorPosition(5, 1);
			int top = 1;
			WriteEntry("", ConsoleColor.Magenta, false, top++);
			WriteEntry("Welcome to Fanatical Health!", ConsoleColor.Green, true, top++);

			if (loginName == null)
			{
				WriteEntry("", ConsoleColor.Magenta, false, top++);
				WriteEntry(" You are currently not logged in", ConsoleColor.Red, true, top++);
			} else
			{
				WriteEntry("", ConsoleColor.Magenta, false, top++);
				WriteEntry(" You are currently logged in as " + loginName, ConsoleColor.Red, true, top++);
			}

			WriteEntry("", ConsoleColor.Magenta, false, top++);
			WriteEntry(" Options:", ConsoleColor.Yellow, false, top++);
			WriteEntry(String.Format(CultureInfo.InvariantCulture, " 1. {0}", loginName == null ? "Login" : "Logout"), ConsoleColor.White, false, top++);

			WriteEntry(" 2. Patient Data", loginName == null ? ConsoleColor.Gray : ConsoleColor.White, false, top++);
			WriteEntry(" 3. Open Doctor Data File", loginName == null ? ConsoleColor.Gray : ConsoleColor.White, false, top++);
			WriteEntry(" 4. Open Nurse Data File", loginName == null ? ConsoleColor.Gray : ConsoleColor.White, false, top++);
			WriteEntry(" 5. Exit", ConsoleColor.White, false, top++);

			Console.ForegroundColor = ConsoleColor.White;
			Console.SetCursorPosition(1, Console.WindowHeight - 1);
			Console.Write("Please enter a selection (1 - 5) ==> ");
			mainCursorLeft = Console.CursorLeft;
			mainCursorTop = Console.CursorTop;
			Console.CursorVisible = true;
		}

		void PaintBorder()
		{
			Console.SetCursorPosition(0, 0);
			// ShowPoint: Change the screen color
			Console.ForegroundColor = ConsoleColor.Magenta;
			Console.Write("*********************************************");
			Console.SetCursorPosition(0, Console.WindowHeight - 3);
			Console.Write("*********************************************");
			for (int i = 1; i < Console.WindowHeight - 2; i++)
			{
				Console.SetCursorPosition(0, i);
				Console.Write("*");
				Console.SetCursorPosition(Console.WindowWidth - 1, i);
				Console.Write("*");
			}
		}

		void WriteEntry(string entry, ConsoleColor color, bool enterMidway, int top)
		{
			Console.ForegroundColor = ConsoleColor.Magenta;
			Console.ForegroundColor = color;
			int firstWidth = 0;
			int secondWidth = 0;
			if (enterMidway)
			{
				firstWidth = ((int)(Math.Ceiling((43d - entry.Length) / 2) + entry.Length));
				secondWidth = ((int)(Math.Floor((43d - entry.Length) / 2)));
			}
			else
			{
				firstWidth = entry.Length;
				secondWidth = 43 - entry.Length;
			}

			Console.SetCursorPosition(1, top);
			Console.Write("{0," + firstWidth + "}{1," + secondWidth + "}", entry, "");
			Console.ForegroundColor = ConsoleColor.Magenta;
		}

		bool LogInOut()
		{
			if (loginName == null)
				return LogIn();
			else
			{
				loginName = null;
				return true;
			}
		}


		bool LogIn()
		{
			password = new SecureString();

			string name = null;
			int top = 5;
			int count = 0;
			string prompt = "Login name ==> ";

			Console.Clear();
			PaintBorder();
			Console.ForegroundColor = ConsoleColor.White;
			Console.SetCursorPosition(3, top++);

			Console.Write(prompt);
			Console.ForegroundColor = ConsoleColor.Magenta;
			// task: turn this into non-blocking code, like the code below
			name = Console.ReadLine();

			Console.ForegroundColor = ConsoleColor.White;
			Console.SetCursorPosition(3, top);
			Console.Write("Password ==> ");
			Console.ForegroundColor = ConsoleColor.Magenta;

			while (true)
			{
				// ShowPoint: intercept the keypress,
				// so we can blank out the password with asterisks
				ConsoleKeyInfo cki = Console.ReadKey(true);
				//SecureString s = new SecureString();

				if (cki.Key == ConsoleKey.Enter)
				{
					count++;
					if (LoginValid(name, prompt))
					{
						loginName = name;
						return true;
					}
					else
					{
						if (count >= 3) {
							return false;
						}

						Console.ForegroundColor = ConsoleColor.White;
						Console.SetCursorPosition(3, top);
						Console.Write("{0,41}", " ");
						Console.SetCursorPosition(3, top);
						Console.Write("Password ==> ");
						Console.ForegroundColor = ConsoleColor.Magenta;
						password.Clear();
					}
				}
				else if (cki.Key == ConsoleKey.BackSpace)
				{
					// remove the last asterisk from the screen...
					if (password.Length > 0)
					{
						ReplaceEntry(" ", Console.CursorLeft - 1, Console.CursorTop);
						password.RemoveAt(password.Length - 1);
					}
				}
				else if (cki.Key == ConsoleKey.Escape)
				{
					Console.Clear();
					PaintMainScreen();
					return true;
				}
				else if (Char.IsLetterOrDigit(cki.KeyChar) || Char.IsSymbol(cki.KeyChar))
				{
					if (password.Length < 15)
					{
						password.AppendChar(cki.KeyChar);
						Console.Write("*");
					}
					else
					{
						Console.Beep();
					}
				} else
				{
					Console.Beep();
				}
			}
		}

		bool LoginValid(string name, string prompt)
		{
			IntPtr tokenHandle = IntPtr.Zero;
			const int LOGON32_PROVIDER_DEFAULT = 0;
            const int LOGON32_LOGON_INTERACTIVE = 2;

			string tempPass = Marshal.PtrToStringBSTR(Marshal.SecureStringToBSTR(password));
			if (NativeMethods.LogonUser(name, "", Marshal.SecureStringToGlobalAllocUni(password),
				LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, ref tokenHandle))
			{
				return true;
			}
			else
			{
				Console.SetCursorPosition(3, 5);
				Console.Write("{0,41}", " ");
				Console.SetCursorPosition(3, 5);
				Console.ForegroundColor = ConsoleColor.White;
				Console.Write(prompt);
				Console.ForegroundColor = ConsoleColor.Magenta;
				Console.Write(name);
				Console.ForegroundColor = ConsoleColor.Red;
				Console.SetCursorPosition(5, 8);
				Console.Write("That password is invalid!");

				Stopwatch sw = new Stopwatch();

				int count = 0;

				Console.Beep();
				while (count < 2)
				{
					sw.Start();

					if (sw.ElapsedMilliseconds > 100)
					{
						Console.Beep();
						count++;
					}
				}
				Console.SetCursorPosition(5, 8);
				Console.Write("{0,38}", " ");
			}

			return false;
		}

		void ReplaceEntry(string replacement, int left, int top)
		{
			Console.SetCursorPosition(left, top);
			Console.Write(replacement);
			Console.SetCursorPosition(left, top);
		}

		void PaintPatientScreen()
		{
			Console.CursorVisible = false;
			MoveMainScreen(true);
			Console.SetCursorPosition(0,0);

			while (true)
			{
				PaintBorder();
				int top = 1;
				WriteEntry("", ConsoleColor.Magenta, false, top++);
				WriteEntry("Patient Maintenance Screen!", ConsoleColor.Green, true, top++);

				if (loginName == null)
				{
					WriteEntry("", ConsoleColor.Magenta, false, top++);
					WriteEntry(" You are currently not logged in", ConsoleColor.Red, true, top++);
				}
				else
				{
					WriteEntry("", ConsoleColor.Magenta, false, top++);
					WriteEntry(" You are currently logged in as " + loginName, ConsoleColor.Red, true, top++);
				}

				WriteEntry("", ConsoleColor.Magenta, false, top++);
				WriteEntry(" Options:", ConsoleColor.Yellow, false, top++);
				WriteEntry(" 1. New Patient", ConsoleColor.White, false, top++);
				WriteEntry(" 2. View Existing Patient", ConsoleColor.White, false, top++);
				WriteEntry(" 3. Patient in Trouble!", ConsoleColor.White, false, top++);
				WriteEntry(" 4. Exit", ConsoleColor.White, false, top++);

				WriteEntry("", ConsoleColor.Magenta, false, top++);

				Console.ForegroundColor = ConsoleColor.White;
				Console.SetCursorPosition(1, Console.WindowHeight - 1);
				Console.Write("Please enter a selection (1 - 4) ==> ");
				Console.CursorVisible = true;

				ConsoleKeyInfo cki = Console.ReadKey();

				if (cki.Key == ConsoleKey.D1 ||
							cki.Key == ConsoleKey.D2 ||
							cki.Key == ConsoleKey.D3)
				{
					ShowNotImplementedScreen("Please enter a selection (1 - 4) ==> ");
				}
				else if (cki.Key == ConsoleKey.Escape || cki.Key == ConsoleKey.D4)
				{
					MoveMainScreen(false);
					Console.SetCursorPosition(mainCursorLeft, mainCursorTop);
					return;
				}
			}
		}

		void MoveMainScreen(bool moveOff)
		{
			Stopwatch sw = new Stopwatch();

			if (moveOff)
			{
				for (int i = 0; i < Console.WindowWidth; i++)
				{
					Console.MoveBufferArea(i, 0, Console.WindowWidth, Console.WindowHeight, i + 1, 0);
					sw.Start();
					while (sw.ElapsedMilliseconds < 50) {}
					sw.Reset();
				}
			} 
			else
			{
				for (int i = Console.WindowWidth; i > 0; i--)
				{
					Console.MoveBufferArea(i, 0, Console.WindowWidth, Console.WindowHeight, i - 1, 0);
					sw.Start();
					while (sw.ElapsedMilliseconds < 50) {}
					sw.Reset();
				}
			}
		}

		void ShowNotImplementedScreen(string line)
		{
			Console.Clear();
			PaintBorder();
			bool cursorVisible = Console.CursorVisible;
			Console.CursorVisible = false;
			int cursorLeft = Console.CursorLeft;
			int cursorTop = Console.CursorTop;
			Console.SetCursorPosition(3, 7);
			Console.WriteLine(line);
			System.Threading.Thread.Sleep(3000);
			Console.SetCursorPosition(cursorLeft, cursorTop);
			Console.Clear();
			Console.CursorVisible = cursorVisible;
		}

		void OpenDoctorFile()
		{

			// this code attempts to open the doctor file, for the current user.
			// if the user has access to the file, then notepad will open, and you will see the file
			// If they DON'T have access (simulating that they aren't a doctor), then notepad
			// will still run, but the file won't open

			// See the instructions in the Doctor file for simulating 
			// someone not being allowed to open the file
			Process p = new Process();
			ProcessStartInfo psi = new ProcessStartInfo();
			psi.Password = password;
			psi.UserName = loginName;
			psi.UseShellExecute = false;

			psi.FileName = "notepad";
			psi.Arguments = Environment.CurrentDirectory + @"\doctorFile.txt";
			p.StartInfo = psi;

			p.Start();

		}
		void OpenNurseFile() {
			// this code attempts to open the doctor file, for the current user.
			// if the user has access to the file, then notepad will open, and you will see the file
			// If they DON'T have access (simulating that they aren't a doctor), then notepad
			// will still run, but the file won't open

			// See the instructions in the Nurse file for simulating someone being allowed to open the file
			Process p = new Process();
			ProcessStartInfo psi = new ProcessStartInfo();
			psi.Password = password;
			psi.UserName = loginName;
			psi.UseShellExecute = false;

			psi.FileName = "notepad";
			psi.Arguments = Environment.CurrentDirectory + @"\nurseFile.txt";
			p.StartInfo = psi;

			p.Start();
		}
	}
}
