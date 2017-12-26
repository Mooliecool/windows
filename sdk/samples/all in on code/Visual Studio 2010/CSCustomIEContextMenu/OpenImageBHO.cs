/****************************** Module Header ******************************\
* Module Name:  OpenImageBHO.cs
* Project:      CSCustomIEContextMenu
* Copyright (c) Microsoft Corporation.
* 
* The class OpenImageBHO is a Browser Helper Object which runs within Internet
* Explorer and offers additional services.
* 
* A BHO is a dynamic-link library (DLL) capable of attaching itself to any new 
* instance of Internet Explorer or Windows Explorer. Such a module can get in touch 
* with the browser through the container's site. In general, a site is an intermediate
* object placed in the middle of the container and each contained object. When the
* container is Internet Explorer (or Windows Explorer), the object is now required 
* to implement a simpler and lighter interface called IObjectWithSite. 
* It provides just two methods SetSite and GetSite. 
* 
* This class is used to set the IDocHostUIHandler of the HtmlDocument.
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
using System.Runtime.InteropServices;
using CSCustomIEContextMenu.NativeMethods;
using Microsoft.Win32;
using SHDocVw;

namespace CSCustomIEContextMenu
{
    /// <summary>
    /// Set the GUID of this class and specify that this class is ComVisible.
    /// A BHO must implement the interface IObjectWithSite. 
    /// </summary>
    [ComVisible(true)]
    [ClassInterface(ClassInterfaceType.None)]
    [Guid("AA0B1334-E7F5-4F75-A1DE-0993098AAF01")]
    public class OpenImageBHO : IObjectWithSite, IDisposable
    {
        private bool disposed = false;

        // To register a BHO, a new key should be created under this key.
        private const string BHORegistryKey =
            "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Browser Helper Objects";

        // Current IE instance. For IE7 or later version, an IE Tab is just 
        // an IE instance.
        private InternetExplorer ieInstance;

        private OpenImageHandler openImageDocHostUIHandler;


        #region Com Register/UnRegister Methods
        /// <summary>
        /// When this class is registered to COM, add a new key to the BHORegistryKey 
        /// to make IE use this BHO.
        /// On 64bit machine, if the platform of this assembly and the installer is x86,
        /// 32 bit IE can use this BHO. If the platform of this assembly and the installer
        /// is x64, 64 bit IE can use this BHO.
        /// </summary>
        [ComRegisterFunction]
        public static void RegisterBHO(Type t)
        {

            // If the key exists, CreateSubKey will open it.
            RegistryKey bhosKey = Registry.LocalMachine.CreateSubKey(
                BHORegistryKey,
                RegistryKeyPermissionCheck.ReadWriteSubTree);

            // 32 digits separated by hyphens, enclosed in braces: 
            // {00000000-0000-0000-0000-000000000000}
            string bhoKeyStr = t.GUID.ToString("B");

            RegistryKey bhoKey = bhosKey.CreateSubKey(bhoKeyStr);

            // NoExplorer:dword = 1 prevents the BHO to be loaded by Explorer
            bhoKey.SetValue("NoExplorer", 1);
            bhosKey.Close();
            bhoKey.Close();
        }

        /// <summary>
        /// When this class is unregistered from COM, delete the key.
        /// </summary>
        [ComUnregisterFunction]
        public static void UnregisterBHO(Type t)
        {
            RegistryKey bhosKey = Registry.LocalMachine.OpenSubKey(BHORegistryKey, true);
            string guidString = t.GUID.ToString("B");
            if (bhosKey != null)
            {
                bhosKey.DeleteSubKey(guidString, false);
            }

            bhosKey.Close();
        }


        #endregion

        #region IObjectWithSite Members
        /// <summary>
        /// This method is called when the BHO is instantiated and when
        /// it is destroyed. The site is an object implemented the 
        /// interface InternetExplorer.
        /// </summary>
        /// <param name="site"></param>
        public void SetSite(Object site)
        {
            if (site != null)
            {
                ieInstance = (InternetExplorer)site;

                openImageDocHostUIHandler = new OpenImageHandler(ieInstance);

                // Register the DocumentComplete event.
                ieInstance.DocumentComplete +=
                    new DWebBrowserEvents2_DocumentCompleteEventHandler(
                        ieInstance_DocumentComplete);
            }
        }

        /// <summary>
        /// Retrieves and returns the specified interface from the last site
        /// set through SetSite(). The typical implementation will query the
        /// previously stored pUnkSite pointer for the specified interface.
        /// </summary>
        public void GetSite(ref Guid guid, out Object ppvSite)
        {
            IntPtr punk = Marshal.GetIUnknownForObject(ieInstance);
            ppvSite = new object();
            IntPtr ppvSiteIntPtr = Marshal.GetIUnknownForObject(ppvSite);
            int hr = Marshal.QueryInterface(punk, ref guid, out ppvSiteIntPtr);
            Marshal.ThrowExceptionForHR(hr);
            Marshal.Release(punk);
        }
        #endregion

        #region event handler

        /// <summary>
        /// Handle the DocumentComplete event.
        /// </summary>
        /// <param name="pDisp">
        /// The pDisp is an an object implemented the interface InternetExplorer.
        /// By default, this object is the same as the ieInstance, but if the page 
        /// contains many frames, each frame has its own document.
        /// </param>
        void ieInstance_DocumentComplete(object pDisp, ref object URL)
        {
            string url = URL as string;

            if (string.IsNullOrEmpty(url)
                || url.Equals("about:blank", StringComparison.OrdinalIgnoreCase))
            {
                return;
            }

            InternetExplorer explorer = pDisp as InternetExplorer;

            // Set the handler of the document in InternetExplorer.
            if (explorer != null)
            {
                NativeMethods.ICustomDoc customDoc = (NativeMethods.ICustomDoc)explorer.Document;
                customDoc.SetUIHandler(openImageDocHostUIHandler);
            }
        }
        #endregion

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }
        protected virtual void Dispose(bool disposing)
        {
            // Protect from being called multiple times.
            if (disposed) return;

            if (disposing)
            {
                // Clean up all managed resources.
                if (openImageDocHostUIHandler != null)
                {
                    openImageDocHostUIHandler.Dispose();
                }
            }
            disposed = true;
        }
    }
}
