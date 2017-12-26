namespace CSOutlookImportedFormRegion
{
    partial class ImportedFormRegion : Microsoft.Office.Tools.Outlook.ImportedFormRegion
    {
        private Microsoft.Office.Interop.Outlook._DRecipientControl _RecipientControl2;
        private Microsoft.Office.Interop.Outlook.OlkCommandButton btnTo;
        private Microsoft.Office.Interop.Outlook.OlkComboBox comboBox1;
        private Microsoft.Office.Interop.Outlook._DDocSiteControl _DocSiteControl1;
        private Microsoft.Office.Interop.Outlook.OlkCommandButton btnSend;
        private Microsoft.Office.Interop.Outlook.OlkCommandButton btnAdd;
        private Microsoft.Office.Interop.Outlook.OlkCommandButton btnRemove;
        private Microsoft.Office.Interop.Outlook.OlkCommandButton btnDisable;

        public ImportedFormRegion(Microsoft.Office.Interop.Outlook.FormRegion formRegion)
            : base(formRegion)
        {
            this.FormRegionShowing += new System.EventHandler(this.ImportedFormRegion_FormRegionShowing);
            this.FormRegionClosed += new System.EventHandler(this.ImportedFormRegion_FormRegionClosed);
        }

        protected override void InitializeControls()
        {
            this._RecipientControl2 = this.GetFormRegionControl<Microsoft.Office.Interop.Outlook._DRecipientControl>("_RecipientControl2");
            this.btnTo = this.GetFormRegionControl<Microsoft.Office.Interop.Outlook.OlkCommandButton>("btnTo");
            this.comboBox1 = this.GetFormRegionControl<Microsoft.Office.Interop.Outlook.OlkComboBox>("ComboBox1");
            this._DocSiteControl1 = this.GetFormRegionControl<Microsoft.Office.Interop.Outlook._DDocSiteControl>("_DocSiteControl1");
            this.btnSend = this.GetFormRegionControl<Microsoft.Office.Interop.Outlook.OlkCommandButton>("btnSend");
            this.btnAdd = this.GetFormRegionControl<Microsoft.Office.Interop.Outlook.OlkCommandButton>("btnAdd");
            this.btnRemove = this.GetFormRegionControl<Microsoft.Office.Interop.Outlook.OlkCommandButton>("btnRemove");
            this.btnDisable = this.GetFormRegionControl<Microsoft.Office.Interop.Outlook.OlkCommandButton>("btnDisable");

        }

        public partial class ImportedFormRegionFactory : Microsoft.Office.Tools.Outlook.IFormRegionFactory
        {
            public event System.EventHandler<Microsoft.Office.Tools.Outlook.FormRegionInitializingEventArgs> FormRegionInitializing;

            private Microsoft.Office.Tools.Outlook.FormRegionManifest _Manifest;

            [System.Diagnostics.DebuggerNonUserCodeAttribute()]
            public ImportedFormRegionFactory()
            {
                this._Manifest = new Microsoft.Office.Tools.Outlook.FormRegionManifest();
                this.InitializeManifest();
                this.FormRegionInitializing += new System.EventHandler<Microsoft.Office.Tools.Outlook.FormRegionInitializingEventArgs>(this.ImportedFormRegionFactory_FormRegionInitializing);
            }

            [System.Diagnostics.DebuggerNonUserCodeAttribute()]
            public Microsoft.Office.Tools.Outlook.FormRegionManifest Manifest
            {
                get
                {
                    return this._Manifest;
                }
            }

            [System.Diagnostics.DebuggerNonUserCodeAttribute()]
            Microsoft.Office.Tools.Outlook.IFormRegion Microsoft.Office.Tools.Outlook.IFormRegionFactory.CreateFormRegion(Microsoft.Office.Interop.Outlook.FormRegion formRegion)
            {
                ImportedFormRegion form = new ImportedFormRegion(formRegion);
                form.Factory = this;
                return form;
            }

            [System.Diagnostics.DebuggerNonUserCodeAttribute()]
            byte[] Microsoft.Office.Tools.Outlook.IFormRegionFactory.GetFormRegionStorage(object outlookItem, Microsoft.Office.Interop.Outlook.OlFormRegionMode formRegionMode, Microsoft.Office.Interop.Outlook.OlFormRegionSize formRegionSize)
            {
                System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(ImportedFormRegion));
                return (byte[])resources.GetObject("ImportedFormRegion");
            }

            [System.Diagnostics.DebuggerNonUserCodeAttribute()]
            bool Microsoft.Office.Tools.Outlook.IFormRegionFactory.IsDisplayedForItem(object outlookItem, Microsoft.Office.Interop.Outlook.OlFormRegionMode formRegionMode, Microsoft.Office.Interop.Outlook.OlFormRegionSize formRegionSize)
            {
                if (this.FormRegionInitializing != null)
                {
                    Microsoft.Office.Tools.Outlook.FormRegionInitializingEventArgs cancelArgs = new Microsoft.Office.Tools.Outlook.FormRegionInitializingEventArgs(outlookItem, formRegionMode, formRegionSize, false);
                    this.FormRegionInitializing(this, cancelArgs);
                    return !cancelArgs.Cancel;
                }
                else
                {
                    return true;
                }
            }

            [System.Diagnostics.DebuggerNonUserCodeAttribute()]
            string Microsoft.Office.Tools.Outlook.IFormRegionFactory.Kind
            {
                get
                {
                    return Microsoft.Office.Tools.Outlook.FormRegionKindConstants.Ofs;
                }
            }
        }
    }

    partial class WindowFormRegionCollection
    {
        internal ImportedFormRegion ImportedFormRegion
        {
            get
            {
                return this.FindFirst<ImportedFormRegion>();
            }
        }
    }
}
