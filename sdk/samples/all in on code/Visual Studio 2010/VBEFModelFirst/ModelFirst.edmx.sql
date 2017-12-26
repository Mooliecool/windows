
-- --------------------------------------------------
-- Date Created: 11/27/2009 17:55:31
-- Generated from EDMX file: ModelFirst.edmx
-- --------------------------------------------------

SET QUOTED_IDENTIFIER OFF;
SET ANSI_NULLS ON;
GO

USE [ModelFirstGeneratedDB]
GO
IF SCHEMA_ID(N'dbo') IS NULL EXECUTE(N'CREATE SCHEMA [dbo]')
GO

-- --------------------------------------------------
-- Dropping existing FK constraints
-- --------------------------------------------------


-- --------------------------------------------------
-- Dropping existing tables
-- --------------------------------------------------


-- --------------------------------------------------
-- Creating all tables
-- --------------------------------------------------

-- Creating table 'People'
CREATE TABLE [dbo].[People] (
    [PersonID] int  NOT NULL,
    [Name_FirstName] nvarchar(max)  NOT NULL,
    [Name_LastName] nvarchar(max)  NOT NULL,
    [Address_City] nvarchar(max)  NULL,
    [Address_Country] nvarchar(max)  NULL,
    [Address_Zipcode] nvarchar(max)  NULL
);
GO
-- Creating table 'Departments'
CREATE TABLE [dbo].[Departments] (
    [DepartmentID] int  NOT NULL,
    [Name] nvarchar(max)  NOT NULL,
    [Budget] decimal(18,0)  NULL,
    [StartDate] datetime  NOT NULL
);
GO
-- Creating table 'Courses'
CREATE TABLE [dbo].[Courses] (
    [CourseID] int  NOT NULL,
    [Title] nvarchar(max)  NOT NULL,
    [Credits] int  NOT NULL,
    [DepartmentID] int  NOT NULL
);
GO
-- Creating table 'CourseStudents'
CREATE TABLE [dbo].[CourseStudents] (
    [Score] nvarchar(max)  NULL,
    [PersonID] int  NOT NULL,
    [CourseID] int  NOT NULL
);
GO
-- Creating table 'People_Instructor'
CREATE TABLE [dbo].[People_Instructor] (
    [HireDate] datetime  NOT NULL,
    [PersonID] int  NOT NULL
);
GO
-- Creating table 'People_Student'
CREATE TABLE [dbo].[People_Student] (
    [EnrollmentDate] nvarchar(max)  NOT NULL,
    [Degreee] int  NULL,
    [Credits] int  NULL,
    [PersonID] int  NOT NULL
);
GO
-- Creating table 'People_Admin'
CREATE TABLE [dbo].[People_Admin] (
    [AdminDate] datetime  NOT NULL,
    [PersonID] int  NOT NULL
);
GO
-- Creating table 'People_BusinessStudent'
CREATE TABLE [dbo].[People_BusinessStudent] (
    [BusinessCredits] int  NULL,
    [PersonID] int  NOT NULL
);
GO
-- Creating table 'CourseInstructor'
CREATE TABLE [dbo].[CourseInstructor] (
    [Courses_CourseID] int  NOT NULL,
    [Instructors_PersonID] int  NOT NULL
);
GO

-- --------------------------------------------------
-- Creating all Primary Key Constraints
-- --------------------------------------------------

-- Creating primary key on [PersonID] in table 'People'
ALTER TABLE [dbo].[People] WITH NOCHECK 
ADD CONSTRAINT [PK_People]
    PRIMARY KEY CLUSTERED ([PersonID] ASC)
    ON [PRIMARY]
GO
-- Creating primary key on [DepartmentID] in table 'Departments'
ALTER TABLE [dbo].[Departments] WITH NOCHECK 
ADD CONSTRAINT [PK_Departments]
    PRIMARY KEY CLUSTERED ([DepartmentID] ASC)
    ON [PRIMARY]
GO
-- Creating primary key on [CourseID] in table 'Courses'
ALTER TABLE [dbo].[Courses] WITH NOCHECK 
ADD CONSTRAINT [PK_Courses]
    PRIMARY KEY CLUSTERED ([CourseID] ASC)
    ON [PRIMARY]
GO
-- Creating primary key on [PersonID], [CourseID] in table 'CourseStudents'
ALTER TABLE [dbo].[CourseStudents] WITH NOCHECK 
ADD CONSTRAINT [PK_CourseStudents]
    PRIMARY KEY CLUSTERED ([PersonID], [CourseID] ASC)
    ON [PRIMARY]
GO
-- Creating primary key on [PersonID] in table 'People_Instructor'
ALTER TABLE [dbo].[People_Instructor] WITH NOCHECK 
ADD CONSTRAINT [PK_People_Instructor]
    PRIMARY KEY CLUSTERED ([PersonID] ASC)
    ON [PRIMARY]
GO
-- Creating primary key on [PersonID] in table 'People_Student'
ALTER TABLE [dbo].[People_Student] WITH NOCHECK 
ADD CONSTRAINT [PK_People_Student]
    PRIMARY KEY CLUSTERED ([PersonID] ASC)
    ON [PRIMARY]
GO
-- Creating primary key on [PersonID] in table 'People_Admin'
ALTER TABLE [dbo].[People_Admin] WITH NOCHECK 
ADD CONSTRAINT [PK_People_Admin]
    PRIMARY KEY CLUSTERED ([PersonID] ASC)
    ON [PRIMARY]
GO
-- Creating primary key on [PersonID] in table 'People_BusinessStudent'
ALTER TABLE [dbo].[People_BusinessStudent] WITH NOCHECK 
ADD CONSTRAINT [PK_People_BusinessStudent]
    PRIMARY KEY CLUSTERED ([PersonID] ASC)
    ON [PRIMARY]
GO
-- Creating primary key on [Courses_CourseID], [Instructors_PersonID] in table 'CourseInstructor'
ALTER TABLE [dbo].[CourseInstructor] WITH NOCHECK 
ADD CONSTRAINT [PK_CourseInstructor]
    PRIMARY KEY NONCLUSTERED ([Courses_CourseID], [Instructors_PersonID] ASC)
    ON [PRIMARY]
GO

-- --------------------------------------------------
-- Creating all Foreign Key Constraints
-- --------------------------------------------------

-- Creating foreign key on [DepartmentID] in table 'Courses'
ALTER TABLE [dbo].[Courses] WITH NOCHECK 
ADD CONSTRAINT [FK_DepartmentCourse]
    FOREIGN KEY ([DepartmentID])
    REFERENCES [dbo].[Departments]
        ([DepartmentID])
    ON DELETE NO ACTION ON UPDATE NO ACTION
GO
-- Creating foreign key on [Courses_CourseID] in table 'CourseInstructor'
ALTER TABLE [dbo].[CourseInstructor] WITH NOCHECK 
ADD CONSTRAINT [FK_CourseInstructor_Course]
    FOREIGN KEY ([Courses_CourseID])
    REFERENCES [dbo].[Courses]
        ([CourseID])
    ON DELETE NO ACTION ON UPDATE NO ACTION
GO
-- Creating foreign key on [Instructors_PersonID] in table 'CourseInstructor'
ALTER TABLE [dbo].[CourseInstructor] WITH NOCHECK 
ADD CONSTRAINT [FK_CourseInstructor_Instructor]
    FOREIGN KEY ([Instructors_PersonID])
    REFERENCES [dbo].[People_Instructor]
        ([PersonID])
    ON DELETE NO ACTION ON UPDATE NO ACTION
GO
-- Creating foreign key on [PersonID] in table 'CourseStudents'
ALTER TABLE [dbo].[CourseStudents] WITH NOCHECK 
ADD CONSTRAINT [FK_StudentCourseStudent]
    FOREIGN KEY ([PersonID])
    REFERENCES [dbo].[People_Student]
        ([PersonID])
    ON DELETE NO ACTION ON UPDATE NO ACTION
GO
-- Creating foreign key on [CourseID] in table 'CourseStudents'
ALTER TABLE [dbo].[CourseStudents] WITH NOCHECK 
ADD CONSTRAINT [FK_CourseCourseStudent]
    FOREIGN KEY ([CourseID])
    REFERENCES [dbo].[Courses]
        ([CourseID])
    ON DELETE NO ACTION ON UPDATE NO ACTION
GO
-- Creating foreign key on [PersonID] in table 'People_Instructor'
ALTER TABLE [dbo].[People_Instructor] WITH NOCHECK 
ADD CONSTRAINT [FK_Instructor_inherits_Person]
    FOREIGN KEY ([PersonID])
    REFERENCES [dbo].[People]
        ([PersonID])
    ON DELETE NO ACTION ON UPDATE NO ACTION
GO
-- Creating foreign key on [PersonID] in table 'People_Student'
ALTER TABLE [dbo].[People_Student] WITH NOCHECK 
ADD CONSTRAINT [FK_Student_inherits_Person]
    FOREIGN KEY ([PersonID])
    REFERENCES [dbo].[People]
        ([PersonID])
    ON DELETE NO ACTION ON UPDATE NO ACTION
GO
-- Creating foreign key on [PersonID] in table 'People_Admin'
ALTER TABLE [dbo].[People_Admin] WITH NOCHECK 
ADD CONSTRAINT [FK_Admin_inherits_Person]
    FOREIGN KEY ([PersonID])
    REFERENCES [dbo].[People]
        ([PersonID])
    ON DELETE NO ACTION ON UPDATE NO ACTION
GO
-- Creating foreign key on [PersonID] in table 'People_BusinessStudent'
ALTER TABLE [dbo].[People_BusinessStudent] WITH NOCHECK 
ADD CONSTRAINT [FK_BusinessStudent_inherits_Student]
    FOREIGN KEY ([PersonID])
    REFERENCES [dbo].[People_Student]
        ([PersonID])
    ON DELETE NO ACTION ON UPDATE NO ACTION
GO

-- --------------------------------------------------
-- Script has ended
-- --------------------------------------------------