<?php

  /***************************************
  * http://www.program-o.com
  * PROGRAM O
  * Version: 2.5.3
  * FILE: file.php
  * AUTHOR: Elizabeth Perreau and Dave Morton
  * DATE: 05-11-2013
  * DETAILS: Allows botmasters and admins to download files
  ***************************************/
  $thisFile = __FILE__;
  if (!file_exists('../config/global_config.php'))
    header('location: ../install/install_programo.php');
  require_once('../config/global_config.php');
  // set up error logging and display
  ini_set('log_errors', true);
  ini_set('error_log', _LOG_PATH_ . 'admin.error.log');
  ini_set('html_errors', false);
  ini_set('display_errors', false);
  //load shared files

  // Set session parameters
  $session_name = 'PGO_Admin';
  session_name($session_name);
  session_start();
  $pageBack = $_SESSION['referer'];
  $req_file = $_SESSION['send_file'];
  $fileserver_path = dirname(__FILE__) . '/downloads';
  if (strstr($pageBack, 'google') !== false)
  {
    header("Location: $pageBack");
    exit;
  }
  else
  {
    header('Content-Description: File Transfer');
    header('Content-Type: application/force-download');
    header('Content-Length: ' . filesize("$fileserver_path/$req_file"));
    header('Content-Disposition: attachment; filename=' . $req_file);
    print file_get_contents("$fileserver_path/$req_file");
    // Remove the created file after downloading
    //unlink("$fileserver_path/$req_file");
    exit;
  }
  header("Location: $pageBack");