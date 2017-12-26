using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Configuration.Install;


namespace CSCOMService
{
    [RunInstaller(true)]
    public partial class COMServiceInstaller : Installer
    {
        public COMServiceInstaller()
        {
            InitializeComponent();
        }
    }
}
