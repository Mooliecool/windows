/****************************** Module Header ******************************\
* Module Name:              SampleContext.cs
* Project:                  CSSL3Navigation
* Copyright (c) Microsoft Corporation.
* 
* SampleContext class file, SampleContext is used for storing sample and author
* collections and relation reference.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Collections.Generic;
using System.Xml.Linq;
using System.Linq;
using CSSL3Navigation.Views;

namespace CSSL3Navigation
{
    // Sample entity definition
    public class Sample
    {
        public string Name{set;get;}
        public string Language { set; get; }
        public AuthorInfo Author { set; get; }
        public string Description { set; get; }
        public string Link { set; get; }
        public string ScrImageUri { set; get; }
    }

    // Author entity definition
    public class AuthorInfo
    {
        public string Name { set; get; }
        public string Description { set; get; }
        public string Gender { set; get; }
        public string PhotoUri { set; get; }
    }

    // Sample context stores sample and author collections
    // and relation reference.
    public class SampleContext
    {
        public List<Sample> Samples { get; protected set; }
        public List<AuthorInfo> AuthorInfos { get; protected set; }
        XDocument _xdoc;

        public SampleContext(string xmlname)
        {
            // Download xml
            WebClient wc = new WebClient();
            var xmluri = new Uri(Application.Current.Host.Source,"/"+ xmlname);
            wc.OpenReadCompleted += new OpenReadCompletedEventHandler(wc_OpenReadCompleted);
            wc.OpenReadAsync(xmluri,xmluri);
        }

        void wc_OpenReadCompleted(object sender, OpenReadCompletedEventArgs e)
        {
            Uri xmluri = e.UserState as Uri;
            if (e.Error == null)
            {
                try
                {
                    _xdoc = XDocument.Load(e.Result);
                    InitializeContext();
                }
                catch
                {
                    new ErrorWindow("Error occured while parsing xml file\n"
                        + xmluri).Show();
                }
            }
            else
            {
                new ErrorWindow("Error occured while downloading xml file\n"
                    +xmluri).Show();
            }
        }

        // Load the entity collection
        void InitializeContext()
        {
            if (_xdoc != null)
            {
                AuthorInfos = (from a in _xdoc.Descendants("AuthorInfo")
                               select new AuthorInfo
                               {
                                   Name = a.Attribute("Name").Value,
                                   Gender = GetStringValue(a.Attribute("Gender")),
                                   PhotoUri = GetStringValue(a.Attribute("Photo")),
                                   Description = GetStringValue(a.Element("Description"))
                               }).ToList<AuthorInfo>();

                Samples = (from s in _xdoc.Descendants("Sample")
                           select new Sample
                           {
                               Name = GetStringValue(s.Attribute("Name")),
                               Language = GetStringValue(s.Attribute("Language")),
                               Author = AuthorInfos.FirstOrDefault(
                                   a1 => a1.Name.Equals(s.Attribute("Author").Value)
                               ),
                               Link = GetStringValue(s.Attribute("Link")),
                               Description = GetStringValue(s.Element("Description")),
                               ScrImageUri = GetStringValue(s.Attribute("ScrImage"))
                           }).ToList<Sample>();
            }
            else
            {
                Samples = new List<Sample>();
                AuthorInfos = new List<AuthorInfo>();
            }
        }

        string GetStringValue(XAttribute xa)
        {
            if (xa != null)
                return xa.Value;
            else
                return null;
        }
        string GetStringValue(XElement xe)
        {
            if (xe != null)
                return xe.Value;
            else
                return null;
        }
    }
}
