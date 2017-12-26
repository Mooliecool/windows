-- Updates to Forum table
ALTER Table Forums
	Add [ParentID] [int] NOT NULL DEFAULT(0)

ALTER Table Forums
	Add [DisplayMask] [binary] (64) NOT NULL DEFAULT(0)

-- Updates to Users table
ALTER Table Users
	Add [Attributes] [binary] (4) NOT NULL DEFAULT(0)

ALTER Table Users
	Add [UserId] [int] IDENTITY (1, 1) NOT NULL

ALTER Table Users
	Add [ScreenName] [nvarchar] (50) COLLATE SQL_Latin1_General_CP1_CI_AS NULL

-- Updates to Posts table
ALTER Table Posts
	Add [PostType] [int] NOT NULL DEFAULT(0)

-- New tables
CREATE TABLE [dbo].[Post_Archive] (
	[PostID] [int] IDENTITY (1, 1) NOT NULL ,
	[ThreadID] [int] NOT NULL ,
	[ParentID] [int] NOT NULL ,
	[PostLevel] [int] NOT NULL ,
	[SortOrder] [int] NOT NULL ,
	[Subject] [nvarchar] (256) COLLATE SQL_Latin1_General_CP1_CI_AS NOT NULL ,
	[PostDate] [datetime] NOT NULL ,
	[Approved] [bit] NOT NULL ,
	[ForumID] [int] NOT NULL ,
	[UserName] [nvarchar] (50) COLLATE SQL_Latin1_General_CP1_CI_AS NOT NULL ,
	[ThreadDate] [datetime] NOT NULL ,
	[TotalViews] [int] NOT NULL ,
	[IsLocked] [bit] NOT NULL ,
	[IsPinned] [bit] NOT NULL ,
	[PinnedDate] [datetime] NOT NULL ,
	[Body] [ntext] COLLATE SQL_Latin1_General_CP1_CI_AS NOT NULL 
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO

CREATE TABLE [dbo].[Vote] (
	[PostID] [int] NOT NULL ,
	[Vote] [nvarchar] (2) COLLATE SQL_Latin1_General_CP1_CI_AS NOT NULL ,
	[VoteCount] [int] NOT NULL 
) ON [PRIMARY]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[Maintenance_CleanForumsRead]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[Maintenance_CleanForumsRead]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[Maintenance_ResetForumGroupsForInsert]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[Maintenance_ResetForumGroupsForInsert]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[Moderate_GetPostHistory]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[Moderate_GetPostHistory]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[Reports_UserVisitsByDay]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[Reports_UserVisitsByDay]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[Search_ForUser]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[Search_ForUser]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[Statistics_GetModerationActions]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[Statistics_GetModerationActions]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[Statistics_GetMostActiveModerators]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[Statistics_GetMostActiveModerators]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[Statistics_GetMostActiveUsers]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[Statistics_GetMostActiveUsers]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[Statistics_ResetForumStatistics]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[Statistics_ResetForumStatistics]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[Statistics_ResetTopPosters]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[Statistics_ResetTopPosters]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[Statistics_UpdateForumStatistics]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[Statistics_UpdateForumStatistics]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_AddForum]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_AddForum]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_AddForumGroup]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_AddForumGroup]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_AddForumToRole]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_AddForumToRole]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_AddModeratedForumForUser]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_AddModeratedForumForUser]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_AddPost]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_AddPost]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_AddUserToRole]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_AddUserToRole]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_ApproveModeratedPost]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_ApproveModeratedPost]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_ApprovePost]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_ApprovePost]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_CanModerate]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_CanModerate]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_CanModerateForum]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_CanModerateForum]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_ChangeForumGroupSortOrder]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_ChangeForumGroupSortOrder]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_ChangeUserPassword]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_ChangeUserPassword]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_CheckUserCredentials]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_CheckUserCredentials]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_CreateNewRole]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_CreateNewRole]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_CreateNewUser]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_CreateNewUser]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_DeleteForum]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_DeleteForum]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_DeleteModeratedPost]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_DeleteModeratedPost]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_DeletePost]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_DeletePost]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_DeletePostAndChildren]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_DeletePostAndChildren]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_DeleteRole]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_DeleteRole]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_FindUsersByName]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_FindUsersByName]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetAllButOneForum]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetAllButOneForum]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetAllForumGroups]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetAllForumGroups]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetAllForumGroupsForModeration]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetAllForumGroupsForModeration]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetAllForums]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetAllForums]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetAllForumsByForumGroupId]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetAllForumsByForumGroupId]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetAllMessages]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetAllMessages]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetAllRoles]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetAllRoles]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetAllTopicsPaged]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetAllTopicsPaged]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetAllUnmoderatedTopicsPaged]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetAllUnmoderatedTopicsPaged]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetAllUsers]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetAllUsers]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetAnonymousUsersOnline]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetAnonymousUsersOnline]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetBannedUsers]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetBannedUsers]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetEmailInfo]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetEmailInfo]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetEmailList]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetEmailList]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetForumByPostID]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetForumByPostID]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetForumByThreadID]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetForumByThreadID]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetForumGroupByForumID]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetForumGroupByForumID]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetForumGroupNameByID]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetForumGroupNameByID]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetForumInfo]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetForumInfo]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetForumMessageTemplateList]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetForumMessageTemplateList]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetForumModerators]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetForumModerators]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetForumViewByUsername]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetForumViewByUsername]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetForumsByForumGroupId]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetForumsByForumGroupId]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetForumsForModerationByForumGroupId]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetForumsForModerationByForumGroupId]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetForumsModeratedByUser]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetForumsModeratedByUser]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetForumsNotModeratedByUser]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetForumsNotModeratedByUser]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetMessage]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetMessage]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetModeratedForums]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetModeratedForums]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetModeratedPosts]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetModeratedPosts]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetModeratorsForEmailNotification]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetModeratorsForEmailNotification]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetNextPostID]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetNextPostID]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetNextThreadID]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetNextThreadID]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetParentID]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetParentID]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetPostInfo]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetPostInfo]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetPostRead]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetPostRead]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetPrevNextThreadID]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetPrevNextThreadID]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetPrevPostID]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetPrevPostID]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetPrevThreadID]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetPrevThreadID]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetRoleDescription]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetRoleDescription]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetRolesByForum]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetRolesByForum]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetRolesByUser]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetRolesByUser]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetSearchResults]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetSearchResults]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetSingleMessage]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetSingleMessage]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetStatistics]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetStatistics]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetSummaryInfo]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetSummaryInfo]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetThread]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetThread]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetThreadByParentID]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetThreadByParentID]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetThreadByPostID]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetThreadByPostID]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetThreadByPostIDPaged]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetThreadByPostIDPaged]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetThreadByPostIDPaged2]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetThreadByPostIDPaged2]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetThreadByPostIDPaged_BackUp]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetThreadByPostIDPaged_BackUp]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetTimezoneByUsername]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetTimezoneByUsername]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetTop25NewPosts]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetTop25NewPosts]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetTopicsUserIsTracking]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetTopicsUserIsTracking]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetTopicsUserMostRecentlyParticipatedIn]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetTopicsUserMostRecentlyParticipatedIn]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetTotalNumberOfForums]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetTotalNumberOfForums]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetTotalPostCount]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetTotalPostCount]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetTotalPostsForThread]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetTotalPostsForThread]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetTotalUsers]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetTotalUsers]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetTrackingEmailsForThread]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetTrackingEmailsForThread]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetUnmoderatedPostStatus]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetUnmoderatedPostStatus]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetUserGroups]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetUserGroups]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetUserInfo]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetUserInfo]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetUserNameFromPostID]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetUserNameFromPostID]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetUsernameByEmail]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetUsernameByEmail]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetUsersByFirstCharacter]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetUsersByFirstCharacter]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetUsersOnline]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetUsersOnline]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_GetVoteResults]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_GetVoteResults]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_IsDuplicatePost]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_IsDuplicatePost]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_IsUserTrackingPost]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_IsUserTrackingPost]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_MarkAllThreadsRead]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_MarkAllThreadsRead]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_MarkPostAsRead]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_MarkPostAsRead]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_MovePost]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_MovePost]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_RemoveForumFromRole]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_RemoveForumFromRole]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_RemoveModeratedForumForUser]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_RemoveModeratedForumForUser]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_RemoveUserFromRole]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_RemoveUserFromRole]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_ReverseTrackingOption]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_ReverseTrackingOption]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_ToggleOptions]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_ToggleOptions]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_TopicCountForForum]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_TopicCountForForum]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_TrackAnonymousUsers]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_TrackAnonymousUsers]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_UnbanUser]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_UnbanUser]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_UpdateEmailTemplate]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_UpdateEmailTemplate]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_UpdateForum]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_UpdateForum]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_UpdateForumGroup]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_UpdateForumGroup]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_UpdateMessageTemplateList]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_UpdateMessageTemplateList]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_UpdatePost]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_UpdatePost]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_UpdateRoleDescription]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_UpdateRoleDescription]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_UpdateUserFromAdminPage]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_UpdateUserFromAdminPage]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_UpdateUserInfo]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_UpdateUserInfo]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_UserHasPostsAwaitingModeration]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_UserHasPostsAwaitingModeration]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[forums_Vote]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[forums_Vote]
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

create procedure Maintenance_CleanForumsRead
(
	@ForumID int
)
AS
BEGIN
	DELETE
		ForumsRead
	WHERE
		MarkReadAfter = 0 AND
		ForumID = @ForumID
END




GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO


CREATE procedure Maintenance_ResetForumGroupsForInsert
AS
BEGIN
DECLARE @SortOrderCount int
DECLARE @ForumGroupID int
DECLARE @SortOrder int

-- Set our seed value
SET @SortOrderCount = 1

-- Use a temp table so we don't get duplicate values
create table #SortForumGroups (
	ForumGroupID int,
	SortOrder int
)

-- Push data into temp table
INSERT INTO #SortForumGroups
SELECT ForumGroupID, SortOrder FROM ForumGroups ORDER BY sortorder

-- Get the lowest value
SELECT TOP 1 @SortOrder = SortOrder, @ForumGroupID = ForumGroupID FROM #SortForumGroups WHERE SortOrder >= 0 ORDER BY SortOrder

WHILE @SortOrderCount < (SELECT count(*) FROM ForumGroups)
BEGIN

  -- Update the forum groups table
  UPDATE ForumGroups SET SortOrder = @SortOrderCount WHERE ForumGroupID = @ForumGroupID

  -- increment our count
  SET @SortOrderCount = @SortOrderCount + 1

  -- Get the next forumgroupid to modify
  SELECT TOP 1 @SortOrder = SortOrder, @ForumGroupID = ForumGroupID FROM #SortForumGroups WHERE SortOrder > @SortOrder ORDER BY SortOrder
END

END

GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

create procedure Moderate_GetPostHistory(
	@PostID int,
	@UserName nvarchar(50)
)
AS
BEGIN
SELECT
	ModeratedOn,
	ModeratedBy,
	Description,
	Notes
FROM
	ModerationAction,
	ModerationAudit
WHERE
	ModerationAction.ModerationAction = ModerationAudit.ModerationAction AND
	PostID = @PostID	
END

GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO



CREATE  procedure Reports_UserVisitsByDay
(
	@DaysBack int,
	@SumDayCount bit
)
AS
DECLARE @UserCount int
DECLARE @PostCount int
DECLARE @AspNetTeamPostCount int
DECLARE @AveragePostPerUser decimal(5,2)
DECLARE @PercentagePostsAspNetTeam decimal(5,2)
DECLARE @ForumStartDate datetime

IF (@DaysBack > 0)
BEGIN
	SELECT TOP 1 @ForumStartDate = PostDate FROM Posts ORDER BY PostDate

	SET @DaysBack = DATEDIFF(d, GetDate(), @ForumStartDate)
END

-- Create a temporary table to insert results into
CREATE Table #UserVisitsByDay (
	StatDate datetime,
	UserCount int,
	PostCount int,
	AvgPostPerUser decimal(5,2),
	PostCountAspNetTeam int,
	PercentagePostsAspNetTeam decimal(5,2)
)


-- Do for each day
WHILE (@DaysBack <= 0)
BEGIN
	IF (@SumDayCount = 1) AND (@DaysBack < -1)
	BEGIN
		-- Users visited in last day
		SELECT @UserCount = Count(*) FROM Users WHERE DatePart(dy, LastActivity) >= DatePart(dy, DateAdd(dy, @DaysBack, GetDate())) AND DatePart(yy, LastActivity) >= DatePart(yy, DateAdd(dy, @DaysBack, GetDate()))
		
		-- Users posted in last day
		SELECT @PostCount = Count(*) FROM Posts WHERE DatePart(dy, PostDate) >= DatePart(dy, DateAdd(dy, @DaysBack, GetDate())) AND DatePart(yy, PostDate) >= DatePart(yy, DateAdd(dy, @DaysBack, GetDate()))

                -- ASPNet Team post count
		SELECT @AspNetTeamPostCount = Count(*) FROM Posts WHERE DatePart(dy, PostDate) >= DatePart(dy, DateAdd(dy, @DaysBack, GetDate())) AND DatePart(yy, PostDate) >= DatePart(yy, DateAdd(dy, @DaysBack, GetDate())) AND Username IN (SELECT Username FROM UsersInRoles WHERE RoleName = 'AspNetTeam')
	END
	ELSE
	BEGIN
		-- Users visited in last day
		SELECT @UserCount = Count(*) FROM Users WHERE DatePart(dy, LastActivity) = DatePart(dy, DateAdd(dy, @DaysBack, GetDate())) AND DatePart(yy, LastActivity) = DatePart(yy, DateAdd(dy, @DaysBack, GetDate()))
		
		-- Users posted in last day
		SELECT @PostCount = Count(*) FROM Posts WHERE DatePart(dy, PostDate) = DatePart(dy, DateAdd(dy, @DaysBack, GetDate())) AND DatePart(yy, PostDate) = DatePart(yy, DateAdd(dy, @DaysBack, GetDate()))

                -- ASPNet Team post count
		SELECT @AspNetTeamPostCount = Count(*) FROM Posts WHERE DatePart(dy, PostDate) = DatePart(dy, DateAdd(dy, @DaysBack, GetDate())) AND DatePart(yy, PostDate) = DatePart(yy, DateAdd(dy, @DaysBack, GetDate())) AND Username IN (SELECT Username FROM UsersInRoles WHERE RoleName = 'AspNetTeam')
	END
	
	-- Calculate avg. post/user
	SET @AveragePostPerUser = CAST(@PostCount as decimal) / CAST(@UserCount as decimal)

	-- Calculate avg. post/user from ASP.NET Team
	SET @PercentagePostsAspNetTeam = CAST(@AspNetTeamPostCount as decimal) / CAST(@PostCount as decimal)

	INSERT INTO 
		#UserVisitsByDay 
	VALUES (
		DATEADD(d, @DaysBack, GetDate()), 
		@UserCount, 
		@PostCount, 
		@AveragePostPerUser,
		@AspNetTeamPostCount,
		@PercentagePostsAspNetTeam
		)

	SET @DaysBack = @DaysBack + 1
END


SELECT 
	StatDate,
	UserCount,
	PostCount,
	AvgPostPerUser,
	PostCountAspNetTeam,
	PercentagePostsAspNetTeam
FROM
	#UserVisitsByDay
ORDER BY
	StatDate DESC




GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO

CREATE procedure Search_ForUser (
	@PageIndex int,
	@PageSize int,
	@UsernameToSearchFor nvarchar(50),
	@Username nvarchar(50)
)
AS
BEGIN
DECLARE @PageLowerBound int
DECLARE @PageUpperBound int

-- Set the page bounds
SET @PageLowerBound = @PageSize * @PageIndex
SET @PageUpperBound = @PageLowerBound + @PageSize + 1

-- Create a temp table to store the select results
CREATE TABLE #PageIndexForSearchResults
(
	IndexId int IDENTITY (1, 1) NOT NULL,
	PostID int
)	

-- SELECT into the table
IF @Username IS NULL
BEGIN
	INSERT INTO #PageIndexForSearchResults (PostID)
	SELECT 
		PostID 
	FROM 
		Posts 
	WHERE
		Username = @UsernameToSearchFor AND
		ForumID NOT IN (SELECT ForumID from PrivateForums)
	ORDER BY
		PostDate DESC
END
ELSE
BEGIN

	INSERT INTO #PageIndexForSearchResults (PostID)
	SELECT 
		PostID 
	FROM 
		Posts 
	WHERE
		Username = @UsernameToSearchFor AND
		(ForumID NOT IN (SELECT ForumID from PrivateForums) OR
		ForumID IN (SELECT ForumID FROM PrivateForums WHERE RoleName IN (SELECT RoleName from UsersInRoles WHERE username = @Username)))
	ORDER BY
		PostDate DESC
END


-- Ok, our temp table has the results now return on the appropriate values
SELECT
	#PageIndexForSearchResults.PostID,
	ParentID,
	ThreadID,
	PostLevel,
	SortOrder,
	UserName,
	Subject,
	PostDate,
	ThreadDate,
	Approved,
	ForumID,
	Replies = (SELECT COUNT(*) FROM Posts P2 (nolock) WHERE P2.ParentID = P.PostID AND P2.PostLevel != 1),
	Body,
	TotalViews,
	IsLocked,
	HasRead = 0 -- not used
FROM
	Posts P,
	#PageIndexForSearchResults
WHERE
	P.PostID = #PageIndexForSearchResults.PostID AND
	#PageIndexForSearchResults.IndexID > @PageLowerBound AND
	#PageIndexForSearchResults.IndexID < @PageUpperBound
ORDER BY
	#PageIndexForSearchResults.IndexId
END



GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO


--exec Statistics_GetMostActiveModerators

create procedure Statistics_GetModerationActions
AS

SELECT DISTINCT
	Description, TotalActions = (SELECT Count(ModerationAction) FROM ModerationAudit M2 WHERE M2.ModerationAction = M.ModerationAction) 
FROM 
	ModerationAudit M, 
	ModerationAction A 
WHERE 
	M.ModerationAction = A.ModerationAction 
ORDER BY 
	TotalActions DESC 




GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO


--exec Statistics_GetMostActiveModerators

CREATE procedure Statistics_GetMostActiveModerators
AS

SELECT DISTINCT TOP 10
	Username = ModeratedBy, 
	PostsModerated = (SELECT Count(ModerationAction) FROM ModerationAudit M2 WHERE M2.ModeratedBy = M.ModeratedBy)  
FROM 
	ModerationAudit M 
ORDER BY 
	PostsModerated DESC




GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

CREATE   procedure Statistics_GetMostActiveUsers
AS
SELECT TOP 3
	Username,
	TotalPosts
FROM
	Users
WHERE
	Username NOT IN (SELECT Username FROM UsersInRoles WHERE Rolename = 'AspNetTeam')
ORDER BY
	TotalPosts DESC




GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

CREATE  procedure Statistics_ResetForumStatistics
(
@ForumID int
)
AS
BEGIN
DECLARE @ForumCount int
DECLARE @ThreadID int
DECLARE @PostID int
set @ForumCount = 1


IF @ForumID = 0
  WHILE @ForumCount < (SELECT Max(ForumID) FROM FORUMS)
  BEGIN


	IF EXISTS(SELECT ForumID FROM Forums WHERE ForumID = @ForumCount)
	BEGIN
		SELECT TOP 1 @ThreadID = ThreadID, @PostID = PostID FROM Posts WHERE ForumID = @ForumCount and Approved = 1 ORDER BY PostID DESC
		IF @ThreadID IS NOT NULL
			exec Statistics_UpdateForumStatistics @ForumCount, @ThreadID, @PostID
	END

	SET @ForumCount = @ForumCount + 1
	SET @ThreadID = NULL


  END
ELSE
	SELECT TOP 1 @ThreadID = ThreadID, @PostID = PostID FROM Posts WHERE ForumID = @ForumID and Approved = 1 ORDER BY PostID DESC
	exec Statistics_UpdateForumStatistics @ForumID, @ThreadID, @PostID
END



GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO


--select top 500 username ,attributes,totalposts from users order by totalposts desc

--exec Statistics_ResetTopPosters
CREATE  procedure Statistics_ResetTopPosters
AS
BEGIN
DECLARE @Usercount int
DECLARE @LoopCounter int

	SET NOCOUNT ON

	CREATE Table #Top500Users (
	  Rank int IDENTITY (1, 1) NOT NULL,
	  UserName nvarchar(50)
	)

	-- Select into temp table
	INSERT INTO #Top500Users (UserName)
	SELECT TOP 500
		UserName
	FROM
		Users
	ORDER BY
		TotalPosts DESC

	-- How many users did we select?
	SELECT @Usercount = count(*) FROM #Top500Users

	-- Set the loop counter
	SET @LoopCounter = 1

	WHILE (@LoopCounter <= @Usercount)
	BEGIN
		-- clear the flags for top 25, 50, and 100
		UPDATE Users
		SET Attributes = (convert(int, Attributes) & 0xFFFFFFF3)
		WHERE Username = (SELECT Username FROM #Top500Users WHERE Rank = @LoopCounter)

		-- top 25 poster
		IF (@LoopCounter < 26)
			UPDATE Users
			SET Attributes = (Attributes ^ 4)
			WHERE Username = (SELECT Username FROM #Top500Users WHERE Rank = @LoopCounter)

		-- top 50 poster
		IF (@LoopCounter > 25) AND (@LoopCounter < 51)
			UPDATE Users
			SET Attributes = (Attributes ^ 8)
			WHERE Username = (SELECT Username FROM #Top500Users WHERE Rank = @LoopCounter)

		-- top 100 poster
		IF (@LoopCounter > 50) AND (@LoopCounter < 101)
		BEGIN
			UPDATE Users
			SET Attributes = (Attributes ^ 16)
			WHERE Username = (SELECT Username FROM #Top500Users WHERE Rank = @LoopCounter)
		END
	
		-- increment the coutner
		SET @LoopCounter = @LoopCounter + 1
	END
END



GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO


--exec Statistics_UpdateForumStatistics 15, 8139, 8139

CREATE  procedure Statistics_UpdateForumStatistics
(
	@ForumID int,
	@ThreadID int,
	@PostID int
)
AS
BEGIN
DECLARE @Username nvarchar(50)
DECLARE @PostDate datetime
DECLARE @TotalPosts int
DECLARE @TotalThreads int

-- Get values necessary to update the forum statistics
SELECT
	@Username = username,
	@PostDate = PostDate,
	@TotalPosts = (SELECT COUNT(*) FROM Posts P2 (nolock) WHERE P2.ForumID = P.ForumID AND P2.Approved=1),
	@TotalThreads = (SELECT COUNT(*) FROM Posts P2 (nolock) WHERE P2.ForumID = P.ForumID AND P2.Approved=1 AND P2.PostLevel=1)
FROM
	Posts P
WHERE
	PostID = @PostID

-- Do the update within a transaction
BEGIN TRAN

	UPDATE 
		Forums
	SET
		TotalPosts = @TotalPosts,
		TotalThreads = @TotalThreads,
		MostRecentPostID = @PostID,
		MostRecentThreadID = @ThreadID,
		MostRecentPostDate = @PostDate,
		MostRecentPostAuthor = @Username
	WHERE
		ForumID = @ForumID

COMMIT TRAN

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



CREATE  PROCEDURE forums_AddForum
(
	@Name		nvarchar(100),
	@Description	nvarchar(3000),
	@ForumGroupId	int,
	@Moderated	bit,
	@DaysToView	int,
	@Active 	bit
)
 AS
	-- insert a new forum
	INSERT INTO Forums (ForumGroupId, Name, Description, Moderated, DaysToView, Active)
	VALUES (@ForumGroupId, @Name, @Description, @Moderated, @DaysToView, @Active)














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



CREATE    PROCEDURE forums_AddForumGroup
(
	@ForumGroupName		nvarchar(256)
)
 AS
	-- insert a new forum
	INSERT INTO 
		ForumGroups 
		(Name)
	VALUES 
		(@ForumGroupName)

	-- Reset the sort order
	exec Maintenance_ResetForumGroupsForInsert





GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO


CREATE PROCEDURE forums_AddForumToRole
(
   @ForumID int,
   @Rolename nvarchar(256)
)
AS
IF NOT EXISTS (SELECT ForumID FROM PrivateForums WHERE ForumID=@ForumID AND Rolename=@Rolename) AND
    EXISTS (SELECT ForumID FROM Forums WHERE ForumID=@ForumID) AND
    EXISTS (SELECT Rolename FROM UserRoles WHERE Rolename=@Rolename)
    BEGIN
        INSERT INTO
            PrivateForums(ForumID, RoleName)
        VALUES
            (@ForumID, @Rolename)
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



create procedure forums_AddModeratedForumForUser
(
	@UserName	nvarchar(50),
	@ForumID	int,
	@EmailNotification	bit
)
 AS
	-- add a row to the Moderators table
	-- if the user wants to add All Forums, go ahead and delete all of the other forums
	IF @ForumID = 0
		DELETE FROM Moderators WHERE Username = @UserName
	-- now insert the new row into the table
	INSERT INTO Moderators (Username, ForumID, EmailNotification)
	VALUES (@UserName, @ForumID, @EmailNotification)














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



CREATE             PROCEDURE forums_AddPost 
(
	@ForumID int,
	@ReplyToPostID int, 
	@Subject nvarchar(256),
	@UserName nvarchar(50),
	@Body ntext,
	@IsLocked bit,
	@Pinned datetime
) AS
DECLARE @MaxSortOrder int
DECLARE @ParentLevel int
DECLARE @ThreadID int
DECLARE @ParentSortOrder int
DECLARE @NextSortOrder int
DECLARE @NewPostID int
DECLARE @ApprovedPost bit
DECLARE @ModeratedForum bit
DECLARE @IsPinned bit


-- Is the post pinned?
IF @Pinned IS NULL
BEGIN
	SET @IsPinned = 0
	SET @Pinned = GetDate()
END
ELSE
	SET @IsPinned = 1

-- Is this forum moderated?
IF @ForumID = 0 AND @ReplyToPostID <> 0
	-- we need to get the forum ID
	SELECT @ForumID = ForumID FROM Posts (nolock) WHERE PostID = @ReplyToPostID
SELECT @ModeratedForum = Moderated FROM Forums (nolock) WHERE ForumID = @ForumID


-- Determine if this post will be approved
-- if the forum is NOT moderated, then the post will be approved
SET NOCOUNT ON
BEGIN TRAN
IF @ModeratedForum = 0
	SELECT @ApprovedPost = 1
ELSE
  BEGIN
	-- ok, this is a moderated forum.  Is this user trusted?  If he is, then the post is approved ; else it is not
	IF (SELECT Trusted FROM Users (nolock) WHERE UserName = @UserName) = 1
		SELECT @ApprovedPost = 1
	ELSE
		SELECT @ApprovedPost = 0
  END
IF @ReplyToPostID = 0 -- New Post
  BEGIN

    -- Do INSERT into Posts table
    INSERT 
	Posts ( ForumID, ThreadID, ParentID, PostLevel, SortOrder, Subject, PinnedDate, IsPinned, UserName, Approved, Body, IsLocked )
    VALUES 
	(@ForumID, 0, 0, 1, 1, @Subject, @Pinned, @IsPinned, @UserName, @ApprovedPost, @Body, @IsLocked)

    -- Get the new post id
    SELECT 
	@NewPostID = @@IDENTITY

    -- update posts with the new post id
    UPDATE 
	Posts
    SET 
	ThreadID = @NewPostID,
        ParentID = @NewPostID
    WHERE 
	PostID = @NewPostID

   -- do we need to track the threads for this user?
   SELECT @ThreadID = @NewPostID

  END
ELSE -- @ReplyToID <> 0 means reply to an existing post
  BEGIN
    -- Get Post Information for what we are replying to
    SELECT 
	   @ParentLevel = PostLevel,
           @ThreadID = ThreadID,
           @ParentSortOrder = SortOrder,
           @ForumID = ForumID
    FROM 
	   Posts
    WHERE 
           PostID = @ReplyToPostID

    -- Is there another post at the same level or higher
    IF EXISTS (SELECT * 
               FROM Posts 
               WHERE PostLevel <= @ParentLevel 
               AND SortOrder > @ParentSortOrder
               AND ThreadID = @ThreadID )
      BEGIN

        -- Find the next post at the same level or higher
        SELECT 
		@NextSortOrder = Min(SortOrder)
        FROM 
		Posts
        WHERE 
		PostLevel <= @ParentLevel 
        	AND SortOrder > @ParentSortOrder
	        AND ThreadID = @ThreadID

        -- Move the existing posts down
	UPDATE 
		Posts
        SET 
		SortOrder = SortOrder + 1
        WHERE 
		ThreadID = @ThreadID
	        AND SortOrder >= @NextSortOrder

        --  And put this one into place
        INSERT 
		Posts (ForumID, ThreadID, ParentID, PostLevel, SortOrder, Subject, PostDate, IsPinned, UserName, Approved, Body, IsLocked )
        VALUES 
		(@ForumID, @ThreadID, @ReplyToPostID, @ParentLevel + 1, @NextSortOrder, @Subject, @Pinned, @IsPinned, @UserName, @ApprovedPost, @Body, @IsLocked )

	-- Clean up PostsRead
	DELETE PostsRead WHERE PostID = @ThreadID AND UserName != @UserName


      END
   ELSE -- There are no posts at this level or above
     BEGIN

    	-- Find the highest sort order for this parent
    	SELECT 
		@MaxSortOrder = MAX(SortOrder)
    	FROM 
		Posts
    	WHERE 
		ThreadID = @ThreadID

	-- Insert the new post
    	INSERT 
		Posts (ForumID, ThreadID, ParentID, PostLevel, SortOrder, Subject, PinnedDate, IsPinned, UserName, Approved, Body, IsLocked )
    	VALUES 
		(@ForumID, @ThreadID, @ReplyToPostID, @ParentLevel + 1, @MaxSortOrder + 1, @Subject, @Pinned, @IsPinned, @UserName, @ApprovedPost, @Body, @IsLocked )

	-- Clean up PostsRead
	DELETE PostsRead WHERE PostID = @ThreadID AND UserName != @UserName

     END 

     SELECT 
	@NewPostID = @@IDENTITY FROM Posts

     -- if this message is approved, update the thread date
     IF @ApprovedPost = 1
	UPDATE 
		Posts 
	SET 
		ThreadDate = getdate()
	WHERE 
		ThreadID = @ThreadID
  END

  -- update the users tracking for the new post (if needed)
  DECLARE @TrackThread bit
  SELECT 
	@TrackThread = TrackYourPosts 
  FROM 
	Users (nolock) 
  WHERE 
	UserName = @UserName

  IF @TrackThread = 1
	-- if a row already exists to track this thread for this user, do nothing - otherwise add the row
	IF NOT EXISTS(SELECT ThreadID FROM ThreadTrackings (nolock) WHERE ThreadID = @ThreadID AND UserName = @UserName)
		INSERT INTO ThreadTrackings (ThreadID, UserName)
		VALUES(@ThreadID, @UserName)
COMMIT TRAN
BEGIN TRAN

 -- Update the user's post count
 DECLARE @PostCount int
 -- Get the current number of posts
 SELECT @PostCount = TotalPosts FROM Users WHERE UserName = @UserName
 -- Update value
 SET @PostCount = @PostCount + 1
 UPDATE Users SET TotalPosts = @PostCount WHERE UserName = @UserName 

 -- Update the forum statitics
 IF @ApprovedPost = 1
   exec Statistics_UpdateForumStatistics @ForumID, @ThreadID, @NewPostID

 -- Clean up unnecessary columns in forumsread
 exec Maintenance_CleanForumsRead @ForumID


COMMIT TRAN
SET NOCOUNT OFF
SELECT PostID = @NewPostID






GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO



create procedure forums_AddUserToRole
(
   @UserName nvarchar(50),
   @Rolename nvarchar(256)
)
AS
IF NOT EXISTS (SELECT Username FROM UsersInRoles WHERE Username = @UserName AND Rolename = @Rolename)
INSERT INTO
	UsersInRoles
VALUES
	(@UserName, @Rolename)






GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO



CREATE      PROCEDURE forums_ApproveModeratedPost
(
	@PostID	int,
	@ApprovedBy nvarchar(50),
	@Trusted nvarchar(50)

)
AS
DECLARE @ForumID int
DECLARE @ThreadID int

-- First make sure that the post is ALREADY non-approved
IF (SELECT Approved FROM Posts (nolock) WHERE PostID = @PostID) = 1
	-- its already been approved, return 0
	SELECT 0

ELSE
BEGIN
	-- Approve the post
	UPDATE 
		Posts
	SET 
		Approved = 1
	WHERE 
		PostID = @PostID

	-- Get details about the thread and forum this post belongs in
	SELECT
		@ForumID = ForumID,
		@ThreadID = ThreadID
	FROM
		Posts
	WHERE
		PostID = @PostID

	-- Update the thread date
	UPDATE 
		Posts
	SET 
		ThreadDate = getdate()
	WHERE 
		ThreadID = @ThreadID

	-- Update the ModerationAudit table
	INSERT INTO
		ModerationAudit
	VALUES
		(GetDate(), @PostID, @ApprovedBy, 1, NULL)

	-- Update the forums statistics
	exec Statistics_ResetForumStatistics @ForumID

	-- Are we updating the status of a user?
 	IF (@Trusted IS NOT NULL)
	BEGIN

		-- Mark the user as trusted
		UPDATE
			Users
		SET
			Trusted = 1
		WHERE
			Username = @Trusted

		-- Update the ModerationAudit table
		INSERT INTO
			ModerationAudit
		VALUES
			(GetDate(), @PostID, @ApprovedBy, 5, NULL)

	END

	-- Send back a success code
	SELECT 1
END










GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO



CREATE     PROCEDURE forums_ApprovePost
(
	@PostID	int
)
 AS
DECLARE @ForumID int
DECLARE @ThreadID int

	-- first make sure that the post is ALREADY non-approved
	IF (SELECT Approved FROM Posts (nolock) WHERE PostID = @PostID) = 1
		-- its already been approved, return 0
		SELECT 0
	ELSE
	  BEGIN
		-- Approve the post
		UPDATE 
			Posts
		SET 
			Approved = 1
		WHERE 
			PostID = @PostID

		-- Get details about the thread and forum this post belongs in
		SELECT
			@ForumID = ForumID,
			@ThreadID = ThreadID
		FROM
			Posts
		WHERE
			PostID = @PostID

		-- Update the thread date
		UPDATE 
			Posts
		SET 
			ThreadDate = getdate()
		WHERE 
			ThreadID = @ThreadID

		-- Update the ModerationAudit table
		INSERT INTO
			ModerationAudit
		VALUES
			(GetDate(), @PostID, 'undone', 1, NULL)

		-- Update the forums statistics
		exec Statistics_ResetForumStatistics @ForumID

		-- Send back a success code
		SELECT 1
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



create procedure forums_CanModerate 
(
	@UserName	nvarchar(50)
)
AS
	-- determine whether or not this user can moderate
	IF EXISTS(SELECT NULL FROM Moderators (nolock) WHERE UserName = @UserName)
		SELECT CanModerate = 1
	ELSE
		SELECT CanModerate = 0














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



create  PROCEDURE forums_CanModerateForum 
(
	@UserName	nvarchar(50),
	@ForumID	int
)
AS


IF EXISTS(SELECT ForumID FROM Moderators WHERE Username = @UserName AND ForumID = 0)
  SELECT CanModerate = 1
ELSE
  IF EXISTS (SELECT ForumID FROM Moderators WHERE Username = @UserName AND ForumID = @ForumID)
    SELECT CanModerate = 1
  ELSE
    SELECT CanModerate = 0




GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO



create procedure forums_ChangeForumGroupSortOrder
(
     @forumGroupID int,
     @MoveUp bit
)
AS
BEGIN
DECLARE @currentSortValue int
DECLARE @replaceSortValue int

-- Get the current sort order
SELECT @currentSortValue = SortOrder FROM ForumGroups WHERE ForumGroupID = @forumGroupID

-- Move the item up or down?
IF (@MoveUp = 1)
  BEGIN
    IF (@currentSortValue != 1)
      BEGIN
        SET @replaceSortValue = @currentSortValue - 1

        UPDATE ForumGroups SET SortOrder = @currentSortValue WHERE SortOrder = @replaceSortValue
        UPDATE ForumGroups SET SortOrder = @replaceSortValue WHERE ForumGroupID = @forumGroupID
      END
  END

ELSE
  BEGIN
    IF (@currentSortValue < (SELECT MAX(ForumGroupID) FROM ForumGroups))
    BEGIN
      SET @replaceSortValue = @currentSortValue + 1

      UPDATE ForumGroups SET SortOrder = @currentSortValue WHERE SortOrder = @replaceSortValue
      UPDATE ForumGroups SET SortOrder = @replaceSortValue WHERE ForumGroupID = @forumGroupID
    END
  END
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



create  PROCEDURE forums_ChangeUserPassword
(
	@UserName nvarchar(50),
	@NewPassword nvarchar(50)
)
AS

UPDATE
	Users
SET
	Password = @NewPassword
WHERE
	Username = @UserName






GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



create procedure forums_CheckUserCredentials
(
	@UserName nvarchar(50),
	@Password nvarchar(20)
)
 AS
	IF EXISTS(SELECT UserName FROM Users WHERE UserName = @UserName AND Password = @Password AND Approved=1)
	BEGIN
		-- update the time the user last logged in
		UPDATE Users
			SET LastLogin = getdate()
		WHERE UserName = @UserName
		SELECT 1
	END
	ELSE
		SELECT 0














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO


CREATE PROCEDURE forums_CreateNewRole
(
    @RoleName nvarchar(256),
    @Description nvarchar(512)
)
AS
    IF NOT EXISTS(SELECT Rolename FROM UserRoles (nolock) WHERE Rolename = @RoleName)
        BEGIN
            INSERT INTO UserRoles (Rolename, Description)
            VALUES(@Rolename, @Description)
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



create procedure forums_CreateNewUser
(
	@UserName nvarchar(50),
	@Email nvarchar(75),
	@RandomPassword nvarchar(20)
)
 AS
	-- this sproc returns various error/success codes
		-- a return value of 1 means success
		-- a return value of 2 means a dup username
		-- a return value of 3 means a dup email address
	-- first, we need to check if the username is a dup
	IF EXISTS(SELECT Username FROM Users (nolock) WHERE Username = @UserName)
		SELECT 2
	ELSE
		-- we need to check if the email is a dup
		IF EXISTS(SELECT Email FROM Users (nolock) WHERE Email = @Email)
			SELECT 3
		ELSE
			-- everything's peachy if we get this far - insert the user
		  BEGIN
			INSERT INTO Users (UserName, Email, Password)
			VALUES(@UserName, @Email, @RandomPassword)
			SELECT 1	-- return Everything's fine status code
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



create procedure forums_DeleteForum
(
	@ForumID	int
)
 AS
	-- delete the specified forum and all of its posts
	-- first we must remove all the thread tracking rows
	DELETE FROM ThreadTrackings
	WHERE ThreadID IN (SELECT DISTINCT ThreadID FROM Posts WHERE ForumID = @ForumID)
	-- we must remove all of the moderators for this forum
	DELETE FROM Moderators
	WHERE ForumID = @ForumID
	-- now we must remove all of the posts
	DELETE FROM Posts
	WHERE ForumID = @ForumID
	-- finally we can delete the actual forum
	DELETE FROM Forums
	WHERE ForumID = @ForumID














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO



CREATE     PROCEDURE forums_DeleteModeratedPost
(
	@PostID	int,
	@ApprovedBy nvarchar(50),
	@Reason nvarchar(1024)
) AS
	-- we must delete all of the posts and replies
	-- first things first, determine if this is the parent of the thread
	DECLARE @ThreadID int
	DECLARE @ForumID int
	DECLARE @UserName nvarchar(50)

	SELECT @ThreadID = ThreadID, @ForumID = ForumID, @UserName = Username FROM Posts (nolock) WHERE PostID = @PostID
	IF @ThreadID = @PostID
	  BEGIN
		-- we are dealing with the parent fo the thread
		-- delete all of the thread tracking
		DELETE 
			ThreadTrackings
		WHERE 
			ThreadID = @ThreadID

		-- Delete the entire thread
		DELETE 
			Posts
		WHERE 
			ThreadID = @ThreadID

		-- Clean up the forum statistics
		exec Statistics_ResetForumStatistics @ForumID

		-- Update users table to decrement post count for this user
		UPDATE
			Users
		SET 
			TotalPosts = (TotalPosts - 1)
		WHERE
			Username = @UserName

		-- Record to our moderation audit log
		INSERT INTO
			ModerationAudit
		VALUES
			(GetDate(), @PostID, @ApprovedBy, 4, @Reason)

	  END
	ELSE
		-- we must recursively delete this post and all of its children
		exec dbo.forums_DeletePostAndChildren @PostID














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



CREATE   PROCEDURE forums_DeletePost
(
	@PostID	int
) AS
	-- we must delete all of the posts and replies
	-- first things first, determine if this is the parent of the thread
	DECLARE @ThreadID int
	DECLARE @ForumID int
	DECLARE @UserName nvarchar(50)

	SELECT @ThreadID = ThreadID, @ForumID = ForumID, @UserName = Username FROM Posts (nolock) WHERE PostID = @PostID
	IF @ThreadID = @PostID
	  BEGIN
		-- we are dealing with the parent fo the thread
		-- delete all of the thread tracking
		DELETE FROM 
			ThreadTrackings
		WHERE 
			ThreadID = @ThreadID

		-- delete the entire thread
		DELETE FROM 
			Posts
		WHERE 
			ThreadID = @ThreadID

		-- Update the forum statistics
		exec Statistics_ResetForumStatistics @ForumID
	  END
	ELSE
		-- we must recursively delete this post and all of its children
		exec dbo.forums_DeletePostAndChildren @PostID
















GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



CREATE   PROCEDURE forums_DeletePostAndChildren
(
	@PostID	int
)
 AS
	DECLARE @ChildPostID int
	DECLARE @UserName nvarchar(50)
	DECLARE @ForumID int

	-- Build a cursor to loop through all the children of this post
	DECLARE c1 CURSOR LOCAL FOR
		SELECT PostID 
		FROM Posts
		WHERE ParentID = @PostID
	OPEN c1
	FETCH NEXT FROM c1
	INTO @ChildPostID
	WHILE @@FETCH_STATUS = 0
	  BEGIN
		exec dbo.forums_DeletePostAndChildren @ChildPostID
		FETCH NEXT FROM c1
		INTO @ChildPostID
	  END

	-- now, go ahead and delete the post
	SELECT 
		@UserName = Username,
		@ForumID = ForumID
	FROM 
		Posts
	WHERE
		PostID = @PostID

	-- Decrement user's total post count
	UPDATE
		Users
	SET
		TotalPosts = (TotalPosts - 1)
	WHERE
		Username = @UserName

	-- Now, delete the post
	DELETE 
		Posts 
	WHERE 
		PostID = @PostID

	-- Update the forum statistics
	exec Statistics_ResetForumStatistics @ForumID













GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO


CREATE PROCEDURE forums_DeleteRole
(
    @RoleName nvarchar(256)
)
AS
    IF EXISTS(SELECT Rolename FROM UserRoles (nolock) WHERE Rolename = @RoleName)
        BEGIN
            DELETE FROM PrivateForums Where Rolename=@Rolename
            DELETE FROM UsersInRoles WHERE Rolename=@Rolename
            DELETE FROM UserRoles WHERE Rolename=@Rolename
        END

GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO




CREATE       PROCEDURE forums_FindUsersByName
(
	@PageIndex int,
	@PageSize int,
	@UserNameToFind nvarchar(50)
)
AS

DECLARE @PageLowerBound int
DECLARE @PageUpperBound int

-- Set the page bounds
SET @PageLowerBound = @PageSize * @PageIndex
SET @PageUpperBound = @PageLowerBound + @PageSize + 1

-- Create a temp table to store the select results
CREATE TABLE #PageIndexForUsers 
(
	IndexId int IDENTITY (1, 1) NOT NULL,
	Username nvarchar(50)
)	

-- Insert into our temp table
INSERT INTO #PageIndexForUsers (Username)
SELECT 
	Username 
FROM 
	Users 
WHERE 
	Approved = 1 AND 
	DisplayInMemberList = 1 AND
	Username like '%' + @UserNameToFind + '%'
ORDER BY 
	DateCreated


SELECT
	U.Username,
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
	AvatarUrl,
	ShowAvatar,
	DateFormat,
	PostViewOrder,
	IsModerator = (select count(*) from moderators where username = U.Username),
	FlatView,
	Attributes
FROM 
	Users U (nolock),
	#PageIndexForUsers
WHERE 
	Approved = 1 AND
	U.Username = #PageIndexForUsers.Username AND
	#PageIndexForUsers.IndexID > @PageLowerBound AND
	#PageIndexForUsers.IndexID < @PageUpperBound

ORDER BY
	#PageIndexForUsers.IndexId








GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



create procedure forums_GetAllButOneForum
(
	@PostID	int
)
 AS
	-- get all of the forums EXCEPT for the forum that PostID exists in
	DECLARE @ExcludeForumID int
	SELECT @ExcludeForumID =  (SELECT ForumID FROM Posts (nolock) WHERE PostID = @PostID)
	SELECT
		*
	FROM Forums (nolock)
	WHERE NOT (ForumID = @ExcludeForumID) AND Active = 1














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO



CREATE          PROCEDURE forums_GetAllForumGroups
(
	@GetAllForumsGroups	bit = 0,
	@UserName nvarchar(50)
)
AS
BEGIN
	IF @GetAllForumsGroups = 0

		IF @UserName IS NOT NULL
		BEGIN
			SELECT 
				ForumGroupId,
				Name,
				SortOrder
			FROM
				ForumGroups
			WHERE
				EXISTS(
					SELECT
						ForumID
					FROM
						FORUMS
					WHERE
						ForumGroups.ForumGroupID = Forums.ForumGroupId AND
						Forums.Active = 1 AND
						(ForumID NOT IN (SELECT ForumID from PrivateForums) OR
						ForumID IN (SELECT ForumID FROM PrivateForums WHERE RoleName IN (SELECT RoleName from UsersInRoles WHERE username = @UserName)))

				)
		END
		ELSE
		BEGIN
			SELECT 
				ForumGroupId,
				Name,
				SortOrder
			FROM
				ForumGroups
			WHERE
				EXISTS(
					SELECT
						ForumID
					FROM
						FORUMS
					WHERE
						ForumGroups.ForumGroupID = Forums.ForumGroupId AND
						Forums.Active = 1 AND
						ForumID NOT IN (SELECT ForumID from PrivateForums)
				)
		END
	ELSE
		SELECT 
			ForumGroupId,
			Name,
			SortOrder
		FROM
			ForumGroups
			
END









GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO



CREATE            PROCEDURE forums_GetAllForumGroupsForModeration
(
	@UserName nvarchar(50)
)
AS

IF (0 = (SELECT ForumID FROM Moderators WHERE Username = @UserName))

	-- Note, we still only allow the user to moderate forums that he/she has access to
	SELECT 
		ForumGroupId,
		Name,
		SortOrder
	FROM
		ForumGroups
	WHERE
		EXISTS(
			SELECT
				ForumID
			FROM
				FORUMS
			WHERE
				ForumGroups.ForumGroupID = Forums.ForumGroupId AND
				Forums.Active = 1 AND
				(ForumID NOT IN (SELECT ForumID from PrivateForums) OR
				ForumID IN (SELECT ForumID FROM PrivateForums WHERE RoleName IN (SELECT RoleName from UsersInRoles WHERE username = @UserName))) AND
				(SELECT COUNT(*) FROM Posts P (nolock) WHERE P.ForumID = Forums.ForumID and P.Approved = 0) > 0

		)
ELSE
	-- Note, we still only allow the user to moderate forums that he/she has access to
	SELECT 
		ForumGroupId,
		Name,
		SortOrder
	FROM
		ForumGroups
	WHERE
		EXISTS(
			SELECT
				ForumID
			FROM
				FORUMS
			WHERE
				ForumGroups.ForumGroupID = Forums.ForumGroupId AND
				Forums.Active = 1 AND
				(ForumID NOT IN (SELECT ForumID from PrivateForums) OR
				ForumID IN (SELECT ForumID FROM PrivateForums WHERE RoleName IN (SELECT RoleName from UsersInRoles WHERE username = @UserName))) AND
				ForumID IN (SELECT ForumID FROM Moderators WHERE Username = @UserName)  AND
				(SELECT COUNT(*) FROM Posts P (nolock) WHERE P.ForumID = Forums.ForumID and P.Approved = 0) > 0

		)






GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO


CREATE                  PROCEDURE forums_GetAllForums
(
	@GetAllForums	bit = 0,
	@UserName nvarchar(50)
)
AS
	-- return all of the columns in all of the forums
	IF @GetAllForums = 0

                -- Is a User Specified?
                IF @UserName IS NOT NULL
			-- get JUST the active forums
			SELECT
				ForumID,
				ForumGroupId,
				ParentId,
				Name,
				Description,
				DateCreated,
				DaysToView,
				Moderated,
				TotalPosts,
				TotalTopics = TotalThreads,
				MostRecentPostID,
				MostRecentThreadID,
				MostRecentPostDate,
				MostRecentPostAuthor,
				Active = 1,
				LastUserActivity = (SELECT LastActivity FROM ForumsRead WHERE Username = @UserName AND ForumID = F.ForumID),
				SortOrder,
				IsPrivate = ISNULL((SELECT DISTINCT ForumID FROM PrivateForums WHERE ForumID = F.ForumID), 0),
				DisplayMask
			FROM 
				Forums F (nolock)
			WHERE 
				Active = 1 AND
				ForumID NOT IN (SELECT ForumID from PrivateForums) OR
				ForumID IN (SELECT ForumID FROM PrivateForums WHERE RoleName IN (SELECT RoleName from UsersInRoles WHERE username = @UserName))
			ORDER BY
				Name
                ELSE
			-- get JUST the active forums
			SELECT
				ForumID,
				ForumGroupId,
				ParentId,
				Name,
				Description,
				DateCreated,
				DaysToView,
				Moderated,
				TotalPosts,
				TotalTopics = TotalThreads,
				MostRecentPostID,
				MostRecentThreadID,
				MostRecentPostDate,
				MostRecentPostAuthor,
				Active = 1,
				LastUserActivity = NULL,
				SortOrder,
				IsPrivate = 0,
				DisplayMask
			FROM 
				Forums F (nolock)
			WHERE 
				Active = 1 AND
				ForumID NOT IN (SELECT ForumID from PrivateForums)
			ORDER BY
				Name

	ELSE
                -- Is a User Specified?
                IF @UserName IS NOT NULL

			-- get all of the forums
			SELECT
				ForumID,
				ForumGroupId,
				ParentId,
				Name,
				Description,
				DateCreated,
				DaysToView,
				Moderated,
				TotalPosts,
				TotalTopics = TotalThreads,
				MostRecentPostID,
				MostRecentThreadID,
				MostRecentPostDate,
				MostRecentPostAuthor,
				Active,
				LastUserActivity = (SELECT LastActivity FROM ForumsRead WHERE Username = @UserName AND ForumID = F.ForumID),
				SortOrder,
				IsPrivate = ISNULL((SELECT DISTINCT ForumID FROM PrivateForums WHERE ForumID = F.ForumID), 0),
				DisplayMask
			FROM 
				Forums F (nolock)
			WHERE 
				(ForumID NOT IN (SELECT ForumID from PrivateForums) OR
				ForumID IN (SELECT ForumID FROM PrivateForums WHERE RoleName IN (SELECT RoleName from UsersInRoles WHERE username = @UserName)))
			ORDER BY
				Name

                ELSE
			-- get JUST the active forums
			SELECT
				ForumID,
				ForumGroupId,
				ParentId,
				Name,
				Description,
				DateCreated,
				DaysToView,
				Moderated,
				TotalPosts,
				TotalTopics = TotalThreads,
				MostRecentPostID,
				MostRecentThreadID,
				MostRecentPostDate,
				MostRecentPostAuthor,
				Active = 1,
				LastUserActivity = NULL,
				SortOrder,
				IsPrivate = 0,
				DisplayMask
			FROM 
				Forums F (nolock)
			WHERE 
				ForumID NOT IN (SELECT ForumID from PrivateForums)
			ORDER BY
				Name

















GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO



CREATE        PROCEDURE forums_GetAllForumsByForumGroupId
(
	@ForumGroupId int,
	@GetAllForums	bit = 0
	
)
AS
	-- return all of the columns in all of the forums
	IF @GetAllForums = 0
		-- get JUST the active forums
		SELECT
			ForumID,
			ForumGroupId,
			Name,
			Description,
			DateCreated,
			DaysToView,
			Moderated,
			TotalPosts = (SELECT COUNT(*) FROM Posts P (nolock) WHERE P.ForumID = F.ForumID AND P.Approved=1),
			TotalTopics = (SELECT COUNT(*) FROM Posts P2 (nolock) WHERE P2.ForumID = F.ForumID AND P2.Approved=1 AND P2.PostLevel=1),
			MostRecentPostID = (SELECT TOP 1 PostID FROM Posts P3 (nolock) WHERE P3.ForumID = F.ForumID AND P3.Approved=1 AND PostDate < GetDate() ORDER BY PostDate DESC),
			MostRecentPostDate = (SELECT TOP 1 PostDate FROM Posts P3 (nolock) WHERE P3.ForumID = F.ForumID AND P3.Approved=1 AND PostDate < GetDate() ORDER BY PostDate DESC),
			MostRecentPostAuthor = (SELECT TOP 1 Username FROM Posts P3 (nolock) WHERE P3.ForumID = F.ForumID AND P3.Approved=1 AND PostDate < GetDate() ORDER BY PostDate DESC),
			Active = 1,
			SortOrder
		FROM Forums F (nolock)
		WHERE Active = 1 AND
			ForumGroupId = @ForumGroupId
	ELSE
		-- get all of the forums
		SELECT
			ForumID,
			ForumGroupId,
			Name,
			Description,
			DateCreated,
			DaysToView,
			Moderated,
			TotalPosts = (SELECT COUNT(*) FROM Posts P (nolock) WHERE P.ForumID = F.ForumID AND P.Approved=1),
			TotalTopics = (SELECT COUNT(*) FROM Posts P2 (nolock) WHERE P2.ForumID = F.ForumID AND P2.Approved=1 AND P2.PostLevel=1),
			MostRecentPostID = (SELECT TOP 1 PostID FROM Posts P3 (nolock) WHERE P3.ForumID = F.ForumID AND P3.Approved=1 AND PostDate < GetDate() ORDER BY PostDate DESC),
			MostRecentPostDate = (SELECT TOP 1 PostDate FROM Posts P3 (nolock) WHERE P3.ForumID = F.ForumID AND P3.Approved=1 AND PostDate < GetDate() ORDER BY PostDate DESC),
			MostRecentPostAuthor = (SELECT TOP 1 Username FROM Posts P3 (nolock) WHERE P3.ForumID = F.ForumID AND P3.Approved=1 AND PostDate < GetDate() ORDER BY PostDate DESC),
			Active,
			SortOrder
		FROM Forums F (nolock)
		WHERE 
			ForumGroupId = @ForumGroupId



















GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



CREATE      PROCEDURE forums_GetAllMessages
(
	@ForumID int,
	@ViewType int,
	@PagesBack int
)
 AS
-- The returned Recordset depends on the viewType option chosen
--	0 == Flat display
--	1 == Mixed display (just top-level posts)
--	2 == Threaded display
DECLARE @DaysToView int, @StartDate datetime, @StopDate datetime
SELECT @DaysToView = DaysToView FROM Forums (nolock) WHERE ForumID = @ForumID
SELECT @StartDate = DateAdd(dd, -@PagesBack * @DaysToView, getdate()) 
SELECT @StopDate = DateAdd(dd, -@DaysToView, @StartDate)
IF @ViewType = 0
	-- FLAT DISPLAY
	SELECT 
		Subject,
		PostID,
		ForumID = @ForumID,
		ThreadID,
		ParentID,
		PostLevel,
		SortOrder,
		Approved,
		PostDate,
		ThreadDate,	
		UserName,
		Replies = (SELECT COUNT(*) FROM Posts P2 (nolock) WHERE P2.ParentID = P.PostID AND P2.PostLevel != 1),
		Body,
		TotalMessagesInThread = 0, -- not used
		TotalViews,
		IsLocked
	FROM Posts P (nolock)
	WHERE Approved = 1 AND ForumID = @ForumID AND PostDate >= @StopDate AND PostDate <= @StartDate
	ORDER BY PostDate DESC
ELSE IF @ViewType = 1
	-- MIXED DISPLAY
	SELECT 
		Subject,
		PostID,
		ThreadID,
		ParentID,
		ForumID = @ForumID,
		PostLevel,
		SortOrder,
		PostDate = (SELECT Max(PostDate) FROM Posts WHERE P.ThreadID = ThreadID),
		Approved,
		ThreadDate,
		UserName,
		Replies = (SELECT COUNT(*) FROM Posts WHERE P.ThreadID = ThreadID AND PostLevel != 1),
		Body,
		TotalMessagesInThread = 0, -- not used
		TotalViews,
		IsLocked
	FROM Posts P (nolock)
	WHERE Approved = 1 AND ForumID = @ForumID AND PostLevel = 1 AND ThreadDate >= @StopDate AND ThreadDate <= @StartDate
	ORDER BY PostDate DESC
	
ELSE IF @ViewType = 2
	-- THREADED DISPLAY
	SELECT 
		Subject,
		PostID,
		ThreadID,
		ParentID,
		ForumID = @ForumID,
		PostLevel,
		SortOrder,
		Approved,
		PostDate,
		ThreadDate,
		UserName,
		Replies = 0,
		Body,
		TotalMessagesInThread = 0, -- not used
		TotalViews,
		IsLocked
	FROM Posts P (nolock)
	WHERE Approved = 1 AND ForumID = @ForumID AND ThreadDate >= @StopDate AND ThreadDate <= @StartDate
	ORDER BY ThreadID DESC, SortOrder
















GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO


CREATE PROCEDURE forums_GetAllRoles
AS
BEGIN
    SELECT 
        Rolename 
    FROM 
        UserRoles
END

GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

CREATE                   PROCEDURE forums_GetAllTopicsPaged
(
	@ForumID int,
	@PageSize int,
	@PageIndex int, 
	@DateFilter Datetime,    -- Filter returned records by date 
	@UserName nvarchar (50),
	@UnReadTopicsOnly bit    -- 0 All / 1 Unread only
)
AS

DECLARE @PageLowerBound int
DECLARE @PageUpperBound int

-- Set the page bounds
SET @PageLowerBound = @PageSize * @PageIndex
SET @PageUpperBound = @PageLowerBound + @PageSize + 1

-- Create a temp table to store the select results
CREATE TABLE #PageIndex 
(
	IndexId int IDENTITY (1, 1) NOT NULL,
	PostID int,
	PostDate datetime
)

IF @UnReadTopicsOnly = 0 AND @DateFilter < DateAdd(ww, -2, GetDate()) AND (@PageSize * @PageIndex) < 100
	-- INSERT into the temp table
	INSERT INTO #PageIndex

	SELECT TOP 100
		PostID,
		PinnedDate = (SELECT Max(PinnedDate) FROM Posts WHERE ThreadID = P.ThreadID)
	FROM 
		Posts P 
	WHERE 
		PostLevel = 1 AND 
		ForumID = @ForumID AND 
		Approved = 1 AND
		ThreadDate >= @DateFilter
	ORDER BY 
		PinnedDate DESC
ELSE IF @UserName IS NULL OR @UnReadTopicsOnly = 0
BEGIN
	-- INSERT into the temp table
	INSERT INTO #PageIndex

	SELECT 
		PostID,
		PinnedDate = (SELECT Max(PinnedDate) FROM Posts WHERE ThreadID = P.ThreadID)
	FROM 
		Posts P 
	WHERE 
		PostLevel = 1 AND 
		ForumID = @ForumID AND 
		Approved = 1 AND
		ThreadDate >= @DateFilter
	ORDER BY 
		PinnedDate DESC


END
ELSE
BEGIN
	-- INSERT into the temp table
	INSERT INTO #PageIndex

	SELECT 
		PostID,
		PinnedDate = (SELECT Max(PinnedDate) FROM Posts WHERE ThreadID = P.ThreadID)
	FROM 
		Posts P 
	WHERE 
		PostLevel = 1 AND 
		ForumID = @ForumID AND 
		Approved = 1 AND
		ThreadDate >= @DateFilter AND
		P.PostID NOT IN (SELECT PostsRead.PostID FROM PostsRead WHERE PostsRead.Username = @UserName)
	ORDER BY 
		PinnedDate DESC
END

IF @UserName IS NULL
BEGIN
	SELECT 
		Subject,
		Body,
		P.PostID,
		ThreadID,
		ParentID,
		PostDate = (SELECT Max(PostDate) FROM Posts WHERE ThreadID = P.ThreadID),
		ThreadDate,
		PinnedDate,
		UserName,
		Replies = (SELECT COUNT(*) FROM Posts WHERE P.ThreadID = ThreadID AND PostLevel != 1 AND Approved = 1),
		Body,
		TotalViews,
		IsLocked,
		IsPinned,
		HasRead = 0,
		MostRecentPostAuthor = (SELECT TOP 1 Username FROM Posts WHERE P.ThreadID = ThreadID AND Approved = 1 ORDER BY PostDate DESC),
		MostRecentPostID = (SELECT TOP 1 PostID FROM Posts WHERE P.ThreadID = ThreadID AND Approved = 1 ORDER BY PostDate DESC)
	FROM 
		Posts P (nolock),
		#PageIndex PageIndex
	WHERE 
		P.PostID = PageIndex.PostID AND
		PageIndex.IndexID > @PageLowerBound AND
		PageIndex.IndexID < @PageUpperBound
	ORDER BY 
		PageIndex.IndexID
END
ELSE
BEGIN
	IF @UnReadTopicsOnly = 1
	BEGIN


		-- Get Unread Topics only

  		SELECT 
			Subject,
			Body,
			P.PostID,
			ThreadID,
			ParentID,
   			PostDate = (SELECT Max(PostDate) FROM Posts WHERE ThreadID = P.ThreadID),
			ThreadDate,
			PinnedDate,
			UserName,
			Replies = (SELECT COUNT(*) FROM Posts WHERE P.ThreadID = ThreadID AND Approved = 1 AND PostLevel != 1),
			Body,
			TotalViews,
			IsLocked,
			IsPinned,
			HasRead = dbo.HasReadPost(@UserName, P.PostID, P.ForumID),
			MostRecentPostAuthor = (SELECT TOP 1 Username FROM Posts WHERE P.ThreadID = ThreadID AND Approved = 1 ORDER BY PostDate DESC),
			MostRecentPostID = (SELECT TOP 1 PostID FROM Posts WHERE P.ThreadID = ThreadID AND Approved = 1 ORDER BY PostDate DESC)
		FROM 
			Posts P (nolock),
			#PageIndex PageIndex
		WHERE 
			P.ForumID = @ForumID AND 
			PostLevel = 1 AND 
			ThreadDate >= @DateFilter AND
			P.PostID NOT IN (SELECT PostsRead.PostID FROM PostsRead WHERE PostsRead.Username = @UserName) AND
			P.PostId >= (select MarkReadAfter from ForumsRead where username = @UserName and forumid = @ForumID) AND
			P.PostID = PageIndex.PostID AND
			PageIndex.IndexID > @PageLowerBound AND
			PageIndex.IndexID < @PageUpperBound
		ORDER BY 
			PageIndex.IndexID
	END
	ELSE
	BEGIN
  		SELECT 
			Subject,
			Body,
			P.PostID,
			ThreadID,
			ParentID,
			PostDate = (SELECT Max(PostDate) FROM Posts WHERE ThreadID = P.ThreadID),
			ThreadDate,
			PinnedDate,
			UserName,
			Replies = (SELECT COUNT(*) FROM Posts WHERE P.ThreadID = ThreadID AND Approved = 1 AND PostLevel != 1),
			Body,
			TotalViews,
			IsLocked,
			IsPinned,
			HasRead = dbo.HasReadPost(@UserName, P.PostID, P.ForumID),
			MostRecentPostAuthor = (SELECT TOP 1 Username FROM Posts WHERE P.ThreadID = ThreadID AND Approved = 1 ORDER BY PostDate DESC),
			MostRecentPostID = (SELECT TOP 1 PostID FROM Posts WHERE P.ThreadID = ThreadID AND Approved = 1 ORDER BY PostDate DESC)
		FROM 
			Posts P (nolock),
			#PageIndex PageIndex
		WHERE 
			P.PostID = PageIndex.PostID AND
			PageIndex.IndexID > @PageLowerBound AND
			PageIndex.IndexID < @PageUpperBound
		ORDER BY 
			PageIndex.IndexID
	END

	-- Update Forum View date
	IF EXISTS (SELECT ForumID FROM ForumsRead WHERE ForumID = @ForumID and Username = @UserName)
		-- Row exists, update
		UPDATE 
			ForumsRead
		SET
			LastActivity = GetDate()
		WHERE
			ForumID = @ForumID AND
			Username = @UserName
	ELSE
		-- Row does not exist, insert
		INSERT INTO
			ForumsRead
			(ForumID, Username, MarkReadAfter, LastActivity)
		VALUES
			(@ForumID, @UserName, 0, GetDate())
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



CREATE                    PROCEDURE forums_GetAllUnmoderatedTopicsPaged
(
	@ForumID int,
	@PageSize int,
	@PageIndex int, 
	@UserName nvarchar (50)
)
AS

DECLARE @PageLowerBound int
DECLARE @PageUpperBound int

-- Set the page bounds
SET @PageLowerBound = @PageSize * @PageIndex
SET @PageUpperBound = @PageLowerBound + @PageSize + 1

-- Create a temp table to store the select results
CREATE TABLE #PageIndex 
(
	IndexID int IDENTITY (1, 1) NOT NULL,
	PostID int,
	PostDate datetime
)
BEGIN
	-- INSERT into the temp table
	INSERT INTO #PageIndex

	SELECT 
		PostID,
		PinnedDate
	FROM 
		POSTS P 
	WHERE 
		ForumID = @ForumID AND 
		Approved = 0
	ORDER BY 
		PinnedDate DESC

	-- Now get the posts
	SELECT 
		Subject,
		Body,
		P.PostID,
		ThreadID,
		ParentID,
		P.PostDate,
		ThreadDate,
		PinnedDate,
		UserName,
		Replies = (SELECT COUNT(*) FROM Posts WHERE P.ThreadID = ThreadID AND PostLevel != 1 AND Approved = 1),
		Body,
		TotalViews,
		IsLocked,
		IsPinned,
		HasRead = 0,
		MostRecentPostAuthor = '',
		MostRecentPostID = 0
	FROM 
		Posts P (nolock),
		#PageIndex PageIndex
	WHERE 
		P.PostID = PageIndex.PostID AND
		PageIndex.IndexID > @PageLowerBound AND
		PageIndex.IndexID < @PageUpperBound
	ORDER BY 
		PageIndex.IndexID

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





CREATE                      PROCEDURE forums_GetAllUsers
(
	@PageIndex int,
	@PageSize int,
	@SortBy int,
	@SortOrder bit,
	@UserNameBeginsWith nvarchar(1)
)
 AS
BEGIN
DECLARE @PageLowerBound int
DECLARE @PageUpperBound int

-- Set the page bounds
SET @PageLowerBound = @PageSize * @PageIndex
SET @PageUpperBound = @PageLowerBound + @PageSize + 1

-- Create a temp table to store the select results
CREATE TABLE #PageIndexForUsers 
(
	IndexID int IDENTITY (1, 1) NOT NULL,
	Username nvarchar(50) COLLATE SQL_Latin1_General_CP1_CI_AS
)	

-- Special case depending on what the user wants and how they want it ordered by

-- Sort by Date Joined
IF @SortBy = 0 AND @SortOrder = 0
    IF @UserNameBeginsWith IS NULL
	    INSERT INTO #PageIndexForUsers (Username)
	    SELECT Username FROM Users WHERE Approved = 1 AND DisplayInMemberList = 1 ORDER BY DateCreated
    ELSE
	    INSERT INTO #PageIndexForUsers (Username)
	    SELECT Username FROM Users WHERE Approved = 1 AND DisplayInMemberList = 1 AND LEFT(UserName, 1) = @UserNameBeginsWith ORDER BY DateCreated
ELSE IF @SortBy = 0 AND @SortOrder = 1
    IF @UserNameBeginsWith IS NULL
	    INSERT INTO #PageIndexForUsers (Username)
	    SELECT Username FROM Users WHERE Approved = 1 AND DisplayInMemberList = 1  ORDER BY DateCreated DESC
    ELSE
	    INSERT INTO #PageIndexForUsers (Username)
	    SELECT Username FROM Users WHERE Approved = 1 AND DisplayInMemberList = 1  AND LEFT(UserName, 1) = @UserNameBeginsWith ORDER BY DateCreated DESC

-- Sort by username
IF @SortBy = 1 AND @SortOrder = 0
    IF @UserNameBeginsWith IS NULL
	    INSERT INTO #PageIndexForUsers (Username)
	    SELECT Username FROM Users WHERE Approved = 1 AND DisplayInMemberList = 1  ORDER BY Username
    ELSE
	    INSERT INTO #PageIndexForUsers (Username)
	    SELECT Username FROM Users WHERE Approved = 1 AND DisplayInMemberList = 1  AND LEFT(UserName, 1) = @UserNameBeginsWith ORDER BY Username
ELSE IF @SortBy = 1 AND @SortOrder = 1
    IF @UserNameBeginsWith IS NULL
	    INSERT INTO #PageIndexForUsers (Username)
	    SELECT Username FROM Users WHERE Approved = 1 AND DisplayInMemberList = 1  ORDER BY Username DESC
    ELSE
	    INSERT INTO #PageIndexForUsers (Username)
	    SELECT Username FROM Users WHERE Approved = 1 AND DisplayInMemberList = 1  AND LEFT(UserName, 1) = @UserNameBeginsWith ORDER BY Username

-- Sort by Website
IF @SortBy = 2 AND @SortOrder = 0
    IF @UserNameBeginsWith IS NULL
	    INSERT INTO #PageIndexForUsers (Username)
	    SELECT Username FROM Users WHERE Approved = 1 AND DisplayInMemberList = 1  ORDER BY Url DESC
    ELSE
	    INSERT INTO #PageIndexForUsers (Username)
	    SELECT Username FROM Users WHERE Approved = 1 AND DisplayInMemberList = 1  AND LEFT(UserName, 1) = @UserNameBeginsWith ORDER BY Url DESC
ELSE IF @SortBy = 2 AND @SortOrder = 1
    IF @UserNameBeginsWith IS NULL
	    INSERT INTO #PageIndexForUsers (Username)
	    SELECT Username FROM Users WHERE Approved = 1 AND DisplayInMemberList = 1  ORDER BY Url
    ELSE
	    INSERT INTO #PageIndexForUsers (Username)
	    SELECT Username FROM Users WHERE Approved = 1 AND DisplayInMemberList = 1  AND LEFT(UserName, 1) = @UserNameBeginsWith ORDER BY Url DESC

-- Sort by Last Active
IF @SortBy = 3 AND @SortOrder = 0
    IF @UserNameBeginsWith IS NULL
	    INSERT INTO #PageIndexForUsers (Username)
	    SELECT Username FROM Users WHERE Approved = 1 AND DisplayInMemberList = 1  ORDER BY LastActivity DESC
    ELSE
	    INSERT INTO #PageIndexForUsers (Username)
	    SELECT Username FROM Users WHERE Approved = 1 AND DisplayInMemberList = 1  AND LEFT(UserName, 1) = @UserNameBeginsWith ORDER BY LastActivity DESC
ELSE IF @SortBy = 3 AND @SortOrder = 1
    IF @UserNameBeginsWith IS NULL
	    INSERT INTO #PageIndexForUsers (Username)
	    SELECT Username FROM Users WHERE Approved = 1 AND DisplayInMemberList = 1  ORDER BY LastActivity
    ELSE
	    INSERT INTO #PageIndexForUsers (Username)
	    SELECT Username FROM Users WHERE Approved = 1 AND DisplayInMemberList = 1  AND LEFT(UserName, 1) = @UserNameBeginsWith ORDER BY LastActivity

-- Sort by TotalPosts
IF @SortBy = 4 AND @SortOrder = 1
    IF @UserNameBeginsWith IS NULL
	    INSERT INTO #PageIndexForUsers (Username)
	    SELECT Username FROM Users WHERE Approved = 1 AND DisplayInMemberList = 1  ORDER BY TotalPosts DESC
    ELSE
	    INSERT INTO #PageIndexForUsers (Username)
	    SELECT Username FROM Users WHERE Approved = 1 AND DisplayInMemberList = 1  AND LEFT(UserName, 1) = @UserNameBeginsWith ORDER BY TotalPosts DESC
ELSE IF @SortBy = 4 AND @SortOrder = 0
    IF @UserNameBeginsWith IS NULL
	    INSERT INTO #PageIndexForUsers (Username)
	    SELECT Username FROM Users WHERE Approved = 1 AND DisplayInMemberList = 1  ORDER BY TotalPosts
    ELSE
	    INSERT INTO #PageIndexForUsers (Username)
	    SELECT Username FROM Users WHERE Approved = 1 AND DisplayInMemberList = 1  AND LEFT(UserName, 1) = @UserNameBeginsWith ORDER BY TotalPosts

-- Get the user details
SELECT
	U.Username,
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
	FlatView,
	IsModerator = (SELECT Count(*) FROM Moderators WHERE Username = U.Username),
	AvatarUrl,
	Attributes
FROM 
	Users U (nolock),
	#PageIndexForUsers
WHERE 
	Approved = 1 AND
	U.Username = #PageIndexForUsers.Username AND
	#PageIndexForUsers.IndexID > @PageLowerBound AND
	#PageIndexForUsers.IndexID < @PageUpperBound
ORDER BY
	#PageIndexForUsers.IndexID
END








GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO



CREATE    PROCEDURE forums_GetAnonymousUsersOnline
AS
BEGIN

	SELECT AnonymousUserCount = Count(*) FROM AnonymousUsers

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



create procedure forums_GetBannedUsers AS
	-- return all of the banned users
	SELECT
		UserName,
		Email,
		DateCreated
	FROM Users (nolock)
	WHERE Approved = 0
	ORDER BY UserName














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



create procedure forums_GetEmailInfo
(
	@EmailID	int
) 
AS
	-- Return information about a row in the Email table
	SELECT
		Subject,
		FromAddress,
		Importance,
		EmailID,
		Description,
		Message
	FROM Emails (nolock)
	WHERE EmailID = @EmailID














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



create procedure forums_GetEmailList AS
	-- get all of the emails
	SELECT
		EmailID,
		Description,
		Subject,
		Importance,
		FromAddress,
		Message
	FROM Emails (nolock)
	ORDER BY Description














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO




CREATE     PROCEDURE forums_GetForumByPostID
(
	@PostID int
)
 AS
	-- get the Forum ID for a particular post
	SELECT
		ForumID,
		ForumGroupId,
		ParentId,
		Name,
		Description,
		DateCreated,
		Moderated,
		DaysToView,
		Active,
		SortOrder,
		IsPrivate = ISNULL((SELECT DISTINCT ForumID FROM PrivateForums WHERE ForumID = F.ForumID), 0),
		DisplayMask
	FROM Forums F (nolock)
	WHERE ForumID = (SELECT ForumID FROM Posts (nolock) WHERE PostID = @PostID)

















GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



CREATE      PROCEDURE forums_GetForumByThreadID
(
	@ThreadID int
)
 AS
	-- get the Forum ID for a particular post
	SELECT
		ForumID,
		ForumGroupId,
		Name,
		Description,
		DateCreated,
		Moderated,
		DaysToView,
		Active,
		SortOrder,
		IsPrivate = ISNULL((SELECT DISTINCT ForumID FROM PrivateForums WHERE ForumID = F.ForumID), 0)
	FROM Forums F (nolock)
	WHERE ForumID = (SELECT DISTINCT ForumID FROM Posts (nolock) WHERE ThreadID = @ThreadID)


















GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO



CREATE             PROCEDURE forums_GetForumGroupByForumID
(
	@ForumID int
)
AS
	SELECT 
		ForumGroups.ForumGroupID,
		ForumGroups.Name,
		ForumGroups.SortOrder
	FROM
		ForumGroups, Forums
	WHERE
		Forums.ForumGroupID = ForumGroups.ForumGroupID AND
		Forums.ForumID = @ForumID
		














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO



CREATE            PROCEDURE forums_GetForumGroupNameByID
(
	@ForumID int
)
AS
	SELECT 
		ForumGroups.Name,
		ForumGroups.SortOrder
	FROM
		ForumGroups, Forums
	WHERE
		Forums.ForumGroupID = ForumGroups.ForumGroupID AND
		Forums.ForumID = @ForumID
		











GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



CREATE           PROCEDURE forums_GetForumInfo
(
	@ForumID int,
	@UserName nvarchar(50)
)
AS
IF @UserName IS NOT NULL
	SELECT
		ForumID = @ForumID,
		ForumGroupId,
		ParentId,
		Name,
		Description,
		Moderated,
		DaysToView,
		DateCreated,
		Active,
		TotalTopics = TotalThreads,
		SortOrder,
		IsPrivate = ISNULL((SELECT DISTINCT ForumID FROM PrivateForums WHERE ForumID = F.ForumID), 0),
		DisplayMask
	FROM 
		Forums F (nolock)
	WHERE 
		ForumID = @ForumID AND
		(ForumID NOT IN (SELECT ForumID from PrivateForums) OR
		ForumID IN (SELECT ForumID FROM PrivateForums WHERE RoleName IN (SELECT RoleName from UsersInRoles WHERE username = @UserName)))
ELSE
	SELECT
		ForumID = @ForumID,
		ForumGroupId,
		ParentId,
		Name,
		Description,
		Moderated,
		DaysToView,
		DateCreated,
		Active,
		TotalTopics = TotalThreads,
		SortOrder,
		IsPrivate = 0,
		DisplayMask
	FROM 
		Forums F (nolock)
	WHERE 
		ForumID = @ForumID AND
		ForumID NOT IN (SELECT ForumID from PrivateForums)
























GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO



create procedure forums_GetForumMessageTemplateList
 AS
SELECT 
	MessageId,
	Title,
	Body
FROM
	Messages






GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



CREATE  PROCEDURE forums_GetForumModerators
(
	@ForumID	int
)
 AS
	-- get a list of forum moderators
	SELECT 
		UserName, EmailNotification, DateCreated
	FROM 
		Moderators (nolock)
	WHERE 
		ForumID = @ForumID OR ForumID = 0















GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



create procedure forums_GetForumViewByUsername
(
	@UserName nvarchar(50)
)
 AS
	-- get the forumView for the user
	SELECT
		ForumView
	FROM Users (nolock)
	WHERE UserName = @UserName














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO



CREATE         PROCEDURE forums_GetForumsByForumGroupId
(
	@ForumGroupId int,
	@GetAllForums	bit = 0,
	@UserName nvarchar(50)
	
)
AS

	-- Do we have a username
	IF @UserName IS NOT NULL
	BEGIN
		-- return all of the columns in all of the forums
		IF @GetAllForums = 0
			-- get JUST the active forums
			SELECT
				ForumID,
				ForumGroupId,
				ParentId,
				Name,
				Description,
				DateCreated,
				DaysToView,
				Moderated,
				TotalPosts,
				TotalTopics = TotalThreads,
				MostRecentPostID,
				MostRecentThreadID,
				MostRecentPostDate,
				MostRecentPostAuthor,
				Active = 1,
				LastUserActivity = (SELECT LastActivity FROM ForumsRead WHERE Username = @UserName AND ForumID = F.ForumID),
				SortOrder,
				IsPrivate = ISNULL((SELECT DISTINCT ForumID FROM PrivateForums WHERE ForumID = F.ForumID), 0),
				DisplayMask
			FROM Forums F (nolock)
			WHERE Active = 1 AND
				ForumGroupId = @ForumGroupId
		ELSE
			-- get all of the forums
			SELECT
				ForumID,
				ForumGroupId,
				ParentId,
				Name,
				Description,
				DateCreated,
				DaysToView,
				Moderated,
				TotalPosts,
				TotalTopics = TotalThreads,
				MostRecentPostID,
				MostRecentThreadID,
				MostRecentPostDate,
				MostRecentPostAuthor,
				Active,
				LastUserActivity = (SELECT LastActivity FROM ForumsRead WHERE Username = @UserName AND ForumID = F.ForumID),
				SortOrder,
				IsPrivate = ISNULL((SELECT DISTINCT ForumID FROM PrivateForums WHERE ForumID = F.ForumID), 0),
				DisplayMask
			FROM Forums F (nolock)
			WHERE 
				ForumGroupId = @ForumGroupId
	

	END
	ELSE
	BEGIN
		-- return all of the columns in all of the forums
		IF @GetAllForums = 0
			-- get JUST the active forums
			SELECT
				ForumID,
				ForumGroupId,
				ParentId,
				Name,
				Description,
				DateCreated,
				DaysToView,
				Moderated,
				TotalPosts,
				TotalTopics = TotalThreads,
				MostRecentPostID,
				MostRecentThreadID,
				MostRecentPostDate,
				MostRecentPostAuthor,
				Active = 1,
				SortOrder,
				IsPrivate = ISNULL((SELECT DISTINCT ForumID FROM PrivateForums WHERE ForumID = F.ForumID), 0),
				DisplayMask
			FROM Forums F (nolock)
			WHERE Active = 1 AND
				ForumGroupId = @ForumGroupId
		ELSE
			-- get all of the forums
			SELECT
				ForumID,
				ForumGroupId,
				ParentId,
				Name,
				Description,
				DateCreated,
				DaysToView,
				Moderated,
				TotalPosts,
				TotalTopics = TotalThreads,
				MostRecentPostID,
				MostRecentThreadID,
				MostRecentPostDate,
				MostRecentPostAuthor,
				Active,
				SortOrder,
				IsPrivate = ISNULL((SELECT DISTINCT ForumID FROM PrivateForums WHERE ForumID = F.ForumID), 0),
				DisplayMask
			FROM Forums F (nolock)
			WHERE 
				ForumGroupId = @ForumGroupId
	
	END


GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO



CREATE         PROCEDURE forums_GetForumsForModerationByForumGroupId
(
	@ForumGroupId int,
	@UserName nvarchar(50)
	
)
AS

IF (0 = (SELECT ForumID FROM Moderators WHERE Username = @UserName))

	SELECT
		ForumID,
		ForumGroupId,
		Name,
		Description,
		DateCreated,
		DaysToView,
		Moderated,
		TotalPosts,
		TotalTopics = TotalThreads,
		MostRecentPostID,
		MostRecentThreadID,
		MostRecentPostDate,
		MostRecentPostAuthor,
		TotalPostsAwaitingModeration = (SELECT COUNT(*) FROM Posts P (nolock) WHERE P.ForumID = F.ForumID and P.Approved = 0),
		Active = 1,
		LastUserActivity = (SELECT LastActivity FROM ForumsRead WHERE Username = @UserName AND ForumID = F.ForumID),
		SortOrder,
		IsPrivate = 0
	FROM 
		Forums F (nolock)
	WHERE 
		Active = 1 AND
		ForumGroupId = @ForumGroupId AND
		(ForumID NOT IN (SELECT ForumID from PrivateForums) OR
		ForumID IN (SELECT ForumID FROM PrivateForums WHERE RoleName IN (SELECT RoleName from UsersInRoles WHERE username = @UserName))) AND
		(SELECT COUNT(*) FROM Posts P (nolock) WHERE P.ForumID = F.ForumID and P.Approved = 0) > 0
ELSE

	SELECT
		ForumID,
		ForumGroupId,
		Name,
		Description,
		DateCreated,
		DaysToView,
		Moderated,
		TotalPosts,
		TotalTopics = TotalThreads,
		MostRecentPostID,
		MostRecentThreadID,
		MostRecentPostDate,
		MostRecentPostAuthor,
		TotalPostsAwaitingModeration = (SELECT COUNT(*) FROM Posts P (nolock) WHERE P.ForumID = F.ForumID and P.Approved = 0),
		Active = 1,
		LastUserActivity = (SELECT LastActivity FROM ForumsRead WHERE Username = @UserName AND ForumID = F.ForumID),
		SortOrder,
		IsPrivate = 0
	FROM 
		Forums F (nolock)
	WHERE 
		Active = 1 AND
		ForumGroupId = @ForumGroupId AND
		(ForumID NOT IN (SELECT ForumID from PrivateForums) OR
		ForumID IN (SELECT ForumID FROM PrivateForums WHERE RoleName IN (SELECT RoleName from UsersInRoles WHERE username = @UserName))) AND
		ForumID IN (SELECT ForumID FROM Moderators WHERE Username = @UserName) AND
		(SELECT COUNT(*) FROM Posts P (nolock) WHERE P.ForumID = F.ForumID and P.Approved = 0) > 0






GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



create procedure forums_GetForumsModeratedByUser
(
	@UserName	nvarchar(50)
)
 AS
	-- determine if this user can moderate ALL forums
	IF EXISTS(SELECT NULL FROM Moderators (nolock) WHERE ForumID = 0 AND Username = @UserName)
		SELECT ForumID, ForumName = 'All Forums', EmailNotification, DateCreated FROM Moderators (nolock)
		WHERE ForumID = 0 AND Username = @UserName
	ELSE
		-- get all of the forums moderated by this particular user
		SELECT
			M.ForumID,
			EmailNotification,
			ForumName = F.Name,
			M.DateCreated
		FROM Moderators M (nolock)
			INNER JOIN Forums F (nolock) ON
				F.ForumID = M.ForumID
		WHERE Username = @UserName














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



CREATE  PROCEDURE forums_GetForumsNotModeratedByUser
(
	@UserName	nvarchar(50)
)
 AS
	-- determine if this user can moderate ALL forums
	IF NOT EXISTS(SELECT NULL FROM Moderators (nolock) WHERE ForumID = 0 AND Username = @UserName)
		-- get all of the forums NOT moderated by this particular user
		SELECT ForumID =  0, ForumName =  'All Forums'
		UNION
		SELECT
			ForumID,
			ForumName = F.Name
		FROM Forums F (nolock) 
		WHERE ForumID NOT IN (SELECT ForumID FROM Moderators (nolock) WHERE Username = @UserName)















GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



create                PROCEDURE forums_GetMessage
(
	@MessageId int
)
 AS
BEGIN
	SELECT
		Title,
		Body
	FROM
		Messages
	WHERE
		MessageId = @MessageId
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



CREATE       PROCEDURE forums_GetModeratedForums
(
	@UserName nvarchar(50)
)
 AS
	-- returns a list of posts awaiting moderation
	-- the posts returned are those that this user can work on
	-- if Moderators.ForumID = 0 for this user, then they can moderate ALL forums
	IF EXISTS(SELECT NULL FROM Moderators (nolock) WHERE UserName=@UserName AND ForumID=0)
		-- return ALL posts awaiting moderation
		SELECT
			ForumID,
			ForumGroupId,
			Name,
			Description,
			DateCreated,
			Moderated,
			DaysToView,
			Active,
			SortOrder
		FROM 
			Forums
		WHERE 	
			Active = 1
		ORDER BY 
			DateCreated
	ELSE
		-- return only those posts in the forum this user can moderate
		SELECT
			ForumID,
			ForumGroupId,
			Name,
			Description,
			DateCreated,
			Moderated,
			DaysToView,
			Active,
			SortOrder

		FROM 
			Forums
		WHERE 
			Active = 1 AND 
			ForumID IN (SELECT ForumID FROM Moderators (nolock) WHERE UserName=@UserName)
		ORDER BY 
			DateCreated



















GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



CREATE      PROCEDURE forums_GetModeratedPosts
(
	@UserName nvarchar(50)
)
 AS
	-- returns a list of posts awaiting moderation
	-- the posts returned are those that this user can work on
	-- if Moderators.ForumID = 0 for this user, then they can moderate ALL forums
	IF EXISTS(SELECT NULL FROM Moderators (nolock) WHERE UserName=@UserName AND ForumID=0)
		-- return ALL posts awaiting moderation
		SELECT
			PostID,
			ThreadID,
			ThreadDate,
			PostLevel,
			P.SortOrder,
			P.ParentID,
			Subject,
			Approved,
			P.ForumID,
			ForumName = F.Name,
			PostDate,
			P.UserName,
			Replies = 0,
			Body,
			TotalViews,
			IsLocked,
			HasRead = 1
		FROM Posts P (nolock)
			INNER JOIN Forums F (nolock) ON
				F.ForumID = P.ForumID
		WHERE Approved = 0
		ORDER BY P.ForumID, PostDate
	ELSE
		-- return only those posts in the forum this user can moderate
		SELECT
			PostID,
			P.ParentID,
			Approved,
			ThreadID,
			ThreadDate,
			PostLevel,
			P.SortOrder,
			Subject,
			P.ForumID,
			ForumName = F.Name,
			PostDate,
			Replies = 0,
			P.UserName,
			Body,
			TotalViews,
			IsLocked,
			HasRead = 1
		FROM Posts P (nolock)
			INNER JOIN Forums F (nolock) ON
				F.ForumID = P.ForumID
		WHERE 
			Approved = 0 AND 
			P.ForumID IN (SELECT ForumID FROM Moderators (nolock) WHERE UserName=@UserName)
		ORDER BY P.ForumID, PostDate


















GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



create procedure forums_GetModeratorsForEmailNotification
(
	@PostID	int
)
 AS
	-- get the ForumID
	DECLARE @ForumID int
	SELECT @ForumID = ForumID FROM Posts (nolock) WHERE PostID = @PostID
	SELECT
		U.Username,
		Email
	FROM Users U (nolock)
		INNER JOIN Moderators M (nolock) ON
			M.UserName = U.UserName
	WHERE (M.ForumID = @ForumID OR M.ForumID = 0) AND M.EmailNotification = 1














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



create procedure forums_GetNextPostID
(
	@ThreadID int,
	@SortOrder int,
	@ForumID int,
	@NextPostID int OUTPUT
)
 AS
SELECT TOP 1 @NextPostID = PostID
FROM Posts (nolock) 
WHERE ThreadID = @ThreadID AND ForumID = @ForumID AND SortOrder = @SortOrder+1 AND Approved = 1
IF @@ROWCOUNT < 1
	SELECT @NextPostID = 0














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



create procedure forums_GetNextThreadID
(
	@ThreadID int,
	@ForumID int,
	@NextThreadID int OUTPUT
)
 AS
SELECT TOP 1 @NextThreadID = ThreadID
FROM Posts (nolock)
WHERE ThreadID > @ThreadID AND ForumID = @ForumID AND Approved = 1
IF @@ROWCOUNT < 1
	SELECT @NextThreadID = 0














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



create procedure forums_GetParentID
(
	@PostID	int
)
 AS
	SELECT ParentID
	FROM Posts (nolock)
	WHERE PostID = @PostID














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



CREATE        PROCEDURE forums_GetPostInfo
(
	@PostID	int,
	@TrackViews bit,
	@UserName nvarchar (50)
)
 AS
BEGIN

	IF @TrackViews = 1
	BEGIN
		DECLARE @views int
	
		-- Update the counter for the number of times this post is viewed
		SELECT @views = TotalViews FROM Posts WHERE PostID = @PostID
		UPDATE Posts SET TotalViews = (@views + 1) WHERE PostID = @PostID
	END

	-- If @UserName is null it is an anonymous user
	IF @UserName IS NOT NULL
	BEGIN
		DECLARE @ForumID int
		DECLARE @PostDate datetime

		-- Mark the post as read
		-- *********************

		-- Only for PostLevel = 1
		IF EXISTS (SELECT PostID FROM Posts WHERE PostID = @PostID AND PostLevel = 1)
			IF NOT EXISTS (SELECT HasRead FROM PostsRead WHERE Username = @UserName and PostID = @PostID)
				INSERT INTO PostsRead (Username, PostID) VALUES (@UserName, @PostID)

	END

	IF @UserName IS NOT NULL
		SELECT
			Subject,
			PostID,
			UserName,
			P.ForumID,
			ForumName = (SELECT Name FROM Forums F (nolock) WHERE F.ForumID = P.ForumID),
			ParentID,
			ThreadID,
			Approved,
			PostDate,
			PostLevel,
			SortOrder,
			ThreadDate,
			Replies = (SELECT COUNT(*) FROM Posts P2 (nolock) WHERE P2.ParentID = P.PostID AND P2.PostLevel != 1),
			Body,
			TotalMessagesInThread = 0, -- not used
			TotalViews,
			IsLocked,
			HasRead = 1
		FROM 
			Posts P (nolock)
		WHERE 
			P.PostID = @PostID AND
			(ForumID NOT IN (SELECT ForumID from PrivateForums) OR
			ForumID IN (SELECT ForumID FROM PrivateForums WHERE RoleName IN (SELECT RoleName from UsersInRoles WHERE username = @UserName)))
	ELSE
		SELECT
			Subject,
			PostID,
			UserName,
			P.ForumID,
			ForumName = (SELECT Name FROM Forums F (nolock) WHERE F.ForumID = P.ForumID),
			ParentID,
			ThreadID,
			Approved,
			PostDate,
			PostLevel,
			SortOrder,
			ThreadDate,
			Replies = (SELECT COUNT(*) FROM Posts P2 (nolock) WHERE P2.ParentID = P.PostID AND P2.PostLevel != 1),
			Body,
			TotalMessagesInThread = 0, -- not used
			TotalViews,
			IsLocked,
			HasRead = 1
		FROM 
			Posts P (nolock)
		WHERE 
			P.PostID = @PostID AND
			ForumID NOT IN (SELECT ForumID from PrivateForums)


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



CREATE    PROCEDURE forums_GetPostRead
(
	@PostID int,
	@UserName nvarchar (50)
)
 AS
BEGIN
	DECLARE @HasRead bit
	SET @HasRead = 0

	IF EXISTS 
	(
		SELECT
			HasRead
		FROM
			PostsRead
		WHERE
			PostID = @PostID AND
			Username = @UserName
	)
		SET @HasRead = 1

	SELECT HasRead = @HasRead
END













GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

CREATE procedure forums_GetPrevNextThreadID (
  @PostID int,
  @NextThread bit
)
AS
BEGIN

DECLARE @CurrentThreadID int
DECLARE @CurrentThreadDate datetime
DECLARE @ForumID int
DECLARE @ThreadID int

SELECT @CurrentThreadID = ThreadID, @CurrentThreadDate = ThreadDate, @ForumID = ForumID FROM Posts WHERE PostID = @PostID


  IF @NextThread = 1
	SELECT TOP 1
		@ThreadID = ThreadID
	FROM
		Posts
	WHERE
		PostLevel = 1 AND
		Approved = 1 AND
		ForumID = @ForumID AND
		ThreadDate < @CurrentThreadDate
  ELSE
	SELECT TOP 1
		@ThreadID = ThreadID
	FROM
		Posts
	WHERE
		PostLevel = 1 AND
		Approved = 1 AND
		ForumID = @ForumID AND
		ThreadDate > @CurrentThreadDate

  IF @ThreadID IS NULL
    SELECT ThreadID = @CurrentThreadID
  ELSE
    SELECT ThreadID = @ThreadID
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



create procedure forums_GetPrevPostID
(
	@ThreadID int,
	@SortOrder int,
	@ForumID int,
	@PrevPostID int OUTPUT
)
 AS
SELECT TOP 1 @PrevPostID = PostID
FROM Posts (nolock)
WHERE ThreadID = @ThreadID AND ForumID = @ForumID AND SortOrder = @SortOrder-1 AND Approved = 1
IF @@ROWCOUNT < 1
	SELECT @PrevPostID = 0














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



create procedure forums_GetPrevThreadID
(
	@ThreadID int,
	@ForumID int,
	@PrevThreadID int OUTPUT
)
 AS
SELECT TOP 1 @PrevThreadID = ThreadID
FROM Posts (nolock)
WHERE ThreadID < @ThreadID AND ForumID = @ForumID AND Approved = 1
ORDER BY ThreadID DESC
IF @@ROWCOUNT < 1
	SELECT @PrevThreadID = 0














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO


CREATE PROCEDURE forums_GetRoleDescription
(
    @Rolename nvarchar(256)
)
AS
    IF EXISTS(SELECT Rolename FROM UserRoles (nolock) WHERE Rolename = @RoleName)
        BEGIN
            SELECT Description FROM UserRoles WHERE Rolename=@Rolename
        END
    ELSE
        BEGIN
            SELECT "" As Description
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


CREATE PROCEDURE forums_GetRolesByForum
(
    @ForumID int
)
AS
BEGIN
    SELECT 
        RoleName
    FROM 
        PrivateForums
    WHERE
        ForumID=@ForumID
END

GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO



create procedure forums_GetRolesByUser
(
@UserName nvarchar(50)
)
AS
BEGIN
	SELECT 
		Rolename 
	FROM 
		UsersInRoles
	WHERE
		Username = @UserName
END









GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

CREATE   PROCEDURE forums_GetSearchResults
(
	@SearchTerms	nvarchar(500),
	@Page int,
	@RecsPerPage int,
	@UserName nvarchar(50)
)
 AS
	CREATE TABLE #tmp
	(
		ID int IDENTITY,
		PostID int
	)
	DECLARE @sql nvarchar(1000)
	SET NOCOUNT ON
	SELECT @sql = 'INSERT INTO #tmp(PostID) SELECT PostID ' + 
			'FROM Posts P (nolock) INNER JOIN Forums F (nolock) ON F.ForumID = P.ForumID ' +
			@SearchTerms + ' ORDER BY ThreadDate DESC'
	EXEC(@sql)
	-- ok, all of the rows are inserted into the table.
	-- now, select the correct subset
	DECLARE @FirstRec int, @LastRec int
	SELECT @FirstRec = (@Page - 1) * @RecsPerPage
	SELECT @LastRec = (@Page * @RecsPerPage + 1)
	DECLARE @MoreRecords int
	SELECT @MoreRecords = COUNT(*)  FROM #tmp -- WHERE ID >= @LastRec


	-- Select the data out of the temporary table
	IF @UserName IS NOT NULL
		SELECT
			T.PostID,
			P.ParentID,
			P.ThreadID,
			P.PostLevel,
			P.SortOrder,
			P.UserName,
			P.Subject,
			P.PostDate,
			P.ThreadDate,
			P.Approved,
			P.ForumID,
			F.Name As ForumName,
			MoreRecords = @MoreRecords,
			Replies = (SELECT COUNT(*) FROM Posts P2 (nolock) WHERE P2.ParentID = P.PostID AND P2.PostLevel != 1),
			P.Body,
			P.TotalViews,
			P.IsLocked,
			HasRead = 0 -- not used
		FROM 
			#tmp T
			INNER JOIN Posts P (nolock) ON
				P.PostID = T.PostID
			INNER JOIN Forums F (nolock) ON
				F.ForumID = P.ForumID
		WHERE 
			T.ID > @FirstRec AND ID < @LastRec AND
			(P.ForumID NOT IN (SELECT ForumID from PrivateForums) OR
			P.ForumID IN (SELECT ForumID FROM PrivateForums WHERE RoleName IN (SELECT RoleName from UsersInRoles WHERE username = @UserName)))
	ELSE
		SELECT
			T.PostID,
			P.ParentID,
			P.ThreadID,
			P.PostLevel,
			P.SortOrder,
			P.UserName,
			P.Subject,
			P.PostDate,
			P.ThreadDate,
			P.Approved,
			P.ForumID,
			F.Name As ForumName,
			MoreRecords = @MoreRecords,
			Replies = (SELECT COUNT(*) FROM Posts P2 (nolock) WHERE P2.ParentID = P.PostID AND P2.PostLevel != 1),
			P.Body,
			P.TotalViews,
			P.IsLocked,
			HasRead = 0 -- not used
		FROM 
			#tmp T
			INNER JOIN Posts P (nolock) ON
				P.PostID = T.PostID
			INNER JOIN Forums F (nolock) ON
				F.ForumID = P.ForumID
		WHERE 
			T.ID > @FirstRec AND ID < @LastRec AND
			P.ForumID NOT IN (SELECT ForumID from PrivateForums)

	SET NOCOUNT OFF

























GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



CREATE    PROCEDURE forums_GetSingleMessage
(
	@PostID int,
	@UserName nvarchar(50)
) AS
DECLARE @NextThreadID int, @PrevThreadID int, @NextPostID int, @PrevPostID int, @ThreadID int, @ForumID int, @SortOrder int
SELECT @ThreadID = ThreadID, @ForumID = ForumID, @SortOrder=SortOrder
FROM Posts (nolock) WHERE PostID = @PostID
exec dbo.forums_GetNextThreadID @ThreadID, @ForumID, @NextThreadID OUTPUT
exec dbo.forums_GetPrevThreadID @ThreadID, @ForumID, @PrevThreadID OUTPUT
exec dbo.forums_GetNextPostID @ThreadID, @SortOrder, @ForumID, @NextPostID OUTPUT
exec dbo.forums_GetPrevPostID @ThreadID, @SortOrder, @ForumID, @PrevPostID OUTPUT
DECLARE @TrackingThread bit
IF EXISTS(SELECT ThreadID FROM ThreadTrackings (nolock) WHERE ThreadID = @ThreadID AND UserName=@UserName)
	SELECT @TrackingThread = 1
ELSE
	SELECT @TrackingThread = 0

IF @UserName IS NOT NULL
	SELECT
		Subject,
		ForumID = @ForumID,
		ForumName = (SELECT Name FROM Forums WHERE ForumID = @ForumID),
		ThreadID = @ThreadID,
		ParentID,
		PostLevel,
		SortOrder = @SortOrder,
		PostDate,
		ThreadDate,
		P.UserName,
		U.FakeEmail,
		U.URL,
		U.Signature,
		P.Approved,
		Replies = (SELECT COUNT(*) FROM Posts P2 (nolock) WHERE P2.ParentID = P.PostID AND P2.PostLevel != 1),
		PrevThreadID = @PrevThreadID,
		NextThreadID = @NextThreadID,
		PrevPostID = @PrevPostID,
		NextPostID = @NextPostID,
		UserIsTrackingThread = @TrackingThread,
		Body,
		IsLocked
	FROM 
		Posts P (nolock)
	INNER 
		JOIN Users U (nolock) ON
		U.UserName = P.UserName
	WHERE 
		P.PostID = @PostID AND
		((ForumID NOT IN (SELECT ForumID from PrivateForums) OR
		ForumID IN (SELECT ForumID FROM PrivateForums WHERE RoleName IN (SELECT RoleName from UsersInRoles WHERE username = @UserName))))
ELSE
	SELECT
		Subject,
		ForumID = @ForumID,
		ForumName = (SELECT Name FROM Forums WHERE ForumID = @ForumID),
		ThreadID = @ThreadID,
		ParentID,
		PostLevel,
		SortOrder = @SortOrder,
		PostDate,
		ThreadDate,
		P.UserName,
		U.FakeEmail,
		U.URL,
		U.Signature,
		P.Approved,
		Replies = (SELECT COUNT(*) FROM Posts P2 (nolock) WHERE P2.ParentID = P.PostID AND P2.PostLevel != 1),
		PrevThreadID = @PrevThreadID,
		NextThreadID = @NextThreadID,
		PrevPostID = @PrevPostID,
		NextPostID = @NextPostID,
		UserIsTrackingThread = @TrackingThread,
		Body,
		IsLocked
	FROM 
		Posts P (nolock)
	INNER 
		JOIN Users U (nolock) ON
		U.UserName = P.UserName
	WHERE 
		P.PostID = @PostID AND
		ForumID NOT IN (SELECT ForumID from PrivateForums)








GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO



--exec forums_GetStatistics


CREATE          PROCEDURE forums_GetStatistics AS
	-- Get summary information - Total Users, Total Posts, TotalTopics, DaysPosts, and DaysTopics
	DECLARE @TotalUsers int
	DECLARE @TotalPosts int
	DECLARE @TotalTopics int
	DECLARE @TotalModerators int
	DECLARE @TotalModeratedPosts int
	DECLARE @NewThreadsInPast24Hours int
	DECLARE @NewPostsInPast24Hours int
	DECLARE @NewUsersInPast24Hours int
	DECLARE @MostViewsPostID int
	DECLARE @MostViewsSubject nvarchar(256)
	DECLARE @MostActivePostID int
	DECLARE @MostActiveSubject nvarchar(256)
	DECLARE @MostReadPostID int
	DECLARE @MostReadSubject nvarchar(256)
	DECLARE @MostActiveUser nvarchar(50)
	DECLARE @NewestUser nvarchar(50)

	SET NOCOUNT ON

	-- Reset top posters
	exec Statistics_ResetTopPosters
	-- Total Moderators
	-- ***********************************************
	SELECT 
		@TotalModerators = count(*) 
	FROM 
		UsersInRoles 
	WHERE 
		RoleName = 'Forum-Moderators'


	-- Total Moderated Posts
	-- ***********************************************
	SELECT 
		@TotalModeratedPosts = count(*) 
	FROM 
		ModerationAudit 

	-- Most views
	-- ***********************************************
	SELECT TOP 1 
		@MostViewsPostID = PostId, 
		@MostViewsSubject = Subject 
	FROM 
		Posts 
	WHERE 
		ThreadDate > DateAdd(d, -2, GetDate()) AND
		ForumID NOT IN (SELECT ForumID from PrivateForums) AND
		Approved = 1
	ORDER BY 
		TotalViews desc

	-- Most active post
	-- ***********************************************
	SELECT TOP 1 
		@MostActivePostID = PostID, 
		@MostActiveSubject = Subject
	FROM 
		Posts P 
	WHERE 
		P.PostLevel = 1 AND 
		ThreadDate > DateAdd(d, -2, GetDate()) AND
		ForumID NOT IN (SELECT ForumID from PrivateForums) AND
		Approved = 1
	ORDER BY 
		(SELECT Count(*) FROM Posts P2 WHERE P2.ThreadID = P.ThreadID) DESC

	-- Most active user
	-- ***********************************************
	SELECT TOP 1 
		@MostActiveUser = Username 
	FROM 
		Users 
	ORDER BY 
		TotalPosts DESC

	-- Newest user
	-- ***********************************************
	SELECT TOP 1 
		@NewestUser = Username 
	FROM 
		Users 
	WHERE
		DisplayInMemberList = 1
	ORDER BY 
		DateCreated DESC

	-- Most read posts
	-- ***********************************************
	SELECT TOP 1 
		@MostReadPostID = PostId, 
		@MostReadSubject = Subject
	FROM 
		Posts P
	WHERE
		ForumID NOT IN (SELECT ForumID from PrivateForums) AND
		Approved = 1
	ORDER BY 
		(SELECT count(HasRead) FROM PostsRead WHERE P.PostID = PostsRead.PostID) DESC

	-- Other stats
	SELECT
		-- Total Users
		-- ***********************************************
		@TotalUsers = (SELECT COUNT(*) FROM Users (nolock)),

		-- Total Posts
		-- ***********************************************
		@TotalPosts = (SELECT COUNT(*) FROM Posts) + (SELECT COUNT(*) FROM Post_Archive (nolock)),

		-- Total Topics
		-- ***********************************************
		@TotalTopics = (SELECT COUNT(*) FROM Posts (nolock) WHERE ParentID = PostID) + (SELECT COUNT(*) FROM Post_Archive WHERE ParentID = PostID),

		-- Total Posts in past 24 hours
		-- ***********************************************
		@NewPostsInPast24Hours = (SELECT COUNT(*) FROM Posts (nolock)
				WHERE PostDate > DATEADD(dd,-1,getdate())),

		-- Total Users in past 24 hours
		-- ***********************************************
		@NewUsersInPast24Hours = (SELECT COUNT(*) FROM Users (nolock)
				WHERE DateCreated > DATEADD(dd,-1,getdate())),


		-- Total Topics in past 24 hours
		-- ***********************************************
		@NewThreadsInPast24Hours = (SELECT COUNT(*) FROM Posts (nolock)
				WHERE ParentID = PostID AND PostDate > DATEADD(dd,-1,getdate()))
	SET NOCOUNT OFF

	-- Test for null values
	IF @MostViewsPostID IS NULL
		SET @MostViewsPostID = 0

	IF @MostViewsSubject IS NULL
		SET @MostViewsSubject = 'No Posts Available'

	IF @MostActivePostID IS NULL
		SET @MostActivePostID = 0

	IF @MostActiveSubject IS NULL
		SET @MostActiveSubject = 'No Posts Available'

	IF @MostActiveUser IS NULL
		SET @MostActiveUser = 'No Posts Available'

	IF @MostReadPostID IS NULL
		SET @MostReadPostID = 0

	IF @MostReadSubject IS NULL
		SET @MostReadSubject = 'No Posts Available'

	SELECT 
		TotalUsers = @TotalUsers,
		TotalPosts = @TotalPosts,
		TotalModerators = @TotalModerators,
		TotalModeratedPosts = @TotalModeratedPosts,
		TotalTopics = @TotalTopics,
		DaysPosts = @NewPostsInPast24Hours, -- TODO remove
		DaysTopics = @NewThreadsInPast24Hours, -- TODO remove
		NewPostsInPast24Hours = @NewPostsInPast24Hours,
		NewThreadsInPast24Hours = @NewThreadsInPast24Hours,
		NewUsersInPast24Hours = @NewUsersInPast24Hours,
		MostViewsPostID = @MostViewsPostID,
		MostViewsSubject = @MostViewsSubject,
		MostActivePostID = @MostActivePostID,
		MostActiveSubject = @MostActiveSubject,
		MostActiveUser = @MostActiveUser,
		MostReadPostID = @MostReadPostID,
		MostReadSubject = @MostReadSubject,
		NewestUser = @NewestUser











GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



create procedure forums_GetSummaryInfo AS
	-- Get summary information - Total Users, Total Posts, TotalTopics, DaysPosts, and DaysTopics
	DECLARE @TotalUsers int, @TotalPosts int, @TotalTopics int, @DaysTopics int, @DaysPosts int
	SET NOCOUNT ON
	SELECT
		@TotalUsers = (SELECT COUNT(*) FROM Users (nolock)),
		@TotalPosts = (SELECT COUNT(*) FROM Posts (nolock)),
		@TotalTopics = (SELECT COUNT(*) FROM Posts (nolock) WHERE ParentID = PostID),
		@DaysPosts = (SELECT COUNT(*) FROM Posts (nolock)
				WHERE PostDate > DATEADD(dd,-1,getdate())),
		@DaysTopics = (SELECT COUNT(*) FROM Posts (nolock)
				WHERE ParentID = PostID AND PostDate > DATEADD(dd,-1,getdate()))
	SET NOCOUNT OFF
	SELECT TotalUsers = @TotalUsers,
		TotalPosts = @TotalPosts,
		TotalTopics = @TotalTopics,
		DaysPosts = @DaysPosts,
		DaysTopics = @DaysTopics











GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



create procedure forums_GetThread
(
	@ThreadID int
) AS
SELECT
	PostID,
	ForumID,
	Subject,
	ParentID,
	ThreadID,
	PostLevel,
	SortOrder,
	PostDate,
	ThreadDate,
	UserName,
	Approved,
	Replies = (SELECT COUNT(*) FROM Posts P2 (nolock) WHERE P2.ParentID = P.PostID AND P2.PostLevel != 1),
	Body
FROM Posts P (nolock)
WHERE Approved = 1 AND ThreadID = @ThreadID
ORDER BY SortOrder














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO



CREATE    PROCEDURE forums_GetThreadByParentID
(
	@ParentID	int
)
 AS
BEGIN
	SELECT 
		PostID,
		ThreadID,
		ForumID,
		Subject,
		ParentID,
		PostLevel,
		SortOrder,
		PostDate,
		ThreadDate,
		UserName,
		Approved,
		Replies = (SELECT COUNT(*) FROM Posts P2 (nolock) WHERE P2.ParentID = P.PostID AND P2.PostLevel != 1),
		Body,
		TotalMessagesInThread = 0, -- not used
		TotalViews,
		IsLocked
	FROM
		Posts P
	WHERE
		Approved = 1 AND
		ParentID = @ParentID
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



CREATE           PROCEDURE forums_GetThreadByPostID
(
	@PostID	int,
	@UserName nvarchar(50)
)
 AS
BEGIN

DECLARE @ThreadID int

	-- Get the thread id of the post
	SELECT @ThreadID = ThreadID FROM Posts WHERE PostID = @PostID

	-- get the thread info for this post
	IF @UserName IS NOT NULL
		SELECT
			PostID,
			ThreadID,
			ForumID,
			ForumName = (SELECT Name FROM Forums F (nolock) WHERE F.ForumID = P.ForumID),
			Subject,
			ParentID,
			PostLevel,
			SortOrder,
			PostDate,
			ThreadDate,
			UserName,
			Approved,
			Replies = (SELECT COUNT(*) FROM Posts P2 (nolock) WHERE P2.ParentID = P.PostID AND P2.PostLevel != 1),
			Body,
			TotalViews,
			IsLocked,
			HasRead = 0 -- not used
		FROM 
			Posts P (nolock)
		WHERE 
			Approved=1 AND 
			ThreadID = @ThreadID AND
			SortOrder >= (SELECT SortOrder FROM Posts (nolock) WHERE PostID = @ThreadID) AND
			(ForumID NOT IN (SELECT ForumID from PrivateForums) OR
			ForumID IN (SELECT ForumID FROM PrivateForums WHERE RoleName IN (SELECT RoleName from UsersInRoles WHERE username = @UserName)))
		ORDER BY 
			SortOrder
	ELSE
		SELECT
			PostID,
			ThreadID,
			ForumID,
			ForumName = (SELECT Name FROM Forums F (nolock) WHERE F.ForumID = P.ForumID),
			Subject,
			ParentID,
			PostLevel,
			SortOrder,
			PostDate,
			ThreadDate,
			UserName,
			Approved,
			Replies = (SELECT COUNT(*) FROM Posts P2 (nolock) WHERE P2.ParentID = P.PostID AND P2.PostLevel != 1),
			Body,
			TotalViews,
			IsLocked,
			HasRead = 0 -- not used
		FROM 
			Posts P (nolock)
		WHERE 
			Approved=1 AND 
			ThreadID = @ThreadID AND
			SortOrder >= (SELECT SortOrder FROM Posts (nolock) WHERE PostID = @ThreadID) AND
			ForumID NOT IN (SELECT ForumID from PrivateForums)
		ORDER BY 
			SortOrder
END






















GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO


CREATE              PROCEDURE forums_GetThreadByPostIDPaged
(
	@PostID	int,
	@PageIndex int,
	@PageSize int,
	@SortBy int,
	@SortOrder bit,
	@UserName nvarchar(50)
)
 AS
BEGIN

DECLARE @PageLowerBound int
DECLARE @PageUpperBound int
DECLARE @ThreadID int
DECLARE @ForumID int
DECLARE @PrivateForumID int
DECLARE @IsPrivateForum bit
DECLARE @IsUserApprovedForPrivateForum bit

-- Get the ForumID, the PrivateForumID, and the ThreadID
SELECT @ForumID = ForumID, @ThreadID = ThreadID FROM Posts WHERE PostID = @PostID
SELECT @PrivateForumID = ForumID FROM PrivateForums WHERE ForumID = @ForumID

-- Is this a private forum?
IF @PrivateForumID IS NOT NULL
  BEGIN
    -- This is a private forum
    SET @IsPrivateForum = 1

    -- Does the user have access to this forum?
    IF (@Username IS NOT NULL)
      BEGIN
        IF EXISTS(SELECT ForumID FROM PrivateForums WHERE RoleName IN (SELECT RoleName from UsersInRoles WHERE username = @UserName))
          SET @IsUserApprovedForPrivateForum = 1
        ELSE
          SET @IsUserApprovedForPrivateForum = 0
      END
    ELSE
      SET @IsUserApprovedForPrivateForum = 0
  END
ELSE
  SET @IsPrivateForum = 0

-- Let's return here if the user is not allowed
IF @IsPrivateForum = 1 AND @IsUserApprovedForPrivateForum = 0
  RETURN

-- Set the page bounds
SET @PageLowerBound = @PageSize * @PageIndex
SET @PageUpperBound = @PageLowerBound + @PageSize + 1

-- Create a temp table to store the select results
CREATE TABLE #PageIndex 
(
	IndexId int IDENTITY (1, 1) NOT NULL,
	PostID int
)

-- Sort by Post Date
IF @SortBy = 0 AND @SortOrder = 0
    INSERT INTO #PageIndex (PostID)
    SELECT PostID FROM Posts P (nolock) WHERE Approved = 1 AND ThreadID = @ThreadID ORDER BY PostDate

ELSE IF @SortBy = 0 AND @SortOrder = 1
    INSERT INTO #PageIndex (PostID)
    SELECT PostID FROM Posts P (nolock) WHERE Approved = 1 AND ThreadID = @ThreadID ORDER BY PostDate DESC

-- Sort by Author
IF @SortBy = 1 AND @SortOrder = 0
    INSERT INTO #PageIndex (PostID)
    SELECT PostID FROM Posts P (nolock) WHERE Approved = 1 AND ThreadID = @ThreadID ORDER BY Username
ELSE IF @SortBy = 1 AND @SortOrder = 1
    INSERT INTO #PageIndex (PostID)
    SELECT PostID FROM Posts P (nolock) WHERE Approved = 1 AND ThreadID = @ThreadID ORDER BY Username DESC

SELECT
	P.PostID,
	ThreadID,
	ForumID,
	ForumName = (SELECT Name FROM Forums F (nolock) WHERE F.ForumID = P.ForumID),
	Subject,
	ParentID,
	PostLevel,
	SortOrder,
	PostDate,
	ThreadDate,
	UserName,
	Approved,
	Replies = (SELECT COUNT(*) FROM Posts P2 (nolock) WHERE P2.ParentID = P.PostID AND P2.PostLevel != 1),
	Body,
	TotalViews,
	IsLocked,
	TotalMessagesInThread = 0, -- not used
	HasRead = 0, -- not used
	PostType
FROM 
	Posts P (nolock),
	#PageIndex
WHERE 
	P.PostID = #PageIndex.PostID AND
	#PageIndex.IndexID > @PageLowerBound AND
	#PageIndex.IndexID < @PageUpperBound
END



GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

CREATE             PROCEDURE forums_GetThreadByPostIDPaged2
(
	@PostID	int,
	@PageIndex int,
	@PageSize int,
	@SortBy int,
	@SortOrder bit,
	@UserName nvarchar(50)
)
 AS
BEGIN

DECLARE @PageLowerBound int
DECLARE @PageUpperBound int
DECLARE @ThreadID int
DECLARE @ForumID int
DECLARE @PrivateForumID int
DECLARE @IsPrivateForum bit
DECLARE @IsUserApprovedForPrivateForum bit

-- Get the ForumID, the PrivateForumID, and the ThreadID
SELECT @ForumID = ForumID, @ThreadID = ThreadID FROM Posts WHERE PostID = @PostID
SELECT @PrivateForumID = ForumID FROM PrivateForums WHERE ForumID = @ForumID

-- Is this a private forum?
IF @PrivateForumID IS NOT NULL
  BEGIN
    -- This is a private forum
    SET @IsPrivateForum = 1

    -- Does the user have access to this forum?
    IF (@Username IS NOT NULL)
      BEGIN
        IF EXISTS(SELECT ForumID FROM PrivateForums WHERE RoleName IN (SELECT RoleName from UsersInRoles WHERE username = @UserName))
          SET @IsUserApprovedForPrivateForum = 1
        ELSE
          SET @IsUserApprovedForPrivateForum = 0
      END
    ELSE
      SET @IsUserApprovedForPrivateForum = 0
  END
ELSE
  SET @IsPrivateForum = 0

-- Let's return here if the user is not allowed
IF @IsPrivateForum = 1 AND @IsUserApprovedForPrivateForum = 0
  RETURN

-- Set the page bounds
SET @PageLowerBound = @PageSize * @PageIndex
SET @PageUpperBound = @PageLowerBound + @PageSize + 1

-- Create a temp table to store the select results
CREATE TABLE #PageIndex 
(
	IndexId int IDENTITY (1, 1) NOT NULL,
	PostID int
)

-- Sort by Post Date
IF @SortBy = 0 AND @SortOrder = 0
    INSERT INTO #PageIndex (PostID)
    SELECT PostID FROM Posts P (nolock) WHERE Approved = 1 AND ThreadID = @ThreadID ORDER BY PostDate

ELSE IF @SortBy = 0 AND @SortOrder = 1
    INSERT INTO #PageIndex (PostID)
    SELECT PostID FROM Posts P (nolock) WHERE Approved = 1 AND ThreadID = @ThreadID ORDER BY PostDate DESC

-- Sort by Author
IF @SortBy = 1 AND @SortOrder = 0
    INSERT INTO #PageIndex (PostID)
    SELECT PostID FROM Posts P (nolock) WHERE Approved = 1 AND ThreadID = @ThreadID ORDER BY Username
ELSE IF @SortBy = 1 AND @SortOrder = 1
    INSERT INTO #PageIndex (PostID)
    SELECT PostID FROM Posts P (nolock) WHERE Approved = 1 AND ThreadID = @ThreadID ORDER BY Username DESC

SELECT
	P.PostID,
	ThreadID,
	ForumID,
	ForumName = (SELECT Name FROM Forums F (nolock) WHERE F.ForumID = P.ForumID),
	Subject,
	ParentID,
	PostLevel,
	SortOrder,
	PostDate,
	ThreadDate,
	UserName,
	Approved,
	Replies = (SELECT COUNT(*) FROM Posts P2 (nolock) WHERE P2.ParentID = P.PostID AND P2.PostLevel != 1),
	Body,
	TotalViews,
	IsLocked,
	TotalMessagesInThread = 0, -- not used
	HasRead = 0 -- not used
FROM 
	Posts P (nolock),
	#PageIndex
WHERE 
	P.PostID = #PageIndex.PostID AND
	#PageIndex.IndexID > @PageLowerBound AND
	#PageIndex.IndexID < @PageUpperBound
END


GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

Create             PROCEDURE forums_GetThreadByPostIDPaged_BackUp
(
	@PostID	int,
	@PageIndex int,
	@PageSize int,
	@SortBy int,
	@SortOrder bit,
	@UserName nvarchar(50)
)
 AS
BEGIN

DECLARE @PageLowerBound int
DECLARE @PageUpperBound int

-- Set the page bounds
SET @PageLowerBound = @PageSize * @PageIndex
SET @PageUpperBound = @PageLowerBound + @PageSize + 1

-- Create a temp table to store the select results
CREATE TABLE #PageIndex 
(
	IndexId int IDENTITY (1, 1) NOT NULL,
	PostID int
)

-- Sort by Post Date
IF @SortBy = 0 AND @SortOrder = 0
    INSERT INTO #PageIndex (PostID)
    SELECT PostID FROM Posts P (nolock) WHERE Approved=1 AND ThreadID = (SELECT ThreadID FROM Posts (nolock) WHERE PostID = @PostID) AND PostID >= @PostID AND ParentID >= (SELECT ParentID FROM Posts (nolock) WHERE PostID = @PostID) AND	SortOrder >= (SELECT SortOrder FROM Posts (nolock) WHERE PostID = @PostID) ORDER BY PostDate
ELSE IF @SortBy = 0 AND @SortOrder = 1
    INSERT INTO #PageIndex (PostID)
    SELECT PostID FROM Posts P (nolock) WHERE Approved=1 AND ThreadID = (SELECT ThreadID FROM Posts (nolock) WHERE PostID = @PostID) AND PostID >= @PostID AND ParentID >= (SELECT ParentID FROM Posts (nolock) WHERE PostID = @PostID) AND	SortOrder >= (SELECT SortOrder FROM Posts (nolock) WHERE PostID = @PostID) ORDER BY PostDate DESC

-- Sort by Author
IF @SortBy = 1 AND @SortOrder = 0
    INSERT INTO #PageIndex (PostID)
    SELECT PostID FROM Posts P (nolock) WHERE Approved=1 AND ThreadID = (SELECT ThreadID FROM Posts (nolock) WHERE PostID = @PostID) AND PostID >= @PostID AND ParentID >= (SELECT ParentID FROM Posts (nolock) WHERE PostID = @PostID) AND	SortOrder >= (SELECT SortOrder FROM Posts (nolock) WHERE PostID = @PostID) ORDER BY Username
ELSE IF @SortBy = 1 AND @SortOrder = 1
    INSERT INTO #PageIndex (PostID)
    SELECT PostID FROM Posts P (nolock) WHERE Approved=1 AND ThreadID = (SELECT ThreadID FROM Posts (nolock) WHERE PostID = @PostID) AND PostID >= @PostID AND ParentID >= (SELECT ParentID FROM Posts (nolock) WHERE PostID = @PostID) AND	SortOrder >= (SELECT SortOrder FROM Posts (nolock) WHERE PostID = @PostID) ORDER BY Username DESC

-- get the thread info for this post
IF @UserName IS NOT NULL
	SELECT
		P.PostID,
		ThreadID,
		ForumID,
		ForumName = (SELECT Name FROM Forums F (nolock) WHERE F.ForumID = P.ForumID),
		Subject,
		ParentID,
		PostLevel,
		SortOrder,
		PostDate,
		ThreadDate,
		UserName,
		Approved,
		Replies = (SELECT COUNT(*) FROM Posts P2 (nolock) WHERE P2.ParentID = P.PostID AND P2.PostLevel != 1),
		Body,
		TotalViews,
		IsLocked,
		TotalMessagesInThread = 0, -- not used
		HasRead = 0 -- not used
	FROM 
		Posts P (nolock),
		#PageIndex
	WHERE 
		P.PostID = #PageIndex.PostID AND
		#PageIndex.IndexID > @PageLowerBound AND
		#PageIndex.IndexID < @PageUpperBound AND
		(ForumID NOT IN (SELECT ForumID from PrivateForums) OR
		ForumID IN (SELECT ForumID FROM PrivateForums WHERE RoleName IN (SELECT RoleName from UsersInRoles WHERE username = @UserName)))
ELSE
	SELECT
		P.PostID,
		ThreadID,
		ForumID,
		ForumName = (SELECT Name FROM Forums F (nolock) WHERE F.ForumID = P.ForumID),
		Subject,
		ParentID,
		PostLevel,
		SortOrder,
		PostDate,
		ThreadDate,
		UserName,
		Approved,
		Replies = (SELECT COUNT(*) FROM Posts P2 (nolock) WHERE P2.ParentID = P.PostID AND P2.PostLevel != 1),
		Body,
		TotalViews,
		IsLocked,
		TotalMessagesInThread = 0, -- not used
		HasRead = 0 -- not used
	FROM 
		Posts P (nolock),
		#PageIndex
	WHERE 
		P.PostID = #PageIndex.PostID AND
		#PageIndex.IndexID > @PageLowerBound AND
		#PageIndex.IndexID < @PageUpperBound AND
		ForumID NOT IN (SELECT ForumID from PrivateForums)


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



create procedure forums_GetTimezoneByUsername
(
	@UserName nvarchar(50)
)
 AS
	-- get this user's timezone offset
	SELECT Timezone
	FROM Users (nolock)
	WHERE UserName = @UserName














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO



CREATE  procedure forums_GetTop25NewPosts
(
	@UserName nvarchar(50)
)
AS

IF @UserName IS NULL
	SELECT TOP 25
		Subject,
		Body,
		P.PostID,
		ThreadID,
		ParentID,
		PostDate = (SELECT Max(PostDate) FROM Posts WHERE P.ThreadID = ThreadID),
		ThreadDate,
		UserName,
		Replies = (SELECT COUNT(*) FROM Posts WHERE P.ThreadID = ThreadID AND PostLevel != 1 AND Approved = 1),
		Body,
		TotalViews,
		IsLocked,
		HasRead = 0,
		MostRecentPostAuthor = (SELECT TOP 1 Username FROM Posts WHERE P.ThreadID = ThreadID AND Approved = 1 ORDER BY PostDate DESC),
		MostRecentPostID = (SELECT TOP 1 PostID FROM Posts WHERE P.ThreadID = ThreadID AND Approved = 1 ORDER BY PostDate DESC)
	FROM 
		Posts P 
	WHERE 
		PostLevel = 1 AND 
		Approved = 1 AND
		ForumID NOT IN (SELECT ForumID from PrivateForums)
	ORDER BY 
		ThreadDate DESC
ELSE
	SELECT TOP 25 
		Subject,
		Body,
		P.PostID,
		ThreadID,
		ParentID,
		PostDate = (SELECT Max(PostDate) FROM Posts WHERE P.ThreadID = ThreadID),
		ThreadDate,
		UserName,
		Replies = (SELECT COUNT(*) FROM Posts WHERE P.ThreadID = ThreadID AND PostLevel != 1 AND Approved = 1),
		Body,
		TotalViews,
		IsLocked,
		HasRead = 0,
		MostRecentPostAuthor = (SELECT TOP 1 Username FROM Posts WHERE P.ThreadID = ThreadID AND Approved = 1 ORDER BY PostDate DESC),
		MostRecentPostID = (SELECT TOP 1 PostID FROM Posts WHERE P.ThreadID = ThreadID AND Approved = 1 ORDER BY PostDate DESC)
	FROM 
		Posts P
	WHERE 
		PostLevel = 1 AND 
		Approved = 1 AND
		(ForumID NOT IN (SELECT ForumID from PrivateForums) OR
		ForumID IN (SELECT ForumID FROM PrivateForums WHERE RoleName IN (SELECT RoleName from UsersInRoles WHERE username = @UserName)))
	ORDER BY 
		ThreadDate DESC






GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO



create procedure forums_GetTopicsUserIsTracking
(
@UserName nvarchar(50)
)
AS
SELECT 
	Subject,
	Body,
	P.PostID,
	P.ThreadID,
	ParentID,
	PostDate = (SELECT Max(PostDate) FROM Posts WHERE ThreadID = P.ThreadID),
	ThreadDate,
	PinnedDate,
	P.UserName,
	Replies = (SELECT COUNT(*) FROM Posts WHERE P.ThreadID = ThreadID AND Approved = 1 AND PostLevel != 1),
	Body,
	TotalViews,
	IsLocked,
	IsPinned,
	HasRead = dbo.HasReadPost(@UserName, P.PostID, P.ForumID),
	MostRecentPostAuthor = (SELECT TOP 1 Username FROM Posts WHERE P.ThreadID = ThreadID AND Approved = 1 ORDER BY PostDate DESC),
	MostRecentPostID = (SELECT TOP 1 PostID FROM Posts WHERE P.ThreadID = ThreadID AND Approved = 1 ORDER BY PostDate DESC)
FROM
	Posts P,
	ThreadTrackings T
WHERE
	PostLevel = 1 AND
	Approved = 1 AND
	P.ThreadID = T.ThreadID AND
        T.Username = @UserName




GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO



create procedure forums_GetTopicsUserMostRecentlyParticipatedIn
(
@UserName nvarchar(50)
)
AS

-- Create a temp table
CREATE Table #ThreadsUserParticipatedIn (
	ThreadID int,
	ThreadDate datetime
)

-- Insert into temp table
INSERT INTO #ThreadsUserParticipatedIn
SELECT DISTINCT TOP 25
	ThreadID, 
	ThreadDate 
FROM 
	Posts 
WHERE 
	Approved = 1 AND 
	Username = @UserName 
ORDER BY
	ThreadDate DESC

SELECT 
	Subject,
	Body,
	P.PostID,
	P.ThreadID,
	ParentID,
	PostDate = (SELECT Max(PostDate) FROM Posts WHERE ThreadID = P.ThreadID),
	P.ThreadDate,
	PinnedDate,
	P.UserName,
	Replies = (SELECT COUNT(*) FROM Posts WHERE P.ThreadID = ThreadID AND Approved = 1 AND PostLevel != 1),
	Body,
	TotalViews,
	IsLocked,
	IsPinned,
	HasRead = dbo.HasReadPost(@UserName, P.PostID, P.ForumID),
	MostRecentPostAuthor = (SELECT TOP 1 Username FROM Posts WHERE P.ThreadID = ThreadID AND Approved = 1 ORDER BY PostDate DESC),
	MostRecentPostID = (SELECT TOP 1 PostID FROM Posts WHERE P.ThreadID = ThreadID AND Approved = 1 ORDER BY PostDate DESC)
FROM
	Posts P,
	#ThreadsUserParticipatedIn T
WHERE
        PostID = T.ThreadID






GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO



Create   PROCEDURE forums_GetTotalNumberOfForums
AS

	SELECT
		COUNT (*)
	FROM
		Forums














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



Create  PROCEDURE forums_GetTotalPostCount
 AS
	SELECT
	  Count(*)
	FROM
	  Posts














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



Create  PROCEDURE forums_GetTotalPostsForThread
(
	@PostID	int
)
 AS
	-- Get the count of posts for a given thread
	SELECT 
		TotalPostsForThread = COUNT(PostID)
	FROM 
		Posts (nolock)
	WHERE 
		ThreadID = @PostID












GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO



CREATE     PROCEDURE forums_GetTotalUsers
(
	@UserNameBeginsWith nvarchar(1),
	@UserNameToFind nvarchar(50)
)
AS

IF @UserNameBeginsWith IS NULL AND @UserNameToFind IS NULL
	SELECT
		COUNT (*)
	FROM
		Users
	WHERE
		DisplayInMemberList = 1 AND
		Approved = 1
ELSE
	IF @UserNameToFind IS NULL
		SELECT
			COUNT (*)
		FROM
			Users
		WHERE
			DisplayInMemberList = 1 AND
			Approved = 1 AND 
			LEFT(UserName, 1) = @UserNameBeginsWith
	ELSE
		SELECT
			COUNT (*)
		FROM
			Users
		WHERE
			DisplayInMemberList = 1 AND
			Approved = 1 AND 
			Username like '%' + @UserNameToFind + '%'







GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO



CREATE    PROCEDURE forums_GetTrackingEmailsForThread
(
	@PostID	int
)
AS
	-- First get the threadID of the Post
	DECLARE @ThreadID int
	DECLARE @UserName nvarchar(50)

	SELECT 
		@ThreadID = ThreadID, 
		@UserName = Username 
	FROM 
		Posts (nolock) 
	WHERE 
		PostID = @PostID

	-- now, get all of the emails of the users who are tracking this thread
	SELECT
		Email
	FROM 
		Users U (nolock),
		ThreadTrackings T
	WHERE
		U.Username = T.Username AND
		T.ThreadID = @ThreadID






GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

CREATE PROCEDURE forums_GetUnmoderatedPostStatus
(
	@ForumID int = null,
	@UserName nvarchar (50) = null
)
AS
BEGIN
DECLARE @DateDiff int
DECLARE @TotalCount int

	IF @ForumID = 0
		SET @ForumID = null

	SELECT 
		OldestPostAgeInMinutes = datediff(mi, isnull(min(postdate),getdate()),getdate()),
		TotalPostsInModerationQueue = count(PostID)
	FROM 
		POSTS P 
	WHERE 
		ForumID = isnull(@ForumID,ForumID) AND 
		Approved = 0


END

GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO



create procedure forums_GetUserGroups
AS
	SELECT
		GroupName,
		IsAdminGroup,
		IsModeratorGroup
	FROM
		UserGroups
	ORDER BY
		SortOrder,
		GroupName














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO





CREATE                PROCEDURE forums_GetUserInfo
(
	@UserName nvarchar(50),
	@UpdateIsOnline bit
)
 AS
BEGIN

	IF @UpdateIsOnline = 1
		-- Update activity
		UPDATE 
			Users 
		SET 
			LastActivity = GetDate() 
		WHERE 
			Username = @UserName

        -- Get the user details
	SELECT
		Username,
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
		IsModerator = (select count(*) from moderators where username = @UserName),
		TotalPosts,
		HasAvatar,
		ShowUnreadTopicsOnly,
		Style,
		AvatarType,
		ShowAvatar,
		DateFormat,
		PostViewOrder,
		FlatView,
		AvatarUrl,
		Attributes
	FROM 
		Users (nolock)
	WHERE 
		UserName = @UserName

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



create procedure forums_GetUserNameFromPostID
(
	@PostID	int
)
 AS
	-- returns who posted a particular post
	SELECT UserName
	FROM Posts (nolock)
	WHERE PostID = @PostID














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



create procedure forums_GetUsernameByEmail
(
	@Email		nvarchar(50)
)
 AS
SELECT 
	Username
FROM
	Users
WHERE
	Email = @Email






GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO




CREATE    PROCEDURE forums_GetUsersByFirstCharacter
(
	@FirstLetter	char(1)
)
 AS
	--- get a list of UNBANNED users whose username begins with @FirstChar
	SELECT
		Username,
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
		AvatarUrl,
		ShowAvatar,
		DateFormat,
		PostViewOrder,
		FlatView,
		IsModerator = (SELECT Count(*) FROM Moderators WHERE Username = U.Username),
		Attributes
	FROM  
		Users U(nolock)
	WHERE 
		LEFT(UserName, 1) = @FirstLetter
















GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO



CREATE      PROCEDURE forums_GetUsersOnline
(
	@PastMinutes int
)
 AS
BEGIN

	-- Get online users
	SELECT
		Username,
		Administrator = (SELECT TOP 1 Count(*) FROM UsersInRoles WHERE Rolename = 'Forum-Administrators' AND Username = U.Username),
		IsModerator = (SELECT Count(*) FROM Moderators where username = U.Username)
	FROM
		Users U
	WHERE
		LastActivity > DateAdd(minute, -@PastMinutes, GetDate())
END














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO


CREATE  procedure forums_GetVoteResults (
	@PostID int
)
AS
BEGIN
  SELECT
	Vote,
	VoteCount
  FROM
	Vote
  WHERE
	PostID = @PostID
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



create procedure forums_IsDuplicatePost
(
	@UserName nvarchar(50),
	@Body ntext
)
 AS
	SELECT COUNT(*)
	FROM Posts (nolock)
	WHERE Username = @UserName AND Body LIKE @Body














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

CREATE procedure forums_IsUserTrackingPost
(
	@ThreadID int,
	@UserName nvarchar(50)
)
AS
DECLARE @TrackingThread bit

IF EXISTS(SELECT ThreadID FROM ThreadTrackings (nolock) WHERE ThreadID = @ThreadID AND UserName=@UserName)
	SELECT IsUserTrackingPost = 1
ELSE
	SELECT IsUserTrackingPost = 0

GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



CREATE       PROCEDURE forums_MarkAllThreadsRead
(
	@ForumID int,
	@UserName nvarchar (50)
)
 AS
BEGIN
	DECLARE @PostID int

	-- first find the max post id for the given forum
	SELECT @PostID = MAX(PostID) FROM Posts WHERE ForumID = @ForumID

	-- Do we need to performa an INSERT or an UPDATE?
	IF EXISTS (SELECT ForumID FROM ForumsRead WHERE ForumID = @ForumID AND Username = @UserName)
		UPDATE 
			ForumsRead
		SET
			MarkReadAfter = @PostID
		WHERE
			ForumID = @ForumID AND
			Username = @UserName
	ELSE
		INSERT INTO
			ForumsRead
			(ForumId, Username, MarkReadAfter)
		VALUES
			(@ForumID, @UserName, @PostID)

	-- Do some clean up
	DELETE PostsRead WHERE PostID < @PostID AND Username = @UserName

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



Create       PROCEDURE forums_MarkPostAsRead
(
	@PostID	int,
	@UserName nvarchar (50)
)
 AS
BEGIN

	-- If @UserName is null it is an anonymous user
	IF @UserName IS NOT NULL
	BEGIN
		DECLARE @ForumID int
		DECLARE @PostDate datetime

		-- Mark the post as read
		-- *********************

		-- Only for PostLevel = 1
		IF EXISTS (SELECT PostID FROM Posts WHERE PostID = @PostID AND PostLevel = 1)
			IF NOT EXISTS (SELECT HasRead FROM PostsRead WHERE Username = @UserName and PostID = @PostID)
				INSERT INTO PostsRead (Username, PostID) VALUES (@UserName, @PostID)

	END

END
















GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO



CREATE     PROCEDURE forums_MovePost
(
	@PostID		int,
	@MoveToForumID	int,
	@UserName	nvarchar(50)
)
 AS
DECLARE @CurrentForum int
DECLARE @ApproveSetting bit
DECLARE @ForumName nvarchar(100)

	SELECT @ApproveSetting = Approved FROM Posts (nolock) WHERE PostID = @PostID
	IF @ApproveSetting = 0
		-- ok, so we're dealing with a post that is being moved via moderation
		-- does the user moving this have rights to moderate in the new forum? (or is the forum unmoderated?
		IF EXISTS(SELECT NULL FROM Moderators (nolock) WHERE (ForumID = @MoveToForumID OR ForumID = 0) AND Username = @UserName)
			OR (SELECT Moderated FROM Forums (nolock) WHERE ForumID = @MoveToForumID) = 0
			-- this user has rights, so we'll want to automagically approve the post in the new forum
			SELECT @ApproveSetting = 1
		
	-- only allow top-level messages to be moved
	IF (SELECT ParentID FROM Posts (nolock) WHERE PostID = @PostID) <> @PostID
		SELECT 0
	ELSE
	  BEGIN

		-- Get the forum we are moving from
		SELECT
			@CurrentForum = ForumID
		FROM
			Posts
		WHERE
			PostID = @PostID	

		-- Update the post with a new forum id
		UPDATE 
			Posts
		SET 
			ForumID = @MoveToForumID,
			Approved = @ApproveSetting
		WHERE 
			PostID = @PostID

		-- Update the forum statistics for the from forum
		exec Statistics_ResetForumStatistics @CurrentForum

		-- Update the forum statistics for the to forum
		exec Statistics_ResetForumStatistics @MoveToForumID

		-- Record to our moderation audit log
		INSERT INTO
			ModerationAudit
		VALUES
			(GetDate(), @PostID, @UserName, 3, null)

		IF @ApproveSetting = 0
			-- the post was moved but not approved
			SELECT 1
		ELSE
			-- the post was moved AND approved
			SELECT 2
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


CREATE procedure forums_RemoveForumFromRole
(
   @ForumID int,
   @Rolename nvarchar(256)
)
AS
IF EXISTS (SELECT ForumID FROM PrivateForums WHERE ForumID=@ForumID AND Rolename=@Rolename)
DELETE FROM
    PrivateForums
WHERE
    ForumID=@ForumID AND Rolename=@Rolename

GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



create procedure forums_RemoveModeratedForumForUser
(
	@UserName	nvarchar(50),
	@ForumID	int
)
 AS
	-- remove a row from the Moderators table
	DELETE FROM Moderators
	WHERE Username = @UserName and ForumID = @ForumID

GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO


CREATE procedure forums_RemoveUserFromRole
(
   @UserName nvarchar(50),
   @Rolename nvarchar(256)
)
AS
IF EXISTS (SELECT Username FROM UsersInRoles WHERE Username = @UserName AND Rolename = @Rolename)
DELETE FROM
    UsersInRoles
WHERE
    Username = @UserName AND Rolename = @Rolename

GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



create procedure forums_ReverseTrackingOption 
(
	@UserName nvarchar(50),
	@PostID	int
)
AS
	-- reverse the user's tracking options for a particular thread
	-- first get the threadID of the Post
	DECLARE @ThreadID int
	SELECT @ThreadID = ThreadID FROM Posts (nolock) WHERE PostID = @PostID
	IF EXISTS(SELECT ThreadID FROM ThreadTrackings WHERE ThreadID = @ThreadID AND UserName=@UserName)
		-- the user is tracking this thread, delete this row
		DELETE FROM ThreadTrackings
		WHERE ThreadID = @ThreadID AND UserName=@UserName
	ELSE
		-- this user isn't tracking the thread, so add her
		INSERT INTO ThreadTrackings (ThreadID, UserName)
		VALUES(@ThreadID, @UserName)














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO



CREATE  procedure forums_ToggleOptions 
(
	@UserName nvarchar(50),
	@HideReadThreads bit,
	@FlatView bit
)
AS

IF @FlatView IS NULL
	UPDATE
		Users
	SET
		ShowUnreadTopicsOnly = @HideReadThreads
	WHERE
		Username = @UserName
ELSE
	UPDATE
		Users
	SET
		ShowUnreadTopicsOnly = @HideReadThreads,
		FlatView = @FlatView
	WHERE
		Username = @UserName






GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO



CREATE      PROCEDURE forums_TopicCountForForum
(
	@ForumID int,
	@MaxDate datetime,
	@MinDate datetime,
	@UserName nvarchar(50),
	@UnReadTopicsOnly bit
)
 AS
	IF @UserName IS NULL OR @UnReadTopicsOnly = 0
		SELECT 
			TotalTopics = COUNT(*) 
		FROM 
			Posts 
		WHERE 
			postlevel = 1 AND 
			forumid = @ForumID AND 
			approved = 1 AND
			ThreadDate >= @MinDate AND 
			ThreadDate <= @MaxDate
	ELSE
		SELECT 
			TotalTopics = COUNT(*) 
		FROM 
			Posts P
		WHERE 
			postlevel = 1 AND 
			forumid = @ForumID AND 
			approved = 1 AND
			ThreadDate >= @MinDate AND 
			ThreadDate <= @MaxDate AND
			P.PostID NOT IN (SELECT PostsRead.PostID FROM PostsRead WHERE PostsRead.Username = @UserName) AND
			P.PostId >= (select MarkReadAfter from ForumsRead where username = @UserName and forumid = @ForumID)
	

		











GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO



CREATE           PROCEDURE forums_TrackAnonymousUsers
(
	@UserId char(36)
)
AS
BEGIN
	-- Does the user already exist?
	IF EXISTS (SELECT UserId FROM AnonymousUsers WHERE UserId = @UserId)
		UPDATE 
			AnonymousUsers
		SET 
			LastLogin = GetDate()
		WHERE
			UserId = @UserId
	ELSE
		INSERT INTO
			AnonymousUsers
			(UserID) 
		VALUES
			(@UserId)
	
	-- Anonymous users also pay tax to clean up table
	DELETE AnonymousUsers WHERE LastLogin < DateAdd(minute, -20, GetDate())	
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



create procedure forums_UnbanUser
(
	@UserName	nvarchar(50)
)
 AS
	-- unban this user
	UPDATE Users SET
		Approved = 1
	WHERE UserName = @UserName














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



create procedure forums_UpdateEmailTemplate
(
	@EmailID		int,
	@Subject		nvarchar(50),
	@Message		ntext
)
 AS
	-- Update a particular email message
	UPDATE Emails SET
		Subject = @Subject,
		Message = @Message
	WHERE EmailID = @EmailID














GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



CREATE  PROCEDURE forums_UpdateForum
(
	@ForumID	int,
	@ForumGroupID	int,
	@Name		nvarchar(100),
	@Description	nvarchar(3000),
	@Moderated	bit,
	@DaysToView	int,
	@Active	bit
)
 AS
	-- if we are making the forum non-moderated, remove all forum moderators for this forum
	IF @Moderated = 0
		DELETE FROM Moderators
		WHERE ForumID = @ForumID
	-- update the forum information
	UPDATE Forums SET
		Name = @Name,
		ForumGroupID = @ForumGroupID,
		Description = @Description,
		Moderated = @Moderated,
		DaysToView = @DaysToView,
		Active = @Active
	WHERE ForumID = @ForumID















GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



CREATE   PROCEDURE forums_UpdateForumGroup
(
	@ForumGroupName		nvarchar(256),
	@ForumGroupId	int
)
 AS
	IF @ForumGroupName IS NULL
		DELETE
			ForumGroups
		WHERE
			ForumGroupId = @ForumGroupId
	ELSE
		-- insert a new forum
		UPDATE 
			ForumGroups 
		SET 
			Name = @ForumGroupName
		WHERE 
			ForumGroupId = @ForumGroupId		













GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



create procedure forums_UpdateMessageTemplateList
(
	@MessageID int,
	@Title NVarChar(256),
	@Body NVarChar(4000)
)
 AS
UPDATE
	Messages
SET
	Title = @Title,
	Body = @Body
WHERE
	MessageID = @MessageID
	






GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO



CREATE    PROCEDURE forums_UpdatePost
(
	@PostID	int,
	@Subject	nvarchar(256),
	@Body		ntext,
	@IsLocked	bit,
	@EditedBy	nvarchar(50)
)
AS
	-- this sproc updates a post (called from the moderate/admin page)
	UPDATE 
		Posts 
	SET
		Subject = @Subject,
		Body = @Body,
		IsLocked = @IsLocked
	WHERE 
		PostID = @PostID

	-- We want to track what happened
	INSERT INTO
		ModerationAudit
	VALUES
		(GetDate(), @PostID, @EditedBy, 2, null)












GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO


CREATE PROCEDURE forums_UpdateRoleDescription
(
    @RoleName nvarchar(256),
    @Description nvarchar(512)
)
AS
    IF EXISTS(SELECT Rolename FROM UserRoles (nolock) WHERE Rolename = @RoleName)
        BEGIN
            UPDATE UserRoles
            SET Description=@Description
            WHERE Rolename=@RoleName
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



create         PROCEDURE forums_UpdateUserFromAdminPage
(
	@UserName	nvarchar(50),
	@ProfileApproved bit,
	@Approved bit,
	@Trusted bit
)
AS
	UPDATE
		Users
	SET 
		ProfileApproved = @ProfileApproved,
		Approved = @Approved,
		Trusted = @Trusted
	WHERE
		Username = @UserName
	





















GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO



CREATE        PROCEDURE forums_UpdateUserInfo
(
	@UserName	nvarchar(50),
	@Email		nvarchar(75),
	@FakeEmail	nvarchar(75),
	@URL		nvarchar(100),
	@Signature	nvarchar(255),
	@ForumView	int,
	@ThreadTracking bit,
	@Timezone 	int,
	@Password	nvarchar(20),
	@Occupation nvarchar(100),
	@Location nvarchar(100),
	@Interests nvarchar(200),
	@MsnIm nvarchar(50),
	@AolIm nvarchar(50),
	@YahooIm nvarchar(50),
	@IcqIm nvarchar(50),
	@ShowUnreadTopicsOnly bit,
	@SiteStyle nvarchar(20),
	@AvatarType nvarchar(3),
	@HasAvatar bit,
	@ShowAvatar bit,
	@DateFormat nvarchar(10),
	@PostViewOrder bit
)
 AS
	-- update the user's info ONLY IF we have a valid password
	IF EXISTS(SELECT UserName FROM Users WHERE UserName = @UserName AND Password = @Password)
	BEGIN
		SET NOCOUNT ON
		-- ok, we have a valid user
		UPDATE 
			Users SET
			Email = @Email,
			FakeEmail = @FakeEmail,
			URL = @URL,
			Signature = @Signature,
			ForumView = @ForumView,
			TrackYourPosts = @ThreadTracking,
			Timezone = @Timezone,
                        Occupation = @Occupation,
			Location = @Location,
			Interests = @Interests,
			MSN = @MsnIm,
			Yahoo = @YahooIm,
			AIM = @AolIm,
			ICQ = @IcqIm,
			ShowUnreadTopicsOnly = @ShowUnreadTopicsOnly,
			Approved = 1,
			Style = @SiteStyle,
			AvatarType = @AvatarType,
			HasAvatar = @HasAvatar,			ShowAvatar = @ShowAvatar,
			DateFormat = @DateFormat,
			PostViewOrder = @PostViewOrder
		WHERE 
			UserName = @UserName AND
			Password = @Password
		SET NOCOUNT OFF
		SELECT 1
	END
	ELSE
		-- cripes, the password doesn't match up!
		SELECT 0




















GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO



create  procedure forums_UserHasPostsAwaitingModeration
(
	@UserName nvarchar(50)
)
AS
BEGIN
	-- Can the user moderate all forums?
	IF EXISTS(SELECT NULL FROM Moderators (nolock) WHERE UserName=@UserName AND ForumID=0)

		-- return ALL posts awaiting moderation
		IF EXISTS(SELECT TOP 1 PostID FROM Posts P (nolock) INNER JOIN Forums F (nolock) ON F.ForumID = P.ForumID WHERE Approved = 0)
		  SELECT 1
		ELSE
		  SELECT 0
	ELSE
		-- return only those posts in the forum this user can moderate
		IF EXISTS (SELECT TOP 1 PostID FROM Posts P (nolock) INNER JOIN Forums F (nolock) ON F.ForumID = P.ForumID WHERE Approved = 0 AND P.ForumID IN (SELECT ForumID FROM Moderators (nolock) WHERE UserName=@UserName))
		  SELECT 1
		ELSE
		  SELECT 0
	
END










GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

create procedure forums_Vote (
  @PostID int,
  @Vote nvarchar(2)
)
AS
IF NOT EXISTS (
    SELECT
        PostID 
    FROM 
        Vote 
    WHERE 
        PostID = @PostID AND Vote = @Vote
)
BEGIN
    -- Transacted insert for download count
    BEGIN TRAN
        INSERT INTO 
            Vote
        VALUES
        (
            @PostID,
            @Vote,
            1
        )
    COMMIT TRAN
END
ELSE
BEGIN
    -- Transacted update for download count
    BEGIN TRAN
        UPDATE 
          Vote
        SET 
          VoteCount  =  VoteCount + 1
        WHERE 
          PostID = @PostID AND
          Vote = @Vote
    COMMIT TRAN
END


GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

