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

namespace AspNetForums.Controls {

    [
    ParseChildren(true),
    Designer(typeof(AspNetForums.Controls.Design.ForumRepeaterDesigner))
    ]
    public class ForumRepeater : ForumRepeaterControl {
        bool showAllForums = false;


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
            forumTitle.NavigateUrl = Globals.UrlShowForum + forum.ForumID;
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

            forumDescription.Text = "<br>" + forum.Description;
        }

        
        // *********************************************************************
        //  HandleDataBindingForSubForums
        //
        /// <summary>
        /// DataBinding event for the sub forums
        /// </summary>
        /// 
        // ********************************************************************/   
        private void HandleDataBindingForSubForums(Object sender, EventArgs e) {
            ForumCollection forums;
            Forums f = new Forums();

            PlaceHolder subForums = (PlaceHolder) sender;
            RepeaterItem container = (RepeaterItem) subForums.NamingContainer;

            Forum forum = (Forum) container.DataItem;

            // Attempt to get the sub forums
            if (ForumUser == null)
                forums = f.GetChildForums(forum.ForumID, null, false);
            else
                forums = f.GetChildForums(forum.ForumID, ForumUser.Username, false);

            // Do we have any sub forums?
            if (forums.Count > 0) {
                subForums.Controls.Add(new LiteralControl("<br><span class=\"normalTextSmallBold\">(sub-forums: "));

                foreach (Forum subforum in forums) {
                    HyperLink l = new HyperLink();
                    l.Text = subforum.Name;
                    l.NavigateUrl = Globals.UrlShowForum + subforum.ForumID;
                    subForums.Controls.Add(l);
                }

                subForums.Controls.Add(new LiteralControl(")</span>"));
            }
        }
        
        // *********************************************************************
        //  HandleDataBindingForTotalThreads
        //
        /// <summary>
        /// DataBinding event for the forum total threads
        /// </summary>
        /// 
        // ********************************************************************/   
        private void HandleDataBindingForTotalThreads(Object sender, EventArgs e) {
            TableCell totalThreads = (TableCell) sender;
            RepeaterItem container = (RepeaterItem) totalThreads.NamingContainer;
            Label label;

            // Create label and set style
            label = new Label();
            label.CssClass = "normalTextSmaller";

            Forum forum = (Forum) container.DataItem;

            if (forum.TotalThreads > 0)
                label.Text = forum.TotalThreads.ToString("n0");
            else
                label.Text = "-";

            totalThreads.Controls.Add(label);

        }

        // *********************************************************************
        //  HandleDataBindingForTotalPosts
        //
        /// <summary>
        /// DataBinding event for the forum total posts
        /// </summary>
        /// 
        // ********************************************************************/   
        private void HandleDataBindingForTotalPosts(Object sender, EventArgs e) {
            TableCell totalPosts = (TableCell) sender;
            RepeaterItem container = (RepeaterItem) totalPosts.NamingContainer;
            Label label;

            Forum forum = (Forum) container.DataItem;

            label = new Label();
            label.CssClass = "normalTextSmaller";

            if (forum.TotalPosts > 0)
                label.Text = forum.TotalPosts.ToString("n0");
            else
                label.Text = "-";

            totalPosts.Controls.Add(label);
        }
        
        // *********************************************************************
        //  HandleDataBindingForMostRecentPost
        //
        /// <summary>
        /// DataBinding event for the forum most recent post
        /// </summary>
        /// 
        // ********************************************************************/   
        private void HandleDataBindingForMostRecentPost(Object sender, EventArgs e) {
            Label postDetails = (Label) sender;
            RepeaterItem container = (RepeaterItem) postDetails.NamingContainer;
            DateTime postDateTime;
            Label postDate = new Label();
            HyperLink newPost = new HyperLink();
            HyperLink author = new HyperLink();
            String dateFormat;

            // Do we have a signed in user?
            if (ForumUser != null)
                dateFormat = ForumUser.DateFormat;
            else
                dateFormat = Globals.DateFormat;

            // Get the forum
            Forum forum = (Forum) container.DataItem;

            // Get the post date
            postDateTime = forum.MostRecentPostDate;

            // Is a user logged in?
            if (ForumUser != null)
                postDateTime = Users.AdjustForTimezone(postDateTime, ForumUser);

            // Did the post occur today?
            if ((postDateTime.DayOfYear == DateTime.Now.DayOfYear) && (postDateTime.Year == DateTime.Now.Year))
                postDate.Text = "<b>Today @ " + postDateTime.ToString(Globals.TimeFormat) + "</b>";
            else if (postDateTime.Year > 0050)
                postDate.Text = postDateTime.ToString(dateFormat + " " + Globals.TimeFormat);
            else
                postDate.Text = "No Posts";

            // Add the post
            postDetails.Controls.Add(postDate);
            
       }

        // *********************************************************************
        //  HandleDataBindingForPostedBy
        //
        /// <summary>
        /// DataBinding event for the forum most recent post by
        /// </summary>
        /// 
        // ********************************************************************/   
        private void HandleDataBindingForPostedBy(Object sender, EventArgs e) {
            HyperLink postedBy = new HyperLink();
            HyperLink newPost = new HyperLink();
            Label label = (Label) sender;
            RepeaterItem container = (RepeaterItem) label.NamingContainer;

            Forum forum = (Forum) container.DataItem;

            // No threads just return
            if (forum.TotalThreads == 0)
                return;

            // Hyper link for author
            postedBy.Text = forum.MostRecentPostAuthor;
            postedBy.NavigateUrl = Globals.UrlUserProfile + forum.MostRecentPostAuthor;

            // Link to new post
            newPost.Text = "<img border=\"0\" src=\"" + Globals.ApplicationVRoot + "/skins/" + Globals.Skin + "/images/icon_mini_topic.gif\">";
            newPost.NavigateUrl = Globals.UrlShowPost + forum.MostRecentThreadId + "#" + forum.MostRecentPostId;

            label.Controls.Add(new LiteralControl("by "));
            label.Controls.Add(postedBy);
            label.Controls.Add(newPost);
            
        }


        // *********************************************************************
        //  HandleDataBindingForStatusImage
        //
        /// <summary>
        /// DataBinding event for the image used to indicate if there are new posts
        /// </summary>
        /// 
        // ********************************************************************/   
        private void HandleDataBindingForStatusImage(Object sender, EventArgs e) {
            Image img = (Image) sender;
            RepeaterItem container = (RepeaterItem) img.NamingContainer;

            Forum forum = (Forum) container.DataItem;

            if (ForumUser == null) {
                img.ImageUrl = Globals.ApplicationVRoot + "/skins/" + SkinName + "/images/forum_status.gif";
            } else {
    
                // Is this a private forum?
                if (forum.IsPrivate) {

                    if (forum.LastUserActivity < forum.MostRecentPostDate) {
                        img.ImageUrl = Globals.ApplicationVRoot + "/skins/" + SkinName + "/images/forum_private_newposts.gif";
                        img.AlternateText = "Private Forums - New Posts";
                    } else {
                        img.ImageUrl = Globals.ApplicationVRoot + "/skins/" + SkinName + "/images/forum_private.gif";
                        img.AlternateText = "Private Forums";
                    }
                         
                } else {
                    if (forum.LastUserActivity < forum.MostRecentPostDate) {
                        img.ImageUrl = Globals.ApplicationVRoot + "/skins/" + SkinName + "/images/forum_status_new.gif";
                        img.AlternateText = "New Posts";
                    } else {
                        img.ImageUrl = Globals.ApplicationVRoot + "/skins/" + SkinName + "/images/forum_status.gif";
                    }
                }
            }

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

            PlaceHolder subForums = new PlaceHolder();
            Label label;
            PlaceHolder placeHolder = new PlaceHolder();
            TableCell td;
            TableRow tr = new TableRow();

            // Column 1
            td = new TableCell();
            td.CssClass = "forumRow";
            td.HorizontalAlign = HorizontalAlign.Center;
            td.VerticalAlign = VerticalAlign.Top;
            td.Width = 34;
            td.Wrap = false;
            Image img = new Image();
            img.Width = 34;
            img.DataBinding += new System.EventHandler(HandleDataBindingForStatusImage);
            td.Controls.Add(img);
            tr.Controls.Add(td);

            // Column 2
            td = new TableCell();
            td.CssClass = "forumRow";
            td.Width = Unit.Percentage(80);
            HyperLink link = new HyperLink();
            link.CssClass = "forumTitle";
            link.DataBinding += new System.EventHandler(HandleDataBindingForForumTitle);

            // Description and sub forums
            Label forumDescription = new Label();
            forumDescription.CssClass = "normalTextSmall";
            forumDescription.DataBinding += new System.EventHandler(HandleDataBindingForForumDescription);
            subForums.DataBinding += new System.EventHandler(HandleDataBindingForSubForums);
            td.Controls.Add(link);
            td.Controls.Add(forumDescription);
            td.Controls.Add(subForums);
            tr.Controls.Add(td);

            // Column 3
            td = new TableCell();
            td.HorizontalAlign = HorizontalAlign.Center;
            td.CssClass = "forumRowHighlight";
            td.DataBinding += new System.EventHandler(HandleDataBindingForTotalThreads);
            tr.Controls.Add(td);

            // Column 4
            td = new TableCell();
            td.HorizontalAlign = HorizontalAlign.Center;
            td.CssClass = "forumRowHighlight";
            td.DataBinding += new System.EventHandler(HandleDataBindingForTotalPosts);
            tr.Controls.Add(td);

            // Column 5
            td = new TableCell();
            td.HorizontalAlign = HorizontalAlign.Center;
            td.CssClass = "forumRowHighlight";
            Label mostRecentPostDate = new Label();
            mostRecentPostDate.CssClass = "normalTextSmaller";
            mostRecentPostDate.DataBinding += new System.EventHandler(HandleDataBindingForMostRecentPost);
            label = new Label();
            label.CssClass = "normalTextSmaller";
            label.DataBinding += new System.EventHandler(HandleDataBindingForPostedBy);
            td.Controls.Add(mostRecentPostDate);
            td.Controls.Add(new LiteralControl("<BR>"));
            td.Controls.Add(label);
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
            string username = null;

            // determine if we want to bind to the default template or not
            ApplyTemplates();

            if (ForumUser != null)
                username = ForumUser.Username;

            // bind the datalist to the SqlDataReader returned by the GetAllForums() method
            ForumCollection forums;
            Forums f = new Forums();
            try {
                if (ForumGroupID == 0) {

                    if (showAllForums)
                        forums = Forums.GetAllForums(true, username);
                    else
                        forums = Forums.GetAllForums(false, username);

                    forums.Sort();
                    DataSource = forums;
                } else {
                    if (showAllForums)
                        DataSource = f.GetForumsByForumGroupId(ForumGroupID, username, true);
                    else
                        DataSource = f.GetForumsByForumGroupId(ForumGroupID, username);
                }
            } catch (Components.ForumNotFoundException) {
                Page.Response.Redirect(Globals.UrlMessage + Convert.ToInt32(Messages.UnknownForum));
                Page.Response.End();
            }

            this.DataBind();
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
            string pathToAlternatingItemTemplate;
            string pathToFooterTemplate;
            string keyForHeaderTemplate;
            string keyForItemTemplate;
            string keyForAlternatingItemTemplate;
            string keyForFooterTemplate;
            
            // Are we using skinned template?
            if (Page != null) {

                // Set the file paths to where the templates should be found
                pathToHeaderTemplate = Globals.ApplicationVRoot + "/Skins/" + SkinName + "/Templates/ForumRepeater-Header.ascx";
                pathToItemTemplate = Globals.ApplicationVRoot + "/skins/" + SkinName + "/Templates/ForumRepeater-Item.ascx";
                pathToAlternatingItemTemplate = Globals.ApplicationVRoot + "/skins/" + SkinName + "/Templates/ForumRepeater-AlternatingItem.ascx";
                pathToFooterTemplate = Globals.ApplicationVRoot + "/skins/" + SkinName + "/Templates/ForumRepeater-Footer.ascx";

                // Set the file paths to where the templates should be found
                keyForHeaderTemplate = SkinName + "/Templates/ForumRepeater-Header.ascx";
                keyForItemTemplate = SkinName + "/Templates/ForumRepeater-Item.ascx";
                keyForAlternatingItemTemplate = SkinName + "/Templates/ForumRepeater-AlternatingItem.ascx";
                keyForFooterTemplate = SkinName + "/Templates/ForumRepeater-Footer.ascx";

                // Attempt to get the skinned header template
                if (HeaderTemplate == null)
                    HeaderTemplate = Globals.LoadSkinnedTemplate(pathToHeaderTemplate, keyForHeaderTemplate, Page);

                // Attempt to get the skinned item template
                if (ItemTemplate == null)
                    ItemTemplate = Globals.LoadSkinnedTemplate(pathToItemTemplate,keyForItemTemplate, Page);

                // Attempt to get the skinned item template
                if (AlternatingItemTemplate == null)
                    AlternatingItemTemplate = Globals.LoadSkinnedTemplate(pathToAlternatingItemTemplate,keyForAlternatingItemTemplate, Page);

                // Attempt to get the footer template
                if (FooterTemplate == null)
                    FooterTemplate = Globals.LoadSkinnedTemplate(pathToFooterTemplate, keyForFooterTemplate, Page);
            }

            // No skinned or user defined template, load the default
            if (ItemTemplate == null)
                ItemTemplate = new CompiledTemplateBuilder(new BuildTemplateMethod(BuildItemTemplate));

        }

        // *********************************************************************
        //  ShowAllForums
        //
        /// <summary>
        /// Returns forums that are not active - however, will not return forums that
        /// the user's credentials do not have permissions to.
        /// </summary>
        /// 
        // ********************************************************************/   
        public bool ShowAllForums {
            get { return showAllForums; }
            set { showAllForums = value; }
        }
    }
}

