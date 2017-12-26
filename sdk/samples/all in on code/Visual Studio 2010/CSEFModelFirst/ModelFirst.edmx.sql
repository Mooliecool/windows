
-- --------------------------------------------------
-- Date Created: 11/26/2009 21:54:31
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

IF OBJECT_ID(N'[dbo].[FK_DepartmentCourse]', 'F') IS NOT NULL
    ALTER TABLE [dbo].[Courses] DROP CONSTRAINT [FK_DepartmentCourse]
GO
IF OBJECT_ID(N'[dbo].[FK_CourseInstructor_Course]', 'F') IS NOT NULL
    ALTER TABLE [dbo].[CourseInstructor] DROP CONSTRAINT [FK_CourseInstructor_Course]
GO
IF OBJECT_ID(N'[dbo].[FK_CourseInstructor_Instructor]', 'F') IS NOT NULL
    ALTER TABLE [dbo].[CourseInstructor] DROP CONSTRAINT [FK_CourseInstructor_Instructor]
GO
IF OBJECT_ID(N'[dbo].[FK_StudentCourseStudent]', 'F') IS NOT NULL
    ALTER TABLE [dbo].[CourseStudents] DROP CONSTRAINT [FK_StudentCourseStudent]
GO
IF OBJECT_ID(N'[dbo].[FK_CourseCourseStudent]', 'F') IS NOT NULL
    ALTER TABLE [dbo].[CourseStudents] DROP CONSTRAINT [FK_CourseCourseStudent]
GO
IF OBJECT_ID(N'[dbo].[FK_Instructor_inherits_Person]', 'F') IS NOT NULL
    ALTER TABLE [dbo].[People_Instructor] DROP CONSTRAINT [FK_Instructor_inherits_Person]
GO
IF OBJECT_ID(N'[dbo].[FK_Student_inherits_Person]', 'F') IS NOT NULL
    ALTER TABLE [dbo].[People_Student] DROP CONSTRAINT [FK_Student_inherits_Person]
GO
IF OBJECT_ID(N'[dbo].[FK_Admin_inherits_Person]', 'F') IS NOT NULL
    ALTER TABLE [dbo].[People_Admin] DROP CONSTRAINT [FK_Admin_inherits_Person]
GO
IF OBJECT_ID(N'[dbo].[FK_BusinessStudent_inherits_Student]', 'F') IS NOT NULL
    ALTER TABLE [dbo].[People_BusinessStudent] DROP CONSTRAINT [FK_BusinessStudent_inherits_Student]
GO

-- --------------------------------------------------
-- Dropping existing tables
-- --------------------------------------------------

IF OBJECT_ID(N'[dbo].[People]', 'U') IS NOT NULL
    DROP TABLE [dbo].[People];
GO
IF OBJECT_ID(N'[dbo].[Courses]', 'U') IS NOT NULL
    DROP TABLE [dbo].[Courses];
GO
IF OBJECT_ID(N'[dbo].[Departments]', 'U') IS NOT NULL
    DROP TABLE [dbo].[Departments];
GO
IF OBJECT_ID(N'[dbo].[CourseStudents]', 'U') IS NOT NULL
    DROP TABLE [dbo].[CourseStudents];
GO
IF OBJECT_ID(N'[dbo].[People_Instructor]', 'U') IS NOT NULL
    DROP TABLE [dbo].[People_Instructor];
GO
IF OBJECT_ID(N'[dbo].[People_Student]', 'U') IS NOT NULL
    DROP TABLE [dbo].[People_Student];
GO
IF OBJECT_ID(N'[dbo].[People_Admin]', 'U') IS NOT NULL
    DROP TABLE [dbo].[People_Admin];
GO
IF OBJECT_ID(N'[dbo].[People_BusinessStudent]', 'U') IS NOT NULL
    DROP TABLE [dbo].[People_BusinessStudent];
GO
IF OBJECT_ID(N'[dbo].[CourseInstructor]', 'U') IS NOT NULL
    DROP TABLE [dbo].[CourseInstructor];
GO

-- --------------------------------------------------
-- Creating all tables
-- --------------------------------------------------

-- Creating table 'People'
CREATE TABLE [dbo].[People] (
    [PersonID] int  NOT NULL,
    [Name_FirstName] nvarchar(max)  NOT NULL,
    [Name_LastName] nvarchar(max)  NOT NULL,
    [Address_Country] nvarchar(max)  NULL,
    [Address_City] nvarchar(max)  NULL,
    [Address_Zipcode] nvarchar(max)  NULL
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
-- Creating table 'Departments'
CREATE TABLE [dbo].[Departments] (
    [DepartmentID] int  NOT NULL,
    [Name] nvarchar(max)  NOT NULL,
    [Budget] decimal(18,0)  NULL,
    [StartDate] datetime  NOT NULL
);
GO
-- Creating table 'CourseStudents'
CREATE TABLE [dbo].[CourseStudents] (
    [PersonID] int  NOT NULL,
    [CourseID] int  NOT NULL,
    [Score] int  NULL
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
    [EnrollmentDate] datetime  NOT NULL,
    [Degree] int  NULL,
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
-- Creating primary key on [CourseID] in table 'Courses'
ALTER TABLE [dbo].[Courses] WITH NOCHECK 
ADD CONSTRAINT [PK_Courses]
    PRIMARY KEY CLUSTERED ([CourseID] ASC)
    ON [PRIMARY]
GO
-- Creating primary key on [DepartmentID] in table 'Departments'
ALTER TABLE [dbo].[Departments] WITH NOCHECK 
ADD CONSTRAINT [PK_Departments]
    PRIMARY KEY CLUSTERED ([DepartmentID] ASC)
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