Install files for Program O v2.
--------------------------------------------------------------------------------------------


step 1 - upload
--------------------------------------------------------------------------------------------
# Upload the files to your server, to the directory of your choice.

step 2 - Create the Database
--------------------------------------------------------------------------------------------
This step will vary, depending on whether you're installing locally or remotely, and also on
where you host your remote installation.

For local installation, access your local MySQL database server through the tool of your choice
(e.g. the MySQL Toolkit, phpMyAdmin, etc.), create an empty database and note the name; then create
a user/password for that database with full permissions.

For remote database creation, it's highly recommended to contact your hosting provider for details.
Often times you will need to log on to a "Dashboard" or "Control Panel" page in order to create
the remote database, and there are literally dozens of different versions, each with their own way
of doing things, so trying to outline remote database creation here would be near impossible.

Please note that we STRONGLY suggest the use of phpMyAdmin for monitoring, maintaining and working
with your database IN ADDITION to the admin pages. The vast majority of hosting providers
offer phpMyAdmin as a part of your database package, so contact them if you're unsure about
whether you have it. For local installations of phpMyAdmin, visit http://www.phpmyadmin.net
for detailed information on installing and usint this very powerful and easy to use database
management tool.

step 3 - Run the installer
--------------------------------------------------------------------------------------------
All that's required is that you point any major web browser to your newly created
Program O directory (e.g. http://www.example.com/ProgramO/) and the install
script will do the rest. Just follow the prompts, and supply the necessary
information in the forms provided.

step 4 - Upload the AIML files
--------------------------------------------------------------------------------------------
To perform this action when the installation is complete, go to the admin page
(e.g. http://www.example.com/ProgramO/admin/) and log in with the admin credentials
you provided during the installation process. Then select "Upload AIML" from
the left nav menu and follow the prompts.