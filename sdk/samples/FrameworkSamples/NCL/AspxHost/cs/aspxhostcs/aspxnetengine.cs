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


#region Using directives

using System;
using System.Collections.Generic;
using System.Text;
using System.Web.Hosting;
using System.Net;
using System.Web;
using System.IO;
using System.Threading;
using System.Diagnostics;
using System.Globalization;
using Microsoft.Samples.AspxHostCS.Properties;
#endregion

namespace Microsoft.Samples.AspxHostCS
{
    public class AspxNetEngine : MarshalByRefObject, IDisposable
    {
        private String m_VirtualAlias;
        private String m_PhysicalDirectory;

        public String VirtualAlias
        {
            get
            {
                return m_VirtualAlias;
            }
        }

        public String PhysicalDirectory
        {
            get
            {
                return m_PhysicalDirectory;
            }
        }

        public AspxNetEngine ExecutingAppDomain
        {
            get
            {
                return m_ExecutingAppDomain;
            }
        }

        //Needed as IDisposable is implemented
        public void Dispose()
        {
            m_ExecutingAppDomain.Dispose();
        }

        private AspxNetEngine m_ExecutingAppDomain;

        /**
         * This is just for the ASPX Hosting API's ApplicationHost object 
         * to create an object of this class when we call ApplicationHost.CreateApplicationHost
         */
        public AspxNetEngine() { }

        /**
         * Called to configure a Physical directory as a Virtual alias.
         * To configure a directory as a AspxApplication, the exe for this project ASPXhostCS.exe
         * and ASPXHostCS.exe.config (if present) should be present in the bin directory under 
         * the physical directory being configured. The reason being, that the call to API
         * ApplicationHost.CreateApplicationhost creates a new app domain and will instantiate
         * a class specified in the typeof variable. Putting it in the bin directory enables
         * the hosting api to load the class.
         */
        internal AspxNetEngine(String virtualAlias, String physicalDir)
        {
            m_VirtualAlias = virtualAlias;
            m_PhysicalDirectory = physicalDir;

            Console.WriteLine("Creating a new AspxEngine.");

            //m_ExecutingEngine will be the actual object that the hosting API created for 
            //us and so to execute a page in the Application we will call this object to 
            //process requests
            m_ExecutingAppDomain = (AspxNetEngine)ApplicationHost.CreateApplicationHost(typeof(AspxNetEngine), m_VirtualAlias,m_PhysicalDirectory);

            Console.WriteLine("New AspxEngine created for alias " + m_VirtualAlias);
        }

        //Utility function
        public override string ToString()
        {
            return String.Format(new CultureInfo("En-us"),"AspNetEngine: (ID : {0}), (Virtual Alias  : {1}), (Physical Directory : {2})", 1, m_VirtualAlias, m_PhysicalDirectory);
        }


        //Will be called when an ASPX page is to be served.
        public void ProcessRequest(String page, string query, AspxRequestInfo dataHolder)
        { 
            Console.WriteLine("Request received for File {0} under alias {1}", page, this.m_VirtualAlias );
            //catch the exception so we dont bring down the whole app

            try
            {
                AspxPage swr = new AspxPage(page, query, dataHolder);
                HttpRuntime.ProcessRequest(swr);
            }
            catch (Exception e1)
            {
                Console.WriteLine("Error:\n" + e1.ToString());
                //Supress the internal exception. If needed we can pass the exception back
                dataHolder.ResponseStreamAsWriter.WriteLine("500 Internal Error.");
                throw new AspxException(Resources.internalError, e1);
            }
            finally
            {
                //Flush the response stream so that the Browser/calling application doesnt time out
                dataHolder.ResponseStreamAsWriter.Flush();
                Console.WriteLine("Done serving Response");
            }
        }

        /**
         * HttpListenerContext is not Marshallable and so we have an Marshallable object
         * that will contain all required objects that we need to pass on to our 
         * implementation of SimpleWorkerRequest so we can handle PUT/POST and 
         * Client Certificate requests. If any other object from HttpListenerContext class
         * is required and that Object extends MarshallByRefObject add it to the data holder
         * object here
         */
        public static AspxRequestInfo PrepareAspxRequestInfoObject(HttpListenerContext context)
        {
            AspxRequestInfo dataHolder = new AspxRequestInfo();
            dataHolder.m_CookieCollection = context.Request.Cookies;
            dataHolder.m_Headers = context.Request.Headers;
            dataHolder.m_RequestStream = context.Request.InputStream;
            dataHolder.m_ResponseStream = context.Response.OutputStream;
            dataHolder.m_UserAgent = context.Request.UserAgent;
            dataHolder.m_HttpMethod = context.Request.HttpMethod;
            dataHolder.m_ResponseStreamAsWriter = new StreamWriter(context.Response.OutputStream);
            return dataHolder;
        }

    }
}
