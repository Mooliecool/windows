using System;
using AspNetForums;
using AspNetForums.Components;

namespace AspNetForums {
    /// <summary>
    /// Summary description for Polls.
    /// </summary>
    public class Votes {

        public static VoteResultCollection GetVoteResults(int postID) {

            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            // Get the current user
            return dp.GetVoteResults(postID);

        }

        public static void Vote(int postID, string selection) {

            // Create Instance of the IDataProviderBase
            IDataProviderBase dp = DataProvider.Instance();

            // Log the vote
            dp.Vote(postID, selection);
        }
    }
}
