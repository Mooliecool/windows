using System;

namespace AspNetForums.Components {
    /// <summary>
    /// This exception is thrown when a new post is attempted to be added that is a duplicate of
    /// an existing post by the same user in some other forum.
    /// </summary>
    public class PostDuplicateException : ApplicationException {
        public PostDuplicateException() {}
        public PostDuplicateException(String s) : base(s) {}
        public PostDuplicateException(String s, Exception inner) : base(s, inner) {}
    }


    /// <summary>
    /// This exception is thrown when a forum is not found.  For example, if the ForumView Web control
    /// is instantiated and passed a ForumID that does not exist in the database.
    /// </summary>
    public class ForumNotFoundException : ApplicationException {
        public ForumNotFoundException() {}
        public ForumNotFoundException(String s) : base(s) {}
        public ForumNotFoundException(String s, Exception inner) : base(s, inner) {}
    }

    public class ForumGroupNotFoundException : ApplicationException {
        public ForumGroupNotFoundException() {}
        public ForumGroupNotFoundException(String s) : base(s) {}
        public ForumGroupNotFoundException(String s, Exception inner) : base(s, inner) {}
    }


    /// <summary>
    /// This exception is thrown when a particular post is not found.  For example, if the PostDisplay 
    /// Web control is instantiated and passed a PostID that does not exist in the database.
    /// </summary>
    public class PostNotFoundException : ApplicationException {
        public PostNotFoundException() {}
        public PostNotFoundException(String s) : base(s) {}
        public PostNotFoundException(String s, Exception inner) : base(s, inner) {}
    }



    /// <summary>
    /// This exception is thrown when a particular user is not found.
    /// </summary>
    public class UserNotFoundException : ApplicationException {
        public UserNotFoundException() {}
        public UserNotFoundException(String s) : base(s) {}
        public UserNotFoundException(String s, Exception inner) : base(s, inner) {}
    }


    /// <summary>
    /// This exception is thrown when a particular email template is not found.
    /// </summary>
    public class EmailTemplateNotFoundException : ApplicationException {
        public EmailTemplateNotFoundException() {}
        public EmailTemplateNotFoundException(String s) : base(s) {}
        public EmailTemplateNotFoundException(String s, Exception inner) : base(s, inner) {}
    }

    /// <summary>
    /// This exception is thrown when a user attempts to edit a post and it's not allowed
    /// </summary>
    public class CannotEditPostException : ApplicationException {
        public CannotEditPostException() {}
        public CannotEditPostException(String s) : base(s) {}
        public CannotEditPostException(String s, Exception inner) : base(s, inner) {}
    }

}
