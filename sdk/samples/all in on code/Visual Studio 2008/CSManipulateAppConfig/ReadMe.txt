=============================================================================
  CONSOLE APPLICATION : CSManipulateAppConfig Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

CSManipulateAppConfig example demonstrates how to use ConfigurationManager to create,
read, update and delete node in the config file content or use XmlDocument to update
the config file content at runtime.


/////////////////////////////////////////////////////////////////////////////
Demo:

1.	You just need to build this project.
2.	And then you can run this project. Then you will get the following result.

1) Dispaly the nodes in the config file before we modify it.
Original node in the config file.
Key: Setting1, Value: 1
Key: Setting2, Value: 2

2) Create a new node(Key is "Setting3", and value is "3").
Create a new node in the config file.
Key: Setting1, Value: 1
Key: Setting2, Value: 2
Key: Setting3, Value: 3

3) Update a node("Setting1")'s value to be "New Value".
Update an existing node value.
Key: Setting1, Value: New Value
Key: Setting2, Value: 2
Key: Setting3, Value: 3

4) Delete a node(Key is "Setting2", and value is "2").
Delete an existing node.
Key: Setting1, Value: New Value
Key: Setting3, Value: 3


/////////////////////////////////////////////////////////////////////////////
Code Logic:

CreateNode
1.	Open the specified client configuration file as a Configuration object.
    To get the Configuration object that applies to all users, set userLevel to None.
	private static Configuration config = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);
2.	Add an Application Setting.
	config.AppSettings.Settings.Add(key, value);
3.	Write the configuration settings contained within this Configuration object 
    to the current XML configuration file, even if the configuration was not modified.
	config.Save(ConfigurationSaveMode.Modified, true);
4.	Refresh the named section so the next time that it is retrieved it 
	will be re-read from disk.
	ConfigurationManager.RefreshSection("appSettings");

ReadNode
1.	Open the specified client configuration file as a Configuration object.
    To get the Configuration object that applies to all users, set userLevel to None.
	private static Configuration config = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);
2.	Enumerate settings information in configuration file.
	foreach (KeyValueConfigurationElement keyValuecfg in config.AppSettings.Settings)
	{
	    Console.WriteLine(String.Format("Key: {0}, Value: {1}", keyValuecfg.Key, keyValuecfg.Value));
	}

UpdateNode
1.	Because of there're some settings are read-only or cannot be modified successfully at runtime, 
	in order to modify the current application settings value, we must use the XmlDocument 
	class to directly update the application configuration file as an XML document.
2.	Load the application configuration file.
	xmlDoc.Load(AppDomain.CurrentDomain.SetupInformation.ConfigurationFile);
3.	Enumerate nodes in configuration file.
4.	Update the node value.
	foreach (XmlElement element in xmlDoc.DocumentElement)
	{
	    if (element.Name.Equals("appSettings"))
	    {
	        foreach (XmlNode node in element.ChildNodes)
	        {
	            if (node.Attributes[0].Value.Equals(key))
	            {
	                // Update the node value.
	                node.Attributes[1].Value = value;
	            }
	        }
	    }
	}
5.	Save the XML document to the specified file.
	xmlDoc.Save(AppDomain.CurrentDomain.SetupInformation.ConfigurationFile);
6.	Refresh the named section so the next time that it is retrieved it 
	will be re-read from disk.
	ConfigurationManager.RefreshSection("appSettings");

DeleteNode
1.	Open the specified client configuration file as a Configuration object.
	To get the Configuration object that applies to all users, set userLevel to None.
2.	Delete node by key.
	config.AppSettings.Settings.Remove(key);
3.	Write the configuration settings contained within this Configuration object 
    to the current XML configuration file, even if the configuration was not modified.
	config.Save(ConfigurationSaveMode.Modified, true);
4.	Refresh the named section so the next time that it is retrieved it 
	will be re-read from disk.
	ConfigurationManager.RefreshSection("appSettings");

/////////////////////////////////////////////////////////////////////////////
References:

ConfigurationManager Class
http://msdn.microsoft.com/en-us/library/system.configuration.configurationmanager.aspx

ConfigurationManager.OpenExeConfiguration Method (ConfigurationUserLevel)
http://msdn.microsoft.com/en-us/library/ms134265.aspx

Configuration.Save Method (ConfigurationSaveMode, Boolean)
http://msdn.microsoft.com/en-us/library/ms134089.aspx

ConfigurationManager.RefreshSection Method
http://msdn.microsoft.com/en-us/library/system.configuration.configurationmanager.refreshsection.aspx

XmlDocument Class
http://msdn.microsoft.com/en-us/library/system.xml.xmldocument.aspx

AppDomain Class
http://msdn.microsoft.com/en-us/library/system.appdomain.aspx

How to Make Your .NET Windows Forms Configuration Files Dynamic
http://www.devx.com/dotnet/Article/11616/1954

/////////////////////////////////////////////////////////////////////////////