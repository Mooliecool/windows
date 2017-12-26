/****************************** Module Header ******************************\
* Module Name:  BHOInstaller.cs
* Project:      CSCustomIEContextMenu
* Copyright (c) Microsoft Corporation.
* 
* The class CustomIEContextMenuInstaller inherits the class System.Configuration.Install.Installer.
* The methods Install and Uninstall will be run when this application is being 
* installed or uninstalled.
* 
* This action has to be added the the custom actions of the installer to take effect. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System.ComponentModel;
using System.Configuration.Install;
using System.Runtime.InteropServices;

namespace CSCustomIEContextMenu
{
    [RunInstaller(true), ComVisible(false)]
    public partial class CustomIEContextMenuInstaller : System.Configuration.Install.Installer
    {
        public CustomIEContextMenuInstaller()
        {
            InitializeComponent();
        }

        /// <summary>
        /// This is called when installer's custom action executes and
        /// registers the toolbar which is extending a bandobject, as COM server.
        /// </summary>
        /// <param name="stateSaver"></param>     
        public override void Install(System.Collections.IDictionary stateSaver)
        {
            base.Install(stateSaver);

            OpenImageMenuExt.RegisterMenuExt();

            RegistrationServices regsrv = new RegistrationServices();
            if (!regsrv.RegisterAssembly(this.GetType().Assembly,
            AssemblyRegistrationFlags.SetCodeBase))
            {
                throw new InstallException("Failed To Register for COM");
            }
        }

        /// <summary>
        /// This is called when installer's custom action executes and
        /// registers the toolbar which is extending a bandobject, as COM server.
        /// </summary>
        /// <param name="stateSaver"></param>     
        public override void Uninstall(System.Collections.IDictionary savedState)
        {
            base.Uninstall(savedState);

            OpenImageMenuExt.UnRegisterMenuExt();

            RegistrationServices regsrv = new RegistrationServices();
            if (!regsrv.UnregisterAssembly(this.GetType().Assembly))
            {
                throw new InstallException("Failed To Unregister for COM");
            }
        }
    }
}
