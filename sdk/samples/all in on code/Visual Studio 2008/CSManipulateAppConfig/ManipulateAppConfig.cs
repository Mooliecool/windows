/************************************ Module Header ***********************************\
* Module Name:  ManipulateAppConfig.cs
* Project:      CSManipulateAppConfig
* Copyright (c) Microsoft Corporation.
* 
* Use ConfigurationManager to create, read, update and delete node in the config file content or 
* use XmlDocument to update the config file content at runtime.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\**************************************************************************************/

#region Using directives
using System;
using System.Configuration;
using System.Xml;
#endregion

namespace CSManipulateAppConfig
{
    public class ManipulateAppConfig
    {
        // Open the specified client configuration file as a Configuration object.
        // To get the Configuration object that applies to all users, set userLevel to None.
        private static Configuration config = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);

        /// <summary>
        /// Create a new node in config file.
        /// </summary>
        /// <param name="key">The key of the node.</param>
        /// <param name="value">The value of the node.</param>
        public static void CreateNode(string key, string value)
        {
            // Add an Application Setting.
            config.AppSettings.Settings.Add(key, value);

            // Write the configuration settings contained within this Configuration object 
            // to the current XML configuration file, even if the configuration was not modified.
            config.Save(ConfigurationSaveMode.Modified, true);

            // Refresh the named section so the next time that it is retrieved it will be re-read from disk.
            ConfigurationManager.RefreshSection("appSettings");
        }

        /// <summary>
        /// Read nodes in config file.
        /// </summary>
        public static void ReadNode()
        {
            // Enumerate settings information in configuration file.
            foreach (KeyValueConfigurationElement keyValuecfg in config.AppSettings.Settings)
            {
                Console.WriteLine(String.Format("Key: {0}, Value: {1}", keyValuecfg.Key, keyValuecfg.Value));
            }
        }

        /// <summary>
        /// Update node in config file.
        /// </summary>
        /// <param name="key">The key of the node.</param>
        /// <param name="value">The new value of the node.</param>
        public static void UpdateNode(string key, string value)
        {
            config.AppSettings.Settings[key].Value = value;
            config.Save(ConfigurationSaveMode.Modified, true);

            #region another method
            // Because there're some settings are read-only or cannot be modified successfully at runtime, 
            // in order to modify the current application settings value, 
            // we must use the XmlDocument class to directly update the 
            // application configuration file as an XML document.
            
            //XmlDocument xmlDoc = new XmlDocument();

            //// Load the application configuration file.
            //xmlDoc.Load(AppDomain.CurrentDomain.SetupInformation.ConfigurationFile);

            //// Enumerate nodes in configuration file.
            //foreach (XmlElement element in xmlDoc.DocumentElement)
            //{
            //    if (element.Name.Equals("appSettings"))
            //    {
            //        foreach (XmlNode node in element.ChildNodes)
            //        {
            //            if (node.Attributes[0].Value.Equals(key))
            //            {
            //                // Update the node value.
            //                node.Attributes[1].Value = value;
            //            }
            //        }
            //    }
            //}

            //// Save the XML document to the specified file.
            //xmlDoc.Save(AppDomain.CurrentDomain.SetupInformation.ConfigurationFile);
            #endregion

            // Refresh the named section so the next time that it is retrieved it will be re-read from disk.
            ConfigurationManager.RefreshSection("appSettings");
        }

        /// <summary>
        /// Delete node in config file.
        /// </summary>
        /// <param name="key">The key of the node.</param>
        public static void DeleteNode(string key)
        {
            // Remove the node.
            config.AppSettings.Settings.Remove(key);

            // Write the configuration settings contained within this Configuration object 
            // to the current XML configuration file, even if the configuration was not modified.
            config.Save(ConfigurationSaveMode.Modified, true);

            // Refresh the named section so the next time that it is retrieved it will be re-read from disk.
            ConfigurationManager.RefreshSection("appSettings");
        }
    }
}
