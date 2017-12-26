/****************************** Module Header ******************************\
* Module Name:  ButtonList
* Project:      CSASPNETAjaxScriptControl
* Copyright (c) Microsoft Corporation.
* ButtonList Ajax Control Class
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
using System.Collections.Specialized;
using System.Globalization;
[assembly: WebResource("CSASPNETAjaxScriptControl.Common.jquery-1.0.js", "application/x-javascript")]
[assembly: WebResource("CSASPNETAjaxScriptControl.ButtonList.ButtonList.js", "application/x-javascript")]
[assembly: WebResource("CSASPNETAjaxScriptControl.ButtonList.ButtonList.css", "text/css",PerformSubstitution=true)]
[assembly: WebResource("CSASPNETAjaxScriptControl.ButtonList.ButtonList.png", "image/png")]
namespace CSASPNETAjaxScriptControl
{
    public class ButtonList : ListControl, IScriptControl,IPostBackDataHandler,INamingContainer
    {
        public ButtonList(){}
        #region IScriptControl Member
        public IEnumerable<ScriptDescriptor> GetScriptDescriptors()
        {
            ScriptControlDescriptor descriptor = new ScriptControlDescriptor("CSASPNETAjaxScriptControl.ButtonList", this.ClientID);
            descriptor.AddElementProperty("buttonListOptionList", ButtonListOptionList.ClientID);
            descriptor.AddElementProperty("buttonListHiddenField", ButtonListHiddenField.ClientID);
            descriptor.AddElementProperty("buttonListLastElement", ButtonListLastElement.ClientID);
            descriptor.AddProperty("autoPostBack", AutoPostBack);
            descriptor.AddProperty("selectedIndex", SelectedIndex);
            descriptor.AddProperty("listItemHighLightCssClass", ButtonListItemHighLightCssClass);
            descriptor.AddProperty("listItemCssClass", ButtonListItemCssClass);
            yield return descriptor;
        }
        public IEnumerable<ScriptReference> GetScriptReferences()
        {
            yield return new ScriptReference(Page.ClientScript.GetWebResourceUrl(this.GetType(), "CSASPNETAjaxScriptControl.Common.jquery-1.0.js"));
            yield return new ScriptReference(Page.ClientScript.GetWebResourceUrl(this.GetType(), "CSASPNETAjaxScriptControl.ButtonList.ButtonList.js"));
        }
        #endregion
        public virtual string ButtonListItemHighLightCssClass{get{ return "listItemHighLight"; }}
        public virtual string ButtonListItemCssClass{get{return "listItem";}}
        public virtual string ButtonListLastElementCssClass{get{return "lastElement";}}
        #region List Properties
        public override int SelectedIndex
        {
            get{
                int selectedIndex = base.SelectedIndex;
                if ((selectedIndex < 0) && (this.Items.Count > 0))
                {
                    this.Items[0].Selected = true;
                    base.SelectedIndex = 0;
                }
                return base.SelectedIndex;
            }
            set{
                base.SelectedIndex = value;
                ButtonListHiddenField.Value = value.ToString();
            }
        }
        [Browsable(true)]
        [DefaultValue("")]
        [Category("Appearance")]
        public override bool AutoPostBack
        {
            get{return base.AutoPostBack;}
            set{base.AutoPostBack = value;}
        }
        #endregion
        #region Child Controls
        private BulletedList _ButtonListOptionList;
        private HiddenField _ButtonListHiddenField;
        private Panel _ButtonListLastElement;
        protected virtual BulletedList ButtonListOptionList
        {
            get{
                if (_ButtonListOptionList == null)
                    _ButtonListOptionList = new BulletedList();
                return _ButtonListOptionList;
            }
        }
        protected virtual HiddenField ButtonListHiddenField
        {
            get{
                if (_ButtonListHiddenField == null)
                    _ButtonListHiddenField = new HiddenField();
                return _ButtonListHiddenField;
            }
        }        
        /// <summary>
        /// To terminate float:left of list item.
        /// </summary>
        protected virtual Panel ButtonListLastElement
        {
            get{
                if (_ButtonListLastElement == null)
                    _ButtonListLastElement = new Panel();
                return _ButtonListLastElement;
            }
        }
        #endregion
        #region Create Child Controls
        protected override void CreateChildControls()
        {
            this.Controls.Clear();
            this.Controls.Add(ButtonListOptionList);
            this.Controls.Add(ButtonListHiddenField);
            this.Controls.Add(ButtonListLastElement);
            base.CreateChildControls();
        }
        #endregion
        #region Render Methods
        protected override HtmlTextWriterTag TagKey
        {
            get { return HtmlTextWriterTag.Div; }
        }
        protected override void AddAttributesToRender(HtmlTextWriter writer)
        {
            base.AddAttributesToRender(writer);
            ButtonListOptionList.CssClass = this.CssClass == string.Empty ? "buttonList" : this.CssClass;
            ButtonListLastElement.CssClass = ButtonListLastElementCssClass;
        }
        /// <summary>
        /// Add Css reference 
        /// </summary>
        private void RenderCssReference()
        {
            string cssUrl = Page.ClientScript.GetWebResourceUrl(this.GetType(), "CSASPNETAjaxScriptControl.ButtonList.ButtonList.css");
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
            // We need register ScriptControl in building ScriptControl in PreRender phase.
            ScriptManager manager = ScriptManager.GetCurrent(this.Page);
            if (manager == null) throw new InvalidOperationException("A ScriptManager is required on the page.");
            manager.RegisterScriptControl<ButtonList>(this);
            Page.RegisterRequiresPostBack(this);
            for(int i=0;i<Items.Count;i++)
            {
                if (i==this.SelectedIndex)
                    Items[i].Attributes["class"] = this.ButtonListItemHighLightCssClass;
                else
                    Items[i].Attributes["class"] = this.ButtonListItemCssClass;
            }
        }
        protected override void RenderContents(HtmlTextWriter writer)
        {
            // We need regiser ScriptDescriptors in building ScriptControl in Render phase.
            ScriptManager.GetCurrent(this.Page).RegisterScriptDescriptors(this);
            ButtonListOptionList.Items.Clear();
            ListItem[] copy = new ListItem[Items.Count];
            Items.CopyTo(copy, 0);
            ButtonListOptionList.Items.AddRange(copy);
            ButtonListOptionList.RenderControl(writer);
            ButtonListHiddenField.RenderControl(writer);
            ButtonListLastElement.RenderControl(writer);
        }       
        #endregion
        #region IPostBackDataHandler Implementation
        bool IPostBackDataHandler.LoadPostData(string postDataKey, NameValueCollection postCollection)
        {
            return LoadPostData(postDataKey, postCollection);
        }
        void IPostBackDataHandler.RaisePostDataChangedEvent()
        {
            RaisePostDataChangedEvent();
        }
        protected virtual bool LoadPostData(string postDataKey, NameValueCollection postCollection)
        {
            if (!Enabled)
                return false;
            int newSelectedIndex = Convert.ToInt32(postCollection.GetValues(ButtonListHiddenField.UniqueID)[0], CultureInfo.InvariantCulture);
            EnsureDataBound();
            if (newSelectedIndex != SelectedIndex)
            { 
                SelectedIndex = newSelectedIndex;
                return true;
            }            
            return false;
        }
        public virtual void RaisePostDataChangedEvent()
        {
            this.OnSelectedIndexChanged(EventArgs.Empty);
        }
        #endregion
    }
}