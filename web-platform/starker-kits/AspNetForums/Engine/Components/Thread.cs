using System;

namespace AspNetForums.Components {

    // *********************************************************************
    //
    //  Thread
    //
    /// <summary>
    /// The thread class is similar to the post class but has additional properties
    /// </summary>
    //
    // ********************************************************************/
    public class Thread : Post {

        string mostRecentAuthor = "";   // Most recent post author
        int mostRecentPostID = 0;       // Most recent post id

        DateTime pinnedDate;            // the date at which the post is pinned

        bool isPinned = false;          // whether or not the post is pinned

        
        // *********************************************************************
        //
        //  IsPinned
        //
        /// <summary>
        /// If post is post date > DateTime.Now
        /// </summary>
        //
        // ********************************************************************/
        public bool IsPinned {
            get { 
                if ((isPinned) && (pinnedDate > PostDate))
                    return true;
                else
                    return false;
            }
            set {
                isPinned = value;
            }
        }

        // *********************************************************************
        //
        //  IsPopular
        //
        /// <summary>
        /// If thread has activity in the last 2 days and > 20 replies
        /// </summary>
        //
        // ********************************************************************/
        public bool IsPopular {
            get { 
                if ((ThreadDate < DateTime.Now.AddDays(2)) && ( (Replies > 25) || (Views > 200) ))
                    return true;

                return false;
            }
        }

        // *********************************************************************
        //
        //  PinnedDate
        //
        /// <summary>
        /// Controls the sort order of posts by date
        /// </summary>
        //
        // ********************************************************************/
        public DateTime PinnedDate {
            get { 
                return pinnedDate; 
            }
            set { 
                pinnedDate = value; 
            }
        }


        // *********************************************************************
        //
        //  MostRecentPostAuthor
        //
        /// <summary>
        /// The author of the most recent post in the thread.
        /// </summary>
        //
        // ********************************************************************/
        public string MostRecentPostAuthor {
            get { 
                return mostRecentAuthor; 
            }
            set { 
                mostRecentAuthor = value; 
            }
        }

        // *********************************************************************
        //
        //  MostRecentPostID
        //
        /// <summary>
        /// The post id of the most recent post in the thread.
        /// </summary>
        //
        // ********************************************************************/
        public int MostRecentPostID {
            get { 
                return mostRecentPostID; 
            }
            set { 
                mostRecentPostID = value; 
            }
        }

    }
}
