using System;
using System.Data;
using System.Data.SqlClient;
using System.Collections;
using System.Collections.Specialized;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using AspNetForums;
using AspNetForums.Controls.Moderation;
using AspNetForums.Controls.Specialized;
using AspNetForums.Components;
using System.ComponentModel;

namespace AspNetForums.Controls {

    /// <summary>
    /// This Web control displays a thread in a flat display.  The developer must pass in
    /// either a PostID.  If a PostID is passed in, the thread that that Post belongs
    /// to is constructed.
    /// </summary>
    [
        ParseChildren(true)
    ]
    public class PostList : DataList {

        User user;
        String skinName;
        int postID;

        // *********************************************************************
        //  CreateChildControls
        //
        /// <summary>
        /// This event handler adds the children controls and is resonsible
        /// for determining the template type used for the control.
        /// </summary>
        /// 
        // ********************************************************************/ 
        protected override void CreateChildControls() {

            // Do we have a user?
            if (HttpContext.Current.Request.IsAuthenticated) {
                user = Users.GetUserInfo(HttpContext.Current.User.Identity.Name, true);
            }

            // Set the siteStyle for the page
            if (user != null)
                skinName = user.Skin;
            else
                skinName = Globals.Skin;

            // Apply Template
            ApplyTemplates();

            // Viewstate is disabled
            EnableViewState = false;

        }

        // *********************************************************************
        //  HandleDataBindingForPostCell
        //
        /// <summary>
        /// Databinds the post cell
        /// </summary>
        /// <remarks>
        /// Used only if a user defined template is not provided.
        /// </remarks>
        /// 
        // ********************************************************************/
        private void HandleDataBindingForPostCell(Object sender, EventArgs e) {
            Table table;
            TableRow tr;
            TableCell td;
            Label label;
            string dateFormat; 
            DateTime postDateTime;
            User postUser;

            // Get the sender
            TableCell postInfo = (TableCell) sender;
            DataListItem container = (DataListItem) postInfo.NamingContainer;
            Post post = (Post) container.DataItem;

            // Get the user that created the post
            postUser = Users.GetUserInfo(post.Username, false);

            // Create the table
            table = new Table();
            table.CellPadding = 3;
            table.CellSpacing = 0;
            table.Width = Unit.Percentage(100);
            
            // Row 1
            tr = new TableRow();
            td = new TableCell();
            td.CssClass = "forumRowHighlight";

            // Add in Subject
            label = new Label();
            label.CssClass = "normalTextSmallBold";
            label.Text = post.Subject + "<a name=\"" + post.PostID + "\"/>";
            td.Controls.Add(label);

            td.Controls.Add(new LiteralControl("<br>"));

            // Add in 'Posted: '
            label = new Label();
            label.CssClass = "normalTextSmaller";
            label.Text = " Posted: ";
            td.Controls.Add(label);

            // Get the postDateTime
            postDateTime = post.PostDate;

            // Personalize
            if (user != null) {
                dateFormat = user.DateFormat;
                postDateTime = Users.AdjustForTimezone(postDateTime, user);
            } else {
                dateFormat = Globals.DateFormat;
            }

            // Add in PostDateTime
            label = new Label();
            label.CssClass = "normalTextSmaller";
            label.Text = postDateTime.ToString(dateFormat + " " + Globals.TimeFormat);
            td.Controls.Add(label);

            // Add column 1
            tr.Controls.Add(td);
/*
 * TODO: Enable Rating of posts
            td = new TableCell();
            td.CssClass = "forumRowHighlight";
            td.HorizontalAlign = HorizontalAlign.Right;
            td.VerticalAlign = VerticalAlign.Top;
            td.Text= "<a href>Rate this post</a>&nbsp;" ;
            tr.Controls.Add(td);
*/
            table.Controls.Add(tr);

            tr.Controls.Add(td);
            table.Controls.Add(tr);

            // Row 2 (body)
            tr = new TableRow();
            td = new TableCell();
            td.ColumnSpan = 2;

            // Add Body
            if (post.PostType == Posts.PostType.Post) {
                label = new Label();
                label.CssClass = "normalTextSmall";
                label.Text = Globals.FormatPostBody(post.Body);
                td.Controls.Add(label);
            } else if (post.PostType == Posts.PostType.Vote) {
                Vote vote = new Vote(post.PostID, post.Subject, post.Body);
                td.Controls.Add(vote);
                td.HorizontalAlign = HorizontalAlign.Center;
            }

            // Add row 2
            tr.Controls.Add(td);
            table.Controls.Add(tr);

            // Row 3 (Signature)
            tr = new TableRow();
            td = new TableCell();
            td.ColumnSpan = 2;

            label = new Label();
            label.CssClass = "normalTextSmaller";

            if (postUser.Signature != "") {
                label.Text = Globals.FormatSignature(postUser.Signature);
            }
            td.Controls.Add(label);
            tr.Controls.Add(td);
            table.Controls.Add(tr);

            // Add whitespace
            tr = new TableRow();
            td = new TableCell();
            td.Height = 2;
            tr.Controls.Add(td);
            table.Controls.Add(tr);

            // Add buttons for user options
            tr = new TableRow();
            td = new TableCell();
            td.ColumnSpan = 2;
            
            // Add the reply button
            if (!post.IsLocked) {
                // Reply button
                HyperLink replyButton = new HyperLink();
                replyButton.Text = "<img border=0 src=" + Globals.ApplicationVRoot + "/skins/" + Globals.Skin + "/images/newpost.gif" + ">";
                replyButton.NavigateUrl = Globals.UrlReplyToPost + post.PostID + "&mode=flat";
                td.Controls.Add(replyButton);
            }

            // Add the edit button
            if ((user != null) && (user.Username.ToLower() == post.Username.ToLower()) && (user.IsTrusted)) {
                // Edit button
                HyperLink editButton = new HyperLink();
                editButton.Text = "<img border=0 src=" + Globals.ApplicationVRoot + "/skins/" + Globals.Skin + "/images/editpost.gif" + ">";
                editButton.NavigateUrl = Globals.UrlUserEditPost + post.PostID + "&RedirectUrl=" + HttpContext.Current.Server.UrlEncode(Globals.UrlShowPost + postID);
                td.Controls.Add(editButton);
            }

            // Anything to add to the table control?
            if (td.Controls.Count > 0) {
                tr.Controls.Add(td);
                table.Controls.Add(tr);
            }

            // Is the current user a moderator?
            if ((user != null) && (user.IsModerator)) {
                tr = new TableRow();
                td = new TableCell();
                td.ColumnSpan = 2;

                // Find the moderation menu
                ModerationMenu moderationMenu = new ModerationMenu();
                moderationMenu.PostID = post.PostID;
                moderationMenu.ThreadID = post.ThreadID;
                moderationMenu.UsernamePostedBy = post.Username;
                moderationMenu.SkinFilename = "Moderation/Skin-ModeratePost.ascx";

                td.Controls.Add(moderationMenu);
                tr.Controls.Add(td);
                table.Controls.Add(tr);

            }

            postInfo.Controls.Add(table);
        }

        // *********************************************************************
        //  HandleDataBindingForAuthorCell
        //
        /// <summary>
        /// Databinds the name of the author.
        /// </summary>
        /// <remarks>
        /// Used only if a user defined template is not provided.
        /// </remarks>
        /// 
        // ********************************************************************/
        private void HandleDataBindingForAuthorCell(Object sender, EventArgs e) {	

            TableCell userInfo = (TableCell) sender;
            DataListItem container = (DataListItem) userInfo.NamingContainer;
            Post post = (Post) container.DataItem;
            DateTime activityDateTime;
            HyperLink link;
            Label label;
            Image image;
            Uri url;
            User postUser;

            // Get the postUser object - note, we are using
            // the cache under the covers, so this doesn't
            // result in a db lookup for each request
            postUser = Users.GetUserInfo(post.Username, false);

            // Build postUser info table
            Table table = new Table();
            TableRow tr;
            TableCell td;

            // Online/Offline Status
            image = new Image();

            // Personalize
            activityDateTime = postUser.LastActivity;
            if (user != null) {
                activityDateTime = Users.AdjustForTimezone(activityDateTime, user);
            }

            if (postUser.LastActivity > DateTime.Now.AddMinutes(-15)) {
                image.ImageUrl = Globals.ApplicationVRoot + "/skins/" + skinName + "/images/user_IsOnline.gif";
                image.AlternateText = postUser.Username + " is online. Last active: " + activityDateTime;
            } else {
                image.ImageUrl = Globals.ApplicationVRoot + "/skins/" + skinName + "/images/user_IsOffline.gif";
                image.AlternateText = postUser.Username + " is not online. Last active: " + activityDateTime;
            }

            // Author
            tr = new TableRow();
            td = new TableCell();
            link = new HyperLink();
            link.CssClass = "normalTextSmallBold";
            link.NavigateUrl = Globals.UrlUserProfile + postUser.Username;
            link.Text = postUser.Username;
            td.Controls.Add(image);
            td.Controls.Add(new LiteralControl("&nbsp;"));
            td.Controls.Add(link);

            // whitespace
            td.Controls.Add(new LiteralControl("<br>"));

            // Web Site
            if (postUser.Url != "") {
                url = new Uri(postUser.Url);
                link = new HyperLink();
                link.CssClass = "normalTextSmaller";
                link.NavigateUrl = postUser.Url;
                link.Text = url.Host;
                link.Target = "_new";
                td.Controls.Add(link);
            }
            tr.Controls.Add(td);
            table.Controls.Add(tr);

            // Do we have any Avatar for the postUser?
            if ((postUser.HasAvatar) && (postUser.ShowAvatar)) {

                tr = new TableRow();
                td = new TableCell();
                image = new Image();
//                image.Width = 80;
//                image.Height = 80;
                image.ImageUrl = postUser.AvatarUrl;
                td.Controls.Add(image);
                tr.Controls.Add(td);
                table.Controls.Add(tr);
            }

            /***** Code specific to www.asp.net/Forums *****/
            // BitMask - Is the postUser an ASP.NET Ace?
            if ((postUser.Attributes[3] & 0x00000002) ==  0x00000002) {
                tr = new TableRow();
                td = new TableCell();
                image = new Image();
                image.ImageUrl = Globals.ApplicationVRoot + "/skins/" + skinName + "/images/ace.gif";
                image.AlternateText = "ASP.NET ACE";
                td.Controls.Add(image);
                tr.Controls.Add(td);
                table.Controls.Add(tr);
            }

            // BitMask - Is the postUser a Microsoft MVP?
            if ((postUser.Attributes[3] & 0x00000001) ==  0x00000001) {

                tr = new TableRow();
                td = new TableCell();
                image = new Image();
                image.ImageUrl = Globals.ApplicationVRoot + "/skins/" + skinName + "/images/mvp.gif";
                image.AlternateText = "Microsoft MVP";
                td.Controls.Add(image);
                tr.Controls.Add(td);
                table.Controls.Add(tr);
            }
            /***** Code specific to www.asp.net/Forums *****/

            // BitMask - Is the postUser a top 25 poster?
            if ((postUser.Attributes[3] & 0x00000004) ==  0x00000004) {
                tr = new TableRow();
                td = new TableCell();
                image = new Image();
                image.ImageUrl = Globals.ApplicationVRoot + "/skins/" + skinName + "/images/users_top25.gif";
                image.AlternateText = "Top 25 Poster";
                td.Controls.Add(image);
                tr.Controls.Add(td);
                table.Controls.Add(tr);

                // Top 50 postUser?
            } else if ((postUser.Attributes[3] & 0x00000008) ==  0x00000008) {
                tr = new TableRow();
                td = new TableCell();
                image = new Image();
                image.ImageUrl = Globals.ApplicationVRoot + "/skins/" + skinName + "/images/users_top50.gif";
                image.AlternateText = "Top 50 Poster";
                td.Controls.Add(image);
                tr.Controls.Add(td);
                table.Controls.Add(tr);

            } else if ((postUser.Attributes[3] & 0x00000010) ==  0x00000010) {
                tr = new TableRow();
                td = new TableCell();
                image = new Image();
                image.ImageUrl = Globals.ApplicationVRoot + "/skins/" + skinName + "/images/users_top100.gif";
                image.AlternateText = "Top 100 Poster";
                td.Controls.Add(image);
                tr.Controls.Add(td);
                table.Controls.Add(tr);
            }

            // Moderator?
            if (postUser.IsModerator) {
                tr = new TableRow();
                td = new TableCell();
                image = new Image();
                image.ImageUrl = Globals.ApplicationVRoot + "/skins/" + skinName + "/images/users_moderator.gif";
                image.AlternateText = "Forum Moderator";
                td.Controls.Add(image);
                tr.Controls.Add(td);
                table.Controls.Add(tr);
            }

            // When did the postUser join?
            tr = new TableRow();
            td = new TableCell();
            label = new Label();
            label.CssClass = "normalTextSmaller";
            label.Text = "<b>Joined:</b> " + postUser.DateCreated.ToString("dd MMM yyyy");
            td.Controls.Add(label);
            tr.Controls.Add(td);
            table.Controls.Add(tr);

            // Total posts for this postUser
            tr = new TableRow();
            td = new TableCell();
            label = new Label();
            label.CssClass = "normalTextSmaller";
            label.Text = "<b>Total Posts: </b>" + postUser.TotalPosts.ToString();
            td.Controls.Add(label);
            tr.Controls.Add(td);
            table.Controls.Add(tr);

            // Add a little whitespace
            tr = new TableRow();
            td = new TableCell();
            td.Text = "&nbsp;";
            tr.Controls.Add(td);
            table.Controls.Add(tr);

            // Add table to placeholder
            userInfo.Controls.Add(table);
        }

        
        // *********************************************************************
        //  HandleDataBindingForAuthorDetails
        //
        /// <summary>
        /// Databinds the name of the author.
        /// </summary>
        /// <remarks>
        /// Used only if a user defined template is not provided.
        /// </remarks>
        /// 
        // ********************************************************************/
        private void HandleDataBindingForAuthorDetails (Object sender, EventArgs e) {
            TableCell userInfo = (TableCell) sender;
            DataListItem container = (DataListItem) userInfo.NamingContainer;
            Post post = (Post) container.DataItem;
            User user;
            Image image = new Image();

            // Get the user object - note, we are using
            // the cache under the covers, so this doesn't
            // result in a db lookup for each request
            user = Users.GetUserInfo(post.Username, false);

            // Build user info table
            Table table = new Table();

            // Add table to placeholder
            userInfo.Controls.Add(table);
        }

        // *********************************************************************
        //  RenderHeaderTemplate
        //
        /// <summary>
        /// This function renders the header template
        /// </summary>
        /// 
        // ********************************************************************/
        private Control RenderHeaderTemplate() {
            PostCollection posts;
            Post post;
            Table table = new Table();
            TableRow tr;
            TableHeaderCell th;
            TableCell td;

            // Set up the table
            table.Width = Unit.Percentage(100);

            // Get details about this post and ensure
            // we get the root post
            posts = (PostCollection) DataSource;
            post = (Post)posts[0];
            postID = post.PostID;

            // Header row to display navigation
            tr = new TableRow();

            // Header display, e.g. Previous | Next navigation
            td = new TableCell();
            td.ColumnSpan = 2;
            td.CssClass = "forumHeaderBackgroundAlternate";
            td.Height = 20;
            td.Controls.Add(HeaderDisplay());
            tr.Controls.Add(td);

            table.Controls.Add(tr);

            // Header row to diplay Author and Title
            tr = new TableRow();

            // Authors
            th = new TableHeaderCell();
            th.Height = Unit.Pixel(25);
            th.CssClass = "tableHeaderText";
            th.Width = 100;
            
            th.HorizontalAlign = HorizontalAlign.Left;
            th.Text = "&nbsp;Author";
            tr.Controls.Add(th);

            // Messages
            th = new TableHeaderCell();
            th.CssClass = "tableHeaderText";
            th.Width = Unit.Percentage(85);
            th.HorizontalAlign = HorizontalAlign.Left;
            th.Text = "&nbsp;Thread: " + post.Subject;
            
            tr.Controls.Add(th);

            // Add header row to table
            table.Controls.Add(tr);

            return table;
        }

        // *********************************************************************
        //  HeaderDisplay
        //
        /// <summary>
        /// This function renders the display for items in the header
        /// </summary>
        /// 
        // ********************************************************************/
        private Table HeaderDisplay() {
            Table table = new Table();
            TableCell tdLeft = new TableCell();
            TableCell tdRight = new TableCell();
            TableRow tr = new TableRow();

            // Set up the table
            table.CellPadding = 0;
            table.CellSpacing = 0;
            table.Width = Unit.Percentage(100);

            // Set up the cells
            tdLeft.HorizontalAlign = HorizontalAlign.Left;
            tdRight.HorizontalAlign = HorizontalAlign.Right;
/*
TODO: Move email tracking into PostList
            // Email tracking
            CheckBox emailTracking = new CheckBox();
            emailTracking.CssClass = "normalTextSmallBold";
            emailTracking.Text = "Email me when someone replies to this post.";
            emailTracking.AutoPostBack = true;
            emailTracking.CheckedChanged += new System.EventHandler(ThreadTracking_CheckChanged);
            tdLeft.Controls.Add(emailTracking);
*/
            // Previous :: Next Display
            LinkButton prev = new LinkButton();
            LinkButton next = new LinkButton();
            prev.Text = "Previous Thread";
            prev.CssClass = "linkSmallBold";
            prev.Click += new System.EventHandler(Prev_Click);
            next.Text = "Next Thread";
            next.CssClass = "linkSmallBold";
            next.Click += new System.EventHandler(Next_Click);
            tdRight.Controls.Add(prev);
            tdRight.Controls.Add(new LiteralControl("&nbsp;<span class=\"normalTextSmallBold\">::</span>&nbsp;"));
            tdRight.Controls.Add(next);
            tdRight.Controls.Add(new LiteralControl("&nbsp;"));

            // Add table cells
            tr.Controls.Add(tdLeft);
            tr.Controls.Add(tdRight);
            table.Controls.Add(tr);

            return table;
        }

        // *********************************************************************
        //  Next_Click
        //
        /// <summary>
        /// Event raised to move to the next thread
        /// </summary>
        /// 
        // ********************************************************************/
        public void Next_Click(object sender, EventArgs e) {
            int nextThreadID = Threads.GetNextThreadID(postID);
            HttpContext.Current.Response.Redirect(Globals.UrlShowPost + nextThreadID);
            HttpContext.Current.Response.End();
        }

        // *********************************************************************
        //  Prev_Click
        //
        /// <summary>
        /// Event raised to move to the previous thread
        /// </summary>
        /// 
        // ********************************************************************/
        public void Prev_Click(object sender, EventArgs e) {
            int prevThreadID = Threads.GetPrevThreadID(postID);
            HttpContext.Current.Response.Redirect(Globals.UrlShowPost + prevThreadID);
            HttpContext.Current.Response.End();
        }

        // *********************************************************************
        //  ThreadTracking_Changed
        //
        /// <summary>
        /// Event raised when the user wants to enable or disable thread tracking
        /// </summary>
        /// 
        // ********************************************************************/
        public event System.EventHandler ThreadTracking_Changed;

        // *********************************************************************
        //  ThreadTracking_CheckChanged
        //
        /// <summary>
        /// Event raised when the thread tracking checkbox is clicked upon.
        /// </summary>
        /// 
        // ********************************************************************/
        private void ThreadTracking_CheckChanged(Object sender, EventArgs e) {

            if (null != ThreadTracking_Changed)
                ThreadTracking_Changed(sender, e);

        }

        // *********************************************************************
        //  RenderItemTemplate
        //
        /// <summary>
        /// This function renders the item template for flat display
        /// </summary>
        /// 
        // ********************************************************************/
        private Control RenderItemTemplate() {
            Table table = new Table();
            TableRow tr = new TableRow();
            TableCell authorCell = new TableCell();
            TableCell postCell = new TableCell();
            TableCell authorDetailsCell = new TableCell();
            TableCell authorContactCell = new TableCell();

            // Author Cell Details
            authorCell.VerticalAlign = VerticalAlign.Top;
            authorCell.CssClass = "forumRow";
            authorCell.Wrap = false;
            authorCell.Width = 150;
            authorCell.DataBinding += new System.EventHandler(HandleDataBindingForAuthorCell);

            // Post Cell Details
            postCell.VerticalAlign = VerticalAlign.Top;
            postCell.CssClass = "forumRow";
            postCell.DataBinding += new System.EventHandler(HandleDataBindingForPostCell);

            // Add controls to control tree
            tr.Controls.Add(authorCell);
            tr.Controls.Add(postCell);

            table.Controls.Add(tr);

            /*
             * TODO: Feature to display more details about post/author
            tr = new TableRow();

            // Author Details Cell
            authorDetailsCell.VerticalAlign = VerticalAlign.Top;
            authorDetailsCell.CssClass = "forumRow";
            authorDetailsCell.Wrap = false;
            authorDetailsCell.Width = 150;
            authorDetailsCell.DataBinding += new System.EventHandler(HandleDataBindingForAuthorDetails);

            // Author Contact Cell
            authorContactCell.VerticalAlign = VerticalAlign.Top;
            authorContactCell.CssClass = "forumRow";

            tr.Controls.Add(authorDetailsCell);
            tr.Controls.Add(authorContactCell);

            table.Controls.Add(tr);
            */
            return table;
        }

        // *********************************************************************
        //  RenderAlternatingItemTemplate
        //
        /// <summary>
        /// This function renders the item template for flat display
        /// </summary>
        /// 
        // ********************************************************************/
        private Control RenderAlternatingItemTemplate() {
            Table table = new Table();
            TableRow tr = new TableRow();
            TableCell authorCell = new TableCell();
            TableCell postCell = new TableCell();
            TableCell authorDetailsCell = new TableCell();
            TableCell authorContactCell = new TableCell();

            // Author Cell Details
            authorCell.VerticalAlign = VerticalAlign.Top;
            authorCell.CssClass = "forumAlternate";
            authorCell.Wrap = false;
            authorCell.DataBinding += new System.EventHandler(HandleDataBindingForAuthorCell);

            // Post Cell Details
            postCell.VerticalAlign = VerticalAlign.Top;
            postCell.CssClass = "forumAlternate";
            postCell.DataBinding += new System.EventHandler(HandleDataBindingForPostCell);

            // Add controls to control tree
            tr.Controls.Add(authorCell);
            tr.Controls.Add(postCell);

            table.Controls.Add(tr);
            /*
             * TODO: Feature to display more details about post/author

            tr = new TableRow();

            // Author Details Cell
            authorDetailsCell.VerticalAlign = VerticalAlign.Top;
            authorDetailsCell.CssClass = "forumRow";
            authorDetailsCell.Wrap = false;
            authorDetailsCell.Width = 150;
            authorDetailsCell.DataBinding += new System.EventHandler(HandleDataBindingForAuthorDetails);

            // Author Contact Cell
            authorContactCell.VerticalAlign = VerticalAlign.Top;
            authorContactCell.CssClass = "forumRow";

            tr.Controls.Add(authorDetailsCell);
            tr.Controls.Add(authorContactCell);

            table.Controls.Add(tr);
            */
            return table;

        }

        // *********************************************************************
        //  RenderFooterTemplate
        //
        /// <summary>
        /// This function renders the footer template
        /// </summary>
        /// 
        // ********************************************************************/
        private Control RenderFooterTemplate() {
            Table table = new Table();
            TableRow tr;
            TableCell td;

            // Titles row
            tr = new TableRow();

            // Messages
            td = new TableCell();
            td.ColumnSpan = 2;
            td.Controls.Add(HeaderDisplay());
            td.CssClass = "forumHeaderBackgroundAlternate";
            td.Height = 20;
            tr.Controls.Add(td);

            // Add header row to table
            table.Controls.Add(tr);

            return table;
        }
        
        // *********************************************************************
        //  BuildHeaderTemplate
        //
        /// <summary>
        /// This function is called to create the template for the header
        /// </summary>
        /// 
        // ********************************************************************/
        private void BuildHeaderTemplate(Control _ctrl) {

            System.Web.UI.IParserAccessor __parser = ((System.Web.UI.IParserAccessor)(_ctrl));

            __parser.AddParsedSubObject(RenderHeaderTemplate());
            
        }

 
        // *********************************************************************
        //  BuildItemTemplate
        //
        /// <summary>
        /// This function is called to create the template for items
        /// </summary>
        /// 
        // ********************************************************************/
        private void BuildItemTemplate(Control _ctrl) {

            System.Web.UI.IParserAccessor __parser = ((System.Web.UI.IParserAccessor)(_ctrl));
            __parser.AddParsedSubObject(RenderItemTemplate());
            
        }

        // *********************************************************************
        //  BuildSeparatorItemTemplate
        //
        /// <summary>
        /// This function is called to create the template for items
        /// </summary>
        /// 
        // ********************************************************************/
        private void BuildSeparatorTemplate(Control _ctrl) {
            Table table = new Table();
            TableRow tr = new TableRow();
            TableCell td = new TableCell();

            td.ColumnSpan = 2;
            td.Height = 2;
            td.CssClass = "flatViewSpacing";

            tr.Controls.Add(td);
            table.Controls.Add(tr);

            System.Web.UI.IParserAccessor __parser = ((System.Web.UI.IParserAccessor)(_ctrl));
            __parser.AddParsedSubObject(table);
        }

        // *********************************************************************
        //  BuildAlternatingItemTemplate
        //
        /// <summary>
        /// This function is called to create the template for items
        /// </summary>
        /// 
        // ********************************************************************/
        private void BuildAlternatingItemTemplate(Control _ctrl) {

            System.Web.UI.IParserAccessor __parser = ((System.Web.UI.IParserAccessor)(_ctrl));
            __parser.AddParsedSubObject(RenderAlternatingItemTemplate());
            
        }
        
        // *********************************************************************
        //  BuildFooterTemplate
        //
        /// <summary>
        /// This function is called to create the template for the header
        /// </summary>
        /// 
        // ********************************************************************/
        private void BuildFooterTemplate(Control _ctrl) {

            System.Web.UI.IParserAccessor __parser = ((System.Web.UI.IParserAccessor)(_ctrl));

            __parser.AddParsedSubObject(RenderFooterTemplate());
            
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
        private void ApplyTemplates() {
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
                keyForHeaderTemplate = Globals.Skin + "/Templates/PostList-Header.ascx";
                keyForItemTemplate = Globals.Skin + "/Templates/PostList-Item.ascx";
                keyForAlternatingItemTemplate = Globals.Skin + "/Templates/PostList-AlternatingItem.ascx";
                keyForFooterTemplate = Globals.Skin + "/Templates/PostList-Footer.ascx";

                // Set the file paths to where the templates should be found
                pathToHeaderTemplate = Globals.ApplicationVRoot + "/Skins/" + keyForHeaderTemplate;
                pathToItemTemplate = Globals.ApplicationVRoot + "/skins/" + keyForItemTemplate;
                pathToAlternatingItemTemplate = Globals.ApplicationVRoot + "/skins/" + keyForAlternatingItemTemplate;
                pathToFooterTemplate = Globals.ApplicationVRoot + "/skins/" + keyForFooterTemplate;

                // Attempt to get the skinned header template
                if (HeaderTemplate == null)
                    HeaderTemplate = Globals.LoadSkinnedTemplate(pathToHeaderTemplate, keyForHeaderTemplate, Page);

                // Attempt to get the skinned item template
                if (ItemTemplate == null)
                    ItemTemplate = Globals.LoadSkinnedTemplate(pathToItemTemplate,keyForItemTemplate, Page);

                // Attempt to get the skinned alternating item template
                if (AlternatingItemTemplate == null)
                    AlternatingItemTemplate = Globals.LoadSkinnedTemplate(pathToAlternatingItemTemplate,keyForAlternatingItemTemplate, Page);

                // Attempt to get the footer template
                if (FooterTemplate == null)
                    FooterTemplate = Globals.LoadSkinnedTemplate(pathToFooterTemplate, keyForFooterTemplate, Page);
            }

            // Are any templates specified yet?
            if (ItemTemplate == null) {
                // Looks like we're using custom templates
                ExtractTemplateRows = true;

                HeaderTemplate = new CompiledTemplateBuilder(new BuildTemplateMethod(BuildHeaderTemplate));
                ItemTemplate = new CompiledTemplateBuilder(new BuildTemplateMethod(BuildItemTemplate));
                //TODO: BUGBUG with DATALIST? SeparatorTemplate = new CompiledTemplateBuilder(new BuildTemplateMethod(BuildSeparatorTemplate));
                AlternatingItemTemplate = new CompiledTemplateBuilder(new BuildTemplateMethod(BuildAlternatingItemTemplate));
                FooterTemplate = new CompiledTemplateBuilder(new BuildTemplateMethod(BuildFooterTemplate));
            }

        }


    }
}