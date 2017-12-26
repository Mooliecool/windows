//------------------------------------------------------------------------------
// <copyright file="DocComment.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

/*
 */
namespace System.Windows.Forms.PropertyGridInternal {

    using System.Diagnostics;
    using System.Diagnostics.CodeAnalysis;

    using System;
    using System.Windows.Forms;
    
    using System.ComponentModel;
    using System.ComponentModel.Design;
    using System.Drawing;
    using Microsoft.Win32;
    using System.Windows.Forms.Layout;

    internal class DocComment : PropertyGrid.SnappableControl {
        
        private Label m_labelTitle;
        private Label m_labelDesc;
        private string fullDesc;
        
        protected int lineHeight;
        private bool needUpdateUIWithFont = true;

        protected const int CBORDER = 3;
        protected const int CXDEF = 0;
        protected const int CYDEF = 59;
        protected const int MIN_LINES = 2;
        
        internal Rectangle rect = Rectangle.Empty;

        internal DocComment(PropertyGrid owner) : base(owner) {
            SuspendLayout();
            m_labelTitle = new Label();
            m_labelTitle.UseMnemonic = false;
            m_labelTitle.Cursor = Cursors.Default;
            m_labelDesc = new Label();
            m_labelDesc.AutoEllipsis = true;
            m_labelDesc.Cursor = Cursors.Default;

            UpdateTextRenderingEngine();

            Controls.Add(m_labelTitle);
            Controls.Add(m_labelDesc);

            Size = new Size(CXDEF,CYDEF);

            this.Text = SR.GetString(SR.PBRSDocCommentPaneTitle);
            SetStyle(ControlStyles.Selectable, false);
            ResumeLayout(false);
        }

        public virtual int Lines {
            get {
                UpdateUIWithFont();
                return Height/lineHeight;
            }
            set {
                UpdateUIWithFont();
                Size = new Size(Width, 1 + value * lineHeight);
            }
        }

        public override int GetOptimalHeight(int width) {
            UpdateUIWithFont();
            // compute optimal label height as one line only.
            int height = m_labelTitle.Size.Height;

            // do this to avoid getting parented to the Parking window.
            //
            if (this.ownerGrid.IsHandleCreated && !IsHandleCreated) {
                CreateControl();
            }

            // compute optimal text height
            Graphics g = m_labelDesc.CreateGraphicsInternal();
            SizeF sizef = PropertyGrid.MeasureTextHelper.MeasureText( this.ownerGrid, g, m_labelTitle.Text, Font, width);
            Size sz = Size.Ceiling(sizef);
            g.Dispose();
            height += (sz.Height * 2) + 2;
            return Math.Max(height + 4, CYDEF);
        }

        internal virtual void LayoutWindow() {
        }

        protected override void OnFontChanged(EventArgs e) {
            needUpdateUIWithFont = true;
            PerformLayout();
            base.OnFontChanged(e);
        }

        protected override void OnLayout(LayoutEventArgs e) {
            UpdateUIWithFont();
            Size size = ClientSize;
            
            // if the client size is 0, setting this to a negative number
            // will force an extra layout.  
            size.Width = Math.Max(0,  size.Width  -  2*CBORDER);
            size.Height = Math.Max(0, size.Height -  2*CBORDER);

            // calling SetBounds is slightly more efficient than allocating 
            // a size struct and setting it into Size prop.
            m_labelTitle.SetBounds(m_labelTitle.Top, 
                                   m_labelTitle.Left, 
                                   size.Width, 
                                   Math.Min(lineHeight, size.Height), 
                                   BoundsSpecified.Size);
     
            m_labelDesc.SetBounds(m_labelDesc.Top, 
                                  m_labelDesc.Left, 
                                  size.Width, 
                                  Math.Max(0,size.Height-lineHeight-1), 
                                  BoundsSpecified.Size);

            m_labelDesc.Text = this.fullDesc;
            m_labelDesc.AccessibleName = this.fullDesc; // Don't crop the description for accessibility clients
            base.OnLayout(e);
        }
        
        protected override void OnResize(EventArgs e) {
            Rectangle newRect = ClientRectangle;
            if (!rect.IsEmpty && newRect.Width > rect.Width) {
                Rectangle rectInvalidate = new Rectangle(rect.Width-1,0,newRect.Width-rect.Width+1,rect.Height);
                Invalidate(rectInvalidate);
            }
            rect = newRect;
            base.OnResize(e);
        }

        protected override void OnHandleCreated(EventArgs e) {
            base.OnHandleCreated(e);
            UpdateUIWithFont();
        }

        public virtual void SetComment(string title, string desc) {
            if (m_labelDesc.Text != title) {
                m_labelTitle.Text = title;
            }
            
            if (desc != fullDesc) {
                this.fullDesc = desc;
                m_labelDesc.Text = fullDesc;
                m_labelDesc.AccessibleName = this.fullDesc; // Don't crop the description for accessibility clients
            }
        }

        public override int SnapHeightRequest(int cyNew) {
            UpdateUIWithFont();
            int lines = Math.Max(MIN_LINES, cyNew/lineHeight);
            return 1 + lines*lineHeight;
        }

        internal void UpdateTextRenderingEngine() {
            m_labelTitle.UseCompatibleTextRendering = this.ownerGrid.UseCompatibleTextRendering;
            m_labelDesc.UseCompatibleTextRendering = this.ownerGrid.UseCompatibleTextRendering;
        }

        private void UpdateUIWithFont() {
            if (IsHandleCreated && needUpdateUIWithFont) {

                // Some fonts throw because Bold is not a valid option
                // for them.  Fail gracefully.
                try {
                    m_labelTitle.Font = new Font(Font, FontStyle.Bold);
                }
                catch {
                }

                lineHeight = (int)Font.Height + 2;

                m_labelTitle.Location = new Point(CBORDER, CBORDER);
                m_labelDesc.Location = new Point(CBORDER, CBORDER + lineHeight);

                needUpdateUIWithFont = false;
                PerformLayout();
            }
        }
    }
}
