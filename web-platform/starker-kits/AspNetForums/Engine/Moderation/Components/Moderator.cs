using System;
using System.Web;

namespace AspNetForums.Components {

    // *********************************************************************
    //  Moderator
    //
    /// <summary>
    /// This class contains the properties for a User.
    /// </summary>
    /// 
    // ********************************************************************/
    public class Moderator : User {

        int moderatedPosts = 0;

        public int TotalPostsModerated {
            get { return moderatedPosts; }
            set { moderatedPosts = value; }
        }
    }
}
