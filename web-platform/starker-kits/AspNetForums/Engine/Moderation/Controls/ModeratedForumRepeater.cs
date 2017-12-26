using System;
using System.Collections;
using System.Collections.Specialized;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using AspNetForums;
using AspNetForums.Components;
using System.ComponentModel;
using System.ComponentModel.Design;

namespace AspNetForums.Controls.Moderation {

    // *********************************************************************
    //  ModeratedForumRepeater
    //
    /// <summary>
    /// This class is used to display all the forums for a given forum id, or
    /// simply all the forums, that require moderation.
    /// </summary>
    /// 
    // ********************************************************************/ 
    public class ModeratedForumRepeater : ForumRepeaterControl {

        // *********************************************************************
        //  ForumRepeater
        //
        /// <summary>
        /// Class contructor
        /// </summary>
        /// 
        // ********************************************************************/ 
        public ModeratedForumRepeater() : base() {
        }

        // *********************************************************************
        //  HandleDataBindingForForumTitle
        //
        /// <summary>
        /// DataBinding event for the forum title
        /// </summary>
        /// 
        // ********************************************************************/   
        private void HandleDataBindingForForumTitle(Object sender, EventArgs e) {
            HyperLink forumTitle = (HyperLink) sender;
            RepeaterItem container = (RepeaterItem) forumTitle.NamingContainer;

            Forum forum = (Forum) container.DataItem;

            forumTitle.Text = forum.Name;
        }

        // *********************************************************************
        //  HandleDataBindingForForumDescription
        //
        /// <summary>
        /// DataBinding event for the forum description
        /// </summary>
        /// 
        // ********************************************************************/   
        private void HandleDataBindingForForumDescription(Object sender, EventArgs e) {
            Label forumDescription = (Label) sender;
            RepeaterItem container = (RepeaterItem) forumDescription.NamingContainer;

            Forum forum = (Forum) container.DataItem;

            forumDescription.Text = forum.Description;
        }

        // *********************************************************************
        //  HandleDataBindingForTotalThreads
        //
        /// <summary>
        /// DataBinding event for the forum total threads
        /// </summary>
        /// 
        // ********************************************************************/   
        private void HandleDataBindingForUnapprovedPosts(Object sender, EventArgs e) {
            TableCell unapprovedPosts = (TableCell) sender;
            RepeaterItem container = (RepeaterItem) unapprovedPosts.NamingContainer;
            HyperLink link;

            // Create label and set style
            link = new HyperLink();
            link.CssClass = "linkSmallBold";

            ModeratedForum forum = (ModeratedForum) container.DataItem;

            if (forum.TotalPostsAwaitingModeration > 0) {
                link.Text = "Moderate (" + forum.TotalPostsAwaitingModeration.ToString("n0") + ")";
                link.NavigateUrl = Globals.UrlModerateForumPosts + forum.ForumID;
            } else {
                link.Text = "-";
            }

            unapprovedPosts.Controls.Add(link);

        }
        
        // *********************************************************************
        //  BeginBuildItemTemplate
        //
        /// <summary>
        /// Builds a default Item template if the user does not specify one
        /// </summary>
        /// 
        // ********************************************************************/   
        public virtual Control BeginBuildItemTemplate() {

            PlaceHolder placeHolder = new PlaceHolder();
            TableCell td;
            TableRow tr = new TableRow();

            // Column 1
            td = new TableCell();
            td.CssClass = "forumRow";
            td.Width = 10;
            tr.Controls.Add(td);

            // Column 2
            td = new TableCell();
            td.CssClass = "forumRow";
            HyperLink link = new HyperLink();
            link.CssClass = "forumTitle";
            link.DataBinding += new System.EventHandler(HandleDataBindingForForumTitle);

            Label forumDescription = new Label();
            forumDescription.CssClass = "normalTextSmall";
            forumDescription.DataBinding += new System.EventHandler(HandleDataBindingForForumDescription);
            td.Controls.Add(link);
            td.Controls.Add(new LiteralControl("<BR>"));
            td.Controls.Add(forumDescription);
            tr.Controls.Add(td);

            // Unapproved Posts
            td = new TableCell();
            td.HorizontalAlign = HorizontalAlign.Left;
            td.CssClass = "forumRowHighlight";
            td.Controls.Add(new LiteralControl("&nbsp; "));
            td.DataBinding += new System.EventHandler(HandleDataBindingForUnapprovedPosts);
            tr.Controls.Add(td);

            // Add the Table Row
            placeHolder.Controls.Add(tr);

            return placeHolder;
        }

        // *********************************************************************
        //  BuildItemTemplate
        //
        /// <summary>
        /// Template builder for the ItemTemplate
        /// </summary>
        /// 
        // ********************************************************************/   
        private void BuildItemTemplate(Control _ctrl) {
		
            // add the DataBoundLiteralControl to the parser
            System.Web.UI.IParserAccessor __parser = ((System.Web.UI.IParserAccessor)(_ctrl));
            __parser.AddParsedSubObject(BeginBuildItemTemplate());

        }

        // *********************************************************************
        //  CreateChildControls
        //
        /// <summary>
        /// Override create child controls
        /// </summary>
        /// 
        // ********************************************************************/   
        protected override void CreateChildControls() {
            EnableViewState = false;

            // determine if we want to bind to the default template or not
            if (this.ItemTemplate == null) {

                // create a new template
                base.ItemTemplate = new CompiledTemplateBuilder(new BuildTemplateMethod(BuildItemTemplate));
                base.CreateChildControls();
            }

            base.CreateChildControls();

            // bind the datalist to the SqlDataReader returned by the GetAllForums() method
            DataSource = Moderate.GetForumsForModerationByForumGroupId(ForumGroupID, ForumUser.Username);
            DataBind();
        }

        // *********************************************************************
        //  ItemTemplate
        //
        /// <summary>
        /// Name the template container so user doesn't have to do this in data
        /// binding code.
        /// </summary>
        /// 
        // ********************************************************************/   
        [TemplateContainer(typeof(RepeaterItem))]
        public ITemplate ForumItemTemplate {
            get {  return this.ItemTemplate;  }
            set {  this.ItemTemplate = value;  }		
        }

    }
}

