using System;
using System.Collections;
using System.Web;
using System.Xml;

namespace AspNetForums.Components {
    /// <summary>
    /// Summary description for Vote.
    /// </summary>
    public class VoteDetails {
        SortedList voteOptions;
        string question;
        bool allowMultipleVotes = false;
        int postID = -1;
        
        // *********************************************************************
        //  VoteDetails
        //
        /// <summary>
        /// Constructor
        /// </summary>
        // ***********************************************************************/        
        public VoteDetails(int postID, string postSubject, string postBody) {

            question = postSubject;
            postID = postID;
            GetVoteQuestions(postBody);

        }


        // *********************************************************************
        //  VoteDetails
        //
        /// <summary>
        /// Constructor
        /// </summary>
        // ***********************************************************************/        
        private void GetVoteQuestions(string postBody) {
            voteOptions = new SortedList();
            XmlDocument dom = new XmlDocument();
            XmlNodeList choicesNodeList;

            // Load the dom
            //
            dom.LoadXml(postBody);

            // Bind to the Choices node and a Node Collection
            //
            choicesNodeList = dom.SelectSingleNode("VoteOptions").ChildNodes;

            // Walk through each node in the Node List and add to Array List
            //
            foreach (XmlNode node in choicesNodeList)
                voteOptions.Add(node.Name, node.InnerText);

        }

        

        public string Question {
            get {
                return question;
            }
        }

        public bool HasVoted {
            get {
                HttpCookie cookie = GetCookie();

                if (cookie == null)
                    return false;

                if (cookie.Values[postID] == null)
                    return false;
                else
                    return true;
            }
        }

        public void SetHtasVotedCookie(string option) {
            HttpCookie cookie = GetCookie();

            if (cookie == null) {
                cookie = new HttpCookie("AspNetForumsPoll");
                cookie.Expires = DateTime.Now.AddMonths(2);
            }

            if (cookie.Values[postID.ToString()] == null) {
                cookie.Values.Add(postID.ToString(), option);

                HttpContext.Current.Response.Cookies.Add(cookie);
            }
        }

        /*
            // Write the Cookie to indicate that this user
            // has voted for this Poll
            //
            String cookieName = "ASPNETPoll_" + polldetails.Name;
            Context.Response.Clear();
            */

        public bool AllowMultipleVotes {
            get {
                return allowMultipleVotes;
            }
        }
        
        private HttpCookie GetCookie() {

            // Attempt to retrieve the cookie
            //
            return HttpContext.Current.Request.Cookies["AspNetForumsPoll"];


        }

        public SortedList VoteOptions {
            get {
                return voteOptions;
            }
        }

    }
}
