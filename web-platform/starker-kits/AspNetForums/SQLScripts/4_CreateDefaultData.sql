print 'Populating table with sample data...'
-- Users in the Forum-Administrators role is allowed to administer the forum
IF NOT EXISTS (SELECT Rolename FROM UserRoles WHERE Rolename='Forum-Administrators')
  INSERT INTO UserRoles (RoleName, Description) VALUES ('Forum-Administrators', 'Forum administrators role.')

-- Add the forum-moderator role
IF NOT EXISTS (SELECT Rolename FROM UserRoles WHERE Rolename='Forum-Moderators')
  INSERT INTO UserRoles (RoleName, Description) VALUES ('Forum-Moderators', 'Forum moderators role.')

-- Default administrative user
IF NOT EXISTS (SELECT Username FROM Users WHERE Username='Admin')
  INSERT INTO Users (Username, [Password], Email, Approved) VALUES ('Admin', 'admin', 'noemail', 1)

-- Add the forum-administrator to the forum-administrators role
IF NOT EXISTS (SELECT Username FROM UsersInRoles WHERE Username='Admin' AND RoleName='Forum-Administrators')
  INSERT INTO UsersInRoles (Username, Rolename) VALUES ('Admin', 'Forum-Administrators')

-- Add the forum-administrator to the forum-moderators role
IF NOT EXISTS (SELECT Username FROM UsersInRoles WHERE Username='Admin' AND RoleName='Forum-Moderators')
  INSERT INTO UsersInRoles (Username, Rolename) VALUES ('Admin', 'Forum-Moderators')

-- Setup the administrative user as a moderator of all forums
IF NOT EXISTS (SELECT Username FROM Moderators WHERE Username='Admin')
  INSERT INTO Moderators VALUES ('Admin', 0, GetDate(), 0)

-- Add Moderation actions
IF NOT EXISTS (SELECT ModerationAction FROM ModerationAction WHERE ModerationAction = 1)
  INSERT INTO ModerationAction VALUES (1, 'Approve')

IF NOT EXISTS (SELECT ModerationAction FROM ModerationAction WHERE ModerationAction = 1)
  INSERT INTO ModerationAction VALUES (2, 'Edit')

IF NOT EXISTS (SELECT ModerationAction FROM ModerationAction WHERE ModerationAction = 1)
  INSERT INTO ModerationAction VALUES (3, 'Move')

IF NOT EXISTS (SELECT ModerationAction FROM ModerationAction WHERE ModerationAction = 1)
  INSERT INTO ModerationAction VALUES (4, 'Delete')

IF NOT EXISTS (SELECT ModerationAction FROM ModerationAction WHERE ModerationAction = 1)
  INSERT INTO ModerationAction VALUES (5, 'Turn off moderation')

-- Emails

SET IDENTITY_INSERT Emails ON
IF NOT EXISTS (SELECT EmailID FROM Emails WHERE EmailID = 1)
  INSERT INTO Emails (EmailID, Subject, Message, Importance, FromAddress, Description) 
  VALUES (1, 'ASP.NET Forums: Your password',	
'<Username>, 

At <TimeSent> you requested that your ASP.NET Forums password be sent to you via email.

Your username and password are:

---------------------------
Username: <Username>
Password:  <Password>
---------------------------

You can login from:
<WebSiteUrl><UrlLogin>', 
1, 'email', 'Send the user their forgotten password.')

IF NOT EXISTS (SELECT EmailID FROM Emails WHERE EmailID = 2)
  INSERT INTO Emails (EmailID, Subject, Message, Importance, FromAddress, Description) 
  VALUES (2, 'ASP.NET Forums: Password Changed',	
'<Username>, 

Your ASP.NET Forums password was changed at <TimeSent>. 

Your password was changed to: 

<Password>', 
2, 'email', 'Notify the user when they have changed their password.')

IF NOT EXISTS (SELECT EmailID FROM Emails WHERE EmailID = 3)
  INSERT INTO Emails (EmailID, Subject, Message, Importance, FromAddress, Description) 
  VALUES (3, 'ASP.NET Forums: <Subject>',	
'At <TimeSent> a message was posted to a thread you were tracking.

--------------------------
<Subject> by <PostedBy>
<PostBody>
--------------------------

To view the complete thread and reply, please visit:
<WebSiteUrl><PostUrl>.  

You were sent this email because you opted to receive email notifications when someone responded to this thread. To unsubscribe to this thread either:
1. Visit the above URL and uncheck ''Email me when someone replies...''
2. Visit your user profile page and uncheck ''Enable email tracking''', 
0, 'email', 'Notify the user when a new message has been posted to a thread they are tracking.')

IF NOT EXISTS (SELECT EmailID FROM Emails WHERE EmailID = 4)
  INSERT INTO Emails (EmailID, Subject, Message, Importance, FromAddress, Description) 
  VALUES (4, 'ASP.NET Forums: Your user account information',	
'<Username>, 

At <TimeSent> you created a new user account at <SiteName>.  

Your username and password are:

---------------------------
Username: <Username>
Password:  <Password>
---------------------------

To begin posting, you will need to log in first.

To login, please visit:
<WebSiteUrl><UrlLogin>

To change your password (after logging in), please visit:
<WebSiteUrl><UrlProfile>

Once you login, you should take a moment to set your user profile, available on the top right of any page within the forum. From the User Information page you can change your password, enter the email address you wish to have display when you post messages, choose how to have the forum posts displayed, and other handy settings.', 
2, 'email', 'Send the user their username/password when they create a new account.')

IF NOT EXISTS (SELECT EmailID FROM Emails WHERE EmailID = 5)
  INSERT INTO Emails (EmailID, Subject, Message, Importance, FromAddress, Description) 
  VALUES (5, 'ASP.NET Forums: Your message has been approved!', 
'At <PostDate> you posted a message titled "<Subject>" to the <ForumName> forum, which is a moderated forum.  This email is to let you know that your message was approved at <TimeSent>.  

You can now view it at: 
<WebSiteUrl><PostUrl>.', 
0, 'email', 'Notify the user that their message to a moderated forum has been approved.')

IF NOT EXISTS (SELECT EmailID FROM Emails WHERE EmailID = 6)
  INSERT INTO Emails (EmailID, Subject, Message, Importance, FromAddress, Description) 
  VALUES (6, 'ASP.NET Forums: Your message has been moved and approved', 
'At <PostDate> you posted a message titled "<Subject>" to a moderated forum.  At <TimeSent> this message was moved to the <ForumName> forum and approved.  

You can view your message at the following URL:
<WebSiteUrl><PostUrl>', 
0, 'email', 'Notify the user that their message has been moved to another forum and approved.')

IF NOT EXISTS (SELECT EmailID FROM Emails WHERE EmailID = 7)
  INSERT INTO Emails (EmailID, Subject, Message, Importance, FromAddress, Description) 
  VALUES (7, 'ASP.NET Forums: Your post has been moved to another forum', 
'At <PostDate> you posted a message titled "<Subject>" to a moderated forum.  At <TimeSent> this message was moved to the <ForumName> forum, where it is still waiting approval.  You will receive a second email once this message is approved in the new forum.', 
0, 'email', 'Notify the user that their message has been moved and NOT yet approved.')

IF NOT EXISTS (SELECT EmailID FROM Emails WHERE EmailID = 8)
  INSERT INTO Emails (EmailID, Subject, Message, Importance, FromAddress, Description) 
  VALUES (8, 'ASP.NET Forums: Your post has been deleted.',	
'At <PostDate> a message you posted title "<Subject>" to the <ForumName> forum was deleted by the forum moderator.  The moderator provided the following reason(s) for deleting your post:

<DeleteReasons>', 
2, 'email', 'Notify the user that their post has been deleted.')

IF NOT EXISTS (SELECT EmailID FROM Emails WHERE EmailID = 9)
  INSERT INTO Emails (EmailID, Subject, Message, Importance, FromAddress, Description) 
  VALUES (9, 'ASP.NET Forums: A post is awaiting moderation.',	
'At <TimeSent> a post was made to the <ForumName> Forum:

Posted By: <PostedBy>
Subject: <Subject>
Posted On: <PostDate>
Post Body:
<PostBody>

Visit <WebSiteUrl><ModerateUrl> to moderate this post...', 
0, 'email', 'Notify the moderator(s) that a new post is awaitng moderation.')
SET IDENTITY_INSERT Emails OFF

-- Error Messages
SET IDENTITY_INSERT Messages ON
IF NOT EXISTS (SELECT MessageID FROM Messages WHERE MessageID = 1)
  INSERT INTO Messages (MessageId, Title, Body) VALUES (1, 'Error: You are Unable to Administer', 'In order to perform <i>any</i> administration duties on this Web site, your user account must be marked as having administrator rights.  Unfortunately, your account does not have such rights.<p>If you believe you''ve reached this message in error, please notify the Web site administrator.')

IF NOT EXISTS (SELECT MessageID FROM Messages WHERE MessageID = 2)
  INSERT INTO Messages (MessageId, Title, Body) VALUES (2, 'Error: You are Unable to Edit this Post', 'Due to security settings, you are not able to edit this post.  Most likely, another moderator has already approved the post you are attempting to edit. Administrators may edit <i>any</i> post.  Moderators may only edit non-Approved posts in forums they have been selected to moderate.<p>If you believe you''ve reached this message in error, please contact the Web site administrator.')

IF NOT EXISTS (SELECT MessageID FROM Messages WHERE MessageID = 3)
  INSERT INTO Messages (MessageId, Title, Body) VALUES (3, 'Error: You Are Not Able to Moderate', 'In order to participate in the moderation of posts, you must have been granted adequate permissions from the Web site administrator.  That is, the Web site administrator must have explicitly setup your User account to allow for post moderation.  Please contact the Web site administrator if you believe you''ve reached this message in error.')

IF NOT EXISTS (SELECT MessageID FROM Messages WHERE MessageID = 4)
  INSERT INTO Messages (MessageId, Title, Body) VALUES (4, 'Error: Attempting to Insert a Duplicate Post', 'You have, in the past, attempted to post a question on this forum, or another forum, with the same body.  Duplicate posts are not allowed.<p><DuplicatePost>')

IF NOT EXISTS (SELECT MessageID FROM Messages WHERE MessageID = 5)
  INSERT INTO Messages (MessageId, Title, Body) VALUES (5, 'Error: File Not Found', 'The file you requested cannot be found.')

IF NOT EXISTS (SELECT MessageID FROM Messages WHERE MessageID = 6)
  INSERT INTO Messages (MessageId, Title, Body) VALUES (6, 'Error: Unknown forum', 'The forum you requested does not exist.')

IF NOT EXISTS (SELECT MessageID FROM Messages WHERE MessageID = 7)
  INSERT INTO Messages (MessageId, Title, Body) VALUES (7, 'New Account Created', 'You will soon receive an email which will contain a randomly generated password.  Once you have this information you may login at the <UrlLogin>.<p>Once you''ve logged in, you may wish to visit your user profile and change your password - all of these details will be provided in the email.')

IF NOT EXISTS (SELECT MessageID FROM Messages WHERE MessageID = 8)
  INSERT INTO Messages (MessageId, Title, Body) VALUES (8, 'Post Pending Moderation', 'Since you posted to a moderated forum, a forum administrator must approve your post before it will become visible.  Please be patient, this may take anywhere from a few minutes to many hours.<p>Note that you will receive an email when your post is approved.<p><PendingModeration>')

IF NOT EXISTS (SELECT MessageID FROM Messages WHERE MessageID = 9)
  INSERT INTO Messages (MessageId, Title, Body) VALUES (9, 'Error: Post Does Not Exist', 'The post you attempted to view does not exist.  Most likely, the message you are trying to view has been deleted by one of the site''s administrators.')

IF NOT EXISTS (SELECT MessageID FROM Messages WHERE MessageID = 10)
  INSERT INTO Messages (MessageId, Title, Body) VALUES (10, 'Error: PostID Parameter Not Specified', 'You have attempted to visit the Web page to display a forum''s post, but, for some reason, the PostID was not successfully passed in.')

IF NOT EXISTS (SELECT MessageID FROM Messages WHERE MessageID = 11)
  INSERT INTO Messages (MessageId, Title, Body) VALUES (11, 'Error: There was a Problem Posting your Message', 'There was a problem posting your message.  This is most likely due to the fact that while you were replying to a message, it has been deleted by the administrator.  We apologize for any inconvenience.')

IF NOT EXISTS (SELECT MessageID FROM Messages WHERE MessageID = 12)
  INSERT INTO Messages (MessageId, Title, Body) VALUES (12, 'Error: The post you are attempting to view has not been approved', 'You are unable to view this message due to the fact that it has not been approved. Most likely this is because you are trying to view a post that was posted to a moderated forum and has not yet been approved by one of the forum administrators.<p>Once this post has been approved, it will appear in the forum list and you will be able to view its contents.')

IF NOT EXISTS (SELECT MessageID FROM Messages WHERE MessageID = 13)
  INSERT INTO Messages (MessageId, Title, Body) VALUES (13, 'Your user profile has been successfully updated', 'Your user information has been updated and will be reflected immediately.<p>Please return to the <UrlHome>')

IF NOT EXISTS (SELECT MessageID FROM Messages WHERE MessageID = 14)
  INSERT INTO Messages (MessageId, Title, Body) VALUES (14, 'Error: User Does Not Exist', 'The user you attempted to view does not exist.')

IF NOT EXISTS (SELECT MessageID FROM Messages WHERE MessageID = 15)
  INSERT INTO Messages (MessageId, Title, Body) VALUES (15, 'User Password Updated', 'Your user password has been updated and mailed to you.')

IF NOT EXISTS (SELECT MessageID FROM Messages WHERE MessageID = 16)
  INSERT INTO Messages (MessageId,  Title, Body) VALUES (16, 'Error: User Password Update Failed', 'Your password update operation failed - your password has not been changed.')

SET IDENTITY_INSERT Messages OFF
-- Create a new forum group
DECLARE @ForumGroupIdent int
DECLARE @ForumIdent int

INSERT INTO ForumGroups VALUES ('Sample Forum Group', 0)
SET @ForumGroupIdent = @@IDENTITY

-- Create a new forum
INSERT INTO Forums VALUES (@ForumGroupIdent, 0, 'Sample Unmoderated Forum', 'A sample unmoderated forum created when the AspNetForums were installed.', GetDate(), 0, 0, 1, 10, 0, 0, 0, 0, null, '', 0)
SET @ForumIdent = @@IDENTITY

-- Create a new post in this forum
exec dbo.forums_AddPost @ForumIdent, 0, 'Sample Post in unmoderated forum', 'Admin', 'Sample post in unmoderated forum', 0, null

INSERT INTO Forums VALUES (@ForumGroupIdent, 0, 'Sample Moderated Forum', 'A sample moderated forum created when the AspNetForums were installed.', GetDate(), 1, 0, 1, 20, 0, 0, 0, 0, null, '', 0)
SET @ForumIdent = @@IDENTITY

-- Create a new post in this forum
exec dbo.forums_AddPost @ForumIdent, 0, 'Sample Post in moderated forum', 'Admin', 'Sample post in moderated forum', 0, null

INSERT INTO Forums VALUES (@ForumGroupIdent, 0, 'Sample Private Forum', 'A sample private forum created when the AspNetForums were installed.', GetDate(), 0, 0, 1, 30, 0, 0, 0, 0, null, '', 0)
SET @ForumIdent = @@IDENTITY

-- Create a new post in this forum
exec dbo.forums_AddPost @ForumIdent, 0, 'Sample Post in private forum', 'Admin', 'Sample post in private forum', 0, null

-- Make the forum private
INSERT INTO PrivateForums VALUES (@ForumIdent, 'Forum-Administrators')