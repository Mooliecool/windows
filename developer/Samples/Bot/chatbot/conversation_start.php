<?php

  /***************************************
  * http://www.program-o.com
  * PROGRAM O
  * Version: 2.5.3
  * FILE: chatbot/conversation_start.php
  * AUTHOR: Elizabeth Perreau and Dave Morton
  * DATE: FEB 01 2016
  * DETAILS: this file is the landing page for all calls to access the bots
  ***************************************/

  $time_start = microtime(true);
  $script_start = $time_start;
  $last_timestamp = $time_start;
  $thisFile = __FILE__;
  require_once("../config/global_config.php");
  //load shared files
  require_once(_LIB_PATH_ . 'PDO_functions.php');
  include_once (_LIB_PATH_ . "error_functions.php");
  include_once(_LIB_PATH_ . 'misc_functions.php');
  ini_set('default_charset', $charset);

  //leave this first debug call in as it wipes any existing file for this session
  runDebug(__FILE__, __FUNCTION__, __LINE__, "Conversation Starting. Program O version " . VERSION . PHP_EOL . 'PHP  version ' . phpversion() . PHP_EOL . "OS: $os version $osv", 0);
  //load all the chatbot functions
  include_once (_BOTCORE_PATH_ . "aiml" . $path_separator . "load_aimlfunctions.php");
  //load all the user functions
  include_once (_BOTCORE_PATH_ . "conversation" . $path_separator . "load_convofunctions.php");
  //load all the user functions
  include_once (_BOTCORE_PATH_ . "user" . $path_separator . "load_userfunctions.php");
  //load all the user addons
  include_once (_ADDONS_PATH_ . "load_addons.php");
  runDebug(__FILE__, __FUNCTION__, __LINE__, "Loaded all Includes", 4);
  //------------------------------------------------------------------------
  // Error Handler
  //------------------------------------------------------------------------
  // set to the user defined error handler
  set_error_handler("myErrorHandler");
  $say = '';
  //open db connection
  $dbConn = db_open();
  //initialise globals
  $convoArr = array();
  //$convoArr = intialise_convoArray($convoArr);
  $new_convo_id = false;
  $old_convo_id = false;
  $say = '';
  $display = "";

  $options = array (
    'convo_id'    => FILTER_SANITIZE_STRING,
    'bot_id'      => FILTER_SANITIZE_NUMBER_INT,
    'say' => array(
      'filter'      => FILTER_SANITIZE_STRING,
      'flags'       => FILTER_FLAG_NO_ENCODE_QUOTES
    ),
    'format'      => FILTER_SANITIZE_STRING,
    'debug_mode'  => FILTER_SANITIZE_NUMBER_INT,
    'debug_level' => FILTER_SANITIZE_NUMBER_INT,
    'name'        => FILTER_SANITIZE_STRING
  );


  $form_vars_post = filter_input_array(INPUT_POST, $options);
  $form_vars_get = filter_input_array(INPUT_GET, $options);

  $form_vars = array_merge((array)$form_vars_get, (array)$form_vars_post);
  if (!isset($form_vars['say']))
  {
    error_log('Empty input! form vars = ' . print_r($form_vars, true) . PHP_EOL, 3, _LOG_PATH_ . 'debug_formvars.txt');
    $form_vars['say'] = '';
  }
  $say = ($say !== '') ? $say : trim($form_vars['say']);
  $session_name = 'PGOv' . VERSION;
  session_name($session_name);
  session_start();
  /** @noinspection PhpUndefinedVariableInspection */
  $debug_level = (isset($_SESSION['programo']['conversation']['debug_level'])) ? $_SESSION['programo']['conversation']['debug_level'] : $debug_level;
  $debug_level = (isset($form_vars['debug_level'])) ? $form_vars['debug_level'] : $debug_level;
  $debug_mode = (isset($form_vars['debug_mode'])) ? $form_vars['debug_mode'] : $debug_mode;
  if (isset($form_vars['convo_id'])) session_id($form_vars['convo_id']);
  $convo_id = session_id();
  runDebug(__FILE__, __FUNCTION__, __LINE__,"Debug level: $debug_level" . PHP_EOL . "session ID = $convo_id", 0);
  //if the user has said something
  if (!empty($say))
  {
    // Chect to see if the user is clearing properties
    $lc_say = (IS_MB_ENABLED) ? mb_strtolower($say) : strtolower($say);
    if ($lc_say == 'clear properties')
    {
      runDebug(__FILE__, __FUNCTION__, __LINE__, "Clearing client properties and starting over.", 4);
      $convoArr = read_from_session();
      $_SESSION = array();
      $user_id = (isset($convoArr['conversation']['user_id'])) ? $convoArr['conversation']['user_id'] : -1;
      $sql = "delete from `$dbn`.`client_properties` where `user_id` = $user_id;";

      $sth = $dbConn->prepare($sql);
      $sth->execute();



      $numRows = $sth->rowCount();
      $convoArr['client_properties'] = null;
      $convoArr['conversation'] = array();
      $convoArr['conversation']['user_id'] = $user_id;
      // Get old convo id, to use for later
      $old_convo_id = session_id();
      // Note: This will destroy the session, and not just the session data!
      // Finally, destroy the session.
      runDebug(__FILE__, __FUNCTION__, __LINE__, "Generating new session ID.", 4);
      session_regenerate_id(true);
      $new_convo_id = session_id();
      $params = session_get_cookie_params();
      setcookie($session_name, $new_convo_id, time() - 42000, $params["path"], $params["domain"], $params["secure"], $params["httponly"]);
      // Update the users table, and clear out any unused client properties as needed
      $sql = "update `$dbn`.`users` set `session_id` = '$new_convo_id' where `session_id` = '$old_convo_id';";
      runDebug(__FILE__, __FUNCTION__, __LINE__, "Update user - SQL:\n$sql", 3);
      
      $sth = $dbConn->prepare($sql);
      $sth->execute();

      $confirm = $sth->rowCount();
      // Get user id, so that we can clear the client properties
      $sql = "select `id` from `$dbn`.`users` where `session_id` = '$new_convo_id' limit 1;";
      $row = db_fetch($sql, null, __FILE__, __FUNCTION__, __LINE__);
      if ($row !== false)
      {
        $user_id = $row['id'];
        $convoArr['conversation']['user_id'] = $user_id;
        $convoArr['conversation']['convo_id'] = $new_convo_id;
        runDebug(__FILE__, __FUNCTION__, __LINE__, "User ID = $user_id.", 4);
        $sql = "delete from `$dbn`.`client_properties` where `user_id` = $user_id;";
        runDebug(__FILE__, __FUNCTION__, __LINE__, "Clear client properties from the DB - SQL:\n$sql", 4);
      }
      
      $say = "Hello";
    }
    //add any pre-processing addons

    $rawSay = $say;
    $say = run_pre_input_addons($convoArr, $say);
    $say = normalize_text($say);
    /** @noinspection PhpUndefinedVariableInspection */
    $bot_id = (isset($form_vars['bot_id'])) ? $form_vars['bot_id'] : $bot_id;
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Details:\nUser say: " . $say . "\nConvo id: " . $convo_id . "\nBot id: " . $bot_id . "\nFormat: " . $form_vars['format'], 2);
    //get the stored vars
    $convoArr = read_from_session();
    if (!empty($form_vars['name']))
    {
      $convoArr['conversation']['user_name'] = $user_name = $form_vars['name'];
    }

    $convoArr = load_default_bot_values($convoArr);
    //now overwrite with the recieved data
    $convoArr = check_set_convo_id($convoArr);
    $convoArr = check_set_bot($convoArr);
    $convoArr = check_set_user($convoArr);
    if (!isset($convoArr['conversation']['user_id']) and isset($user_id)) $convoArr['conversation']['user_id'] = $user_id;
    $convoArr = check_set_format($convoArr);
    $convoArr = load_that($convoArr);
    //save_file(_LOG_PATH_ . 'ca.txt', print_r($convoArr,true));
    $convoArr = buildNounList($convoArr);
    $convoArr['time_start'] = $time_start;
    $convoArr = load_bot_config($convoArr);
    //if totallines isn't set then this is new user
    runDebug(__FILE__, __FUNCTION__, __LINE__,"Debug level = $debug_level", 0);
    $debug_level = isset($convoArr['conversation']['debug_level']) ? $convoArr['conversation']['debug_level'] : $debug_level;
    runDebug(__FILE__, __FUNCTION__, __LINE__,"Debug level = $debug_level", 0);
    if (!isset ($convoArr['conversation']['totallines']))
    {
    //load the chatbot configuration for a new user
      $convoArr = intialise_convoArray($convoArr);
      //add the bot_id dependant vars
      $convoArr = add_firstturn_conversation_vars($convoArr);
      $convoArr['conversation']['totallines'] = 0;
      $convoArr = get_user_id($convoArr);
    }
    $convoArr['aiml'] = array();
    //add the latest thing the user said
    $convoArr = add_new_conversation_vars($say, $convoArr);

    //parse the aiml
    $convoArr = make_conversation($convoArr);
    $convoArr = run_mid_level_addons($convoArr);
    $convoArr = log_conversation($convoArr);
    #$convoArr = log_conversation_state($convoArr);
    $convoArr = write_to_session($convoArr);
    $convoArr = get_conversation($convoArr);
    $convoArr = run_post_response_useraddons($convoArr);
    //return the values to display
    $display = $convoArr['send_to_user'];
    $time_start = $convoArr['time_start'];
    unset($convoArr['nounList']);
    $final_convoArr = $convoArr;
  }
  else
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Conversation intialised waiting user", 2);
    $convoArr['send_to_user'] = '';
  }
  runDebug(__FILE__, __FUNCTION__, __LINE__, "Closing Database", 2);
  $dbConn = db_close();
  $time_end = microtime(true);
  $time = number_format(round(($time_end - $script_start) * 1000,7), 3);
  display_conversation($convoArr);
  runDebug(__FILE__, __FUNCTION__, __LINE__, "Conversation Ending. Elapsed time: $time milliseconds.", 0);
  $convoArr = handleDebug($convoArr, $time); // Make sure this is the last line in the file, so that all debug entries are captured.
  session_gc();