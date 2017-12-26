if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetSearchResultsByText]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetSearchResultsByText]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetSearchResultsByText_FTQ]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetSearchResultsByText_FTQ]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetSearchResultsByUser]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetSearchResultsByUser]
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO

CREATE  PROCEDURE forums_GetSearchResultsByText (
    @Page int,
    @RecsPerPage int,
    @ForumID int = 0,
    @FullTextSearch bit = 0,
    @AndSearch bit = 1,
    @Pattern1 nvarchar(250),
    @Pattern2 nvarchar(50) = NULL,
    @Pattern3 nvarchar(50) = NULL,
    @Pattern4 nvarchar(50) = NULL,
    @UserName nvarchar(50) = NULL,
    @MoreRecords bit output,
    @Status bit output
) AS
    -- Performance optimizations
    SET NOCOUNT ON
    -- Global declarations
    DECLARE @sql nvarchar(1000)
    DECLARE @FirstRec int, @LastRec int, @MoreRec int
    SET @FirstRec = (@Page - 1) * @RecsPerPage;

    SET @LastRec = (@FirstRec + @RecsPerPage);
    SET @MoreRec = @LastRec + 1;
    SET @MoreRecords = 0;
    
    SET @Status = 0;

    CREATE TABLE #SearchResults (
        IndexID int IDENTITY(1,1),
        PostID int
    )

    -- Turn on rowcounting for performance
    SET ROWCOUNT @MoreRec;
    IF @FullTextSearch = 1 BEGIN
        -- First check to see if full text is enabled on the column.  If it is then do the
        -- search.  Else, don't do the search and set the status bit to 1 for Full Text Error
        IF COLUMNPROPERTY(OBJECT_ID('Posts'), 'Body', 'IsFulltextIndexed') = 0 SET @Status = 1
        ELSE
            EXEC forums_GetSearchsResultsByText_FTQ @Pattern1, @ForumID, @Username
    END
    ELSE BEGIN
        INSERT INTO #SearchResults(PostID)
        SELECT PostID
        FROM Posts P (nolock)
        WHERE
            Approved = 1 AND
            (
                @ForumID = -1 OR
                ForumID = @ForumID
            ) AND
            (
                P.ForumID NOT IN (SELECT ForumID FROM PrivateForums) OR
                P.ForumID IN (SELECT ForumID FROM PrivateForums WHERE RoleName IN (SELECT RoleName FROM UsersInRoles WHERE Username = @Username))
            ) AND
            (
                (
                    @AndSearch = 1 AND
                    (
                        0 < ISNULL(PATINDEX(@Pattern1, Body), 0) AND
                        0 < ISNULL(PATINDEX(@Pattern2, Body), 1) AND
                        0 < ISNULL(PATINDEX(@Pattern3, Body), 1) AND
                        0 < ISNULL(PATINDEX(@Pattern4, Body), 1)
                    ) 
                ) OR
                (
                    @AndSearch = 0 AND
                    (
                        0 < ISNULL(PATINDEX(@Pattern1, Body), 0) OR
                        0 < ISNULL(PATINDEX(@Pattern2, Body), 0) OR
                        0 < ISNULL(PATINDEX(@Pattern3, Body), 0) OR
                        0 < ISNULL(PATINDEX(@Pattern4, Body), 0)
                    )
                )
            )
        ORDER BY ThreadDate DESC
    END
    IF @@ROWCOUNT > @LastRec SET @MoreRecords = 1
    SET ROWCOUNT 0
    -- Turn off rowcounting

    -- Select the data out of the temporary table
    SELECT
        forums_Post.*,
        HasRead = 0 -- not used
    FROM 
        forums_Post (nolock), #SearchResults
    WHERE
        forums_Post.PostID = #SearchResults.PostID AND
        #SearchResults.IndexID > @FirstRec AND
        #SearchResults.IndexID <= @LastRec
    ORDER BY #SearchResults.IndexID ASC
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO

CREATE PROCEDURE forums_GetSearchResultsByText_FTQ (
    @Pattern1 nvarchar(250),
    @ForumID int,
    @Username nvarchar(50)
) AS
    IF @@NESTLEVEL > 1 BEGIN
        INSERT INTO #SearchResults(PostID)
        SELECT PostID
        FROM Posts P (nolock)
        WHERE
            Approved = 1 AND
            (
                @ForumID = 0 OR
                ForumID = @ForumID
            ) AND
            (
                P.ForumID NOT IN (SELECT ForumID FROM PrivateForums) OR
                P.ForumID IN (SELECT ForumID FROM PrivateForums WHERE RoleName IN (SELECT RoleName FROM UsersInRoles WHERE Username = @Username))
            ) AND
            CONTAINS(Body, @Pattern1)
        ORDER BY ThreadDate DESC
    END
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO


CREATE PROCEDURE forums_GetSearchResultsByUser (
    @Page int,
    @RecsPerPage int,
    @ForumID int = 0,
    @UserPattern nvarchar(50),
    @UserName nvarchar(50) = NULL,
    @MoreRecords bit output
) AS
    -- Performance optimizations
    SET NOCOUNT ON
    -- Global declarations
    DECLARE @sql nvarchar(1000)
    DECLARE @FirstRec int, @LastRec int, @MoreRec int

    SET @FirstRec = (@Page - 1) * @RecsPerPage;
    SET @LastRec = (@FirstRec + @RecsPerPage);
    SET @MoreRec = @LastRec + 1;
    SET @MoreRecords = 0;

    CREATE TABLE #SearchResults (
        IndexID int IDENTITY(1,1),
        PostID int
    )

    -- Turn on rowcounting for performance
    SET ROWCOUNT @MoreRec;
    INSERT INTO #SearchResults(PostID)
    SELECT PostID
    FROM Posts P (nolock)
    WHERE
        Approved = 1 AND
        (
            @ForumID = 0 OR
            ForumID = @ForumID
        ) AND
        (
            P.ForumID NOT IN (SELECT ForumID FROM PrivateForums) OR
            P.ForumID IN (SELECT ForumID FROM PrivateForums WHERE RoleName IN (SELECT RoleName FROM UsersInRoles WHERE Username = @Username))
        ) AND
        0 < ISNULL(PATINDEX(@UserPattern, Username), 1)
    ORDER BY ThreadDate DESC
    IF @@ROWCOUNT > @LastRec SET @MoreRecords = 1
    SET ROWCOUNT 0
    -- Turn off rowcounting

    -- Select the data out of the temporary table
    SELECT
        P.*,
        HasRead = 0 -- not used
    FROM 
        forums_Post P (nolock), #SearchResults
    WHERE
        forums_Post.PostID = #SearchResults.PostID AND
        #SearchResults.IndexID > @FirstRec AND
        #SearchResults.IndexID <= @LastRec
    ORDER BY #SearchResults.IndexID ASC
GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO