using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.Xml.XPath;
using System.Windows;

namespace Microsoft.Samples.WinFX.BuildDemo
{
    public static class SettingsSystem
    {
        private static XmlDocument m_SettingsDocument = null;
        private static string m_SettingsPath = null;

        public static void SaveSetting(string SettingName, string SettingValue)
        {
            if (m_SettingsDocument == null)
            {
                loadXMLSource();
            }

            string settingsPath = "Settings/" + SettingName.Replace('.', '/');

            XmlNode settingNode = m_SettingsDocument.SelectSingleNode(settingsPath);

            //Node doesn't exist, create the path

            if (settingNode==null)
            {

                string[] settingLevels = SettingName.Split('.');

                XmlNode currentNode = m_SettingsDocument.ParentNode;

                if (m_SettingsDocument.SelectSingleNode("Settings") == null)
                {
                    //root doesn't exist, i.e. an empty document, so create the basics

                    XmlNode tempNode=null;

                    tempNode = m_SettingsDocument.CreateNode(XmlNodeType.XmlDeclaration, "", "");
                    m_SettingsDocument.AppendChild(currentNode);

                    tempNode = m_SettingsDocument.CreateNode(XmlNodeType.Element, "Settings", "");
                    m_SettingsDocument.AppendChild(currentNode);
                }


                //create each level of the path if it doesn't exist

                currentNode = m_SettingsDocument.SelectSingleNode("Settings");

                foreach (string levelName in settingLevels)
                {
                    XmlNode newNode=currentNode.SelectSingleNode(levelName);

                    if (newNode == null)
                    {
                        newNode = m_SettingsDocument.CreateNode(XmlNodeType.Element, levelName,"");
                        currentNode.AppendChild(newNode);
                    }

                    currentNode = newNode;
                }

                settingNode = currentNode;

            }

            //set the value

            settingNode.InnerText = SettingValue;

            //save the XML file

            saveXMLSource();
        }

        public static bool HasSetting(string SettingName)
        {
            if (m_SettingsDocument == null)
            {
                loadXMLSource();
            }

            string settingsPath = "Settings/" + SettingName.Replace('.', '/');

            XmlNode settingNode = m_SettingsDocument.SelectSingleNode(settingsPath);

            return (settingNode!=null);
        }

        public static void GetSetting<T>(string SettingName, ref T SettingReturn)
        {
            if (m_SettingsDocument == null)
            {
                loadXMLSource();
            }

            //change the settings heirachy into an XPath

            string settingsPath = "Settings/" + SettingName.Replace('.', '/');

            XmlNode settingNode=m_SettingsDocument.SelectSingleNode(settingsPath);

            if (settingNode == null)
            {
                throw new InvalidSettingException(SettingName);
            }

            if (typeof(T) == typeof(bool))
            {
                SettingReturn = (T) bool.Parse(settingNode.InnerText);
            }
            else
            {
                SettingReturn = (T) settingNode.InnerText;
            }
        }

        private static void loadXMLSource()
        {

            m_SettingsPath = System.Environment.GetFolderPath(System.Environment.SpecialFolder.LocalApplicationData) + @"\builddemo.xml";

            m_SettingsDocument = new XmlDocument();

            if (System.IO.File.Exists(m_SettingsPath))
            {
                m_SettingsDocument.Load(m_SettingsPath);
            }
        }

        private static void saveXMLSource()
        {
            m_SettingsDocument.Save(m_SettingsPath);
        }
    }

    public class InvalidSettingException : System.Exception {

        private string m_SettingName = "";

        public string SettingName
        {
            get { return m_SettingName; }
            set { m_SettingName = value; }
        }

        public InvalidSettingException(string settingName)
        {
            m_SettingName = settingName;
        }
    }
}
