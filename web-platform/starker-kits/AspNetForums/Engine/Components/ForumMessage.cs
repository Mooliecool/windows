using System;

namespace AspNetForums.Components {
    /// <summary>
    /// This class defines the properties that makeup a forum.
    /// </summary>
    public class ForumMessage {

        string title;
        string body;
        int messageId = 0;

        public int MessageID {
            get { return messageId; }
            set { messageId = value; }
        }

        public string Title {
            get { return title; }
            set { title = value; }
        }

        public string Body {
            get { return body; }
            set { body = value; }
        }
    }
}
