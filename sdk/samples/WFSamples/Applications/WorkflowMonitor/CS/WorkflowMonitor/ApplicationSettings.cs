//---------------------------------------------------------------------
//  This file is part of the Windows Workflow Foundation SDK Code Samples.
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
using System.IO;
using System.Xml.Serialization;

namespace Microsoft.Samples.Workflow.WorkflowMonitor
{
    //This class is used to store user settings such as server and database names
    public class ApplicationSettings
    {
        //Set to true if the settings have changed since last saved
        private bool applicationSettingsChanged;
        private int pollingInterval;
        private string serverName;
        private string databaseName;
        private bool autoSelectLatest;

        internal ApplicationSettings()
        {
            applicationSettingsChanged = false;
        }

        //Save app info to the config file
        internal void SaveSettings(string path)
        { 
            if (applicationSettingsChanged)
            {
                StreamWriter writer = null;
                XmlSerializer serializer = null;
                try
                {
                    // Create an XmlSerializer for the 
                    // ApplicationSettings type.
                    serializer = new XmlSerializer(typeof(ApplicationSettings));
                    writer = new StreamWriter(path, false);
                    // Serialize this instance of the ApplicationSettings 
                    // class to the config file.
                    serializer.Serialize(writer, this);
                }
                catch
                {
                }
                finally
                {
                    // If the FileStream is open, close it.
                    if (writer != null)
                    {
                        writer.Close();
                    }
                }
            }
        }

        //Load app info from the config file
        internal bool LoadAppSettings(string path)
        {
            XmlSerializer serializer = null;
            FileStream fileStream = null;
            bool fileExists = false;

            try
            {
                // Create an XmlSerializer for the ApplicationSettings type.
                serializer = new XmlSerializer(typeof(ApplicationSettings));
                FileInfo info = new FileInfo(path);
                // If the config file exists, open it.
                if (info.Exists)
                {
                    fileStream = info.OpenRead();
                    // Create a new instance of the ApplicationSettings by
                    // deserializing the config file.
                    ApplicationSettings applicationSettings = (ApplicationSettings)serializer.Deserialize(fileStream);
                    // Assign the property values to this instance of 
                    // the ApplicationSettings class.
                    this.databaseName = applicationSettings.databaseName;
                    this.serverName = applicationSettings.serverName;
                    this.pollingInterval = applicationSettings.pollingInterval;
                    this.autoSelectLatest = applicationSettings.autoSelectLatest;

                    fileExists = true;
                }
            }
            catch
            {
            }
            finally
            {
                // If the FileStream is open, close it.
                if (fileStream != null)
                {
                    fileStream.Close();
                }
            }
            return fileExists;
        }

        public int PollingInterval
        {
            get { return pollingInterval; }
            set
            {
                if (value != pollingInterval)
                {
                    pollingInterval = value;
                    applicationSettingsChanged = true;
                }
            }
        }

        public bool ApplicationSettingsChanged
        {
            get { return applicationSettingsChanged; }
        }

        public string ServerName
        {
            get { return serverName; }
            set
            {
                if (value != serverName)
                {
                    serverName = value;
                    applicationSettingsChanged = true;
                }
            }
        }

        public string DatabaseName
        {
            get { return databaseName; }
            set
            {
                if (value != databaseName)
                {
                    databaseName = value;
                    applicationSettingsChanged = true;
                }
            }
        }

        public bool AutoSelectLatest
        {
            get { return autoSelectLatest; }
            set
            {
                if (value != autoSelectLatest)
                {
                    autoSelectLatest = value;
                    applicationSettingsChanged = true;
                }
            }
        }
    }
}
