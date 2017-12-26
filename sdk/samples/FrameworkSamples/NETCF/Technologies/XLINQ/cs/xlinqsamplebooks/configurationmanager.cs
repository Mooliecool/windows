//---------------------------------------------------------------------
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
//---------------------------------------------------------------------

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections.Specialized;
using System.Xml.Linq;
using System.IO;
using System.Diagnostics;
using System.Reflection;
using System.Globalization;
namespace Microsoft.Samples.XLinq
{
    /// <summary>
    /// Class to Read /Write Configuration Settings from the App Config file
    /// </summary>
    static class ConfigurationManager
    {

        /// <summary>
        /// private NameValueCollection that stores the key value config settings
        /// </summary>
        private static NameValueCollection appSettings;

        /// <summary>
        /// Public property that returns the app config settings
        /// </summary>
        public static NameValueCollection AppSettings
        {
            get
            {
                //if not loaded, then load app config settings
                if (appSettings == null)
                    LoadApplicationSettings();
                return appSettings;
            }
            set
            {
                appSettings = value;
            }

        }

        /// <summary>
        /// Save the new set of config settings back to the app config file.
        /// </summary>
        public static void Save()
        {
            //if no settings to save, just return
            if (appSettings == null)
                return;

            //get the config file name
            string name = string.Format(CultureInfo.CurrentCulture,"{0}.config", Assembly.GetExecutingAssembly().GetName().CodeBase);

            //check for existence
            //if config file does not exist, then create new one , else load existing one
            XDocument doc = null;
            if (!File.Exists(name))
                doc = new XDocument ( new XElement ("configuration",new XElement ("appSettings")));
            else
                doc = XDocument.Load(name);


            //retrieve the appSettings element in the XML app config file
            XElement appSetttingsElement = doc.Element("configuration").Element("appSettings");

            //remove all the nodes in the appSettings Elements
            appSetttingsElement.RemoveNodes();            

            //create a new XElement with the key value pairs as child nodes.
            XElement newAppSettingsElement = new XElement("appSettings",
                from key in appSettings.Keys.OfType <string>() 
                select new XElement("add", new XAttribute("key", key),
                                                             new XAttribute("value", appSettings[key])));

            //replace the existing appSettings element with the newly created one
            appSetttingsElement.ReplaceWith(newAppSettingsElement);

            //finally save the xml back to the app config file.
            doc.Save(name);

        }

        /// <summary>
        /// Loads the key value pairs on appSettings Element to a NameValueCollection 
        /// </summary>
        private static void LoadApplicationSettings()
        {
            appSettings = new NameValueCollection();

            //config file name is {applicationName}.exe.config
            //Assembly.GetExecutingAssembly().GetName().CodeBase returns full path of the current executable
            string name = string.Format(CultureInfo.CurrentCulture , "{0}.config", Assembly.GetExecutingAssembly().GetName().CodeBase);

            //check if the config file exists.
            if(!File.Exists(name))
                return;


            //load the configuration file
            XDocument doc = XDocument.Load(name);

            //check existence configuration element and existence of the appSettings element inside configuration element
            if (doc.Element("configuration") != null &&
                doc.Element("configuration").Element("appSettings") != null)
            {
             
                //select the elements inside appSettings section named add. 
                //the add element will have two attributes 1)key 2)value
                var q = from e in doc.Element("configuration").Element("appSettings").Elements()
                        where e.Attribute ("key") != null  && e.Attribute ("value") != null                        
                        select new { key = e.Attribute("key").Value, value = e.Attribute("value").Value };
                        
                
                //loop through every element and add the key ,value pair to the NamedValueCollection
                foreach (var v in q)
                    appSettings.Add(v.key, v.value);
            }

        }


    }
}
