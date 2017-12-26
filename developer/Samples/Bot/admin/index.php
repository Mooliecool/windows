<?PHP
/***************************************
  * http://www.program-o.com
  * PROGRAM O
  * Version: 2.5.3
  * FILE: index.php
  * AUTHOR: Elizabeth Perreau and Dave Morton
  * DATE: FEB 01 2016
  * DETAILS: Gateway to the admin functions for the script
  ***************************************/

  $thisFile = __FILE__;
  if (!file_exists('../config/global_config.php')) header('location: ../install/install_programo.php');
  require_once('../config/global_config.php');

  // set up error logging and display
  ini_set('log_errors', true);
  ini_set('error_log', _LOG_PATH_ . 'admin.error.log');
  ini_set('html_errors', false);
  ini_set('display_errors', false);
  set_exception_handler("handle_exceptions");

  //load shared files
  require_once(_LIB_PATH_ . 'PDO_functions.php');
  require_once(_LIB_PATH_ . 'error_functions.php');
  require_once(_LIB_PATH_ . 'misc_functions.php');
  require_once(_LIB_PATH_ . 'template.class.php');
  require_once(_ADMIN_PATH_ . 'allowedPages.php');

  // Set session parameters
  $session_name = 'PGO_Admin';
  session_name($session_name);
  session_start();
  $msg = '';

  // Get form inputs
  $pc = print_r($_GET, true) . "\n" . print_r($_POST, true);
  $page = filter_input(INPUT_GET, 'page', FILTER_SANITIZE_STRING);
  $page = ($page === false || $page === null) ? 'main' : $page;
  if (!array_key_exists($page, $allowed_pages))
  {
    $msg = 'Invalid argument!';
  }
  $filters = $allowed_pages[$page];
  $post_vars = filter_input_array(INPUT_POST, $filters);
  $get_vars = filter_input_array(INPUT_GET, $filters);
  $input_vars = array_merge((array) $get_vars, (array) $post_vars);

  // Set default values
  $bot_name = '<b class="red">not selected</b>';
  $hide_logo = '';
  $curPage = '';

  // Begin script execution
  $thisPath = dirname(__FILE__);
  $template = new Template("$thisPath/default.page.htm");
  $githubVersion = getCurrentVersion();
  $version = ($githubVersion == VERSION) ? 'Program O version ' . VERSION : 'Program O ' . $githubVersion . ' is now available. <a href="https://github.com/Program-O/Program-O/archive/master.zip">Click here</a> to download it.';
  $dbConn = db_open();
  if ($get_vars['page'] == 'logout') logout();
  $logged_in = getLoginStatus();
  $curPage = 'logout';
  switch ($logged_in)
  {
    case true:
      $curPage = (isset($get_vars['page'])) ? $get_vars['page'] : 'main';
      break;
    default:
      $curPage = ($get_vars['page'] == 'login') ? login() : 'logout';
  }

      $name       = (isset($_SESSION['poadmin']['name'])) ? $_SESSION['poadmin']['name'] : '';
      $ip         = (isset($_SESSION['poadmin']['ip'])) ? $_SESSION['poadmin']['ip'] : '';
      $last       = (isset($_SESSION['poadmin']['last_login'])) ? $_SESSION['poadmin']['last_login'] : '';
      $lip        = (isset($_SESSION['poadmin']['lip'])) ? $_SESSION['poadmin']['lip'] : '';
      $llast      = (isset($_SESSION['poadmin']['prior_login'])) ? $_SESSION['poadmin']['prior_login'] : '';
      $bot_name   = (isset($_SESSION['poadmin']['bot_name'])) ? $_SESSION['poadmin']['bot_name'] : $bot_name;
      $bot_id     = (isset($_SESSION['poadmin']['bot_id'])) ? $_SESSION['poadmin']['bot_id'] : 1;
      $hide_logo  = (isset($_SESSION['display'])) ? $_SESSION['display'] : '';
      $bot_format = (isset($_SESSION['poadmin']['bot_format'])) ? $_SESSION['poadmin']['bot_format'] : '';


# Build page sections
# ordered here in the order that the page is constructed
  $logo            = $template->getSection('Logo');
  $titleSpan       = $template->getSection('TitleSpan');
  $main            = $template->getSection('Main');
  $divDecoration   = '';
  $mainContent     = $template->getSection('LoginForm');
  $noLeftNav       = $template->getSection('NoLeftNav');
  $noRightNav      = $template->getSection('NoRightNav');
  $navHeader       = $template->getSection('NavHeader');
  $footer          = $template->getSection('Footer');
  $topNav          = '';
  $leftNav         = '';
  $rightNav        = '';
  $rightNavLinks   = '';
  $lowerScripts    = $template->getSection('LogoLinkScript');
  $pageTitleInfo   = '';
  $topNavLinks     = makeLinks('top', makeTopLinks());
  $leftNavLinks    = makeLinks('left', makeLeftLinks());
  $mediaType       = ' media="screen"';
  $mainTitle       = 'Program O Login';
  $FooterInfo      = '<p>&copy; 2011-2014 My Program-O<br /><a href="http://www.program-o.com">www.program-o.com</a></p>';
  $headerTitle     = '';
  $pageTitle       = 'My-Program O - Login';
  $upperScripts    = '';
  $extraCSS = '';


    //if we get to the login page and we are still actually logged in
    //just destroy the session to prevent weirdness
    if($curPage == 'login' && !empty($_SESSION['poadmin']['logged_in'])){
        $_SESSION = array();
    }

  $_SESSION['poadmin']['curPage'] = $curPage;
  ($curPage != 'logout' || $curPage == 'login') ? include ("$curPage.php") : false;

  $bot_format_link = (!empty($bot_format)) ? "&amp;format=$bot_format" : '';
  $curPage = (isset($curPage)) ? $curPage : 'main';
  $upperScripts .= ($hide_logo == 'HideLogoCSS') ? $template->getSection('HideLogoCSS') : '';




  # Build page content from the template

  $content  = $template->getSection('Header');
  #$content .= "hide_logo = $hide_logo";
  $content .= $template->getSection('PageBody');

  # Replace template labels with real data
  $styleSheet = 'style.css';
  $errMsgClass   = (!empty($msg)) ? "ShowError" : "HideError";
  $errMsgStyle   = $template->getSection($errMsgClass);
  $bot_id = ($bot_id == 'new') ? 0 : $bot_id;
  $searches = array(
                    '[charset]'         => $charset,
                    '[myPage]'          => $curPage,
                    '[pageTitle]'       => $pageTitle,
                    '[styleSheet]'      => $styleSheet,
                    '[mediaType]'       => $mediaType,
                    '[extraCSS]'        => $extraCSS,
                    '[upperScripts]'    => $upperScripts,
                    '[logo]'            => $logo,
                    '[pageTitleInfo]'   => $pageTitleInfo,
                    '[topNav]'          => $topNav,
                    '[leftNav]'         => $leftNav,
                    '[rightNav]'        => $rightNav,
                    '[main]'            => $main,
                    '[footer]'          => $footer,
                    '[lowerScripts]'    => $lowerScripts,
                    '[titleSpan]'       => $titleSpan,
                    '[divDecoration]'   => $divDecoration,
                    '[topNavLinks]'     => $topNavLinks,
                    '[navHeader]'       => $navHeader,
                    '[leftNavLinks]'    => $leftNavLinks,
                    '[mainTitle]'       => $mainTitle,
                    '[mainContent]'     => $mainContent,
                    '[rightNavLinks]'   => $rightNavLinks,
                    '[FooterInfo]'      => $FooterInfo,
                    '[headerTitle]'     => $headerTitle,
                    '[errMsg]'          => $msg,
                    '[bot_id]'          => $bot_id,
                    '[bot_name]'        => $bot_name,
                    '[errMsgStyle]'     => $errMsgStyle,
                    '[noRightNav]'      => $noRightNav,
                    '[noLeftNav]'       => $noLeftNav,
                    '[version]'         => $version,
                    '[bot_format_link]' => $bot_format_link,
                   );
  foreach ($searches as $search => $replace) {
    $content = str_replace($search, $replace, $content);
  }
  $content = str_replace('[myPage]', $curPage, $content);
  $content = str_replace('[divDecoration]', $divDecoration, $content);
  $content = str_replace('[blank]', '', $content);

  session_gc();
  exit($content);

  /**
   * Function makeLinks
   *
   * * @param $section
   * @param     $linkArray
   * @return string
   */
  function makeLinks($section, $linkArray) {
    $out = "<!-- making links for section $section -->\n";
    global $template, $curPage;
    $curPage = (empty($curPage)) ? 'main' : $curPage;
    $botName = (isset($_SESSION['poadmin']['bot_name'])) ? $_SESSION['poadmin']['bot_name'] : '<b class="red">not selected</b>';
    $botId = (isset($_SESSION['poadmin']['bot_id'])) ? $_SESSION['poadmin']['bot_id'] : 1;
    $botId = ($botId == 'new') ? 1 : $botId;
    # [linkClass][linkHref][linkOnclick][linkAlt][linkTitle]>[linkLabel]
    $linkText = $template->getSection('NavLink');
    foreach ($linkArray as $needle) {
      $tmp = $linkText;
      foreach ($needle as $search => $replace) {
        $tmp = str_replace($search, $replace, $tmp);
      }
      $linkClass = $needle['[linkHref]'];
      $linkClass = str_replace(' href="index.php?page=', '', $linkClass);
      $linkClass = str_replace('"', '', $linkClass);
      $curClass = ($linkClass == $curPage) ? 'selected' : 'noClass';
      if ($curPage == 'main') $curClass = (stripos($linkClass,'main') !== false) ? 'selected' : 'noClass';
      $tmp = str_replace('[curClass]', $curClass, $tmp);
      $out .= "$tmp\n";
    }
    #print "<!-- returning links for section $section:\n\n out = $out\n\n -->\n";
    $strippedBotName = preg_replace('~\<b class="red"\>(.*?)\</b\>~', '$1', $botName);
    $out = str_replace('[botId]', $botId, $out);
    $out = str_replace('[curBot]', $botName, $out);
    $out = str_replace('[curBot2]', $strippedBotName, $out);
    return trim($out);
  }



  /**
   * Function makeTopLinks
   *
   *
   * @return array
   */
  function makeTopLinks() {
    $out = array(

                         array(
                               '[linkClass]' => ' class="[curClass]"',
                               '[linkHref]' => ' href="'.DOCS_URL.'"',
                               '[linkOnclick]' => '',
                               '[linkAlt]' => ' alt="The Program O User\'s Guide"',
                               '[linkTitle]' => ' title="The Program O User\'s Guide"',
                               '[linkLabel]' => 'Documentation'
                               ),
                         array(
                               '[linkClass]' => ' class="[curClass]"',
                               '[linkHref]' => ' href="https://github.com/Program-O/Program-O/issues"',
                               '[linkOnclick]' => '',
                               '[linkAlt]' => ' alt="Bug reporting"',
                               '[linkTitle]' => ' title="Bug reporting"',
                               '[linkLabel]' => 'Bug Reporting'
                               ),
                         array(
                               '[linkClass]' => ' class="[curClass]"',
                               '[linkHref]' => ' href="index.php?page=stats"',
                               '[linkOnclick]' => '',
                               '[linkAlt]' => ' alt="Get bot statistics"',
                               '[linkTitle]' => ' title="Get bot statistics"',
                               '[linkLabel]' => 'Stats'
                               ),
                         array(
                               '[linkClass]' => '',
                               '[linkHref]' => ' href="index.php?page=logout"',
                               '[linkOnclick]' => '',
                               '[linkAlt]' => ' alt="Log out"',
                               '[linkTitle]' => ' title="Log out"',
                               '[linkLabel]' => 'Log Out'
                               )
                        );
    return $out;
  }

  /**
   * Function makeLeftLinks
   *
   *
   * @return array
   */
  function makeLeftLinks() {
    $out = array(
                 array( # Change bot
                       '[linkClass]' => ' class="[curClass]"',
                       '[linkHref]' => ' href="index.php?page=select_bots"',
                       '[linkOnclick]' => '',
                       '[linkAlt]' => ' alt="Change or edit the current bot"',
                       '[linkTitle]' => ' title="Change or edit the current bot"',
                       '[linkLabel]' => 'Current Bot: ([curBot])'
                 ),
                 array(
                       '[linkClass]' => ' class="[curClass]"',
                       '[linkHref]' => ' href="index.php?page=botpersonality"',
                       '[linkOnclick]' => '',
                       '[linkAlt]' => ' alt="Edit your bot\'s personality"',
                       '[linkTitle]' => ' title="Edit your bot\'s personality"',
                       '[linkLabel]' => 'Bot Personality'
                 ),
                 array(
                       '[linkClass]' => ' class="[curClass]"',
                       '[linkHref]' => ' href="index.php?page=logs"',
                       '[linkOnclick]' => '',
                       '[linkAlt]' => ' alt="View the log files"',
                       '[linkTitle]' => ' title="View the log files"',
                       '[linkLabel]' => 'Logs'
                 ),
                 array(
                       '[linkClass]' => ' class="[curClass]"',
                       '[linkHref]' => ' href="index.php?page=teach"',
                       '[linkOnclick]' => '',
                       '[linkAlt]' => ' alt="Train your bot"',
                       '[linkTitle]' => ' title="Train your bot"',
                       '[linkLabel]' => 'Teach'
                 ),
                 array(
                       '[linkClass]' => ' class="[curClass]"',
                       '[linkHref]' => ' href="index.php?page=upload"',
                       '[linkOnclick]' => '',
                       '[linkAlt]' => ' alt="Upload AIML files"',
                       '[linkTitle]' => ' title="Upload AIML files"',
                       '[linkLabel]' => 'Upload AIML'
                 ),
                 array(
                       '[linkClass]' => ' class="[curClass]"',
                       '[linkHref]' => ' href="index.php?page=download"',
                       '[linkOnclick]' => '',
                       '[linkAlt]' => ' alt="Download AIML files"',
                       '[linkTitle]' => ' title="Download AIML files"',
                       '[linkLabel]' => 'Download AIML'
                 ),
                 array(
                       '[linkClass]' => ' class="[curClass]"',
                       '[linkHref]' => ' href="index.php?page=clear"',
                       '[linkOnclick]' => '',
                       '[linkAlt]' => ' alt="Clear AIML Categories"',
                       '[linkTitle]' => ' title="Clear AIML Categories"',
                       '[linkLabel]' => 'Clear AIML Categories'
                 ),
                 array(
                       '[linkClass]' => ' class="[curClass]"',
                       '[linkHref]' => ' href="index.php?page=spellcheck"',
                       '[linkOnclick]' => '',
                       '[linkAlt]' => ' alt="Edit the SpellCheck entries"',
                       '[linkTitle]' => ' title="Edit the SpellCheck entries"',
                       '[linkLabel]' => 'Spell Check'
                 ),
                 array(
                       '[linkClass]' => ' class="[curClass]"',
                       '[linkHref]' => ' href="index.php?page=wordcensor"',
                       '[linkOnclick]' => '',
                       '[linkAlt]' => ' alt="Edit the Word Censor entries"',
                       '[linkTitle]' => ' title="Edit the Word Censor entries"',
                       '[linkLabel]' => 'Word Censor'
                 ),
                 array(
                       '[linkClass]' => ' class="[curClass]"',
                       '[linkHref]' => ' href="index.php?page=editAiml"',
                       '[linkOnclick]' => '',
                       '[linkAlt]' => ' alt="Search and edit specific AIML categories"',
                       '[linkTitle]' => ' title="Search and edit specific AIML categories"',
                       '[linkLabel]' => 'Search/Edit AIML'
                 ),
                 array(
                       '[linkClass]' => ' class="[curClass]"',
                       '[linkHref]' => ' href="index.php?page=srai_lookup"',
                       '[linkOnclick]' => '',
                       '[linkAlt]' => ' alt="Search and edit entries in the srai_lookup table"',
                       '[linkTitle]' => ' title="Search and edit entries in the srai_lookup table"',
                       '[linkLabel]' => 'SRAI Lookup'
                 ),
                 array(
                       '[linkClass]' => ' class="[curClass]"',
                       '[linkHref]' => ' href="index.php?page=demochat"',
                       '[linkOnclick]' => '',
                       '[linkAlt]' => ' alt="Run a demo version of your bot"',
                       '[linkTitle]' => ' title="Run a demo version of your bot"',
                       '[linkLabel]' => 'Test Your Bot'
                 ),
                 array(
                       '[linkClass]' => '',
                       '[linkHref]' => ' href="index.php?page=members"',
                       '[linkOnclick]' => '',
                       '[linkAlt]' => ' alt="Edit Admin Accounts"',
                       '[linkTitle]' => ' title="Edit Admin Accounts"',
                       '[linkLabel]' => 'Edit Admin Accounts'
                 ),
                 array(
                       '[linkClass]' => '',
                       '[linkHref]' => ' href="index.php?page=logout"',
                       '[linkOnclick]' => '',
                       '[linkAlt]' => ' alt="Log out"',
                       '[linkTitle]' => ' title="Log out"',
                       '[linkLabel]' => 'Log Out'
                 ),
                 array(
                       '[linkClass]' => '',
                       '[linkHref]' => ' href="#"',
                       '[linkOnclick]' => ' onclick="toggleLogo(); return false;"',
                       '[linkAlt]' => ' alt="Toggle the Logo"',
                       '[linkTitle]' => ' title="Toggle the Logo"',
                       '[linkLabel]' => 'Toggle the Logo'
                 ),
                 array(
                       '[linkClass]' => '',
                       '[linkHref]' => ' href="' . _BASE_URL_ . '?bot_id=[botId][bot_format_link]"',
                       '[linkOnclick]' => ' target="_blank"',
                       '[linkAlt]' => ' alt="open the page for [curBot] in a new tab/window"',
                       '[linkTitle]' => ' title="open the page for [curBot2] in a new tab/window"',
                       '[linkLabel]' => 'Talk to [curBot]'
                 )
    );
    return $out;
  }



  /**
   * Function getCurrentVersion
   *
   *
   * @return bool|mixed|string
   */
  function getCurrentVersion()
  {
    if(isset($_SESSION['GitHubVersion'])) return $_SESSION['GitHubVersion'];
    $url = 'https://api.github.com/repos/Program-O/Program-O/contents/version.txt';
    $out = false;
    if (function_exists('curl_init'))
    {
      $ch = curl_init();
      curl_setopt($ch, CURLOPT_URL, $url);
      curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
      curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
      curl_setopt($ch, CURLOPT_USERAGENT, 'Program-O/Program-O');
      $out = curl_exec($ch);
      //if (false === $out) trigger_error('Not sure what it is, but there\'s a problem with checking the current version on GitHub. Maybe this will help: "' . curl_error($ch) . '"');
      curl_close($ch);
      if (false === $out) return VERSION;
      $repoArray = json_decode($out, true);
      //save_file(_LOG_PATH_ . 'repoArray.txt', print_r($repoArray, true));
      if (!isset($repoArray['content'])) return VERSION;
      $versionB64 = $repoArray['content'];
      $version = base64_decode($versionB64);
      #save_file(_DEBUG_PATH_ . 'version.txt', "out = " . print_r($out, true) . "\r\nVersion = $versionB64 = $version");
      $out = $version;
    }
    $_SESSION['GitHubVersion'] = $out;
    return ($out !== false) ? $out : VERSION;
  }

  /**
   * Function handle_exceptions
   *
   * * @param exception $e
   * @return void
   */
  function handle_exceptions(exception $e)
  {
    global $msg;
    $trace = $e->getTrace();
    file_put_contents(_LOG_PATH_ . 'admin.exception.log', print_r($trace, true), FILE_APPEND);
    $msg .= $e->getMessage();
    return 'logout';
  }

  function login ()
  {
    global $post_vars, $get_vars, $dbConn, $msg;
    if((!isset($post_vars['user_name'])) ||(!isset($post_vars['pw']))) return 'logout';
    //$_SESSION['poadmin']['display'] = $hide_logo;
    $user_name = $post_vars['user_name'];
    $pw_hash = md5($post_vars['pw']);
    $sql = "SELECT * FROM `myprogramo` WHERE user_name = :user_name AND password = :pw_hash";
    $params = array(':user_name' => $user_name, ':pw_hash' => $pw_hash);
    $row = db_fetch($sql, $params, __FILE__, __FUNCTION__, __LINE__);
    if(!empty($row)) {
      $_SESSION['poadmin']['uid'] = $row['id'];
      $_SESSION['poadmin']['name'] = $row['user_name'];
      $_SESSION['poadmin']['lip']=$row['last_ip'];
      $_SESSION['poadmin']['prior_login']=date('l jS \of F Y h:i:s A', strtotime($row['last_login']));
      switch (true)
      {
        case (!empty($_SERVER['HTTP_CLIENT_IP'])):
          $ip = $_SERVER['HTTP_CLIENT_IP'];
          break;
        case (!empty($_SERVER['HTTP_X_FORWARDED_FOR'])):
          $ip = $_SERVER['HTTP_X_FORWARDED_FOR'];
          break;
        default:
          $ip = $_SERVER['REMOTE_ADDR'];
      }

      $sql = "UPDATE `myprogramo` SET `last_ip` = :ip, `last_login` = CURRENT_TIMESTAMP WHERE user_name = :user_name limit 1";
      $params = array(':ip' => $ip, ':user_name' => $user_name);
      $transact = db_write($sql, $params, false, __FILE__, __FUNCTION__, __LINE__);
      $_SESSION['poadmin']['ip'] = $ip;
      $_SESSION['poadmin']['last_login'] = date('l jS \of F Y h:i:s A');

      $sql = "SELECT * FROM `bots` WHERE bot_active = '1' ORDER BY bot_id ASC LIMIT 1";
      $row = db_fetch($sql, null, __FILE__, __FUNCTION__, __LINE__);
      $count = count($row);
      if($count > 0) {
        $_SESSION['poadmin']['bot_id'] = $row['bot_id'];
        $_SESSION['poadmin']['bot_name'] = $row['bot_name'];
      }
      else {
        $_SESSION['poadmin']['bot_id'] = -1;
        $_SESSION['poadmin']['bot_name'] = "unknown";
      }
    }
    else {
      $msg .= "incorrect username/password<br>\n";
    }
    if (empty($msg))
    {
      $_SESSION['poadmin']['logged_in'] = true;
      header('Location: index.php');
      return 'main';
    }
    return 'logout';
  }

  function logout()
  {
    global $session_name, $session_cookie_domain, $session_cookie_path;
    $_SESSION = array();
    session_destroy();
    setcookie($session_name, '', time()-3600, $session_cookie_path, $session_cookie_domain, false, false);
    header('Location: ./');
    exit();
  }

  function getLoginStatus()
  {
    return (isset($_SESSION['poadmin']['logged_in']) && $_SESSION['poadmin']['logged_in'] === true) ? true : false;
  }
