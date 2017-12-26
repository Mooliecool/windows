using System;
using System.Windows;
using System.Xml;
using System.Configuration;

namespace ImageView
{
	/// <summary>
	/// Interaction logic for app.xaml
	/// </summary>

	public partial class app : Application
	{
		void AppStartingUp(object sender, StartupEventArgs e)
		{
			ImageViewExample defaultWindow = new ImageViewExample();
			defaultWindow.InitializeComponent();
			defaultWindow.Show();
		}

	}
}