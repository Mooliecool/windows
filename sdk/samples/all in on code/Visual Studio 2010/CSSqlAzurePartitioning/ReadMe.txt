================================================================================
       Windows Azure APPLICATION: CSSqlAzurePartitioning Overview                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:  

This sample demonstrates how to partition your data in SQL Azure. It shows two
ways: one is vertical partitioning and the other is horizontal partitioning.

In this version of vertically partitioning for SQL Azure we are dividing all the 
tables in the schema across two or more SQL Azure databases. In choosing which 
tables to group together on a single database you need to understand how large 
each of your tables are and their potential future growth 每 the goal is to evenly 
distribute the tables so that each database is the same size.

When partitioning your workload across SQL Azure databases, you lose some of the 
features of having all the tables in a single database. Some of the considerations 
when using this technique include:

1. Foreign keys across databases are not support. In other words, a primary key in 
   a lookup table in one database cannot be referenced by a foreign key in a table 
   on another database. This is a similar restriction to SQL Server＊s cross database 
   support for foreign keys.
2. You cannot have transactions that span databases, even if you are using Microsoft 
   Distributed Transaction Manager on the client side. This means that you cannot 
   rollback an insert on one database, if an insert on another database fails. This 
   restriction can be negated through client side coding 每 you need to catch exceptions 
   and execute "undo" scripts against the successfully completed statements.

In this version of horizontal partitioning, every table exists in all the databases 
in the partition set. We are using a hash base partitioning schema in this example 
每 hashing on the primary key of the row. The middle layer determines which database 
to write each row based on the primary key of the data being written. This allows 
us to evenly divide the data across all the databases, regardless of individual 
table growth. The data access knows how to find the data based on the primary key, 
and combines the results to return one result set to the caller.

When horizontal partitioning your database you lose some of the features of 
having all the data in a single database. Some considerations when using this 
technique include:

1. Foreign keys across databases are not supported. In other words, a primary 
   key in a lookup table in one database cannot be referenced by a foreign 
   key in a table on another database. This is a similar restriction to SQL 
   Server＊s cross database support for foreign keys.
2. You cannot have transactions that span two databases, even if you are 
   using Microsoft Distributed Transaction Manager on the client side. This 
   means that you cannot rollback an insert on one database, if an insert on 
   another database fails. This restriction can be mitigated through client 
   side coding 每 you need to catch exceptions and execute "undo" scripts 
   against the successfully completed statements.
3. All the primary keys need to be unique identifier. This allows us to 
   guarantee the uniqueness of the primary key in the middle layer.
4. The example code shown in this sample doesn＊t allow you to dynamically 
   change the number of databases that are in the partition set. The number 
   of databases is hard coded in the SqlAzureHelper class in the 
   ConnectionStringNames property.
5. Importing data from SQL Server to a horizontally partitioned database 
   requires that you move each row one at a time emulating the hashing of the 
   primary keys.


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

If you would like to test this sample project locally, you need to install

Microsoft SQL Server 2008 R2 Express 
Microsoft SQL Server 2008 R2 Management Studio Express

You can get them from:
http://blogs.msdn.com/b/petersad/archive/2009/11/13/how-to-install-sql-server-2008-r2-express-edition-november-ctp.aspx


/////////////////////////////////////////////////////////////////////////////
Demo:         

Step 1. You can run this sample by logging into SQL Azure or local SQL Server.
	    [NOTE: Since the SQL Azure only support the SQL Server Authentication 
		Mode, so if you run this sample against your local server you should 
		configure SQL Server to accept SQL Authentication mode.]

Step 2. Create two databases(either on SQL Azure or local Server): "Courses" 
        and "Students", then execute the scripts in Course.sql and Student.sql 
		respectively. 

Step 3. Execute the scripts in StudentSQLQuery.sql on database "Students" and 
        execute CourseSQLQuery.sql on database "Courses" to generate some 
		trivial data for testing purpose.

Step 4. Create another two databases, and name them "Database001", 
        "Database002".
  
Step 5. Execute the scripts in Accounts.sql on both databases "Database001" 
        and "Database002". Then Database001 and Database002 will both have a 
		same schema table "Accounts".

Step 6. You have to modify the connectionString in Web.config to make them 
        point to your SQL Azure server or local SQL server.

Step 7. Build the solution and make sure the Default.aspx is the Startup Page, 
        then run it.


/////////////////////////////////////////////////////////////////////////////
Implementation:

Step 1. Please refer to Demo section to create databases/tables.

Step 2. Create a Windows Azure project and add an ASP.NET Web Role with the 
        default name.

Step 3. Add two WebForms, name them "Vertical Partitioning.aspx", "Horizontal 
        Partitioning.aspx".

Step 4. In Default.aspx, add two HyperLink controls and set the 
        NavigateUrl="~/Vertical Partitioning.aspx" and 
		NavigateUrl="~/Horizontal Partitioning.aspx" respectively.

Step 5. In Vertical Partitioning.aspx, add three GridViews, use GridView 
        Tasks Wizard to set data source for GridView1 and GridView2.  
		GridView1's DataSource should be Students.Student table. GridView2's 
		DataSource should be Courses.Course table. For more detail, you can 
		look into Vertical Partitioning.aspx.

Step 6. In Vertical Partitioning.aspx.cs, set the data source for GridView3 
        as follows.
		[Code snippets]
		        // Load data.
                var studentDataReader = SQLAzureHelper.ExecuteReader(
                        ConfigurationManager.ConnectionStrings["StudentsConnectionString"].ConnectionString,
                    sqlConnection =>
                    {
                        SqlCommand sqlCommand =
                            new SqlCommand("SELECT StudentId, StudentName FROM Student",
                                sqlConnection);
                        return (sqlCommand.ExecuteReader());
                    });

                var courseDataReader = SQLAzureHelper.ExecuteReader(
                    ConfigurationManager.ConnectionStrings["CoursesConnectionString"].ConnectionString,
                    sqlConnection =>
                    {
                        SqlCommand sqlCommand =
                            new SqlCommand("SELECT CourseName, StudentId FROM Course",
                                sqlConnection);
                        return (sqlCommand.ExecuteReader());
                    });

                // Join two tables on different SQL Azure databases using LINQ. 
                var query =
                    from student in studentDataReader
                    join course in courseDataReader on
                        (Int32)student["StudentId"] equals (Int32)course["StudentId"]
                    select new
                    {
                        CourseName = (string)course["CourseName"],
                        StudentName = (string)student["StudentName"]
                    };

                this.GridView3.DataSource = query;
                this.GridView3.DataBind();
		[/Code snippets]

Step 7. In Horizontal Partioning.aspx. add two GridViews, use GridView Tasks 
        Wizard to set data source for GridView1 and GridView2.  GridView1's 
        DataSource should be Database001.Accounts table. GridView2's 
		DataSource should be Database002.Accounts table. For more detail, you 
		can look into Horizontal Partioning.aspx.


/////////////////////////////////////////////////////////////////////////////
References:  

Development Considerations in SQL Azure
http://msdn.microsoft.com/en-us/library/ee730903.aspx

How to: Connect to SQL Azure Through ASP.NET
http://msdn.microsoft.com/en-us/library/ee621781.aspx

Unique identifier and Clustered Indexes
http://blogs.msdn.com/b/sqlazure/archive/2010/05/05/10007304.aspx

Vertical Partitioning in SQL Azure: Part 1
http://blogs.msdn.com/b/sqlazure/archive/2010/05/17/10014011.aspx

SQL Azure Horizontal Partitioning: Part 2
http://blogs.msdn.com/b/sqlazure/archive/2010/06/24/10029719.aspx


/////////////////////////////////////////////////////////////////////////////