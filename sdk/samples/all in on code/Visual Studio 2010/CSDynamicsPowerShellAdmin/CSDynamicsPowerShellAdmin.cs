/****************************** Module Header ******************************\
 * Module Name: CSDynamicsPowerShellAdmin.cs
 * Project: CSDynamicsPowerShellAdmin
 * Copyright (c) Microsoft Corporation. 
 * 
 * This project shows code samples for administrating NAV Services using PowerSHell.
 * Running it requires a running installation of NAV 2009 or a higher version.
 * 
 * This source is subject to the Microsoft Public License. 
 * See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL. 
 * All other rights reserved. 
 * 
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES 
 * OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE. 
\***************************************************************************/


using System;
using System.ComponentModel;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;
using System.Management.Automation;
using System.Management.Automation.Host;
using System.Management.Automation.Runspaces;
using System.Collections.ObjectModel;
using System.Xml;

namespace CSDynamicsPowerShellAdmin
{
    public partial class CSDynamicsPowerShellAdmin : Form
    {
        public CSDynamicsPowerShellAdmin()
        {
            InitializeComponent();
        }

        string[,] NAVProcessInfo = new string[99, 12]; // up to 99 NAV Services, up to 12 properties for each        
        int CurrentListIndex = 0;
        int CurrentConfigIndex = -1;

        private void btnGetNAVServices_Click(object sender, EventArgs e)
        {
            using (Runspace runSpace = RunspaceFactory.CreateRunspace())
            {
                runSpace.Open();
                Pipeline pipeLine = runSpace.CreatePipeline();
                Command getNAVServices = new Command("Get-WmiObject");
                getNAVServices.Parameters.Add("query", "select * from win32_service where pathname like \"%Dynamics.Nav%\"");
                
                if (NAVServerName.Text != "")
                {
                    getNAVServices.Parameters.Add("ComputerName", NAVServerName.Text);
                }

                pipeLine.Commands.Add(getNAVServices);
                Collection<PSObject> NAVProcesses = pipeLine.Invoke();
                int NumberOfProcesses = NAVProcesses.Count;
                int Counter = 0;

                NAVServicesList.Items.Clear();

                foreach (PSObject NAVProcess in NAVProcesses)
                {
                    UpdateNAVService(Counter, NAVProcess);
                    Counter++;
                }

                UpdateNAVConfigInfo(0);
            }
        }

        private void btnGetConfig_Click(object sender, EventArgs e)
        {
            XmlDocument ConfigDoc = new XmlDocument();
            ConfigDoc.Load(GetConfigFileName());

            // DatabaseServer - ConfigDBServer
            // DatabaseInstance - ConfigDBInstance
            // DatabaseName - ConfigDBName
            // ServerInstance - ConfigServerInstance
            // ServerPort - ConfigServerPort
            // WebServicePort - ConfigWebServicePort
            // WebServicesUseNTLMAuthentication - ConfigWebServiceUseNTLM
            // EnableDebugging - ConfigEnableDebugging

            ConfigDBServer.Text = GetConfigKey(ConfigDoc, "DatabaseServer");
            ConfigDBInstance.Text = GetConfigKey(ConfigDoc, "DatabaseInstance");
            ConfigDBName.Text = GetConfigKey(ConfigDoc, "DatabaseName");
            ConfigServerInstance.Text = GetConfigKey(ConfigDoc, "ServerInstance");
            ConfigServerPort.Text = GetConfigKey(ConfigDoc, "ServerPort");
            ConfigWebServicePort.Text = GetConfigKey(ConfigDoc, "WebServicePort");
            ConfigWebServiceUseNTLM.Text = GetConfigKey(ConfigDoc, "WebServicesUseNTLMAuthentication");
            ConfigEnableDebugging.Text = GetConfigKey(ConfigDoc, "EnableDebugging");

            CurrentConfigIndex = CurrentListIndex;
        }

        private void btnUpdateConfig_Click(object sender, EventArgs e)
        {

            if (CurrentConfigIndex == CurrentListIndex)
            {
                XmlDocument ConfigDoc = new XmlDocument();
                ConfigDoc.Load(GetConfigFileName());
                SetConfigKey(ref ConfigDoc, "DatabaseServer", ConfigDBServer.Text);
                SetConfigKey(ref ConfigDoc, "DatabaseInstance", ConfigDBInstance.Text);
                SetConfigKey(ref ConfigDoc, "DatabaseName", ConfigDBName.Text);
                SetConfigKey(ref ConfigDoc, "ServerInstance", ConfigServerInstance.Text);
                SetConfigKey(ref ConfigDoc, "ServerPort", ConfigServerPort.Text);
                SetConfigKey(ref ConfigDoc, "WebServicePort", ConfigWebServicePort.Text);
                SetConfigKey(ref ConfigDoc, "WebServicesUseNTLMAuthentication", ConfigWebServiceUseNTLM.Text);
                SetConfigKey(ref ConfigDoc, "EnableDebugging", ConfigEnableDebugging.Text);

                ConfigDoc.Save(GetConfigFileName());
            }
            else
            {
                MessageBox.Show("Please update config for this service first.");
            }
        }

        private void btnStartService_Click(object sender, EventArgs e)
        {
            StartStopNAVService("StartService");
        }

        private void btnStopService_Click(object sender, EventArgs e)
        {
            StartStopNAVService("StopService");
        }

        private void UpdateNAVService(int IndexNo, PSObject NAVProcess)
        {
            // 0 = Name
            // 1 = Caption
            // 2 = DisplayName
            // 3 = PathName (path to executable)
            // 4 = ProcessID
            // 5 = ServiceType (own / shared)                
            // 6 = Started 
            // 7 = StartMode (Automatic / Manual)
            // 8 = StartName (Account name)
            // 9 = State (status)
            // 10 = File (executable) name

            NAVProcessInfo[IndexNo, 0] = NAVProcess.Properties["Name"].Value.ToString();
            NAVProcessInfo[IndexNo, 1] = NAVProcess.Properties["Caption"].Value.ToString();
            NAVProcessInfo[IndexNo, 2] = NAVProcess.Properties["DisplayName"].Value.ToString();
            NAVProcessInfo[IndexNo, 3] = NAVProcess.Properties["PathName"].Value.ToString();
            NAVProcessInfo[IndexNo, 4] = NAVProcess.Properties["ProcessID"].Value.ToString();
            NAVProcessInfo[IndexNo, 5] = NAVProcess.Properties["ServiceType"].Value.ToString();
            NAVProcessInfo[IndexNo, 6] = NAVProcess.Properties["Started"].Value.ToString();
            NAVProcessInfo[IndexNo, 7] = NAVProcess.Properties["StartMode"].Value.ToString();
            NAVProcessInfo[IndexNo, 8] = NAVProcess.Properties["StartName"].Value.ToString();
            NAVProcessInfo[IndexNo, 9] = NAVProcess.Properties["State"].Value.ToString();

            // Seperate file and path 
            string[] SplitString = NAVProcessInfo[IndexNo, 3].Split(new char[] { '\\' });
            NAVProcessInfo[IndexNo, 10] = SplitString[SplitString.Length - 1];
            NAVProcessInfo[IndexNo, 3] = "";
            
            for (int j = 0; j < SplitString.Length - 1; j++)
            {
                NAVProcessInfo[IndexNo, 3] += SplitString[j] + "\\";
            }

            // Remove leading and traililng "
            if (Convert.ToString(NAVProcessInfo[IndexNo, 3][0]) == "\"") 
            { 
                NAVProcessInfo[IndexNo, 3] = NAVProcessInfo[IndexNo, 3].Substring(1);
            }

            int Len = NAVProcessInfo[IndexNo, 10].Length;

            if (Convert.ToString(NAVProcessInfo[IndexNo, 10][Len - 1]) == "\"") 
            {
                NAVProcessInfo[IndexNo, 10] = NAVProcessInfo[IndexNo, 10].Remove(Len - 1); 
            }

            NAVServicesList.Items.Add(NAVProcessInfo[IndexNo, 1] + "    " + NAVProcessInfo[IndexNo, 9] + "    " + NAVProcessInfo[IndexNo, 8]);
        }

        private void UpdateNAVConfigInfo(int NAVServiceNo)
        {
            //Name (0)
            //Account (8)
            //Path (3)
            //Status (9)
            //Mode (7)

            NAVServiceName.Text = NAVProcessInfo[NAVServiceNo, 0];
            NAVServiceAccount.Text = NAVProcessInfo[NAVServiceNo, 8];
            NAVServicePath.Text = NAVProcessInfo[NAVServiceNo, 3];
            NAVServiceStatus.Text = NAVProcessInfo[NAVServiceNo, 9];
            NAVServiceMode.Text = NAVProcessInfo[NAVServiceNo, 7];
            NAVServiceFile.Text = NAVProcessInfo[NAVServiceNo, 10];
        }

        private void NAVServicesList_SelectedIndexChanged(object sender, EventArgs e)
        {
            CurrentListIndex = NAVServicesList.SelectedIndex;
            UpdateNAVConfigInfo(CurrentListIndex);
        }

        private string GetConfigKey(XmlDocument XMLConfig, string KeyValue)
        {
            try
            {
                XmlNode KeyNode = XMLConfig.SelectSingleNode("appSettings/add[@key=\"" + KeyValue + "\"]");
                KeyNode = KeyNode.Attributes.GetNamedItem("value");
                return (Convert.ToString(KeyNode.Value));
            }
            catch (Exception e)
            {
                MessageBox.Show(KeyValue + " not found in Config file.\n\n" + e.Message);
            }
            return ("");
        }

        private void SetConfigKey(ref XmlDocument XMLConfig, string KeyName, string KeyValue)
        {
            XmlNode ConfigNode = XMLConfig.SelectSingleNode("appSettings/add[@key=\"" + KeyName + "\"]");
            XmlNode ValueNode = ConfigNode.Attributes.GetNamedItem("value");
            ValueNode.Value = KeyValue;
        }

        private void CSDynamicsPowerShellAdmin_Load(object sender, EventArgs e)
        {

        }

        private string GetConfigFileName()
        {
            string ConfigFileName = NAVProcessInfo[CurrentListIndex, 3] + "CustomSettings.config";

            if (NAVServerName.Text != "")
            {
                ConfigFileName = ConfigFileName.Replace(":", "$");
                ConfigFileName = "\\\\" + NAVServerName.Text + "\\" + ConfigFileName;
            }

            ConfigFileName.Replace("\"", "");
            return (ConfigFileName);
        }

        private void StartStopNAVService(string MethodName)
        {
            string computerParameter = "";

            if (NAVServerName.Text != "")
            {
                computerParameter = "-computer " + NAVServerName.Text + " ";
            }

            string Script =
                  "(Get-WmiObject " + computerParameter + "Win32_Service -Filter \"Name='" +
                  NAVServiceName.Text.Replace("$", "`$") +
                  "'\").InvokeMethod(\"" +
                  MethodName + "\",$null)";

            using (Runspace runSpace = RunspaceFactory.CreateRunspace())
            {
                runSpace.Open();
                Pipeline pipeLine = runSpace.CreatePipeline();
                pipeLine.Commands.AddScript(Script);
                Collection<PSObject> NAVProcesses = pipeLine.Invoke();
                int ReturnValue = Convert.ToInt16(NAVProcesses[0].ToString());

                // Return Codes:
                // http://msdn.microsoft.com/en-us/library/aa393660(VS.85).aspx 

                switch (ReturnValue)
                {
                    case 0:
                        MessageBox.Show("Operation succeeded.\nClick Get NAV Services to refresh status in the list.");
                        break;
                    case 1:
                        MessageBox.Show("Error code 1 - Not Supported.");
                        break;

                    case 2:
                        MessageBox.Show("Error code 2 - Permission denied.\nMake sure you have permissions and run as administrator.");
                        break;       
                    case 5:
                        MessageBox.Show("Error code 5 - Service Cannot Accept Control.\nPossible reason: Service is (already) stopped.");
                        break;

                    case 10:
                        MessageBox.Show("Error code 10 - Service already running.");
                        break;
                    case 15:
                        MessageBox.Show("Error code 15 - Service Logon Failure.");
                        break;

                    default:
                        MessageBox.Show("Operation failed with error code " + ReturnValue.ToString() + ".\n" +
                            "Look up error codes here http://msdn.microsoft.com/en-us/library/aa393660(VS.85).aspx");
                        break;
                }
            }
        }
    }
}

