using System;
using System.Drawing;
using System.Collections;
using System.Collections.Specialized;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using AspNetForums;
using AspNetForums.Components;
using System.ComponentModel;

namespace AspNetForums.Controls {


    /******************************* SearchPagerStyle Web control ****************************
     * 
     * SUMMARY:
     *		This class extends the TableItemStyle class and defines how the Search Pager
     *		should look and operate.  The Search Pager is what allows the user to navigate
     *		through multiple pages of search results.
     *
     ******************************************************************************************/
    /// <summary>
    /// This class extends the TableItemStyle class and defines how the Search Pager
    /// should look and operate.  The Search Pager is what allows the user to navigate
    /// through multiple pages of search results.
    /// </summary>
	
    [ ToolboxItemAttribute(false) ]
    public class SearchPagerStyle : TableItemStyle {
        /********** DECLARE PRIVATE CONSTANTS **************/
        const String _defaultNextPageText = "Next";
        const String _defaultPrevPageText = "Prev";
        const int _defaultPageButtonCount = 10;
        /***************************************************/


        /************ PROPERTY SET/GET STATEMENTS **************/
        /// <summary>
        /// This property indicates whether or not the pager is shown.
        /// </summary>
        public bool Visible {
            get {
                if (ViewState["spsVisible"] == null) 
                    return true;
                return (bool) ViewState["spsVisible"];
            }
            set { ViewState["spsVisible"] = value; }
        }

        /// <summary>
        /// Specifies the pager's mode.  The options are NextPrev (the default) or NumericPages.
        /// </summary>
        public PagerMode Mode {
            get {
                if (ViewState["spsMode"] == null) 
                    return PagerMode.NextPrev;
                return (PagerMode) ViewState["spsMode"];
            }
            set { ViewState["spsMode"] = value; }
        }

        /// <summary>
        /// Specifies the text for the next page link when the Mode property is set to NextPrev.
        /// <seealso cref="Mode"/>
        /// </summary>
        public String NextPageText {
            get {
                if (ViewState["spsNextPageText"] == null) return _defaultNextPageText;
                return (String) ViewState["spsNextPageText"];
            }
            set { ViewState["spsNextPageText"] = value; }
        }

        /// <summary>
        /// Specifies the text for the previous page link when the Mode property is set to NextPrev.
        /// <seealso cref="Mode"/>
        /// </summary>
        public String PrevPageText {
            get {
                if (ViewState["spsPrevPageText"] == null) return _defaultPrevPageText;
                return (String) ViewState["spsPrevPageText"];
            }
            set { ViewState["spsPrevPageText"] = value; }
        }

        /// <summary>
        /// Specifies the position of the pager.  The options are Top, Bottom, and TopAndBottom.
        /// </summary>
        public PagerPosition Position {
            get {
                if (ViewState["spsPosition"] == null) 
                    return PagerPosition.TopAndBottom;
                return (PagerPosition) ViewState["spsPosition"];
            }
            set { ViewState["spsPosition"] = value; }
        }

        /// <summary>
        /// When the Mode property is set to NumericPages, this property specifies how many pages
        /// should be shown in the list of pages to navigate to.
        /// <seealso cref="Mode"/>
        /// </summary>
        public int PageButtonCount {
            get {
                if (ViewState["spsPageButtonCount"] == null) return _defaultPageButtonCount;
                return (int) ViewState["spsPageButtonCount"];
            }
            set { ViewState["spsPageButtonCount"] = value; }
        }
        /*******************************************************/
    }

	
    /********************* DeletePostStyle Class ********************
        This class extends the base Style class and provides stylistic and
        advanced settings for the Delete Post panel.  Not only can the developer
        define stylistic settings here, but also specify the textual messages
        to appear in the Delete Post confirmation panel.
     *********************************************************************/
    /// <summary>
    /// This class extends the base Style class and provides stylistic and
    /// advanced settings for the Delete Post panel.  Not only can the developer
    /// define stylistic settings here, but also specify the textual messages
    /// to appear in the Delete Post confirmation panel.
    /// </summary>
	
    [ ToolboxItemAttribute(false) ]
    public class DeletePostStyle : Style {
        /********** DECLARE PRIVATE CONSTANTS **************/
        const String _defaultDeletePostText = "Are you <b>sure</b> you want to <b>permanently delete</b> this post?  Once you confirm, you will <b>not</b> be able to un-delete the post!";			// the default delete post message
        const String _defaultReasonsTextBoxMessage = "If you do decide to delete the post, please provide the reason why in the textbox below:";
        const bool _defaultShowReasonsTextBox = true;			// whether or not to display the "Reasons for Deleting the Post" textbox
        const int _defaultReasonsTextBoxColumns = 40;			// how many columns to show in the text box
        const int _defaultReasonsTextBoxRows = 8;				// how many rows to show in the text box
        /***************************************************/


        /************ PROPERTY SET/GET STATEMENTS **************/
        /// <summary>
        /// If the ShowReasonsForDeletingTextBox property is set to true, a text box is displayed in 
        /// the Delete Post confirmation panel for the moderator to enter a reason why the post
        /// is being deleted.  This property, ReasonsTextBoxMessage, specifies the textual message 
        /// that should appear before the text box.
        /// <seealso cref="ShowReasonsForDeletingTextBox"/>
        /// </summary>
        public String ReasonsTextBoxMessage {
            get {
                if (ViewState["deletePostStyleTextBoxMsg"] == null) return _defaultReasonsTextBoxMessage;
                return (String) ViewState["deletePostStyleTextBoxMsg"];
            }
            set {  ViewState["deletePostStyleTextBoxMsg"] = value;  }
        }

        /// <summary>
        /// Indicates the textual message that should appear on the Delete Post confirmation page, explaining
        /// to the user the full ramifications of their actions.
        /// </summary>
        public String DeletePostText {
            get {
                if (ViewState["deletePostStyleDeletePostText"] == null) return _defaultDeletePostText;
                return (String) ViewState["deletePostStyleDeletePostText"];
            }
            set {  ViewState["deletePostStyleDeletePostText"] = value;  }
        }

        /// <summary>
        /// This property determines whether or not a text box should be displayed for the moderator to
        /// explain why the post is being deleted.
        /// </summary>
        public bool ShowReasonsForDeletingTextBox {
            get {
                if (ViewState["deletePostStyleShowTextBox"] == null) return _defaultShowReasonsTextBox;
                return (bool) ViewState["deletePostStyleShowTextBox"];
            }
            set { ViewState["deletePostStyleShowTextBox"] = value; }
        }

        /// <summary>
        /// Determines how many columns the Reasons For Deleting the Post text box should contain.
        /// The default is 40.
        /// </summary>
        public int ReasonsTextBoxColumns {
            get {
                if (ViewState["deletePostStyleTextBoxCols"] == null) return _defaultReasonsTextBoxColumns;
                return (int) ViewState["deletePostStyleTextBoxCols"];
            }
            set { ViewState["deletePostStyleTextBoxCols"] = value; }
        }

        /// <summary>
        /// Determines how many rows the Reasons For Deleting the Post text box should contain.
        /// The default is 8.
        /// </summary>
        public int ReasonsTextBoxRows {
            get {
                if (ViewState["deletePostStyleTextBoxRows"] == null) return _defaultReasonsTextBoxRows;
                return (int) ViewState["deletePostStyleTextBoxRows"];
            }
            set { ViewState["deletePostStyleTextBoxRows"] = value; }
        }
        /********************************************************/
    }


    /*********************** ReplyToPostStyle Class ***********************
            This class represents the style and miscellaneous options for the pane
            that displays the post being replied to.
         *********************************************************************/
    /// <summary>
    /// This class represents the style and miscellaneous options for the pane
    /// that displays the post being replied to.
    /// </summary>
	
    [ ToolboxItemAttribute(false) ]
    public class ReplyToPostStyle : Style {
        /********** DECLARE PRIVATE CONSTANTS **************/
        const int _defaultCellPadding = 3;						// the default cell padding for the reply-to table
        const int _defaultCellSpacing = 0;						// the default cell spacing for the reply-to table
        /***************************************************/

        /************ PROPERTY SET/GET STATEMENTS **************/
        /// <summary>
        /// Indicates how the reply-to panel will be horizontally aligned.  The default is to
        /// have the pane Center aligned.
        /// </summary>
        public HorizontalAlign HorizontalAlign {
            get {
                if (ViewState["previewPostStyleHZ"] == null) return HorizontalAlign.Center;
                return (HorizontalAlign) ViewState["previewPostStyleHZ"];
            }
            set {  ViewState["previewPostStyleHZ"] = value;  }
        }

        /// <summary>
        /// Specifies the cellpadding for the HTML table; the default is 3 pixels.
        /// </summary>
        public int CellPadding {
            get {
                if (ViewState["previewPostStyleCellPadding"] == null) return _defaultCellPadding;
                return (int) ViewState["previewPostStyleCellPadding"];
            }
            set {  ViewState["previewPostStyleCellPadding"] = value;  }
        }

        /// <summary>
        /// Specifies the cellspacing for the HTML table; the default is zero pixels..
        /// </summary>
        public int CellSpacing {
            get {
                if (ViewState["previewPostStyleCellSpacing"] == null) return _defaultCellSpacing;
                return (int) ViewState["previewPostStyleCellSpacing"];
            }
            set {  ViewState["previewPostStyleCellSpacing"] = value;  }
        }
        /********************************************************/
    }

    /********************* PostNewMessageStyle Class **********************
        This class represents the style and text for the Post a New Message
        table in the upper right hand corner.  It is utilized in the ForumView
        control via the _postNewMessageStyle member variable.
     *********************************************************************/
    /// <summary>
    /// This class defines the style for the PostNewMessage label Web control.
    /// This class extends the base Style class to include a Text property, which
    /// specifies the text message to display in the PostNewMessage label (defaults
    /// to a value of "Post a New Message").
    /// <seealso cref="PostNewMessageLabel"/>
    /// <seealso cref="Text"/>
    /// <seealso cref="ShowPostNewMessage"/>
    /// </summary>
    /// <remarks>This Post a New Message label is only displayed if the
    /// ShowPostNewMessage property of the ForumView class is set to true.</remarks>
	
    [ ToolboxItemAttribute(false) ]
    public class PostNewMessageStyle : Style {
        /********** DECLARE PRIVATE CONSTANTS **************/
        const String _defaultText = "Post a New Message";			// the default text message
        /***************************************************/
	
		
        /************ PROPERTY SET/GET STATEMENTS **************/
        /// <summary>
        /// Specifies the text label to use when displaying the Post a New Message
        /// label.
        /// </summary>
        public String Text {
            get {
                if (ViewState["postNewMessageStyleText"] == null) return _defaultText;
                return (String) ViewState["postNewMessageStyleText"];
            }
            set {  ViewState["postNewMessageStyleText"] = value;  }
        }
        /********************************************************/
    }
    /**********************************************************************/





    /********************* NextPrevMessagesStyle Class ********************
        This class represents the style and text for the next and previous
        messages links.  This class allows the user to customize the placement
        and look and feel of these options
     *********************************************************************/
    /// <summary>
    /// This class defines the style for the Next and Previous page links.
    /// This class extends the base Style class, providing four additional properties:
    /// Position, HorizontalAlign, UpArrowImageUrl, DownArrowImageUrl.  These properties
    /// allow you to specify the position and UI elements of the links for the next and previous
    /// pages.
    /// <seealso cref="Position"/>
    /// <seealso cref="UpArrowImageUrl"/>
    /// <seealso cref="DownArrowImageUrl"/>
    /// <seealso cref="HorizontalAlign"/>
    /// </summary>
	
    [ ToolboxItemAttribute(false) ]
    public class NextPrevMessagesStyle : Style {
        /********** DECLARE PRIVATE CONSTANTS **************/
        const NextPrevMessagesPosition _defaultPosition = NextPrevMessagesPosition.Bottom;
        const String _defaultUpArrow = "up.gif";					// the default filename of the up arrow
        const String _defaultDownArrow = "dn.gif";					// the default filename of the down arrow
        /***************************************************/

		
        /************ PROPERTY SET/GET STATEMENTS **************/
        /// <summary>
        /// Indicates the position the next/previous page links should be replaced.  Available options
        /// must be members of the NextPrevMessagesPosition enumeration, and are: Top, Bottom, or Both.
        /// The default is Bottom.
        /// <seealso cref="NextPrevMessagesPosition"/>
        /// </summary>		
        public NextPrevMessagesPosition Position {
            get {
                if (ViewState["nextPrevPosition"] == null) return _defaultPosition;
                return (NextPrevMessagesPosition) ViewState["nextPrevPosition"];
            }
            set {  ViewState["nextPrevPosition"] = value;  }
        }

        /// <summary>
        /// Indicates the Url for the image of an up-arrow, to display next to the link that
        /// shows posts in the future.
        /// </summary>
        /// <remarks>This setting should specify the image's Url path relative to the imagesPath
        /// setting in Web.config.</remarks>
        public String UpArrowImageUrl {
            get {
                return _defaultUpArrow;
            }
            set { ViewState["nextprevMessagesStyleUpArrowUrl"] = value; }
        }

        /// <summary>
        /// Indicates the Url for the image of a down-arrow, to display next to the link that
        /// shows past posts.
        /// </summary>
        /// <remarks>This setting should specify the image's Url path relative to the imagesPath
        /// setting in Web.config.</remarks>
        public String DownArrowImageUrl {
            get {
                return (String) ViewState["nextprevMessagesStyleDownArrowUrl"];
            }
            set { ViewState["nextprevMessagesStyleDownArrowUrl"] = value; }
        }

        /// <summary>
        /// Specifies how the next/previous page links should be horizontally aligned.  The
        /// default is Right aligned.
        /// </summary>
        public HorizontalAlign HorizontalAlign {
            get {
                if (ViewState["nextprevMessagesStyleAlign"] == null) return HorizontalAlign.Right;
                return (HorizontalAlign) ViewState["nextprevMessagesStyleAlign"];
            }
            set { ViewState["nextprevMessagesStyleAlign"] = value; }
        }
        /********************************************************/
    }
    /**********************************************************************/
}