using System;

namespace AspNetForums.Components {
    /// <summary>
    /// Summary description for VoteResults.
    /// </summary>
    public class VoteResult {
        string vote;
        int voteCount;

        public string Vote {
            get {
                return vote;
            }
            set {
                vote = value;
            }
        }

        public int VoteCount {
            get {
                return voteCount;
            }
            set {
                voteCount = value;
            }
        }
    }
}
