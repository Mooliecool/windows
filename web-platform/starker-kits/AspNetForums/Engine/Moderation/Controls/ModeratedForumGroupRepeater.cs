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

namespace AspNetForums.Controls.Moderation {
	
    // *********************************************************************
    //  ModeratedForumGroupRepeater
    //
    /// <summary>
    /// A repeater that repeats forum groups
    /// </summary>
    /// 
    // ********************************************************************/ 
    public class ModeratedForumGroupRepeater : ForumGroupRepeater {

        // *********************************************************************
        //  ModeratedForumRepeater
        //
        /// <summary>
        /// Class contructor - read in the PostId that was sent via the
        /// QueryString or Post body of the request
        /// </summary>
        /// 
        // ********************************************************************/ 
        public ModeratedForumGroupRepeater() : base() {
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
            forumGroups = Moderate.GetForumGroupsForModeration(ForumUser.Username);

            // Databind if we have values
            if (forumGroups.Count > 0) {
                DataSource = forumGroups;
                DataBind();
            } else { // No forums
                Label label;

                // Set some properties on the label
                label = new Label();
                label.CssClass = "normalTextSmallBold";
                label.Text = "No threads to moderate.";

                // Clear the controls collection and add our label
                Controls.Clear();
                Controls.Add(label);
            }

        }

        // *********************************************************************
        //  BuildHeaderTemplate
        //
        /// <summary>
        /// Builds the default header template if the user does not specify one
        /// </summary>
        /// 
        // ********************************************************************/
        public override void BuildHeaderTemplate(Control _ctrl) {
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
            th.Wrap = false;
            th.CssClass = "tableHeaderText";
            th.Text = "&nbsp;&nbsp;Unapproved Posts&nbsp;&nbsp;";
            tr.Controls.Add(th);
            
            System.Web.UI.IParserAccessor __parser = ((System.Web.UI.IParserAccessor)(_ctrl));

            __parser.AddParsedSubObject(new LiteralControl("<table cellpadding=\"2\" cellspacing=\"1\" border=\"0\" width=\"100%\" class=\"tableBorder\">"));
            __parser.AddParsedSubObject(tr);

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
            ModeratedForumRepeater forumRepeater = (ModeratedForumRepeater) sender;
            RepeaterItem container = (RepeaterItem) forumRepeater.NamingContainer;

            ForumGroup forumGroup = (ForumGroup) container.DataItem;

            forumRepeater.ForumGroupID = forumGroup.ForumGroupID;
        }

        
        // *********************************************************************
        //  HandleDataBindingForGroupTitle
        //
        /// <summary>
        /// DataBinding event for the forum group title
        /// </summary>
        /// 
        // ********************************************************************/        
        override protected void HandleDataBindingForGroupTitle(Object sender, EventArgs e) {

            HyperLink link = (HyperLink) sender;
            RepeaterItem container = (RepeaterItem) link.NamingContainer;

            ForumGroup forumGroup = (ForumGroup) container.DataItem;

            link.Text = forumGroup.Name;
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
        public override Control BeginBuildItemTemplate() {

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

            td.Controls.Add(link);
            tr.Controls.Add(td);

            tr.Controls.Add(td);

            // Add the Table Row
            placeHolder.Controls.Add(tr);

            // Add a new ForumGroupRepeater
            ModeratedForumRepeater forumRepeater = new ModeratedForumRepeater();
            forumRepeater.ID = "ForumRepeater";
            placeHolder.Controls.Add(forumRepeater);

            // wire up the handleDataBinding event handler to the DataBinding event
            link.DataBinding += new System.EventHandler(HandleDataBindingForGroupTitle);			
            forumRepeater.DataBinding += new System.EventHandler(HandleDataBindingForForumRepeater);

            return placeHolder;
        }
    }
}
