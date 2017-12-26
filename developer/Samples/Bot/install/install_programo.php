<?PHP
  /***************************************
  * http://www.program-o.com
  * PROGRAM O
  * Version: 2.5.3
  * FILE: install_programo.php
  * AUTHOR: Elizabeth Perreau and Dave Morton
  * DATE: FEB 01 2016
  * DETAILS: Program O's Automatic install script
  ***************************************/
  session_name('PGO_install');
  session_start();
  $thisFile = __FILE__;
  # Test for PHP version 5+
  $fatalError = '';
  $myPHP_Version = (float) phpversion();
  $pdoSupport = (class_exists('PDO'));
  If ($myPHP_Version < 5.2) $fatalError .= "<p class='red bold'>We're sorry, but Program O requires PHP version 5.2 or greater to function. Please ask your hosting provider to upgrade.</p>";
  If (!$pdoSupport) $fatalError .= "<p class='red bold'>Support for PHP Data Objects (PDO) was not detected! This is required for Program O to function. Please ask your hosting provider to upgrade.</p>";
  $no_unicode_message = (function_exists('mb_check_encoding')) ? '' : "<p class=\"red bold\">Warning! Unicode Support is not available on this server. Non-English languages will not display properly. Please ask your hosting provider to enable the PHP mbstring extension to correct this.</p>\n";
  $errorMessage = (!empty ($_SESSION['errorMessage'])) ? $_SESSION['errorMessage'] : '';
  $errorMessage .= $no_unicode_message;
  require_once ('install_config.php');
  $dirArray = glob(_ADMIN_PATH_ . "ses_*",GLOB_ONLYDIR);
  $session_dir = (empty($dirArray)) ? 'ses_' . md5(time()) : basename($dirArray[0]);
  $dupPS = "$path_separator$path_separator";
  $session_dir = str_replace($dupPS, $path_separator, $session_dir); // remove double path separators when necessary
  define('_SESSION_PATH_', _ADMIN_PATH_ . $session_dir . DIRECTORY_SEPARATOR);
  $writeCheckArray = array('config' => _CONF_PATH_, 'debug' => _DEBUG_PATH_, 'logs' => _LOG_PATH_);
  $errFlag = false;

  foreach ($writeCheckArray as $key => $folder) {
    if (!is_writable($folder)) {
      $errFlag = true;
      $errorMessage .= <<<endWarning
      <p class="red bold">
        The $key folder cannot be written to, or does not exist. Please correct this before you continue.
      </p>
endWarning;
    }
  }
  $additionalInfo = <<<endInfo
      <p>
        This is usually a permissions issue, and most often occurs with Linux-based systems. Check
        file/folder permissions for the following directories:
        <ul>
          <li>The base install folder (where you unzipped or uploaded the script to)</li>
          <li>admin</li>
          <li>config</li>
          <li>chatbot/debug</li>
        </ul>
        Permissions for these folders should be 0755. If they are not, then you need to change that. If you
        have trouble with this, or have questions, please visit us at
        <a href="http://www.program-o.com">Program O</a>.
      </p>
endInfo;
  if ($errFlag) $errorMessage .= $additionalInfo;
  define('SECTION_START', '<!-- Section [section] Start -->'); # search params for start and end of sections
  define('SECTION_END', '<!-- Section [section] End -->'); # search params for start and end of sections
  define('PHP_SELF', $_SERVER['SCRIPT_NAME']); # This is more secure than $_SERVER['PHP_SELF'], and returns more or less the same thing
  ini_set("display_errors", 0);
  ini_set("log_errors", true);
  ini_set("error_log", _LOG_PATH_ . "install.error.log");
  $myHost = $_SERVER['SERVER_NAME'];
  chdir(dirname(realpath(__FILE__)));
  $page_template = file_get_contents('install.tpl.htm');
  $page = (isset ($_REQUEST['page'])) ? $_REQUEST['page'] : 1;
  $action = (isset ($_REQUEST['action'])) ? $_REQUEST['action'] : '';
  $message = '';
  if (!empty ($action)) {
    $message = $action($page);
  }
  $pageTemplate = 'Container';
  $pageNotes = ucwords("Page $page Notes");
  $content = getSection('Header', $page_template, false);
  $content .= getSection($pageTemplate, $page_template);
  $content .= getSection('Footer', $page_template);
  $content .= getSection("jQuery$page", $page_template);
  $notes = getSection($pageNotes, $page_template);
  $submitButton = getSection('SubmitButton', $page_template);
  $main = ($page == 1) ? getSection('InstallForm', $page_template) : $message;
  $main = (empty($fatalError)) ? $main : $fatalError;
  $tmpSearchArray = array();
  $content = str_replace('[mainPanel]', $main, $content);
  $content = str_replace('[http_host]', $myHost, $content);
  $content = str_replace('[error_response]', $error_response, $content);
  $content = str_replace('[notes]', $notes, $content);
  $content = str_replace('[PHP_SELF]', PHP_SELF, $content);
  $content = str_replace('[errorMessage]', $errorMessage, $content);
  $content = str_replace('[cr6]', "\n ", $content);
  $content = str_replace('[cr4]', "\n ", $content);
  $content = str_replace("\r\n", "\n", $content);
  $content = str_replace("\n\n", "\n", $content);
  $content = str_replace('[admin_url]', _ADMIN_URL_, $content);
  $content .= <<<endPage

</body>
</html>
endPage;

  exit($content);

  /**
   * Function getSection
   *
   * * @param $sectionName
   * @param      $page_template
   * @param bool $notFoundReturn
   * @return string
   */
  function getSection($sectionName, $page_template, $notFoundReturn = true) {
    $sectionStart = str_replace('[section]', $sectionName, SECTION_START);
    $sectionStartLen = strlen($sectionStart);
    $sectionEnd = str_replace('[section]', $sectionName, SECTION_END);
    $startPos = strpos($page_template, $sectionStart, 0);
    if ($startPos === false) {
      if ($notFoundReturn) {
        return '';
      }
      else
        $startPos = 0;
    }
    else
      $startPos += $sectionStartLen;
    $endPos = strpos($page_template, $sectionEnd, $startPos) - 1;
    $sectionLen = $endPos - $startPos;
    $out = substr($page_template, $startPos, $sectionLen);
    return trim($out);
  }

  /**
   * Function Save
   *
   *
   * @return string
   */
  function Save() {
    global $page_template, $error_response, $session_dir;
    $tagSearch = array();
    $varReplace = array();
    $pattern = "RANDOM PICKUP LINE";
    $errorMessage = '';
    #$error_response = "No AIML category found. This is a Default Response.";
    $conversation_lines = '1';
    $remember_up_to = '10';
    $_SESSION['errorMessage'] = '';
    // First off, write the config file
    $myPostVars = filter_input_array(INPUT_POST, FILTER_SANITIZE_STRING);
    ksort($myPostVars);
    $configContents = file_get_contents(_INSTALL_PATH_ . 'config.template.php');
    $configContents = str_replace('[session_dir]', $session_dir, $configContents);
    if (!file_exists(_SESSION_PATH_)) {
      // Create the sessions folder, and set permissions
      mkdir(_SESSION_PATH_, 0755);

      // Place an empty index file in the sessions folder to prevent direct access to the folder from a web browser
      file_put_contents(_SESSION_PATH_ . 'index.html', '');
    }
    foreach ($myPostVars as $key => $value) {
      $tagSearch[] = "[$key]";
      $varReplace[] = $value;
    }
    $configContents = str_replace($tagSearch, $varReplace, $configContents);
    $saveFile = file_put_contents(_CONF_PATH_ . 'global_config.php', $configContents);
    // Now, update the data to the database, starting with making sure the tables are installed
    $dbh = $myPostVars['dbh'];
    $dbn = $myPostVars['dbn'];
    $dbu = $myPostVars['dbu'];
    $dbp = $myPostVars['dbp'];
    try {
      $dbConn = new PDO("mysql:host=$dbh;dbname=$dbn;charset=utf8", $dbu, $dbp);
      $dbConn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
      $dbConn->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_ASSOC);
    }
    catch (Exception $e) {
      header('Content-type: text/plain');
      var_dump($e);
      exit('Cannot connect to the database! ' . $e->getMessage());
    }

    $sql = "show tables;";
    $sth = $dbConn->prepare($sql);
    $sth->execute();
    $row = $sth->fetch();
    if (empty ($row)) {
      $sql = file_get_contents('new.sql');
      $sth = $dbConn->prepare($sql);
      $sth->execute();
      $affectedRows = $sth->rowCount();
    }
    else { // Let's make sure that the srai lookup table exists
      try {
        $sql = 'select bot_id from srai_lookup;';
        $sth = $dbConn->prepare($sql);
        $sth->execute();
        $result = $sth->fetchAll();
      }
      catch(Exception $e) {
        try {
          $sql = "DROP TABLE IF EXISTS `srai_lookup`; CREATE TABLE IF NOT EXISTS `srai_lookup` (`id` int(11) NOT NULL AUTO_INCREMENT, `bot_id` int(11) NOT NULL, `pattern` text NOT NULL, `template_id` int(11) NOT NULL, PRIMARY KEY (`id`), KEY `pattern` (`pattern`(64)) COMMENT 'Search against this for performance boost') ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='Contains previously stored SRAI calls' AUTO_INCREMENT=1;";
          $sth = $dbConn->prepare($sql);
          $sth->execute();
        }
        catch(Exception $e) {
          $errorMessage .= 'Could not add SRAI lookup table! Error is: ' . $e->getMessage();
        }
      }
    }
    $sql = 'select `error_response` from `bots` where 1 limit 1';
    $sth = $dbConn->prepare($sql);
    $sth->execute();
    $row = $sth->fetch();
    $error_response = $row['error_response'];
    $sql = 'select `bot_id` from `bots`;';
    $sth = $dbConn->prepare($sql);
    $sth->execute();
    $result = $sth->fetchAll();
    if (count($result) == 0) {
      $sql_template = "
INSERT IGNORE INTO `bots` (`bot_id`, `bot_name`, `bot_desc`, `bot_active`, `bot_parent_id`, `format`, `save_state`, `conversation_lines`, `remember_up_to`, `debugemail`, `debugshow`, `debugmode`, `error_response`, `default_aiml_pattern`)
VALUES ([default_bot_id], '[bot_name]', '[bot_desc]', '[bot_active]', '[bot_parent_id]', '[format]', '[save_state]',
'$conversation_lines', '$remember_up_to', '[debugemail]', '[debugshow]', '[debugmode]', '$error_response', '$pattern');";
      $bot_id = 1;
      $sql = str_replace('[default_bot_id]', $bot_id, $sql_template);
      $sql = str_replace('[bot_name]', $myPostVars['bot_name'], $sql);
      $sql = str_replace('[bot_desc]', $myPostVars['bot_desc'], $sql);
      $sql = str_replace('[bot_active]', $myPostVars['bot_active'], $sql);
      $sql = str_replace('[bot_parent_id]', 1, $sql);
      $sql = str_replace('[format]', $myPostVars['format'], $sql);
      // "Use PHP from DB setting
      // "Update PHP in DB setting
      $sql = str_replace('[save_state]', $myPostVars['save_state'], $sql);
      $sql = str_replace('[conversation_lines]', $conversation_lines, $sql);
      $sql = str_replace('[remember_up_to]', $remember_up_to, $sql);
      $sql = str_replace('[debugemail]', $myPostVars['debugemail'], $sql);
      $sql = str_replace('[debugshow]', $myPostVars['debug_level'], $sql);
      $sql = str_replace('[debugmode]', $myPostVars['debug_mode'], $sql);
      $sql = str_replace('[error_response]', $error_response, $sql);
      $sql = str_replace('[aiml_pattern]', $pattern, $sql);
      try {
        $sth = $dbConn->prepare($sql);
        $sth->execute();
        $affectedRows = $sth->rowCount();
        $errorMessage .= ($affectedRows > 0) ? '' : ' Could not create new bot!';
      }
      catch(Exception $e) {
        $errorMessage .= $e->getMessage();
      }
    }
    $cur_ip = $_SERVER['REMOTE_ADDR'];
    $encrypted_adm_dbp = md5($myPostVars['adm_dbp']);
    $adm_dbu = $myPostVars['adm_dbu'];
    $sql = "select id from `myprogramo` where `user_name` = '$adm_dbu' and `password` = '$encrypted_adm_dbp';";
    $sth = $dbConn->prepare($sql);
    $sth->execute();
    $result = $sth->fetchAll();
    if (count($result) == 0) {
      $sql = "insert ignore into `myprogramo` (`id`, `user_name`, `password`, `last_ip`) values(null, '$adm_dbu', '$encrypted_adm_dbp', '$cur_ip');";
      try {
        $sth = $dbConn->prepare($sql);
        $sth->execute();
        $affectedRows = $sth->rowCount();
        $errorMessage .= ($affectedRows > 0) ? '' : ' Could not create new Admin!';
      }
      catch(Exception $e) {
        $errorMessage .= $e->getMessage();
      }
    }

    if (empty($errorMessage)) {
      $out = getSection('InstallComplete', $page_template);
    }
    else $out = getSection('InstallError', $page_template);
    return $out . $errorMessage;
  }


