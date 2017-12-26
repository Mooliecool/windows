# [Program O](http://www.program-o.com)

Readme info:
- Version: 2.5.3
- Authors: Elizabeth Perreau and Dave Morton
- Date: May 17th 2014

**NEWS:** Program O version 2.4.6 introduces changes to the config file, so will require at least an overlay install.

## Introduction

Program O is an AIML interpretor written in PHP, and uses a MySQL database to store
chatbot information, including the AIML files used to formulate the chatbot's responses.


## Configuration

Program O supports the creation of multiple chatbots. The individual bots are configured
from the admin area. Bot specific variables are stored in the database. Configuration
of Program O itself is done in `/config/global_config.php`

## Using the bot

After installation you can chat with your Program O chatbot using several methods:

- Through a static HTML page (example in [`/gui/plain/index.php`](gui/plain/index.php))
- Through an AJAX enabled web page (example in [`/gui/jquery/index.php`](gui/jquery/index.php))
- Through the Program O API (example in [`/gui/xml/index.php`](gui/xml/index.php))

For advanced botmasters and programmers, it is possible to use the Program O
API to access a chatbot from stand-alone applications through HTTP requests.
for information on how to create and/or use such a stand-alone application, please
visit [the Program O website](http://www.program-o.com).

## Foreign Language Support

v2.2 and higher now supports Unicode characters, allowing chatbots to be created that use
non-English languages, such as Arabic, Chinese, French, Greek, Russian, Turkish, Thai,
and many others.

## Upgrade

Upgrading from version 1 to version 2 is no longer supported. If you want to "upgrade"
your chatbot from version 1 to version 2, you will need to save your AIML files from the
current chatbot, perform a "clean install" of Program O, and then upload the AIML files
to the new installation. With the release of Program O version 2.2, the differences
in structure and functionality between version 1 and version 2 became too great to
continue supporting the transition.

## Clean install

Starting with version 2.0.1 and later, there is an automatic installer provided for clean
installation. Simply point your web browser to your freshly uploaded Program O directory
to run the install script. Then follow the instructions on the page. The installer is
designed to only run once.

Please note that the installer script will NOT create your database for you, nor will
it install your bot's AIML files. The database, along with the username/password used to
access it, needs to be created in advance. See the file [fresh_install_README.txt](fresh_install_README.txt) for more
information.

After you create the database, and once you complete the installation process, simply go to
your new Program O admin page (e.g. http://www.example.com/ProgramO/admin) and log in using
the admin credentials you provided during the install process. Then select "Upload AIML"
from the navigation link on the left and upload your AIML files. Then sit back and enjoy your
new bot!

## Program O has a WordPress Plugin!

If you have a WordPress driven website, and wish to incorporate your Program O chatbot into your
website with a minimum of fuss and hassle, then maybe the Elizaibots plugin is what you need!
Add an Elizaibots chatbot to your wordpress site using the shortcode to embed the bot and conversation
on your blog. Keep your readers amused! Check out http://wordpress.org/plugins/elizaibot-chatbots/
for more details.
