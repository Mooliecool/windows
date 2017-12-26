using System;
using System.Collections;
using System.Collections.Specialized;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using AspNetForums;
using AspNetForums.Components;
using System.ComponentModel;

namespace AspNetForums.Controls {

    /// <summary>
    /// This Web control displays a thread in a threaded display.  The developer may pass in
    /// either a PostID or a ThreadID.  If a PostID is passed in, the thread that that Post belongs
    /// to is constructed.
    /// </summary>
    [
    ParseChildren(true)
    ]
    public class PostThreaded : WebControl, INamingContainer {

        // Class constants
        const String defaultPostDateTimeFormat = "ddd MMM dd, yyyy hh:mm tt";

        // Member variable used to display threads
        Repeater threadedRepeater;
        ITemplate threadItemTemplate;
        ITemplate threadBelowThreshHoldItemTemplate;
        ITemplate bodyTemplate;

        // Member variables used to keep track of where we are in the thread
        int threadedStartPostLevel = 1;
        int threshHold = 1;
        User postUser;

        // *********************************************************************
        //  ThreadView
        //
        /// <summary>
        /// Class constructor. We'll attempt to get the PostID or ThreadID
        /// </summary>
        /// 
        // ********************************************************************/
        public PostThreaded() {

            // If we have an instance of context, let's attempt to
            // get the PostID so we can save the user from writing
            // the code
            try {
                if (null != Context.Request.QueryString["PostID"]) {
                    string postID = Context.Request.QueryString["PostID"];

                    // Contains a #
                    if (postID.IndexOf("#") > 0)
                        postID = postID.Substring(0, postID.IndexOf("#"));

                    this.PostID = Convert.ToInt32(postID);
                } else if (null != Context.Request.Form["PostId"]) {
                    this.PostID = Convert.ToInt32(Context.Request.Form["PostID"]);
                }
            } catch (Exception) {
                HttpContext.Current.Response.Redirect(Globals.UrlMessage + Convert.ToInt32(Messages.PostDoesNotExist));
                HttpContext.Current.Response.End();
            }

        }


        // *********************************************************************
        //  BuildItemTemplate
        //
        /// <summary>
        /// Used to create the ItemTemplate value if a template is not provided.
        /// </summary>
        /// 
        // ********************************************************************/
        private void BuildItemTemplate(Control _ctrl) {

            System.Web.UI.IParserAccessor __parser = ((System.Web.UI.IParserAccessor)(_ctrl));
            __parser.AddParsedSubObject(RenderItemTemplate());
            
        }

        // *********************************************************************
        //  RenderItemTemplate
        //
        /// <summary>
        /// Creates, DataBinds, and returns a table Control. This table control
        /// is where the body of the thread is injected. We DataBind the table since
        /// multiple rows may be added depending on the format chosen.
        /// </summary>
        /// 
        // ********************************************************************/
        private Control RenderItemTemplate() {

            Table table = new Table();

            table.CellPadding = 2;
            table.CellSpacing = 0;
            table.Width = Unit.Percentage(100);
            table.DataBinding += new System.EventHandler(DataBindThread);

            return table;
        }

        // *********************************************************************
        //  DataBindThread
        //
        /// <summary>
        /// Performs the DataBinding for each thread item bound to the repeater.
        /// </summary>
        /// 
        // ********************************************************************/
        private void DataBindThread(Object sender, EventArgs e) {
            Table table;
            TableRow tr;
            TableCell td;
            RepeaterItem container;
            Post post;
            bool isBelowThreshHold = false;
            bool isFirstPost = false;

            // Get the table instance we databound
            table = (Table) sender;
            
            // Get the data item to be bound to
            container = (RepeaterItem) table.NamingContainer;

            // Extract the original databound class type
            post = (Post) container.DataItem;

            // If we're the first item, set the threadedStartPostLevel.
            // This private member variable determines what nesting level
            // to start at.
            if (container.ItemIndex == 0) {
                threadedStartPostLevel = post.PostLevel;
                isFirstPost = true;
            }

            // Determine if we are below the threshold of threads to display
            // with body.
            if ((post.PostLevel - threadedStartPostLevel) > Threshhold)
                isBelowThreshHold = true;

            // This whitespace is used to separate thread items
            if ((!isBelowThreshHold) && (!isFirstPost)) {
                // Add some white space
                tr = new TableRow();
                td = new TableCell();
                td.ColumnSpan = 2;
                td.Height = Unit.Pixel(12);
                tr.Controls.Add(td);
                table.Controls.Add(tr);
            }

            // Prepare to display the thread
            tr = new TableRow();

            // If we're not the first post, indent
            if (!isFirstPost) {
                // Indenting
                td = new TableCell();
                td.Wrap = false;
                td.VerticalAlign = VerticalAlign.Top;
                Label indent = new Label();
                indent.CssClass = "normalTextSmallBold";
                indent.DataBinding += new System.EventHandler(HandleIndentingForThreaded);
                td.Controls.Add(indent);
                tr.Controls.Add(td);
            }

            // Databind this cell for the actual thread display
            td = new TableCell();
            td.DataBinding += new System.EventHandler(HandleDataBindingForThreadTitle);

            if (isFirstPost) {
                td.ColumnSpan = 2;
            } else {
                td.Width = Unit.Percentage(100);
            }

            tr.Controls.Add(td);
            table.Controls.Add(tr);

            // Display body of post
            if (!isBelowThreshHold) {
                td = new TableCell();
                tr = new TableRow();

                // Set some properties on the row
                td.CssClass = "forumRow";

                // If we're not the first item, let's add a bit of whitespace
                if (isFirstPost) {
                    td.ColumnSpan = 2;
                } else {
                    tr.Controls.Add(new TableCell());
                }

                // Template?
                if (null != this.BodyTemplate) {

                    BodyTemplate.InstantiateIn(td);
                    tr.Controls.Add(td);
                    table.Controls.Add(tr);
                    return;
                }

                Label body = new Label();

                // Set up the body text
                body.CssClass = "normalTextSmall";
                body.Text = Globals.FormatPostBody(post.Body);

                postUser = Users.GetUserInfo(post.Username, false);

                if (postUser.Signature != "") {
                    body.Text += Globals.FormatSignature(postUser.Signature);
                }


                // Add the body to the cell
                td.Controls.Add(body);

                tr.Controls.Add(td);
                table.Controls.Add(tr);

            }

        }

        // *********************************************************************
        //  HandleDataBindingForThreadDisplay
        //
        /// <summary>
        /// Perform databinding and control the display of a given thread.
        /// </summary>
        /// 
        // ********************************************************************/
        private void HandleDataBindingForThreadTitle (Object sender, EventArgs e) {
            TableCell td;
            RepeaterItem container;
            Post post;
            bool isBelowThreshHold = false;

            // Bound to type TableCell
            td = (TableCell) sender;

            // Get the repeater item containing the bound data
            container = (RepeaterItem) td.NamingContainer;

            // Get the original class type we're bound to
            post = (Post) container.DataItem;

            // Determine if we are below the threshold of threads to display
            // with body.
            if ((post.PostLevel - threadedStartPostLevel) > Threshhold)
                isBelowThreshHold = true;

            // Format differently based on the threshold to view nested posts
            if (isBelowThreshHold) {

                // Do we have a template
                if (this.ThreadBelowThreshHoldItemTemplate != null){
                    ThreadBelowThreshHoldItemTemplate.InstantiateIn(td);
                    return;
                }

                // Label for display
                Label label = new Label();

                // Add bullet
                LiteralControl bullet = new LiteralControl();
                bullet.Text = "<img src=\"" + Globals.ApplicationVRoot + "/skins/" + Globals.Skin + "/images/node.gif" + "\">";
                label.Controls.Add(bullet);

                // Subject
                HyperLink subject = new HyperLink();
                subject.Text = post.Subject;
                subject.CssClass = "linkSmallBold";
                subject.NavigateUrl = Globals.UrlShowPost + post.PostID + "&View=Threaded&ThreshHold=" + (Convert.ToInt32(post.PostLevel) + 1) + "#" + post.PostID;
                label.Controls.Add(subject);

                // Whitespace
                label.Controls.Add(new LiteralControl(" "));

                // Author
                HyperLink author = new HyperLink();
                author.CssClass = "linkSmallBold";
                author.Text = post.Username;
                author.NavigateUrl = Globals.UrlUserProfile + post.Username;
                label.Controls.Add(author);

                // Whitespace
                label.Controls.Add(new LiteralControl(" "));
            
                // Special formatting for the date
                Label datePosted = new Label();
                datePosted.CssClass = "normalTextSmall";
                if (Page.Request.IsAuthenticated) {
                    datePosted.Text = Users.AdjustForTimezone(post.PostDate, ((User) Users.GetUserInfo(Context.User.Identity.Name, true)).Timezone, PostDateTimeFormat);
                } else {
                    datePosted.Text = "Posted: " + post.PostDate.ToString(PostDateTimeFormat);
                }
                label.Controls.Add(datePosted);

                td.Controls.Add(label);

            } else {
                // Set the style on the cell we render in
                td.CssClass = "threadTitle";
                td.Controls.Add(RenderDetailedThreadTitle(post));

            }

        }

        // *********************************************************************
        //  RenderDetailedThreadTitle
        //
        /// <summary>
        /// Renders the thread title used when a thread body is displayed
        /// </summary>
        /// 
        // ********************************************************************/
        private Control RenderDetailedThreadTitle(Post post) {
            Table table;
            TableRow tr;
            TableCell td;
            
            // Create the table
            table = new Table();
            table.CellPadding = 0;
            table.CellSpacing = 2;

            // Get the user that created the post
            postUser = Users.GetUserInfo(post.Username, false);

            // Create the row
            tr = new TableRow();

            // Create the cell where the actual data goes
            td = new TableCell();
            td.VerticalAlign = VerticalAlign.Top;

            // Add the location anchor
            td.Controls.Add(new LiteralControl("<a name=\"" + post.PostID + "\"/>"));

            // Display the subject
            Label label = new Label();
            label.CssClass = "normalTextSmallBold";
            label.Text = post.Subject;
            td.Controls.Add(label);

            // Line Break
            td.Controls.Add(new LiteralControl("<br>"));

            // Label for data under subject
            label = new Label();
            label.CssClass = "normalTextSmall";

            // Whitespace
            label.Controls.Add(new LiteralControl("Posted by: "));

            // Author
            HyperLink author = new HyperLink();
            author.Text = post.Username;
            author.NavigateUrl = Globals.UrlUserProfile + post.Username;
            label.Controls.Add(author);

            // Whitespace
            label.Controls.Add(new LiteralControl("&nbsp;"));
            
            // Date
            Label datePosted = new Label();
            if (Page.Request.IsAuthenticated) {
                datePosted.Text = Users.AdjustForTimezone(post.PostDate, ((User) Users.GetUserInfo(Context.User.Identity.Name, true)).Timezone, PostDateTimeFormat);
            } else {
                datePosted.Text = "Posted: " + post.PostDate.ToString(PostDateTimeFormat);
            }
            label.Controls.Add(datePosted);

            // Add existing line and line break
            td.Controls.Add(label);
            td.Controls.Add(new LiteralControl("<br>"));

            // Add Reply Link
            label = new Label();
            label.CssClass = "threadDetailTextSmall";
            HyperLink postReply = new HyperLink();
            postReply.Text = "Reply";
            postReply.NavigateUrl = Globals.UrlReplyToPost + post.PostID;
            label.Controls.Add(postReply);
/*
            // Whitespace
            label.Controls.Add(new LiteralControl("&nbsp; | &nbsp;"));

            // Add new thread Link
            HyperLink newThread = new HyperLink();
            newThread.Text = "New Thread";
            newThread.NavigateUrl = Globals.UrlAddNewPost + post.ForumID;
            label.Controls.Add(newThread);
*/
            // Only add the Parent link if we have a parent
            if (post.ParentID != post.PostID) {
                // Whitespace
                label.Controls.Add(new LiteralControl("&nbsp; | &nbsp;"));

                HyperLink parentPost = new HyperLink();
                parentPost.Text = "Parent";
                parentPost.NavigateUrl = Globals.UrlShowPost + post.PostID + "&View=Threaded&ThreshHold=" + (Convert.ToInt32(post.PostLevel) + 1) + "#" + post.ParentID;
                label.Controls.Add(parentPost);
            }

            td.Controls.Add(label);

            tr.Controls.Add(td);
            table.Controls.Add(tr);

            return table;
        }

        // *********************************************************************
        //  HandleIndentingForThreaded
        //
        /// <summary>
        /// Used to indent thread items based on the nested depth of the thread 
        /// relative to its parent.
        /// </summary>
        /// 
        // ********************************************************************/
        public void HandleIndentingForThreaded(Object sender, EventArgs e) {
            Label indent;
            RepeaterItem container;
            Post post;

            // Bound to type Label
            indent = (Label) sender;

            // Get the RepeaterItem containing the data
            container = (RepeaterItem) indent.NamingContainer;

            // Original class type we're bound to
            post = (Post) container.DataItem;

            // If we're the first item don't indent
            if (container.ItemIndex == 0) {
                return;
            }

            // Perform nesting. This gets written within a TableCell
            for (int i = threadedStartPostLevel; i < post.PostLevel; i++ ) {
                indent.Text += " &nbsp; &nbsp; &nbsp;";
            }

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
        private void ApplyTemplates(Repeater repeater) {
            string pathToHeaderTemplate;
            string pathToItemTemplate;
            string pathToFooterTemplate;
            string keyForHeaderTemplate;
            string keyForItemTemplate;
            string keyForFooterTemplate;

            // Are we using skinned template?
            if (Page != null) {

                // Set the file paths to where the templates should be found
                keyForHeaderTemplate = Globals.Skin + "/Templates/ThreadView-Header.ascx";
                keyForItemTemplate = Globals.Skin + "/Templates/ThreadView-Item.ascx";
                keyForFooterTemplate = Globals.Skin + "/Templates/ThreadView-Footer.ascx";

                // Set the file paths to where the templates should be found
                pathToHeaderTemplate = Globals.ApplicationVRoot + "/Skins/" + keyForHeaderTemplate;
                pathToItemTemplate = Globals.ApplicationVRoot + "/skins/" + keyForItemTemplate;
                pathToFooterTemplate = Globals.ApplicationVRoot + "/skins/" + Globals.Skin + keyForFooterTemplate;

                // Attempt to get the skinned header template
                if (repeater.HeaderTemplate == null)
                    repeater.HeaderTemplate = Globals.LoadSkinnedTemplate(pathToHeaderTemplate, keyForHeaderTemplate, Page);

                // Attempt to get the skinned item template
                if (repeater.ItemTemplate == null)
                    repeater.ItemTemplate = Globals.LoadSkinnedTemplate(pathToItemTemplate,keyForItemTemplate, Page);

                // Attempt to get the footer template
                if (repeater.FooterTemplate == null)
                    repeater.FooterTemplate = Globals.LoadSkinnedTemplate(pathToFooterTemplate, keyForFooterTemplate, Page);
            }

            // Item template
            if (repeater.ItemTemplate == null)
                repeater.ItemTemplate = new CompiledTemplateBuilder(new BuildTemplateMethod(BuildItemTemplate));

        }

        // *********************************************************************
        //  CreateChildControls
        //
        /// <summary>
        /// Override CreateChildControls
        /// </summary>
        /// 
        // ********************************************************************/
        protected override void CreateChildControls() {

            // Make sure a postID was specified
            if (PostID == -1)
                throw new Exception("Attempted to create the ThreadDiaplay control without passing in either a PostID.");

            // Create the threadedRepeater Repeater
            threadedRepeater = new Repeater();

            // Apply templates
            ApplyTemplates(threadedRepeater);

            // We're threaded so we only want to show children of the PostID
//            threadedRepeater.DataSource = GetChildrenByPostID();
            threadedRepeater.DataSource = Posts.GetThreadByPostID(PostID);

            // DataBind
            threadedRepeater.DataBind();

            this.Controls.Add(threadedRepeater);
        }		

        private PostCollection GetChildrenByPostID() {

            PostCollection posts;
            PostCollection threaded = new PostCollection();
            Stack threadStack = new Stack();

            // Get posts for the post id
            posts = Posts.GetThreadByPostID(PostID);

            // Push the PostID on to the stack
            threadStack.Push(PostID);


            foreach (Post post in posts) {

                if (post.PostID != (int) threadStack.Peek()) {

                    if (threadStack.Contains((int)post.ParentID)) {
                        threadStack.Push(post.PostID);
                        threaded.Add(post);
                    }

                } else {
                    threaded.Add(post);
                }


            }

            return threaded;


        }
		
        // *********************************************************************
        //  PostID
        //
        /// <summary>
        /// Specifies the ID of the Post whose thread you wish to view.
        /// </summary>
        /// <remarks>If this property is not set and the ThreadID property is not set, an Exception is 
        /// thrown.  If this property is set, the RightArrowImageUrl will be displayed for the appropriate
        /// Post in the thread.
        /// </remarks>
        /// 
        // ********************************************************************/
        public int PostID {
            get {
                if (ViewState["postID"] == null) return -1;
                return (int) ViewState["postID"];
            }
            set {
                if (value < 0)
                    ViewState["postID"] = 0;
                else
                    ViewState["postID"] = value;
            }
        }


        // *********************************************************************
        //  PostDateTimeFormat
        //
        /// <summary>
        /// Specifies the date/time format to display the post date in.
        /// The string should contain valid DateTimeFormatInfo characters.  For example, to display the
        /// date range as MM/DD/YYYY - HH:MM AM/PM, use the string: MM/dd/yyyy - HH:mm tt
        /// </summary>
        /// 
        // ********************************************************************/
        [
        Category("Style"),
        Description("Specifies the date/time format to display the post date in.")
        ]
        public String PostDateTimeFormat {
            get {
                if (ViewState["postDateTimeFormat"] == null) return defaultPostDateTimeFormat;
                return (String) ViewState["postDateTimeFormat"];
            }
            set { ViewState["postDateTimeFormat"] = value; }
        }

        // *********************************************************************
        //  Threshhold
        //
        /// <summary>
        /// Controls the depth at which the body of the thread is no longer displayed
        /// </summary>
        /// 
        // ********************************************************************/
        [
        Description("Controls the depth at which the body of the thread is no longer displayed.")
        ]
        public int Threshhold {
            get { return this.threshHold; }
            set { threshHold = value; }
        }

        // *********************************************************************
        //  ThreadItemTemplate
        //
        /// <summary>
        /// Allows user to define the template for the thread title when the body
        /// is shown
        /// </summary>
        /// 
        // ********************************************************************/
        [TemplateContainer(typeof(RepeaterItem))]
        public ITemplate ThreadItemTemplate {
            get {  return threadItemTemplate;  }
            set {  threadItemTemplate = value;  }		
        }

        // *********************************************************************
        //  ThreadBelowThreshHoldItemTemplate
        //
        /// <summary>
        /// Allows user to define the template used to render thread titles
        /// below the defined threshold (body of thread is not shown)
        /// </summary>
        /// 
        // ********************************************************************/
        [TemplateContainer(typeof(RepeaterItem))]
        public ITemplate ThreadBelowThreshHoldItemTemplate {
            get {  return threadBelowThreshHoldItemTemplate;  }
            set {  threadBelowThreshHoldItemTemplate = value;  }		
        }

        // *********************************************************************
        //  BodyTemplate
        //
        /// <summary>
        /// Template used to show body of thread
        /// </summary>
        /// 
        // ********************************************************************/
        [TemplateContainer(typeof(RepeaterItem))]
        public ITemplate BodyTemplate {
            get {  return bodyTemplate;  }
            set {  bodyTemplate = value;  }		
        }
    }
}