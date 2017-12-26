namespace CSWinFormTreeViewTraversal
{
    partial class MainForm
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
            System.Windows.Forms.TreeNode treeNode1 = new System.Windows.Forms.TreeNode("Windows 2000");
            System.Windows.Forms.TreeNode treeNode2 = new System.Windows.Forms.TreeNode("Windows XP");
            System.Windows.Forms.TreeNode treeNode3 = new System.Windows.Forms.TreeNode("Windows 2003");
            System.Windows.Forms.TreeNode treeNode4 = new System.Windows.Forms.TreeNode("Windows Vista");
            System.Windows.Forms.TreeNode treeNode5 = new System.Windows.Forms.TreeNode("Windows 2008");
            System.Windows.Forms.TreeNode treeNode6 = new System.Windows.Forms.TreeNode("Windows 7");
            System.Windows.Forms.TreeNode treeNode7 = new System.Windows.Forms.TreeNode("Windows", new System.Windows.Forms.TreeNode[] {
            treeNode1,
            treeNode2,
            treeNode3,
            treeNode4,
            treeNode5,
            treeNode6});
            System.Windows.Forms.TreeNode treeNode8 = new System.Windows.Forms.TreeNode("Office 2000");
            System.Windows.Forms.TreeNode treeNode9 = new System.Windows.Forms.TreeNode("Office XP");
            System.Windows.Forms.TreeNode treeNode10 = new System.Windows.Forms.TreeNode("Office 2003");
            System.Windows.Forms.TreeNode treeNode11 = new System.Windows.Forms.TreeNode("Office 2007");
            System.Windows.Forms.TreeNode treeNode12 = new System.Windows.Forms.TreeNode("Office", new System.Windows.Forms.TreeNode[] {
            treeNode8,
            treeNode9,
            treeNode10,
            treeNode11});
            System.Windows.Forms.TreeNode treeNode13 = new System.Windows.Forms.TreeNode("Product", new System.Windows.Forms.TreeNode[] {
            treeNode7,
            treeNode12});
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.treeView1 = new System.Windows.Forms.TreeView();
            this.listBox1 = new System.Windows.Forms.ListBox();
            this.btnTravel = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.btnClear = new System.Windows.Forms.Button();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.btnFindNext = new System.Windows.Forms.Button();
            this.ckMatchWholeWord = new System.Windows.Forms.CheckBox();
            this.SuspendLayout();
            // 
            // treeView1
            // 
            this.treeView1.Location = new System.Drawing.Point(12, 100);
            this.treeView1.Name = "treeView1";
            treeNode1.Name = "Node2";
            treeNode1.Text = "Windows 2000";
            treeNode2.Name = "Node4";
            treeNode2.Text = "Windows XP";
            treeNode3.Name = "Node5";
            treeNode3.Text = "Windows 2003";
            treeNode4.Name = "Node6";
            treeNode4.Text = "Windows Vista";
            treeNode5.Name = "Node7";
            treeNode5.Text = "Windows 2008";
            treeNode6.Name = "Node8";
            treeNode6.Text = "Windows 7";
            treeNode7.Name = "Node1";
            treeNode7.Text = "Windows";
            treeNode8.Name = "Node10";
            treeNode8.Text = "Office 2000";
            treeNode9.Name = "Node11";
            treeNode9.Text = "Office XP";
            treeNode10.Name = "Node12";
            treeNode10.Text = "Office 2003";
            treeNode11.Name = "Node13";
            treeNode11.Text = "Office 2007";
            treeNode12.Name = "Node9";
            treeNode12.Text = "Office";
            treeNode13.Name = "Node0";
            treeNode13.Text = "Product";
            this.treeView1.Nodes.AddRange(new System.Windows.Forms.TreeNode[] {
            treeNode13});
            this.treeView1.Size = new System.Drawing.Size(223, 321);
            this.treeView1.TabIndex = 0;
            // 
            // listBox1
            // 
            this.listBox1.FormattingEnabled = true;
            this.listBox1.Location = new System.Drawing.Point(241, 100);
            this.listBox1.Name = "listBox1";
            this.listBox1.Size = new System.Drawing.Size(191, 225);
            this.listBox1.TabIndex = 1;
            // 
            // btnTravel
            // 
            this.btnTravel.Location = new System.Drawing.Point(357, 331);
            this.btnTravel.Name = "btnTravel";
            this.btnTravel.Size = new System.Drawing.Size(75, 23);
            this.btnTravel.TabIndex = 2;
            this.btnTravel.Text = "Travel";
            this.btnTravel.UseVisualStyleBackColor = true;
            this.btnTravel.Click += new System.EventHandler(this.btnTravel_Click);
            // 
            // label1
            // 
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(420, 80);
            this.label1.TabIndex = 3;
            this.label1.Text = resources.GetString("label1.Text");
            // 
            // btnClear
            // 
            this.btnClear.Location = new System.Drawing.Point(276, 331);
            this.btnClear.Name = "btnClear";
            this.btnClear.Size = new System.Drawing.Size(75, 23);
            this.btnClear.TabIndex = 4;
            this.btnClear.Text = "Clear";
            this.btnClear.UseVisualStyleBackColor = true;
            this.btnClear.Click += new System.EventHandler(this.btnClear_Click);
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(241, 361);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(191, 20);
            this.textBox1.TabIndex = 5;
            // 
            // btnFindNext
            // 
            this.btnFindNext.Location = new System.Drawing.Point(357, 398);
            this.btnFindNext.Name = "btnFindNext";
            this.btnFindNext.Size = new System.Drawing.Size(75, 23);
            this.btnFindNext.TabIndex = 6;
            this.btnFindNext.Text = "Find Next";
            this.btnFindNext.UseVisualStyleBackColor = true;
            this.btnFindNext.Click += new System.EventHandler(this.btnFindNext_Click);
            // 
            // ckMatchWholeWord
            // 
            this.ckMatchWholeWord.AutoSize = true;
            this.ckMatchWholeWord.Location = new System.Drawing.Point(241, 402);
            this.ckMatchWholeWord.Name = "ckMatchWholeWord";
            this.ckMatchWholeWord.Size = new System.Drawing.Size(113, 17);
            this.ckMatchWholeWord.TabIndex = 7;
            this.ckMatchWholeWord.Text = "MatchWholeWord";
            this.ckMatchWholeWord.UseVisualStyleBackColor = true;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(446, 437);
            this.Controls.Add(this.ckMatchWholeWord);
            this.Controls.Add(this.btnFindNext);
            this.Controls.Add(this.textBox1);
            this.Controls.Add(this.btnClear);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.btnTravel);
            this.Controls.Add(this.listBox1);
            this.Controls.Add(this.treeView1);
            this.Name = "MainForm";
            this.Text = "MainForm";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TreeView treeView1;
        private System.Windows.Forms.ListBox listBox1;
        private System.Windows.Forms.Button btnTravel;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btnClear;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Button btnFindNext;
        private System.Windows.Forms.CheckBox ckMatchWholeWord;
    }
}

