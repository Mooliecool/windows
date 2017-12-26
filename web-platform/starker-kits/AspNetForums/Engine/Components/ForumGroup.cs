using System;

namespace AspNetForums.Components {

    /// <summary>
	/// This class defines the properties that makeup a forum.
	/// </summary>
	public class ForumGroup : IComparable {
		/*************************** PRIVATE VARIABLES *****************************/
		int _forumGroupId = 0;				// the unique identifier for the Forum Group
        string _forumName = "";
        int sortOrder = 0;
		/*****************************************************************************/

        public int CompareTo(object value) {

            if (value == null) return 1;

            int compareOrder = ((ForumGroup)value).SortOrder;

            if (this.SortOrder == compareOrder) return 0;
            if (this.SortOrder < compareOrder) return -1;
            if (this.SortOrder > compareOrder) return 1;
            return 0;
        }

		/*************************** PROPERTY STATEMENTS *****************************/
		/// <summary>
		/// Specifies the unique identifier for the each forum.
		/// </summary>
        public int ForumGroupID {
            get { 
                return _forumGroupId; 
            }

            set {
                if (value < 0)
                    _forumGroupId = 0;
                else
                    _forumGroupId = value;
            }
        }

        /// <summary>
        /// Specifies the name of the forum.
        /// </summary>
        public String Name {
            get { 
                return _forumName; 
            }
            set { 
                _forumName = value; 
            }
        }

        public int SortOrder {
            get { return sortOrder; }
            set { sortOrder = value; }
        }

	}
}
