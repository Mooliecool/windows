using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;


[assembly: WebResource("CSASPNETAjaxExtender.TimePicker.TimePicker.js", "application/x-javascript")]
[assembly: WebResource("CSASPNETAjaxExtender.TimePicker.TimePicker.css", "text/css", PerformSubstitution = true)]
[assembly: WebResource("CSASPNETAjaxExtender.TimePicker.backgroundImage.png", "image/png")]
[assembly: WebResource("CSASPNETAjaxExtender.TimePicker.centerImage.png", "image/png")]
[assembly: WebResource("CSASPNETAjaxExtender.TimePicker.hourImage.png", "image/png")]
[assembly: WebResource("CSASPNETAjaxExtender.TimePicker.minImage.png", "image/png")]
[assembly: WebResource("CSASPNETAjaxExtender.TimePicker.closeImage.png", "image/png")]

namespace CSASPNETAjaxExtender
{

    [ToolboxData("<{0}:TimePicker runat=server></{0}:TimePicker>")]
    [TargetControlType(typeof(TextBox))]    
    public class TimePicker : ExtenderControl
    {
        public TimePicker()
        {
            
        }

        #region IExtenderControl Member

        //Generate descriptor to render the related $create script function that will create behavior on client.
        protected override IEnumerable<ScriptDescriptor> GetScriptDescriptors(Control targetControl)
        {
            ScriptControlDescriptor descriptor = new ScriptControlDescriptor("CSASPNETAjaxExtender.TimePicker", targetControl.ClientID);

            //Register ErrorPresentControl element to client 
            if(ErrorPresentControlID!=string.Empty)
                descriptor.AddElementProperty("errorSpan", this.NamingContainer.FindControl(ErrorPresentControlID).ClientID);

            //Register property
            descriptor.AddProperty("cssClass", CssClass);
            descriptor.AddProperty("timeType", TimeType);

            //Register client event
            if(OnClientShowing!=string.Empty)
                descriptor.AddEvent("showing", OnClientShowing);
            if (OnClientShown != string.Empty)
                descriptor.AddEvent("shown", OnClientShown);
            if (OnClientHiding != string.Empty)
                descriptor.AddEvent("hiding", OnClientHiding);
            if (OnClientHidden != string.Empty)
                descriptor.AddEvent("hidden", OnClientHidden);
            if (OnClientHourSelectionChanged != string.Empty)
                descriptor.AddEvent("hourSelectionChanged", OnClientHourSelectionChanged);
            if (OnClientMinuteSelectionChanged != string.Empty)
                descriptor.AddEvent("minuteSelectionChanged", OnClientMinuteSelectionChanged);

            yield return descriptor;
        }

        // Generate the script reference
        protected override IEnumerable<ScriptReference> GetScriptReferences()
        {
            yield return new ScriptReference(Page.ClientScript.GetWebResourceUrl(this.GetType(), "CSASPNETAjaxExtender.TimePicker.TimePicker.js"));
        }

        #endregion

        #region Properties


        [Browsable(true)]
        [Category("Appearance")]
        [Description("")]
        public virtual string CssClass
        {
            get { return (String)(ViewState["CssClass"] ?? String.Empty); }
            set { ViewState["CssClass"] = value; }
        }


        [Browsable(true)]
        [Category("Appearance")]
        [Description("")]
        public virtual string ErrorPresentControlID
        {
            get { return (String)(ViewState["ErrorPresentControlID"] ?? String.Empty); }
            set { ViewState["ErrorPresentControlID"] = value; }
        }
    

        [Browsable(true)]
        [Category("Appearance")]
        [Description("")]
        [DefaultValue(CSASPNETAjaxExtender.TimeType.H24)]
        public CSASPNETAjaxExtender.TimeType TimeType
        {
            get { return (CSASPNETAjaxExtender.TimeType)(ViewState["TimeType"] ?? CSASPNETAjaxExtender.TimeType.H24); }
            set { ViewState["TimeType"] = value; }
        }

        [DefaultValue("")]
        [Browsable(true)]
        [Category("Appearance")]
        [Description("")]
        public virtual string OnClientShowing
        {
            get { return (string)(ViewState["OnClientShowing"] ?? string.Empty); }
            set { ViewState["OnClientShowing"] = value; }
        }

        [DefaultValue("")]
        [Browsable(true)]
        [Category("Appearance")]
        [Description("")]
        public virtual string OnClientShown
        {
            get { return (string)(ViewState["OnClientShown"] ?? string.Empty); }
            set { ViewState["OnClientShown"] = value; }
        }

        [DefaultValue("")]
        [Browsable(true)]
        [Category("Appearance")]
        [Description("")]
        public virtual string OnClientHiding
        {
            get { return (string)(ViewState["OnClientHiding"] ?? string.Empty); }
            set { ViewState["OnClientHiding"] = value; }
        }

        [DefaultValue("")]
        [Browsable(true)]
        [Category("Appearance")]
        [Description("")]
        public virtual string OnClientHidden
        {
            get { return (string)(ViewState["OnClientHidden"] ?? string.Empty); }
            set { ViewState["OnClientHidden"] = value; }
        }

        [DefaultValue("")]
        [Browsable(true)]
        [Category("Appearance")]
        [Description("")]
        public virtual string OnClientHourSelectionChanged
        {
            get { return (string)(ViewState["OnClientHourSelectionChanged"] ?? string.Empty); }
            set { ViewState["OnClientHourSelectionChanged"] = value; }
        }

        [DefaultValue("")]
        [Browsable(true)]
        [Category("Appearance")]
        [Description("")]
        public virtual string OnClientMinuteSelectionChanged
        {
            get { return (string)(ViewState["OnClientMinuteSelectionChanged"] ?? string.Empty); }
            set { ViewState["OnClientMinuteSelectionChanged"] = value; }
        }

        #endregion

        #region Render Phase


        // Add Css reference 
        private void RenderCssReference()
        {
            string cssUrl = Page.ClientScript.GetWebResourceUrl(this.GetType(), "CSASPNETAjaxExtender.TimePicker.TimePicker.css");

            HtmlLink link = new HtmlLink();
            link.Href = cssUrl;
            link.Attributes.Add("type", "text/css");
            link.Attributes.Add("rel", "stylesheet");
            Page.Header.Controls.Add(link);
        }



        protected override void OnPreRender(EventArgs e)
        {
            base.OnPreRender(e);
            //Register css reference 
            RenderCssReference();

        }

        #endregion

    }
}
