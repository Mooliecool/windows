namespace CSDynamicsPowerShellAdmin
{
    partial class CSDynamicsPowerShellAdmin
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.NAVServerName = new System.Windows.Forms.TextBox();
            this.lblNAVServerName = new System.Windows.Forms.Label();
            this.btnGetNAVServices = new System.Windows.Forms.Button();
            this.NAVServicesList = new System.Windows.Forms.ListBox();
            this.NAVServiceName = new System.Windows.Forms.TextBox();
            this.lblNAVServiceName = new System.Windows.Forms.Label();
            this.lblNAVServiceAccount = new System.Windows.Forms.Label();
            this.NAVServiceAccount = new System.Windows.Forms.TextBox();
            this.lblNAVServicePath = new System.Windows.Forms.Label();
            this.NAVServicePath = new System.Windows.Forms.TextBox();
            this.lblNAVServiceStatus = new System.Windows.Forms.Label();
            this.NAVServiceStatus = new System.Windows.Forms.TextBox();
            this.lblNAVServiceMode = new System.Windows.Forms.Label();
            this.NAVServiceMode = new System.Windows.Forms.TextBox();
            this.btnGetConfig = new System.Windows.Forms.Button();
            this.lblNAVServiceFile = new System.Windows.Forms.Label();
            this.NAVServiceFile = new System.Windows.Forms.TextBox();
            this.ConfigDBServer = new System.Windows.Forms.TextBox();
            this.lblConfigDBServer = new System.Windows.Forms.Label();
            this.lblConfigDBInstance = new System.Windows.Forms.Label();
            this.ConfigDBInstance = new System.Windows.Forms.TextBox();
            this.lblConfigDBName = new System.Windows.Forms.Label();
            this.ConfigDBName = new System.Windows.Forms.TextBox();
            this.lblConfigServerInstance = new System.Windows.Forms.Label();
            this.ConfigServerInstance = new System.Windows.Forms.TextBox();
            this.lblConfigServerPort = new System.Windows.Forms.Label();
            this.ConfigServerPort = new System.Windows.Forms.TextBox();
            this.lblConfigWebServicePort = new System.Windows.Forms.Label();
            this.ConfigWebServicePort = new System.Windows.Forms.TextBox();
            this.lblConfigWebServiceUseNTLM = new System.Windows.Forms.Label();
            this.ConfigWebServiceUseNTLM = new System.Windows.Forms.TextBox();
            this.lblConfigEnableDebugging = new System.Windows.Forms.Label();
            this.ConfigEnableDebugging = new System.Windows.Forms.TextBox();
            this.btnUpdateConfig = new System.Windows.Forms.Button();
            this.btnStartService = new System.Windows.Forms.Button();
            this.btnStopService = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // NAVServerName
            // 
            this.NAVServerName.Location = new System.Drawing.Point(217, 16);
            this.NAVServerName.Name = "NAVServerName";
            this.NAVServerName.Size = new System.Drawing.Size(100, 20);
            this.NAVServerName.TabIndex = 0;
            // 
            // lblNAVServerName
            // 
            this.lblNAVServerName.AutoSize = true;
            this.lblNAVServerName.Location = new System.Drawing.Point(5, 19);
            this.lblNAVServerName.Name = "lblNAVServerName";
            this.lblNAVServerName.Size = new System.Drawing.Size(195, 13);
            this.lblNAVServerName.TabIndex = 1;
            this.lblNAVServerName.Text = "NAV Server Name (Leave blank if local)";
            // 
            // btnGetNAVServices
            // 
            this.btnGetNAVServices.Location = new System.Drawing.Point(8, 44);
            this.btnGetNAVServices.Name = "btnGetNAVServices";
            this.btnGetNAVServices.Size = new System.Drawing.Size(147, 23);
            this.btnGetNAVServices.TabIndex = 2;
            this.btnGetNAVServices.Text = "Get NAV Services";
            this.btnGetNAVServices.UseVisualStyleBackColor = true;
            this.btnGetNAVServices.Click += new System.EventHandler(this.btnGetNAVServices_Click);
            // 
            // NAVServicesList
            // 
            this.NAVServicesList.FormattingEnabled = true;
            this.NAVServicesList.Location = new System.Drawing.Point(8, 73);
            this.NAVServicesList.Name = "NAVServicesList";
            this.NAVServicesList.Size = new System.Drawing.Size(783, 160);
            this.NAVServicesList.TabIndex = 3;
            this.NAVServicesList.SelectedIndexChanged += new System.EventHandler(this.NAVServicesList_SelectedIndexChanged);
            // 
            // NAVServiceName
            // 
            this.NAVServiceName.Location = new System.Drawing.Point(81, 272);
            this.NAVServiceName.Name = "NAVServiceName";
            this.NAVServiceName.ReadOnly = true;
            this.NAVServiceName.Size = new System.Drawing.Size(219, 20);
            this.NAVServiceName.TabIndex = 4;
            // 
            // lblNAVServiceName
            // 
            this.lblNAVServiceName.AutoSize = true;
            this.lblNAVServiceName.Location = new System.Drawing.Point(9, 279);
            this.lblNAVServiceName.Name = "lblNAVServiceName";
            this.lblNAVServiceName.Size = new System.Drawing.Size(35, 13);
            this.lblNAVServiceName.TabIndex = 5;
            this.lblNAVServiceName.Text = "Name";
            // 
            // lblNAVServiceAccount
            // 
            this.lblNAVServiceAccount.AutoSize = true;
            this.lblNAVServiceAccount.Location = new System.Drawing.Point(9, 305);
            this.lblNAVServiceAccount.Name = "lblNAVServiceAccount";
            this.lblNAVServiceAccount.Size = new System.Drawing.Size(47, 13);
            this.lblNAVServiceAccount.TabIndex = 7;
            this.lblNAVServiceAccount.Text = "Account";
            // 
            // NAVServiceAccount
            // 
            this.NAVServiceAccount.Location = new System.Drawing.Point(81, 298);
            this.NAVServiceAccount.Name = "NAVServiceAccount";
            this.NAVServiceAccount.ReadOnly = true;
            this.NAVServiceAccount.Size = new System.Drawing.Size(219, 20);
            this.NAVServiceAccount.TabIndex = 6;
            // 
            // lblNAVServicePath
            // 
            this.lblNAVServicePath.AutoSize = true;
            this.lblNAVServicePath.Location = new System.Drawing.Point(9, 331);
            this.lblNAVServicePath.Name = "lblNAVServicePath";
            this.lblNAVServicePath.Size = new System.Drawing.Size(29, 13);
            this.lblNAVServicePath.TabIndex = 9;
            this.lblNAVServicePath.Text = "Path";
            // 
            // NAVServicePath
            // 
            this.NAVServicePath.Location = new System.Drawing.Point(81, 324);
            this.NAVServicePath.Name = "NAVServicePath";
            this.NAVServicePath.ReadOnly = true;
            this.NAVServicePath.Size = new System.Drawing.Size(219, 20);
            this.NAVServicePath.TabIndex = 8;
            // 
            // lblNAVServiceStatus
            // 
            this.lblNAVServiceStatus.AutoSize = true;
            this.lblNAVServiceStatus.Location = new System.Drawing.Point(9, 383);
            this.lblNAVServiceStatus.Name = "lblNAVServiceStatus";
            this.lblNAVServiceStatus.Size = new System.Drawing.Size(37, 13);
            this.lblNAVServiceStatus.TabIndex = 11;
            this.lblNAVServiceStatus.Text = "Status";
            // 
            // NAVServiceStatus
            // 
            this.NAVServiceStatus.Location = new System.Drawing.Point(81, 376);
            this.NAVServiceStatus.Name = "NAVServiceStatus";
            this.NAVServiceStatus.ReadOnly = true;
            this.NAVServiceStatus.Size = new System.Drawing.Size(219, 20);
            this.NAVServiceStatus.TabIndex = 10;
            // 
            // lblNAVServiceMode
            // 
            this.lblNAVServiceMode.AutoSize = true;
            this.lblNAVServiceMode.Location = new System.Drawing.Point(9, 409);
            this.lblNAVServiceMode.Name = "lblNAVServiceMode";
            this.lblNAVServiceMode.Size = new System.Drawing.Size(34, 13);
            this.lblNAVServiceMode.TabIndex = 13;
            this.lblNAVServiceMode.Text = "Mode";
            // 
            // NAVServiceMode
            // 
            this.NAVServiceMode.Location = new System.Drawing.Point(81, 402);
            this.NAVServiceMode.Name = "NAVServiceMode";
            this.NAVServiceMode.ReadOnly = true;
            this.NAVServiceMode.Size = new System.Drawing.Size(219, 20);
            this.NAVServiceMode.TabIndex = 12;
            // 
            // btnGetConfig
            // 
            this.btnGetConfig.Location = new System.Drawing.Point(366, 244);
            this.btnGetConfig.Name = "btnGetConfig";
            this.btnGetConfig.Size = new System.Drawing.Size(75, 23);
            this.btnGetConfig.TabIndex = 14;
            this.btnGetConfig.Text = "Get Config";
            this.btnGetConfig.UseVisualStyleBackColor = true;
            this.btnGetConfig.Click += new System.EventHandler(this.btnGetConfig_Click);
            // 
            // lblNAVServiceFile
            // 
            this.lblNAVServiceFile.AutoSize = true;
            this.lblNAVServiceFile.Location = new System.Drawing.Point(9, 357);
            this.lblNAVServiceFile.Name = "lblNAVServiceFile";
            this.lblNAVServiceFile.Size = new System.Drawing.Size(23, 13);
            this.lblNAVServiceFile.TabIndex = 17;
            this.lblNAVServiceFile.Text = "File";
            // 
            // NAVServiceFile
            // 
            this.NAVServiceFile.Location = new System.Drawing.Point(81, 350);
            this.NAVServiceFile.Name = "NAVServiceFile";
            this.NAVServiceFile.ReadOnly = true;
            this.NAVServiceFile.Size = new System.Drawing.Size(219, 20);
            this.NAVServiceFile.TabIndex = 16;
            // 
            // ConfigDBServer
            // 
            this.ConfigDBServer.Location = new System.Drawing.Point(467, 272);
            this.ConfigDBServer.Name = "ConfigDBServer";
            this.ConfigDBServer.Size = new System.Drawing.Size(125, 20);
            this.ConfigDBServer.TabIndex = 18;
            // 
            // lblConfigDBServer
            // 
            this.lblConfigDBServer.AutoSize = true;
            this.lblConfigDBServer.Location = new System.Drawing.Point(363, 279);
            this.lblConfigDBServer.Name = "lblConfigDBServer";
            this.lblConfigDBServer.Size = new System.Drawing.Size(84, 13);
            this.lblConfigDBServer.TabIndex = 19;
            this.lblConfigDBServer.Text = "DatabaseServer";
            // 
            // lblConfigDBInstance
            // 
            this.lblConfigDBInstance.AutoSize = true;
            this.lblConfigDBInstance.Location = new System.Drawing.Point(366, 305);
            this.lblConfigDBInstance.Name = "lblConfigDBInstance";
            this.lblConfigDBInstance.Size = new System.Drawing.Size(94, 13);
            this.lblConfigDBInstance.TabIndex = 21;
            this.lblConfigDBInstance.Text = "DatabaseInstance";
            // 
            // ConfigDBInstance
            // 
            this.ConfigDBInstance.Location = new System.Drawing.Point(467, 298);
            this.ConfigDBInstance.Name = "ConfigDBInstance";
            this.ConfigDBInstance.Size = new System.Drawing.Size(125, 20);
            this.ConfigDBInstance.TabIndex = 20;
            // 
            // lblConfigDBName
            // 
            this.lblConfigDBName.AutoSize = true;
            this.lblConfigDBName.Location = new System.Drawing.Point(366, 331);
            this.lblConfigDBName.Name = "lblConfigDBName";
            this.lblConfigDBName.Size = new System.Drawing.Size(81, 13);
            this.lblConfigDBName.TabIndex = 23;
            this.lblConfigDBName.Text = "DatabaseName";
            // 
            // ConfigDBName
            // 
            this.ConfigDBName.Location = new System.Drawing.Point(467, 324);
            this.ConfigDBName.Name = "ConfigDBName";
            this.ConfigDBName.Size = new System.Drawing.Size(125, 20);
            this.ConfigDBName.TabIndex = 22;
            // 
            // lblConfigServerInstance
            // 
            this.lblConfigServerInstance.AutoSize = true;
            this.lblConfigServerInstance.Location = new System.Drawing.Point(363, 357);
            this.lblConfigServerInstance.Name = "lblConfigServerInstance";
            this.lblConfigServerInstance.Size = new System.Drawing.Size(79, 13);
            this.lblConfigServerInstance.TabIndex = 25;
            this.lblConfigServerInstance.Text = "ServerInstance";
            // 
            // ConfigServerInstance
            // 
            this.ConfigServerInstance.Location = new System.Drawing.Point(467, 350);
            this.ConfigServerInstance.Name = "ConfigServerInstance";
            this.ConfigServerInstance.Size = new System.Drawing.Size(125, 20);
            this.ConfigServerInstance.TabIndex = 24;
            // 
            // lblConfigServerPort
            // 
            this.lblConfigServerPort.AutoSize = true;
            this.lblConfigServerPort.Location = new System.Drawing.Point(601, 275);
            this.lblConfigServerPort.Name = "lblConfigServerPort";
            this.lblConfigServerPort.Size = new System.Drawing.Size(57, 13);
            this.lblConfigServerPort.TabIndex = 27;
            this.lblConfigServerPort.Text = "ServerPort";
            // 
            // ConfigServerPort
            // 
            this.ConfigServerPort.Location = new System.Drawing.Point(702, 268);
            this.ConfigServerPort.Name = "ConfigServerPort";
            this.ConfigServerPort.Size = new System.Drawing.Size(91, 20);
            this.ConfigServerPort.TabIndex = 26;
            // 
            // lblConfigWebServicePort
            // 
            this.lblConfigWebServicePort.AutoSize = true;
            this.lblConfigWebServicePort.Location = new System.Drawing.Point(601, 305);
            this.lblConfigWebServicePort.Name = "lblConfigWebServicePort";
            this.lblConfigWebServicePort.Size = new System.Drawing.Size(85, 13);
            this.lblConfigWebServicePort.TabIndex = 29;
            this.lblConfigWebServicePort.Text = "WebServicePort";
            // 
            // ConfigWebServicePort
            // 
            this.ConfigWebServicePort.Location = new System.Drawing.Point(702, 298);
            this.ConfigWebServicePort.Name = "ConfigWebServicePort";
            this.ConfigWebServicePort.Size = new System.Drawing.Size(91, 20);
            this.ConfigWebServicePort.TabIndex = 28;
            // 
            // lblConfigWebServiceUseNTLM
            // 
            this.lblConfigWebServiceUseNTLM.AutoSize = true;
            this.lblConfigWebServiceUseNTLM.Location = new System.Drawing.Point(601, 331);
            this.lblConfigWebServiceUseNTLM.Name = "lblConfigWebServiceUseNTLM";
            this.lblConfigWebServiceUseNTLM.Size = new System.Drawing.Size(74, 13);
            this.lblConfigWebServiceUseNTLM.TabIndex = 31;
            this.lblConfigWebServiceUseNTLM.Text = "WSUseNTLM";
            // 
            // ConfigWebServiceUseNTLM
            // 
            this.ConfigWebServiceUseNTLM.Location = new System.Drawing.Point(702, 324);
            this.ConfigWebServiceUseNTLM.Name = "ConfigWebServiceUseNTLM";
            this.ConfigWebServiceUseNTLM.Size = new System.Drawing.Size(91, 20);
            this.ConfigWebServiceUseNTLM.TabIndex = 30;
            // 
            // lblConfigEnableDebugging
            // 
            this.lblConfigEnableDebugging.AutoSize = true;
            this.lblConfigEnableDebugging.Location = new System.Drawing.Point(601, 357);
            this.lblConfigEnableDebugging.Name = "lblConfigEnableDebugging";
            this.lblConfigEnableDebugging.Size = new System.Drawing.Size(92, 13);
            this.lblConfigEnableDebugging.TabIndex = 33;
            this.lblConfigEnableDebugging.Text = "EnableDebugging";
            // 
            // ConfigEnableDebugging
            // 
            this.ConfigEnableDebugging.Location = new System.Drawing.Point(702, 350);
            this.ConfigEnableDebugging.Name = "ConfigEnableDebugging";
            this.ConfigEnableDebugging.Size = new System.Drawing.Size(91, 20);
            this.ConfigEnableDebugging.TabIndex = 32;
            // 
            // btnUpdateConfig
            // 
            this.btnUpdateConfig.Location = new System.Drawing.Point(467, 244);
            this.btnUpdateConfig.Name = "btnUpdateConfig";
            this.btnUpdateConfig.Size = new System.Drawing.Size(99, 23);
            this.btnUpdateConfig.TabIndex = 34;
            this.btnUpdateConfig.Text = "Update Config";
            this.btnUpdateConfig.UseVisualStyleBackColor = true;
            this.btnUpdateConfig.Click += new System.EventHandler(this.btnUpdateConfig_Click);
            // 
            // btnStartService
            // 
            this.btnStartService.Location = new System.Drawing.Point(81, 239);
            this.btnStartService.Name = "btnStartService";
            this.btnStartService.Size = new System.Drawing.Size(75, 23);
            this.btnStartService.TabIndex = 35;
            this.btnStartService.Text = "Start";
            this.btnStartService.UseVisualStyleBackColor = true;
            this.btnStartService.Click += new System.EventHandler(this.btnStartService_Click);
            // 
            // btnStopService
            // 
            this.btnStopService.Location = new System.Drawing.Point(162, 239);
            this.btnStopService.Name = "btnStopService";
            this.btnStopService.Size = new System.Drawing.Size(75, 23);
            this.btnStopService.TabIndex = 36;
            this.btnStopService.Text = "Stop";
            this.btnStopService.UseVisualStyleBackColor = true;
            this.btnStopService.Click += new System.EventHandler(this.btnStopService_Click);
            // 
            // CSDynamicsPowerShellAdmin
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(838, 485);
            this.Controls.Add(this.btnStopService);
            this.Controls.Add(this.btnStartService);
            this.Controls.Add(this.btnUpdateConfig);
            this.Controls.Add(this.lblConfigEnableDebugging);
            this.Controls.Add(this.ConfigEnableDebugging);
            this.Controls.Add(this.lblConfigWebServiceUseNTLM);
            this.Controls.Add(this.ConfigWebServiceUseNTLM);
            this.Controls.Add(this.lblConfigWebServicePort);
            this.Controls.Add(this.ConfigWebServicePort);
            this.Controls.Add(this.lblConfigServerPort);
            this.Controls.Add(this.ConfigServerPort);
            this.Controls.Add(this.lblConfigServerInstance);
            this.Controls.Add(this.ConfigServerInstance);
            this.Controls.Add(this.lblConfigDBName);
            this.Controls.Add(this.ConfigDBName);
            this.Controls.Add(this.lblConfigDBInstance);
            this.Controls.Add(this.ConfigDBInstance);
            this.Controls.Add(this.lblConfigDBServer);
            this.Controls.Add(this.ConfigDBServer);
            this.Controls.Add(this.lblNAVServiceFile);
            this.Controls.Add(this.NAVServiceFile);
            this.Controls.Add(this.btnGetConfig);
            this.Controls.Add(this.lblNAVServiceMode);
            this.Controls.Add(this.NAVServiceMode);
            this.Controls.Add(this.lblNAVServiceStatus);
            this.Controls.Add(this.NAVServiceStatus);
            this.Controls.Add(this.lblNAVServicePath);
            this.Controls.Add(this.NAVServicePath);
            this.Controls.Add(this.lblNAVServiceAccount);
            this.Controls.Add(this.NAVServiceAccount);
            this.Controls.Add(this.lblNAVServiceName);
            this.Controls.Add(this.NAVServiceName);
            this.Controls.Add(this.NAVServicesList);
            this.Controls.Add(this.btnGetNAVServices);
            this.Controls.Add(this.lblNAVServerName);
            this.Controls.Add(this.NAVServerName);
            this.Name = "CSDynamicsPowerShellAdmin";
            this.Text = "CSDynamicsNAVPowerShellAdmin";
            this.Load += new System.EventHandler(this.CSDynamicsPowerShellAdmin_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox NAVServerName;
        private System.Windows.Forms.Label lblNAVServerName;
        private System.Windows.Forms.Button btnGetNAVServices;
        private System.Windows.Forms.ListBox NAVServicesList;
        private System.Windows.Forms.TextBox NAVServiceName;
        private System.Windows.Forms.Label lblNAVServiceName;
        private System.Windows.Forms.Label lblNAVServiceAccount;
        private System.Windows.Forms.TextBox NAVServiceAccount;
        private System.Windows.Forms.Label lblNAVServicePath;
        private System.Windows.Forms.TextBox NAVServicePath;
        private System.Windows.Forms.Label lblNAVServiceStatus;
        private System.Windows.Forms.TextBox NAVServiceStatus;
        private System.Windows.Forms.Label lblNAVServiceMode;
        private System.Windows.Forms.TextBox NAVServiceMode;
        private System.Windows.Forms.Button btnGetConfig;
        private System.Windows.Forms.Label lblNAVServiceFile;
        private System.Windows.Forms.TextBox NAVServiceFile;
        private System.Windows.Forms.TextBox ConfigDBServer;
        private System.Windows.Forms.Label lblConfigDBServer;
        private System.Windows.Forms.Label lblConfigDBInstance;
        private System.Windows.Forms.TextBox ConfigDBInstance;
        private System.Windows.Forms.Label lblConfigDBName;
        private System.Windows.Forms.TextBox ConfigDBName;
        private System.Windows.Forms.Label lblConfigServerInstance;
        private System.Windows.Forms.TextBox ConfigServerInstance;
        private System.Windows.Forms.Label lblConfigServerPort;
        private System.Windows.Forms.TextBox ConfigServerPort;
        private System.Windows.Forms.Label lblConfigWebServicePort;
        private System.Windows.Forms.TextBox ConfigWebServicePort;
        private System.Windows.Forms.Label lblConfigWebServiceUseNTLM;
        private System.Windows.Forms.TextBox ConfigWebServiceUseNTLM;
        private System.Windows.Forms.Label lblConfigEnableDebugging;
        private System.Windows.Forms.TextBox ConfigEnableDebugging;
        private System.Windows.Forms.Button btnUpdateConfig;
        private System.Windows.Forms.Button btnStartService;
        private System.Windows.Forms.Button btnStopService;
    }
}

