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
//-----------------------------------------------------------------------

namespace MInternetExplorer
{

	using System;
	using System.Runtime.InteropServices;
	using SHDocVw;

	public class Explorer 
	{
		public static void Main()
		{
			Explorer explorer = new Explorer();
			explorer.Run();
		}
		
		public void Run()
		{
			Object o = null;
			String s;

			try
			{
				// start the browser
				m_IExplorer = new SHDocVw.InternetExplorer();
			}
			catch(Exception e)
			{
				Console.WriteLine("Exception when creating IE object {0}", e);
				return;
			}			

			// Set the events
			SetAllEvents();

			try
			{	
				// go to home page
				m_WebBrowser = (IWebBrowserApp) m_IExplorer;
				m_WebBrowser.Visible = true;
				m_WebBrowser.GoHome();


				// start navigating to different urls
				Console.Write("Enter URL (or enter to quit): ");		
				s = Console.ReadLine();
				while (s != "" && m_IExplorer != null && m_WebBrowser != null)
				{
					m_WebBrowser.Navigate(s, ref o, ref o, ref o, ref o);
					Console.Write("Enter URL (or enter to quit): ");		
					s = Console.ReadLine();
				}

				m_WebBrowser.Quit();
			}
			catch(Exception sE)
			{
				if (m_IExplorer == null && m_WebBrowser == null)
				{
					Console.WriteLine("IE has gone away");
				}
				else
				{
					Console.WriteLine("Exception happens {0}", sE);
				}
			}
			
		}

		void SetAllEvents()
		{
			if (m_IExplorer != null)
			{
				//1.Navigate Complete event
				DWebBrowserEvents2_NavigateComplete2EventHandler ncd
					= new DWebBrowserEvents2_NavigateComplete2EventHandler(OnNavigateComplete);
				m_IExplorer.NavigateComplete2 += ncd;

				//2.Quit Event
				DWebBrowserEvents2_OnQuitEventHandler DQuitE 
					= new DWebBrowserEvents2_OnQuitEventHandler(OnQuit);
				m_IExplorer.OnQuit += DQuitE;

				//3.Statusbar text changed event
				DWebBrowserEvents2_StatusTextChangeEventHandler DStatusE
					= new DWebBrowserEvents2_StatusTextChangeEventHandler(OnStatusTextChange);
				m_IExplorer.StatusTextChange += DStatusE;

				//4.Fired when download progress is updated.
				DWebBrowserEvents2_ProgressChangeEventHandler DProgressE
					= new DWebBrowserEvents2_ProgressChangeEventHandler(OnProgressChange);
				m_IExplorer.ProgressChange += DProgressE;

				//5.Command State change event
				DWebBrowserEvents2_CommandStateChangeEventHandler DCommandE
					= new DWebBrowserEvents2_CommandStateChangeEventHandler(OnCommandStateChange);
				m_IExplorer.CommandStateChange += DCommandE;

				//6.DownLoad begin event
				DWebBrowserEvents2_DownloadBeginEventHandler DDownLoadStartE
					= new DWebBrowserEvents2_DownloadBeginEventHandler(OnDownloadBegin);
				m_IExplorer.DownloadBegin += DDownLoadStartE;

				//7.DownLoad complete event
				DWebBrowserEvents2_DownloadCompleteEventHandler DDownLoadEndE
					= new DWebBrowserEvents2_DownloadCompleteEventHandler(OnDownloadComplete);
				m_IExplorer.DownloadComplete += DDownLoadEndE;

				//8.Title Change event
				DWebBrowserEvents2_TitleChangeEventHandler DTitleChangeE
					= new DWebBrowserEvents2_TitleChangeEventHandler(OnTitleChange);
				m_IExplorer.TitleChange += DTitleChangeE;

				//9.Property change event
				DWebBrowserEvents2_PropertyChangeEventHandler DPropertyChangeE
					= new DWebBrowserEvents2_PropertyChangeEventHandler(OnPropertyChange);
				m_IExplorer.PropertyChange += DPropertyChangeE;

				//10.Before navigate event
				DWebBrowserEvents2_BeforeNavigate2EventHandler DBeforeNavigateE
					= new DWebBrowserEvents2_BeforeNavigate2EventHandler(OnBeforeNavigate2);
				m_IExplorer.BeforeNavigate2 += DBeforeNavigateE;

				//11.Document complete event
				DWebBrowserEvents2_DocumentCompleteEventHandler DDocumentCompleteE
					= new DWebBrowserEvents2_DocumentCompleteEventHandler(OnDocumentComplete);
				m_IExplorer.DocumentComplete += DDocumentCompleteE;

				//12.Creating a new window event
				DWebBrowserEvents2_NewWindow2EventHandler DNewWindowE
					= new DWebBrowserEvents2_NewWindow2EventHandler(OnNewWindow2);
				m_IExplorer.NewWindow2 += DNewWindowE;

				//13.Full Screen event
				DWebBrowserEvents2_OnFullScreenEventHandler DFullScreenE
					= new DWebBrowserEvents2_OnFullScreenEventHandler(OnFullScreen);
				m_IExplorer.OnFullScreen += DFullScreenE;

				//14.Menubar change event
				DWebBrowserEvents2_OnMenuBarEventHandler DMenuBarE
					= new DWebBrowserEvents2_OnMenuBarEventHandler(OnMenuBar);
				m_IExplorer.OnMenuBar += DMenuBarE;

				//15.Toolbar change event
				DWebBrowserEvents2_OnToolBarEventHandler DToolBarE
					= new DWebBrowserEvents2_OnToolBarEventHandler(OnToolBar);
				m_IExplorer.OnToolBar += DToolBarE;

				//16.Visibility change event
				DWebBrowserEvents2_OnVisibleEventHandler DOnVisibleE
					= new DWebBrowserEvents2_OnVisibleEventHandler(OnVisible);
				m_IExplorer.OnVisible += DOnVisibleE;

				//17.Threat Mode event
				DWebBrowserEvents2_OnTheaterModeEventHandler DTheaterE
					= new DWebBrowserEvents2_OnTheaterModeEventHandler(OnTheaterMode);
				m_IExplorer.OnTheaterMode += DTheaterE;

			 }

		}

		///////////////////////////////////////////////////////////////////////////////////////
		//event handlers are below
		//1.NavigateComplete Event
		static void OnNavigateComplete(Object o1, ref Object o2)
		{
			Console.WriteLine("Navigate complete");
		}

		//2.Quit event
		static void OnQuit()
		{
			Console.WriteLine("Internet explorer is quiting");
			m_IExplorer = null;
			m_WebBrowser = null;
		}

		//3.Statusbar text changed
		static void OnStatusTextChange(String sIn)
		{
			Console.WriteLine("Status text changed with {0}", sIn);
		}

		//4.Fired when download progress is updated.
		static void OnProgressChange(int Progress, int ProgressMax)
		{
			Console.WriteLine("Progress change : progress = {0}, progressMax = {1}", Progress, ProgressMax);
		}

		//5.The enabled state of a command changed.
		static void OnCommandStateChange(int Command, bool Enable)
		{
			Console.WriteLine("Command State change : Command = {0}, Enable = {1}", Command, Enable);
		}

        //6.Download of a page started
		static void OnDownloadBegin()
		{
			Console.WriteLine("Download begins now");
		}

        //7.Download of page complete
		static void OnDownloadComplete()
		{
			Console.WriteLine("Download completes");
		}

        //8.Document title changed
		static void OnTitleChange(String Text)
		{
			Console.WriteLine("Title changes to {0}", Text);
		}

		//9.Fired when the PutProperty method has been called.
		static void OnPropertyChange(String szProperty)
		{
			Console.WriteLine("Property {0} changed", szProperty);
		}
	
		//10.Fired before navigate occurs in the given WebBrowser (window or
		//   frameset element). The processing of this navigation may be modified
        static void OnBeforeNavigate2(Object ob1, ref Object URL, ref Object Flags, ref Object TargetFrameName, 
									  ref Object PostData, ref Object Headers, ref bool Cancel)
		{
			Console.WriteLine("Before Navigate2");
			m_nCounter += 1;
			if (m_nCounter >= 10)
			{
				Console.WriteLine("Counter is {0}. Refuse to navigate to the site", m_nCounter);
				Cancel = true;
				m_nCounter = 0;
			}
			else
			{
				Console.WriteLine("Counter is {0}. Permit to navigate to the site", m_nCounter);
				Cancel = false;
			}			

		}
			
		//11.Fired when document complete
		static void OnDocumentComplete(Object o, ref Object ro)
		{
			Console.WriteLine("Document complete");
		}

		//12.Fired when creating new window
		static void OnNewWindow2(ref Object o, ref bool rb)
		{
			Console.WriteLine("New window");
		}
		
		//13.Fired when Fullscreen
		static void OnFullScreen(bool b)
		{
			if (b)
				Console.WriteLine("Full Screen");
			else
				Console.WriteLine("Not full screen");
		}

		//14.Fired when menubar change
		static void OnMenuBar(bool b)
		{
			if (b)
				Console.WriteLine("Menu bar visible");
			else
				Console.WriteLine("MenuBar invisible");
		}

		//15.Fired when Toolbar change
		static void OnToolBar(bool b)
		{
			if (b)
				Console.WriteLine("Toolbar visible");
			else
				Console.WriteLine("ToolBar invisible");
		}

		//16.Fired when Visible
		static void OnVisible(bool b)
		{
			if (b)
				Console.WriteLine("Visible");
			else
				Console.WriteLine("Invisible");
		}

		//17.Theater Mode
		static void OnTheaterMode(bool b)
		{
			if (b)
				Console.WriteLine("Theater Mode");
			else
				Console.WriteLine("Not Threater mode");
		}

		///////////////////////////////////////////////////////////////////////////////////////

		//These are class data members
		static private	SHDocVw.InternetExplorer m_IExplorer = null;
		static private	IWebBrowserApp m_WebBrowser = null;
		static private int m_nCounter = 0;

	}

}


