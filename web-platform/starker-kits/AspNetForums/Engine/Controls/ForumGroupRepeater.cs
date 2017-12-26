using System;
using System.Collections;
using System.Collections.Specialized;
using System.Web;
using System.Web.UI;
using System.Web.Caching;
using System.Web.UI.WebControls;
using AspNetForums;
using AspNetForums.Components;
using System.ComponentModel;
using System.ComponentModel.Design;

namespace AspNetForums.Controls {
	
    // *********************************************************************
    //  ForumGroupRepeater
    //
    /// <summary>
    /// This Web control displays a list of the available forum groups.
    /// </summary>
    /// 
    // ********************************************************************/        
    [
    ParseChildren(true),
    Designer(typeof(AspNetForums.Controls.Design.ForumRepeaterDesigner))
    ]
    public class ForumGroupRepeater : ForumRepeaterControl {

        ITemplate forumItemTemplate;
        bool showAllForumGroups = false;

        
        // *********************************************************************
        //  HandleDataBindingForGroupTitle
        //
        /// <summary>
        /// DataBinding event for the forum group title
        /// </summary>
        /// 
        // ********************************************************************/        
        protected virtual void HandleDataBindingForGroupTitle(Object sender, EventArgs e) {

            HyperLink link = (HyperLink) sender;
            RepeaterItem container = (RepeaterItem) link.NamingContainer;

            ForumGroup forumGroup = (ForumGroup) container.DataItem;

            link.Text = forumGroup.Name;
            link.NavigateUrl = Globals.UrlShowForumGroup + forumGroup.ForumGroupID;
        }

        // *********************************************************************
        //  HandleDataBindingForForumRepeater
        //
        /// <summary>
        /// DataBinding event for the ForumRepeater control
        /// </summary>
        /// 
        // ********************************************************************/        
        private void HandleDataBindingForForumRepeater(Object sender, EventArgs e) {
            ForumRepeater forumRepeater = (ForumRepeater) sender;
            RepeaterItem container = (RepeaterItem) forumRepeater.NamingContainer;

            ForumGroup forumGroup = (ForumGroup) container.DataItem;

            forumRepeater.ForumGroupID = forumGroup.ForumGroupID;
        }

        // *********************************************************************
        //  BeginBuildItemTemplate
        //
        /// <summary>
        /// Create and populates a PlaceHolder control for the content rendered
        /// for each Item in the item template used - when not specified by the
        /// user. Additionally wires up databinding events.
        /// </summary>
        /// 
        // ********************************************************************/        
        public virtual Control BeginBuildItemTemplate() {

            PlaceHolder placeHolder = new PlaceHolder();

            TableRow tr = new TableRow();
            tr.ID = "ForumGroup";

            // Display the title of the forum
            TableCell td = new TableCell();
            td.ColumnSpan = 5;
            td.CssClass = "forumHeaderBackgroundAlternate";
            td.Height = 20;

            HyperLink link = new HyperLink();
            link.CssClass = "forumTitle";
            link.NavigateUrl = "";
            link.ID = "GroupTitle";

            td.Controls.Add(link);
            tr.Controls.Add(td);

            tr.Controls.Add(td);

            // Add the Table Row
            placeHolder.Controls.Add(tr);

            // Add a new ForumGroupRepeater
            ForumRepeater forumRepeater = new ForumRepeater();
            forumRepeater.ID = "ForumRepeater";
            placeHolder.Controls.Add(forumRepeater);

            // Did the user specify a template for the forum item?
            if (forumItemTemplate != null)
                forumRepeater.ItemTemplate = forumItemTemplate;

            // wire up the handleDataBinding event handler to the DataBinding event
            link.DataBinding += new System.EventHandler(HandleDataBindingForGroupTitle);			
            forumRepeater.DataBinding += new System.EventHandler(HandleDataBindingForForumRepeater);

            return placeHolder;
        }


        // *********************************************************************
        //  BuildItemTemplate
        //
        /// <summary>
        /// Builds the default item template if the user does not specify one
        /// </summary>
        /// 
        // ********************************************************************/
        private void BuildItemTemplate(Control _ctrl) {
            // add the DataBoundLiteralControl to the parser
            System.Web.UI.IParserAccessor __parser = ((System.Web.UI.IParserAccessor)(_ctrl));
            __parser.AddParsedSubObject(BeginBuildItemTemplate());
        }

        // *********************************************************************
        //  BuildHeaderTemplate
        //
        /// <summary>
        /// Builds the default header template if the user does not specify one
        /// </summary>
        /// 
        // ********************************************************************/
        public virtual void BuildHeaderTemplate(Control _ctrl) {
            TableRow tr = new TableRow();
            TableHeaderCell th;

            // Build table headers
            th = new TableHeaderCell();
            th.ColumnSpan = 2;
            th.Height = 20;
            th.CssClass = "tableHeaderText";
            th.Text = "Forum";
            tr.Controls.Add(th);

            th = new TableHeaderCell();
            th.Width = 50;
            th.Wrap = false;
            th.CssClass = "tableHeaderText";
            th.Text = "&nbsp;&nbsp;Threads&nbsp;&nbsp;";
            tr.Controls.Add(th);

            th = new TableHeaderCell();
            th.Width = 50;
            th.Wrap = false;
            th.CssClass = "tableHeaderText";
            th.Text = "&nbsp;&nbsp;Posts&nbsp;&nbsp;";
            tr.Controls.Add(th);

            th = new TableHeaderCell();
            th.Width = 135;
            th.Wrap = false;
            th.CssClass = "tableHeaderText";
            th.Text = "&nbsp;Last Post&nbsp;";
            tr.Controls.Add(th);
            
            System.Web.UI.IParserAccessor __parser = ((System.Web.UI.IParserAccessor)(_ctrl));

            __parser.AddParsedSubObject(new LiteralControl("<table cellpadding=\"2\" cellspacing=\"1\" border=\"0\" width=\"100%\" class=\"tableBorder\">"));
            __parser.AddParsedSubObject(tr);

        }

        public virtual void BuildFooterTemplate(Control _ctrl) {	
            System.Web.UI.IParserAccessor __parser = ((System.Web.UI.IParserAccessor)(_ctrl));
            __parser.AddParsedSubObject(new LiteralControl("\n</table>"));
        }

        // *********************************************************************
        //  ApplyTemplates
        //
        /// <summary>
        /// Applies templates to control the ui generated by the control. If no
        /// template is specified a custom template is used. If a template is found
        /// in the skins directory, that template is loaded and used. If a user defined
        /// template is found, that template takes priority.
        /// </summary>
        /// 
        // ********************************************************************/
        public virtual void ApplyTemplates() {
            string pathToHeaderTemplate;
            string pathToItemTemplate;
            string pathToFooterTemplate;
            string keyForHeaderTemplate;
            string keyForItemTemplate;
            string keyForFooterTemplate;
            
            // Are we using skinned template?
            if (Page != null) {

                // Set the file paths to where the templates should be found
                pathToHeaderTemplate = Globals.ApplicationVRoot + "/Skins/" + SkinName + "/Templates/ForumGroupRepeater-Header.ascx";
                pathToItemTemplate = Globals.ApplicationVRoot + "/skins/" + SkinName + "/Templates/ForumGroupRepeater-Item.ascx";
                pathToFooterTemplate = Globals.ApplicationVRoot + "/skins/" + SkinName + "/Templates/ForumGroupRepeater-Footer.ascx";

                // Set the file paths to where the templates should be found
                keyForHeaderTemplate = SkinName + "/Templates/ForumGroupRepeater-Header.ascx";
                keyForItemTemplate = SkinName + "/Templates/ForumGroupRepeater-Item.ascx";
                keyForFooterTemplate = SkinName + "/Templates/ForumGroupRepeater-Footer.ascx";

                // Attempt to get the skinned header template
                if (HeaderTemplate == null)
                    HeaderTemplate = Globals.LoadSkinnedTemplate(pathToHeaderTemplate, keyForHeaderTemplate, Page);

                // Attempt to get the skinned item template
                if (ItemTemplate == null)
                    ItemTemplate = Globals.LoadSkinnedTemplate(pathToItemTemplate,keyForItemTemplate, Page);

                // Attempt to get the footer template
                if (FooterTemplate == null)
                    FooterTemplate = Globals.LoadSkinnedTemplate(pathToFooterTemplate, keyForFooterTemplate, Page);
            }

            // No skinned or user defined template, load the default
            if (HeaderTemplate == null) 
                HeaderTemplate = new CompiledTemplateBuilder(new BuildTemplateMethod(BuildHeaderTemplate));

            // No skinned or user defined template, load the default
            if (ItemTemplate == null)
                ItemTemplate = new CompiledTemplateBuilder(new BuildTemplateMethod(BuildItemTemplate));


            // No skinned or user defined template, load the default
            if (FooterTemplate == null)
                FooterTemplate = new CompiledTemplateBuilder(new BuildTemplateMethod(BuildFooterTemplate));

        }

        // *********************************************************************
        //  CreateChildControls
        //
        /// <summary>
        /// This event handler adds the children controls and is responsible
        /// for determining the templates used for the display of this control.
        /// If template skins are enabled, the templates for the control are
        /// loaded from the skins/.../templates directory. If no templates are
        /// specified, a default template is used.
        /// </summary>
        /// 
        // ********************************************************************/
        protected override void CreateChildControls() {

            // We don't care about post backs
            if (Page.IsPostBack)
                return;

            EnableViewState = false;

            // Attempt to load templates from skins
            ApplyTemplates();

            // Call base CreateChildControls
            base.CreateChildControls();

        }

		
        // *********************************************************************
        //  OnPreRender
        //
        /// <summary>
        /// This event handler binds the template to the datalist - this action
        /// is only carried out if the user DID NOT specify an ItemTemplate.
        /// In such a case, the default databinding is used.
        /// </summary>
        /// 
        // ********************************************************************/
        protected override void OnPreRender(EventArgs e) {
            ForumGroupCollection forumGroups;

            // Get all forum groups (cached call)
            if (showAllForumGroups)
                forumGroups = ForumGroups.GetAllForumGroups(true, false);
            else
                forumGroups = ForumGroups.GetAllForumGroups(false, true);

            // Have we asked for a particular forum group?
            if (ForumGroupID > 0) {

                ForumGroupCollection newForumGroups = new ForumGroupCollection();

                // Try to find the requested forum group
                foreach (ForumGroup f in forumGroups) {

                    if (f.ForumGroupID == ForumGroupID)
                        newForumGroups.Add(f);

                }

                forumGroups = newForumGroups;

            }

            // Databind if we have values
            if (forumGroups.Count > 0) {
                DataSource = forumGroups;
                DataBind();
            } else { // No forums
                Label label;

                // Set some properties on the label
                label = new Label();
                label.CssClass = "normalTextSmallBold";
                label.Text = "Sorry, no forums are available.";

                // Clear the controls collection and add our label
                Controls.Clear();
                Controls.Add(label);
            }

        }

        // *********************************************************************
        //  ForumItemTemplate
        //
        /// <summary>
        /// Allows user to define the template used to render the Forum Items
        /// </summary>
        /// 
        // ********************************************************************/
        [TemplateContainer(typeof(RepeaterItem))]
        public ITemplate ForumItemTemplate {
            get {  return forumItemTemplate;  }
            set {  forumItemTemplate = value;  }		
        }

        // *********************************************************************
        //  ShowAllForumGroups
        //
        /// <summary>
        /// Displays all forum groups, even those that don't return forums
        /// </summary>
        /// 
        // ********************************************************************/
        public bool ShowAllForumGroups {
            get { return showAllForumGroups; }
            set { showAllForumGroups = value; }
        }

    }
}
