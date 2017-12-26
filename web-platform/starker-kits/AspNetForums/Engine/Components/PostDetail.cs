using System;

namespace AspNetForums.Components {

    // *********************************************************************
    //
    //  PostDetails
    //
    /// <summary>
    /// Summary description for PostDetails.
    /// </summary>
    //
    // ********************************************************************/    
    public class PostDetails : Post {
        /*************************** PRIVATE VARIABLES *****************************/
        int _prevPostID = 0;			// the PostID of the next Post
        int _nextPostID = 0;			// the PostID of the previous Post
        int _prevThreadID = 0;			// the PostID of the first Post in the previous Thread
        int _nextThreadID = 0;			// the PostID of the first Post in the next Thread
		
        User _userInfo;					// Information about the user who posted the post
		
        bool _trackingThread;			// Whether or not the current user has email tracking enabled for the thread he/she's viewing
        /*****************************************************************************/



        /*************************** CONSTRUCTORS *****************************/
        public PostDetails() : base() {}		// default construtor
        /**********************************************************************/

		


        /*************************** PROPERTY STATEMENTS *****************************/


        // Property get/set Statements
        public int PrevPostID {
            get { return _prevPostID; }
            set {
                if (value < 0)
                    _prevPostID = 0;
                else
                    _prevPostID = value;
            }
        }

        public int NextPostID {
            get { return _nextPostID; }
            set {
                if (value < 0)
                    _nextPostID = 0;
                else
                    _nextPostID = value;
            }
        }

        public int PrevThreadID {
            get { return _prevThreadID; }
            set {
                if (value < 0)
                    _prevThreadID = 0;
                else
                    _prevThreadID = value;
            }
        }

        public int NextThreadID {
            get { return _nextThreadID; }
            set {
                if (value < 0)
                    _nextThreadID = 0;
                else
                    _nextThreadID = value;
            }
        }

        public User UserInfo {
            get { return _userInfo; }
            set {
                _userInfo = value;
            }
        }

        public bool ThreadTracking {
            get { return _trackingThread; }
            set { _trackingThread = value; }
        }


    }
}
