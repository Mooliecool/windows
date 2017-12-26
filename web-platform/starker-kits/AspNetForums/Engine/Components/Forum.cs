using System;

namespace AspNetForums.Components {
    /// <summary>
    /// This class defines the properties that makeup a forum.
    /// </summary>
    public class Forum : IComparable {

        // Member Variables
        int forumID = 0;				// Unique forum identifier
        int parentId = 0;
        int daysToView = 0;			    // Number of days to view for the forum
        int totalPosts = -1;			// Total posts in the forum
        int totalThreads = -1;			// Total threads in the forum
        int forumGroupId = -1;          // Identifier for the forum group this forum belongs to
        int sortOrder = 0;              // Used to control sorting of forums
        String name = "";				// Name of the forum
        String description = "";		// Description of the forum
        bool isModerated;				// Is the forum isModerated?
        bool isActive = false;          // Is the forum isActive?
        bool isPrivate = false;         // Is the forum private?
        DateTime mostRecentPostDate = DateTime.MinValue.AddMonths(1);	        // The date of the most recent post to the forum
        String mostRecentUser = "";		// The author of the most recent post to the forum
        int mostRecentPostId = 0;       // the most recent post id
        int mostRecentThreadId = 0;     // Post ID of the most recent thread
        DateTime dateCreated;			// The date/time the forum was created
        DateTime lastUserActivity;      // Last time the user was isActive in the forum
        byte[] displayMask;



        // *********************************************************************
        //  CompareTo
        //
        /// <summary>
        /// All forums have a SortOrder property. CompareTo compares on SortOrder
        /// to sort the forums appropriately.
        /// </summary>
        // ********************************************************************/
        public int CompareTo(object value) {

            if (value == null) return 1;

            int compareOrder = ((Forum)value).SortOrder;

            if (this.SortOrder == compareOrder) return 0;
            if (this.SortOrder < compareOrder) return -1;
            if (this.SortOrder > compareOrder) return 1;
            return 0;
        }


        // *********************************************************************
        //  IsAnnouncement
        //
        /// <summary>
        /// If post is locked and post date > 2 years
        /// </summary>
        // ********************************************************************/
        public virtual bool IsAnnouncement {
            get { 
                if (MostRecentPostDate > DateTime.Now.AddYears(2))
                    return true;
                else
                    return false;
            }
        }

        // *********************************************************************
        //  IsPrivate
        //
        /// <summary>
        /// Is the forum private, e.g. a role is required to access?
        /// </summary>
        // ********************************************************************/
        public virtual bool IsPrivate {
            get { return isPrivate; }
            set { isPrivate = value; }
        }

        /*************************** PROPERTY STATEMENTS *****************************/
        /// <summary>
        /// Specifies the unique identifier for the each forum.
        /// </summary>
        public int ForumID {
            get { return forumID; }
            set {
                if (value < 0)
                    forumID = 0;
                else
                    forumID = value;
            }
        }

        
        // *********************************************************************
        //  ParentId
        //
        /// <summary>
        /// If ParentId > 0 this forum has a parent and is not a top-level forum
        /// </summary>
        // ********************************************************************/
        public int ParentId {
            get { return parentId; }
            set {
                if (value < 0)
                    parentId = 0;
                else
                    parentId = value;
            }
        }

        
        // *********************************************************************
        //  DisplayMask
        //
        /// <summary>
        /// Bit mask used to control what forums to display for the current user
        /// </summary>
        // ********************************************************************/
        public byte[] DisplayMask {
            get { 
                return displayMask; 
            }
            set {
                displayMask = value;
            }
        }

        

        public int ForumGroupId {
            get { return forumGroupId; }
            set {
                if (value < 0)
                    forumGroupId = 0;
                else
                    forumGroupId = value;
            }
        }

        public int SortOrder {
            get { return sortOrder; }
            set { sortOrder = value; }
        }

        public DateTime LastUserActivity {
            get { return lastUserActivity; }
            set {
                    lastUserActivity = value;
            }
        }
        /// <summary>
        /// Indicates how many total posts the forum has received.
        /// </summary>
        public int TotalPosts {
            get { return totalPosts; }
            set {
                if (value < 0)
                    totalPosts = -1;
                else
                    totalPosts = value;
            }
        }


        /// <summary>
        /// Specifies the date/time of the most recent post to the forum.
        /// </summary>
        public DateTime MostRecentPostDate {
            get { return mostRecentPostDate; }
            set {
                mostRecentPostDate = value;
            }
        }

        /// <summary>
        /// Specifies the most recent post to the forum.
        /// </summary>
        public int MostRecentPostId {
            get { return mostRecentPostId; }
            set {
                mostRecentPostId = value;
            }
        }

        /// <summary>
        /// Specifies the most recent thread id to the forum.
        /// </summary>
        public int MostRecentThreadId {
            get { return mostRecentThreadId; }
            set {
                mostRecentThreadId = value;
            }
        }

        /// <summary>
        /// Specifies the author of the most recent post to the forum.
        /// </summary>
        public String MostRecentPostAuthor {
            get { return mostRecentUser; }
            set {
                mostRecentUser = value;
            }
        }

        /// <summary>
        /// Indicates how many total threads are in the forum.  A thread is a top-level post.
        /// </summary>
        public int TotalThreads {
            get { return totalThreads; }
            set {
                if (value < 0)
                    totalThreads = -1;
                else
                    totalThreads = value;
            }
        }

        /// <summary>
        /// Specifies how many days worth of posts to view per page when listing a forum's posts.
        /// </summary>
        public int DaysToView {
            get { return daysToView; }
            set {
                if (value < 0)
                    daysToView = 0;
                else
                    daysToView = value;
            }
        }

        /// <summary>
        /// Specifies the name of the forum.
        /// </summary>
        public String Name {
            get { return name; }
            set { name = value; }
        }

        /// <summary>
        /// Specifies the description of the forum.
        /// </summary>
        public String Description {
            get { return description; }
            set { description = value; }
        }

        /// <summary>
        /// Specifies if the forum is isModerated or not.
        /// </summary>
        public bool Moderated {
            get { return isModerated; }
            set { isModerated = value; }
        }

        /// <summary>
        /// Specifies if the forum is currently isActive or not.  InisActive forums do not appear in the
        /// ForumListView Web control listing.
        /// </summary>
        public bool Active {
            get { return isActive; }
            set { isActive = value; }
        }

        /// <summary>
        /// Returns the date/time the forum was created.
        /// </summary>
        public DateTime DateCreated {
            get { return dateCreated; }
            set { dateCreated = value; }
        }
        /****************************************************************************/
    }
}
