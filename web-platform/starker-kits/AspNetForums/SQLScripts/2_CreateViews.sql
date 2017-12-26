if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_Post]') and OBJECTPROPERTY(id, N'IsView') = 1)
drop view [dbo].[forums_Post]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_User]') and OBJECTPROPERTY(id, N'IsView') = 1)
drop view [dbo].[forums_User]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_Forum]') and OBJECTPROPERTY(id, N'IsView') = 1)
drop view [dbo].[forums_Forum]
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

CREATE VIEW forums_Forum
AS
    SELECT
        ForumID,
        ForumGroupId, 
        ParentID, 
        Name, 
        Description, 
        DateCreated, 
        DaysToView, 
        Moderated, 
        TotalPosts, 
        TotalThreads AS TotalTopics,
        MostRecentPostID, 
        MostRecentThreadID, 
        MostRecentPostDate, 
        MostRecentPostAuthor, 
        Active,
        SortOrder, 
        DisplayMask
    FROM
        Forums
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

CREATE VIEW forums_Post
AS
    SELECT
        Subject,
        Body, 
        PostID, 
        ThreadID, 
        ParentID,
        TotalViews, 
        IsLocked, 
        IsPinned,
        ThreadDate, 
        PinnedDate, 
        UserName,
        ForumID,
        PostLevel,
        SortOrder,
        Approved,
        PostType,
        PostDate,
        (
            SELECT Name
            FROM Forums
            WHERE ForumID = P.ForumID
        ) AS ForumName,
        (
            SELECT COUNT(*)
            FROM Posts
            WHERE P.ThreadID = ThreadID AND Approved = 1 AND PostLevel != 1
        ) AS Replies, 
        (
            SELECT TOP 1 Username
            FROM Posts
            WHERE P.ThreadID = ThreadID AND Approved = 1
            ORDER BY PostDate DESC
        ) AS MostRecentPostAuthor,
        (
            SELECT TOP 1 PostID
            FROM Posts
            WHERE P.ThreadID = ThreadID AND Approved = 1
            ORDER BY PostDate DESC
        ) AS MostRecentPostID
    FROM Posts P
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

CREATE VIEW forums_User
AS
    SELECT
        UserName,
        Password, 
        Email, 
        ForumView, 
        Approved, 
        ProfileApproved, 
        Trusted, 
        FakeEmail, 
        URL, 
        Signature, 
        DateCreated, 
        TrackYourPosts, 
        LastLogin, 
        LastActivity, 
        TimeZone, 
        Location, 
        Occupation, 
        Interests, 
        MSN, 
        Yahoo, 
        AIM, 
        ICQ, 
        TotalPosts, 
        HasAvatar, 
        ShowUnreadTopicsOnly, 
        Style, 
        AvatarType, 
        ShowAvatar, 
        DateFormat, 
        PostViewOrder,
        AvatarUrl,
        (
            SELECT COUNT(*)
            FROM Moderators
            WHERE Moderators.Username = Users.Username
        ) AS IsModerator,
        FlatView,
        Attributes
    FROM Users
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO