/****************************** Module Header ******************************\
* Module Name:  DropPanel
* Project:      CSASPNETAjaxScriptControl
* Copyright (c) Microsoft Corporation.
* DropPanel Ajax Control Class
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved. 
* History:
* * 2009/10/22 5:00 PM Vince Xu Created
\***************************************************************************/
using System;
using System.Collections.Generic;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;
using System.ComponentModel;
using AjaxControlToolkit;
[assembly: WebResource("CSASPNETAjaxScriptControl.Common.common.js", "application/x-javascript")]
[assembly: WebResource("CSASPNETAjaxScriptControl.DropPanel.DropPanel.js", "application/x-javascript")]
[assembly: WebResource("CSASPNETAjaxScriptControl.DropPanel.DropPanel.css", "text/css", PerformSubstitution = true)]
[assembly: WebResource("CSASPNETAjaxScriptControl.DropPanel.closeImage.png", "image/png")]
namespace CSASPNETAjaxScriptControl
{
    [ParseChildren(true)]
    [PersistChildren(false)]
    [ToolboxData("<{0}:DropPanel runat=\"server\"></{0}:DropPanel>")]
    public class DropPanel : Panel, IScriptControl,INamingContainer
    {
        public DropPanel(){}
        #region IScriptControl Member 
        public IEnumerable<ScriptDescriptor> GetScriptDescriptors()
        {
            ScriptControlDescriptor descriptor = new ScriptControlDescriptor("CSASPNETAjaxScriptControl.DropPanel", this.ClientID);
            descriptor.AddProperty("cssClass", CssClass);
            descriptor.AddProperty("headerCssClass", HeaderCssClass);
            descriptor.AddProperty("contentCssClass", ContentCssClass);
            descriptor.AddProperty("closeImageCssClass", CloseImageCssClass);           
            descriptor.AddProperty("fromContextKey", FromContextKey);
            descriptor.AddProperty("toContextKey", ToContextKey);
            descriptor.AddElementProperty("headerContainer", HeaderContainer.ClientID);
            descriptor.AddElementProperty("titleContainer", TitleContainer.ClientID);
            descriptor.AddElementProperty("closeImage", CloseImage.ClientID);
            descriptor.AddElementProperty("contentContainer", ContentContainer.ClientID);
            if (OnClosed != string.Empty)
                descriptor.AddEvent("closed", OnClosed);
            yield return descriptor;
        }
        public IEnumerable<ScriptReference> GetScriptReferences()
        {
            yield return new ScriptReference(Page.ClientScript.GetWebResourceUrl(this.GetType(), "CSASPNETAjaxScriptControl.Common.common.js"));
            yield return new ScriptReference(Page.ClientScript.GetWebResourceUrl(this.GetType(), "CSASPNETAjaxScriptControl.DropPanel.DropPanel.js"));
        }
        #endregion
        #region Attributes
        AjaxControlToolkit.RoundedCornersExtender RoundExtender;
        private Panel _HeaderContainer,_CloseImage;
        private DropPanelContent _TitleContainer,_ContentContainer;
        private ITemplate _HeadTemplate, _ContentTemplate;
        #region Css Class
        [Browsable(true)]
        [DefaultValue("")]
        [Category("Appearance")]
        public override string CssClass
        {
            get { return base.CssClass; }
            set { base.CssClass = value; }
        }        
        [Browsable(true)]
        [DefaultValue("")]
        [Category("Appearance")]
        public string HeaderCssClass
        {
            get{
                EnsureChildControls();
                return HeaderContainer.CssClass;
            }
            set{
                EnsureChildControls();
                HeaderContainer.CssClass = value;
            }
        }
        [Browsable(true)]
        [DefaultValue("")]
        [Category("Appearance")]
        public string CloseImageCssClass
        {
            get{
                EnsureChildControls();
                return CloseImage.CssClass;
            }
            set{
                EnsureChildControls();
                CloseImage.CssClass = value;
            }
        }
        [Browsable(true)]
        [DefaultValue("")]
        [Category("Appearance")]
        public string ContentCssClass
        {
            get{
                EnsureChildControls();
                return ContentContainer.CssClass;
            }
            set{
                EnsureChildControls();
                ContentContainer.CssClass = value;
            }
        }
        #endregion
        [Browsable(true)]
        [DefaultValue("")]
        [Category("Appearance")]
        public string HeaderText
        {
            get { return (string)(ViewState["HeaderText"] ?? string.Empty); }
            set { ViewState["HeaderText"] = value; }
        }
        [Browsable(true)]
        [DefaultValue("")]
        [Category("Appearance")]
        public string ContentText
        {
            get { return (string)(ViewState["ContentText"] ?? string.Empty); }
            set { ViewState["ContentText"] = value; }
        }
        [Browsable(false)]
        [DefaultValue(null)]
        [PersistenceMode(PersistenceMode.InnerProperty)]
        [TemplateContainer(typeof(DropPanelContent))]
        [TemplateInstance(TemplateInstance.Single)]
        public virtual ITemplate Header
        {
            get { return _HeadTemplate; }
            set { _HeadTemplate = value; }
        }
        [Browsable(false)]
        [DefaultValue(null)]
        [PersistenceMode(PersistenceMode.InnerProperty)]
        [TemplateContainer(typeof(DropPanelContent))]
        [TemplateInstance(TemplateInstance.Single)]
        public virtual ITemplate Content
        {
            get { return _ContentTemplate;}
            set { _ContentTemplate = value; }
        }
        public Panel HeaderContainer {
            get{
                if (_HeaderContainer == null)
                    _HeaderContainer = new Panel();
                return _HeaderContainer;
            }        
        }
        public Panel CloseImage {
            get{
                if (_CloseImage == null)
                    _CloseImage = new Panel();
                return _CloseImage;
            }
        }                
        public DropPanelContent TitleContainer
        {
            get{
                if (_TitleContainer == null)
                    _TitleContainer = new DropPanelContent();
                return _TitleContainer;
            }
        }
        public DropPanelContent ContentContainer
        {
            get{
                if (_ContentContainer == null)
                    _ContentContainer = new DropPanelContent();
                return _ContentContainer;
            }
        }
        public object FromContextKey { get; set; }
        public object ToContextKey { get; set; }
        #endregion
        #region CreateChildControls
        protected override void CreateChildControls()
        {
            this.Controls.Clear();             
            if (Header == null)
            {
                LiteralControl lc = new LiteralControl(HeaderText);
                TitleContainer.Controls.Add(lc);     
            }
            HeaderContainer.Controls.Add(TitleContainer);
            HeaderContainer.Controls.Add(CloseImage);
            this.Controls.Add(HeaderContainer); 
            if (_HeadTemplate != null)
                _HeadTemplate.InstantiateIn(TitleContainer);
            if (Content == null)
            {
                LiteralControl lc = new LiteralControl(ContentText);
                ContentContainer.Controls.Add(lc);          
            }
            this.Controls.Add(ContentContainer);
            if (_ContentTemplate != null)
                _ContentTemplate.InstantiateIn(ContentContainer);
            CreateRoundCornersExtender();            
            base.CreateChildControls();
        }
        private void CreateRoundCornersExtender()
        {
            RoundExtender = new AjaxControlToolkit.RoundedCornersExtender();
            RoundExtender.TargetControlID = this.UniqueID;
            RoundExtender.Radius = 3;
            this.Controls.Add(RoundExtender);
        }
        #endregion
        #region Client Event
        [DefaultValue("")]
        [Browsable(true)]
        [Category("Appearance")]
        public virtual string OnClosed
        {
            get { return (string)(ViewState["OnClosed"] ?? string.Empty); }
            set { ViewState["OnClosed"] = value; }
        }
        #endregion
        #region Render Methods
        protected override void AddAttributesToRender(HtmlTextWriter writer)
        {
            this.CssClass = this.CssClass == string.Empty ? "dropPanel" : this.CssClass;
            this.HeaderCssClass = this.HeaderCssClass == string.Empty ? "dropPanelHeader" : this.HeaderCssClass;
            this.ContentCssClass = this.ContentCssClass == string.Empty ? "dropPanelContent" : this.ContentCssClass;
            this.CloseImageCssClass = this.CloseImageCssClass == string.Empty ? "dropPanelCloseImg" : this.CloseImageCssClass;
            TitleContainer.Style.Add("height", "100%");
            TitleContainer.Style.Add("float", "left");
            TitleContainer.Style.Add("padding-left", "3px");
            RoundExtender.BorderColor = System.Drawing.Color.FromArgb(232, 161, 161);
            base.AddAttributesToRender(writer);   
        } 
        private void RenderCssReference()
        {
            string cssUrl = Page.ClientScript.GetWebResourceUrl(this.GetType(), "CSASPNETAjaxScriptControl.DropPanel.DropPanel.css");
            HtmlLink link = new HtmlLink();
            link.Href = cssUrl;
            link.Attributes.Add("type", "text/css");
            link.Attributes.Add("rel", "stylesheet");
            Page.Header.Controls.Add(link);
        }
        protected override void OnPreRender(EventArgs e)
        {
            base.OnPreRender(e);
            RenderCssReference();
            ScriptManager manager = ScriptManager.GetCurrent(this.Page);
            if (manager == null) throw new InvalidOperationException("A ScriptManager is required on the page.");
            manager.RegisterScriptControl<DropPanel>(this);
        }
        protected override void Render(HtmlTextWriter writer)
        { 
            ScriptManager.GetCurrent(this.Page).RegisterScriptDescriptors(this);
            base.Render(writer);
        }
        #endregion
    }
}
