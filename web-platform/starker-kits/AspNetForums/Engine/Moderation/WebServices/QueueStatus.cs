using System;
using System.Web.Services;
using AspNetForums.Moderation;
using AspNetForums.Components;

namespace AspNetForums.Moderation.WebServices {

    public class QueueStatus {
        [WebMethod]
        public ModerationQueueStatus GetQueueStatus(int forumID, string username) {

            return Moderate.GetQueueStatus(forumID, username);

        }
    }
}
