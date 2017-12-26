CREATE DATABASE LinqToSqlSample
GO

USE LinqToSqlSample
GO

CREATE TABLE Roles
(
   Code     varchar(4),
   Name     varchar(256)
)
GO


CREATE TABLE Employees
(
   Id       int,
   Name     varchar(256),
   eMail    varchar(256),
   Role     varchar(4),
   Location varchar(128)
)
GO


INSERT INTO Roles (code, name) VALUES ('PM', 'Program Manager')
GO

INSERT INTO Roles (code, name) VALUES ('SDE', 'Software Development Engineer')
GO

INSERT INTO Roles (code, name) VALUES ('SDET', 'Software Development Engineer in Test')
GO

INSERT INTO Employees (id, name, email, role, location) VALUES (1, 'Employee 1', 'employee1@microsoft.com', 'PM', 'Redmond')
GO

INSERT INTO Employees (id, name, email, role, location) VALUES (2, 'Employee 2', 'employee2@microsoft.com', 'SDE', 'Redmond')
GO

INSERT INTO Employees (id, name, email, role, location) VALUES (3, 'Employee 3', 'employee3@microsoft.com', 'SDE', 'Redmond')
GO

INSERT INTO Employees (id, name, email, role, location) VALUES (4, 'Employee 4', 'employee4@microsoft.com', 'SDE', 'Redmond')
GO

INSERT INTO Employees (id, name, email, role, location) VALUES (5, 'Employee 5', 'employee5@microsoft.com', 'SDET', 'Redmond')
GO

INSERT INTO Employees (id, name, email, role, location) VALUES (6, 'Employee 6', 'employee6@microsoft.com', 'SDET', 'Redmond')
GO

INSERT INTO Employees (id, name, email, role, location) VALUES (7, 'Employee 7', 'employee7@microsoft.com', 'SDE', 'Redmond')
GO

INSERT INTO Employees (id, name, email, role, location) VALUES (8, 'Employee 8', 'employee8@microsoft.com', 'PM', 'Redmond')
GO

INSERT INTO Employees (id, name, email, role, location) VALUES (9, 'Employee 9', 'employee9@microsoft.com', 'PM', 'Shanghai')
GO

INSERT INTO Employees (id, name, email, role, location) VALUES (10, 'Employee 10', 'employee10@microsoft.com', 'SDE', 'Shanghai')
GO

INSERT INTO Employees (id, name, email, role, location) VALUES (11, 'Employee 11', 'employee11@microsoft.com', 'SDE', 'Shanghai')
GO

INSERT INTO Employees (id, name, email, role, location) VALUES (11, 'Employee 12', 'employee11@microsoft.com', 'SDET', 'Shanghai')
GO
