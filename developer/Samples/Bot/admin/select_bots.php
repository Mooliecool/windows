<?php
  /***************************************
   * http://www.program-o.com
   * PROGRAM O
   * Version: 2.5.3
   * FILE: select_bots.php
   * AUTHOR: Elizabeth Perreau and Dave Morton
   * DATE: 12-09-2014
   * DETAILS: Selects the current chatbot and displays editable config data
   ***************************************/

  $selectBot = '';
  $curBot = array();
  $post_vars = filter_input_array(INPUT_POST);

  if((isset($post_vars['action']))&&($post_vars['action']=="update")) {
    $selectBot .= getChangeList();
    $msg = updateBotSelection();
    $selectBot .= getSelectedBot();
  }
  elseif((isset($post_vars['action']))&&($post_vars['action']=="change")) {
    $bot_id = $post_vars['bot_id'];
    changeBot();
    $selectBot .= getChangeList();
    $selectBot .= getSelectedBot();
  }
  elseif((isset($post_vars['action']))&&($post_vars['action']=="add")) {
    $selectBot .= addBot();
    $selectBot .= getChangeList();
    $selectBot .= getSelectedBot();
  }
  else {
    $selectBot .= getChangeList();
    $selectBot .= getSelectedBot();
  }
  $bot_format = (isset($curBot['format'])) ? $curBot['format'] : $format;
  $_SESSION['poadmin']['format'] = $bot_format;
  $topNav        = $template->getSection('TopNav');
  $leftNav       = $template->getSection('LeftNav');
  $main          = $template->getSection('Main');
  $navHeader     = $template->getSection('NavHeader');
  $FooterInfo    = getFooter();
  $errMsgClass   = (!empty($msg)) ? "ShowError" : "HideError";
  $errMsgStyle   = $template->getSection($errMsgClass);
  $noLeftNav     = '';
  $noTopNav      = '';
  $noRightNav    = $template->getSection('NoRightNav');
  $headerTitle   = 'Actions:';
  $pageTitle     = 'My-Program O - Select or Edit a Bot';
  $mainContent   = $selectBot;
  $mainTitle     = 'Choose/Edit a Bot';

  /**
   * Returns a list of current, active chatbots, for selecting a parent chatbot
   *
   * @param $current_parent
   * @return string
   */
  function getBotParentList($current_parent) {
    //db globals
    global$dbConn;
  
  //get active bots from the db
  if(empty($current_parent)) $current_parent = 0;
  $sql = "SELECT * FROM `bots` where bot_active = '1'";
  $result = db_fetchAll($sql, null, __FILE__, __FUNCTION__, __LINE__);
  $options = '                  <option value="0"[noBot]>No Parent Bot</option>';

  foreach ($result as $row) {
    if ($row['bot_id'] == 0) $options = str_replace('[noBot]', 'selected="selected"', $options);
    if($current_parent==$row['bot_id']) {
      $sel = "selected=\"selected\"";
    }
    else {
      $sel = '';
    }
    $options .= '                  <option value="'.$row['bot_id'].'" '.$sel.'>'.$row['bot_name'].'</option>';
  }
  $options = str_replace('[noBot]', 'selected="selected"', $options);

  return $options;
}


  /**
   * Returns an HTML form, filled with the current chatbot's configuration data
   *
   *
   * @return string
   */
  function getSelectedBot() {
  global $dbConn, $template, $pattern, $remember_up_to, $conversation_lines, $error_response, $curBot, $unknown_user;
  $bot_conversation_lines = $conversation_lines;
  $bot_default_aiml_pattern = $pattern;
  $bot_error_response = $error_response;
  $bot_unknown_user = $unknown_user;
  
  $inputs='';
  $aiml_count = 'no';
  $form = $template->getSection('SelectBotForm');
  $sel_session = '';
  $sel_db = '';
  $sel_html = '';
  $sel_xml = '';
  $sel_json = '';
  $sel_yes = '';
  $sel_no = '';
  $sel_fyes = '';
  $sel_fno = '';
  $sel_fuyes = '';
  $sel_funo = '';
  $ds_ = '';
  $ds_i = '';
  $ds_ii = '';
  $ds_iii = '';
  $ds_iv = '';
  $dm_ = '';
  $dm_i = '';
  $dm_ii = '';
  $dm_iii = '';
  $dm_iv = '';
  $bot_id = (isset($_SESSION['poadmin']['bot_id'])) ? $_SESSION['poadmin']['bot_id'] : 'new';
  if($bot_id != "new")
  {
    $sql = "SELECT count(*) FROM `aiml` where bot_id = :bot_id;";
    $row = db_fetch($sql, array(':bot_id'=>$bot_id), __FILE__, __FUNCTION__, __LINE__);
    $aiml_count = ($row['count(*)'] == 0) ? 'no' : number_format($row['count(*)']);
    $sql = "SELECT * FROM `bots` where bot_id = :bot_id;";
    $row = db_fetch($sql, array(':bot_id'=>$bot_id), __FILE__, __FUNCTION__, __LINE__);
    $curBot = $row;
      foreach ($row as $key => $value) {
        if (strstr($key,'bot_') != false){
          $tmp = '';
          $$key = $value;
        }
        else {
          $tmp = "bot_$key";
          $$tmp = $value;
        }
      }
      if($bot_active == "1") {
        $sel_yes = ' selected="selected"';
      }
      else {
        $sel_no = ' selected="selected"';
      }
      if($bot_save_state=="database") {
        $sel_db = ' selected="selected"';
      }
      else {
        $sel_session = ' selected="selected"';
      }
      if($bot_format=="html") {
        $sel_html = ' selected="selected"';
      }
      elseif($bot_format=="xml") {
        $sel_xml = ' selected="selected"';
      }
      elseif($bot_format=="json") {
        $sel_json = ' selected="selected"';
      }
      if($bot_debugshow=="0") {
        $ds_ = ' selected="selected"';
      }
      elseif($bot_debugshow=="1") {
        $ds_i = ' selected="selected"';
      }
      elseif($bot_debugshow=="2") {
        $ds_ii = ' selected="selected"';
      }
      elseif($bot_debugshow=="3") {
        $ds_iii = ' selected="selected"';
      }
      elseif($bot_debugshow=="4") {
        $ds_iv = ' selected="selected"';
      }
    /** @noinspection PhpUndefinedVariableInspection */
    if($bot_debugmode=="0") {
        $dm_ = ' selected="selected"';
      }
      elseif($bot_debugmode=="1") {
        $dm_i = ' selected="selected"';
      }
      elseif($bot_debugmode=="2") {
        $dm_ii = ' selected="selected"';
      }
      elseif($bot_debugmode=="3") {
        $dm_iii = ' selected="selected"';
      }
      elseif($bot_debugmode=="4") {
        $dm_iv = ' selected="selected"';
      }
      $action = "update";
  }
  else {
    $bot_id = '';
    $bot_parent_id = 0;
    $bot_name = 'new or unnamed bot';
    $bot_desc = '';
    $bot_active = '';
    $action = "add";
    $bot_format = '';
    $bot_conversation_lines = $conversation_lines;
    //$remember_up_to = $remember_up_to;
    $bot_default_aiml_pattern = $pattern;
    $bot_error_response = $error_response;
    $bot_debugemail = '';
    $debugemail = '';
    $bot_debugshow = '';
    $bot_debugmode = '';
  }
  $parent_options = getBotParentList($bot_parent_id);
  $searches = array(
    '[bot_id]','[bot_name]','[aiml_count]','[bot_desc]','[parent_options]','[sel_yes]','[sel_no]',
    '[sel_html]','[sel_xml]','[sel_json]','[sel_session]','[sel_db]','[sel_fyes]',
    '[sel_fno]','[sel_fuyes]','[sel_funo]','[bot_conversation_lines]','[remember_up_to]',
    '[bot_debugemail]','[dm_]','[dm_i]','[dm_ii]','[dm_iii]','[ds_]','[ds_i]','[ds_ii]',
    '[ds_iii]','[ds_iv]','[action]', '[bot_default_aiml_pattern]', '[bot_error_response]', '[bot_unknown_user]', '[unknown_user]',
  );
  foreach ($searches as $search) {
    $replace = str_replace('[', '', $search);
    $replace = str_replace(']', '', $replace);
    $form = str_replace($search, $$replace, $form);
  }
  return $form;
}

  /**
   * Updates the database whth the current chatbot's modified configuration data
   *
   *
   * @return string
   */
  function updateBotSelection()
  {
    global $dbConn, $msg, $format, $post_vars;
    $logFile = _LOG_URL_ . 'admin.error.log';
    $msg = '';
    $bot_id = $post_vars['bot_id'];
    $sql = "select * from bots where bot_id = $bot_id;";
    $result = db_fetch($sql, null, __FILE__, __FUNCTION__, __LINE__);
    $sql = '';
    $params = array();
    $skipVars = array('bot_id', 'action');
    foreach($post_vars as $key => $value)
    {
      if (in_array($key, $skipVars) || !isset($result[$key])) continue;
      $safeVal = addslashes($value);
/*
      $value = str_replace("'", "\'", $value);
      $value = str_replace("\\'", "\'", $value);
      $value = str_replace('"', '\"', $value);
      $value = str_replace('\\"', '\"', $value);
*/
      if($result[$key] != $post_vars[$key]) {
        $sql .= "UPDATE `bots` SET `$key` = '$safeVal' where `bot_id` = $bot_id limit 1;\n";
      }
    }
    if (!empty($sql))
    {
      $affectedRows = db_write($sql, null, false, __FILE__, __FUNCTION__, __LINE__);
      if($affectedRows == 0) {
        $msg = "Error updating bot details. See the <a href=\"$logFile\">error log</a> for details.<br />";
        trigger_error("There was a problem adding '$key' to the database. The value was '$value'.");
        //return $msg;
      }
    }
    else
    {
      $msg = 'Nothing seems to have been modified. No changes made.';
    }
    $format = filter_input(INPUT_POST,'format');

    if (strtoupper($format) !== strtoupper($format))
    {
      $format = strtoupper($format);
      $cfn = _CONF_PATH_ . 'global_config.php';
      $configFile = file(_CONF_PATH_ . 'global_config.php',FILE_IGNORE_NEW_LINES);
      $search = '    $format = \'' . $format . '\';';
      $replace = '    $format = \'' . $format . '\';';
      $index = array_search($search, $configFile);
      if (false === $index)
      {
        $msg .= "Error updating the config file. See the <a href=\"$logFile\">error log</a> for details.<br />";
        trigger_error("There was a problem with updating the default format in the config file. Please edit the value manually and submit a bug report.");
      }
      else
      {
        $configFile[$index] = $replace;
        $configContent = implode("\n", $configFile);
        $x = file_put_contents(_CONF_PATH_ . 'global_config.php', $configContent);
      }
    }
    if($msg == '') {
      $msg = 'Bot details updated.';
    }
    return $msg;
  }


  /**
   * Adds a new chatbot to the database
   *
   *
   * @return string
   */
  function addBot() {
  //db globals
  global $dbConn, $msg, $post_vars;
  
  foreach ($post_vars as $key => $value) {
    $$key = trim($value);
  }

  /** @noinspection PhpUndefinedVariableInspection */
  $sql = 'INSERT INTO `bots`(`bot_id`, `bot_name`, `bot_desc`, `bot_active`, `bot_parent_id`, `format`, `save_state`, `conversation_lines`, `remember_up_to`, `debugemail`, `debugshow`, `debugmode`, `default_aiml_pattern`, `error_response`)
VALUES (NULL,:bot_name,:bot_desc,:bot_active,:bot_parent_id,:format,:save_state,:conversation_lines,:remember_up_to,:debugemail,:debugshow,:debugmode,:aiml_pattern,:error_response);';
  $params = array(
    ':bot_name'           => $bot_name,
    ':bot_desc'           => $bot_desc,
    ':bot_desc'           => $bot_desc,
    ':bot_active'         => $bot_active,
    ':bot_parent_id'      => $bot_parent_id,
    ':format'             => $format,
    ':save_state'         => $save_state,
    ':conversation_lines' => $conversation_lines,
    ':remember_up_to'     => $remember_up_to,
    ':debugemail'         => $debugemail,
    ':debugshow'          => $debugshow,
    ':debugmode'          => $debugmode,
    ':aiml_pattern'       => $default_aiml_pattern,
    ':error_response'     => $error_response
  );
  $affectedRows = db_write($sql, $params, false, __FILE__, __FUNCTION__, __LINE__);
  if($affectedRows > 0)
  {
    $msg = "$bot_name Bot details added, please dont forget to create the bot personality and add the aiml.";

  }
  else
  {
    $msg = "$bot_name Bot details could not be added.";
  }

  $_SESSION['poadmin']['bot_id'] = $dbConn->lastInsertId();
  $bot_id = $_SESSION['poadmin']['bot_id'];
  $_SESSION['poadmin']['bot_name'] = $post_vars['bot_name'];
  $bot_name = $_SESSION['poadmin']['bot_name'];
  $msg .= make_bot_predicates($bot_id);
  return $msg;
}

  /**
   * Adds default predicate (personality) data to the database for the current chatbot
   *
   * @param $bot_id
   * @return string
   */
  function make_bot_predicates($bot_id)
{
  global $dbConn, $bot_name;
  $msg = '';

  $sql = <<<endSQL
INSERT INTO `botpersonality` VALUES
  (NULL,  $bot_id, 'age', ''),
  (NULL,  $bot_id, 'baseballteam', ''),
  (NULL,  $bot_id, 'birthday', ''),
  (NULL,  $bot_id, 'birthplace', ''),
  (NULL,  $bot_id, 'botmaster', ''),
  (NULL,  $bot_id, 'boyfriend', ''),
  (NULL,  $bot_id, 'build', ''),
  (NULL,  $bot_id, 'celebrities', ''),
  (NULL,  $bot_id, 'celebrity', ''),
  (NULL,  $bot_id, 'class', ''),
  (NULL,  $bot_id, 'email', ''),
  (NULL,  $bot_id, 'emotions', ''),
  (NULL,  $bot_id, 'ethics', ''),
  (NULL,  $bot_id, 'etype', ''),
  (NULL,  $bot_id, 'family', ''),
  (NULL,  $bot_id, 'favoriteactor', ''),
  (NULL,  $bot_id, 'favoriteactress', ''),
  (NULL,  $bot_id, 'favoriteartist', ''),
  (NULL,  $bot_id, 'favoriteauthor', ''),
  (NULL,  $bot_id, 'favoriteband', ''),
  (NULL,  $bot_id, 'favoritebook', ''),
  (NULL,  $bot_id, 'favoritecolor', ''),
  (NULL,  $bot_id, 'favoritefood', ''),
  (NULL,  $bot_id, 'favoritemovie', ''),
  (NULL,  $bot_id, 'favoritesong', ''),
  (NULL,  $bot_id, 'favoritesport', ''),
  (NULL,  $bot_id, 'feelings', ''),
  (NULL,  $bot_id, 'footballteam', ''),
  (NULL,  $bot_id, 'forfun', ''),
  (NULL,  $bot_id, 'friend', ''),
  (NULL,  $bot_id, 'friends', ''),
  (NULL,  $bot_id, 'gender', ''),
  (NULL,  $bot_id, 'genus', ''),
  (NULL,  $bot_id, 'girlfriend', ''),
  (NULL,  $bot_id, 'hockeyteam', ''),
  (NULL,  $bot_id, 'kindmusic', ''),
  (NULL,  $bot_id, 'kingdom', ''),
  (NULL,  $bot_id, 'language', ''),
  (NULL,  $bot_id, 'location', ''),
  (NULL,  $bot_id, 'looklike', ''),
  (NULL,  $bot_id, 'master', ''),
  (NULL,  $bot_id, 'msagent', ''),
  (NULL,  $bot_id, 'name', '$bot_name'),
  (NULL,  $bot_id, 'nationality', ''),
  (NULL,  $bot_id, 'order', ''),
  (NULL,  $bot_id, 'orientation', ''),
  (NULL,  $bot_id, 'party', ''),
  (NULL,  $bot_id, 'phylum', ''),
  (NULL,  $bot_id, 'president', ''),
  (NULL,  $bot_id, 'question', ''),
  (NULL,  $bot_id, 'religion', ''),
  (NULL,  $bot_id, 'sign', ''),
  (NULL,  $bot_id, 'size', ''),
  (NULL,  $bot_id, 'species', ''),
  (NULL,  $bot_id, 'talkabout', ''),
  (NULL,  $bot_id, 'version', ''),
  (NULL,  $bot_id, 'vocabulary', ''),
  (NULL,  $bot_id, 'wear', ''),
  (NULL,  $bot_id, 'website', '');
endSQL;

  $affectedRows = db_write($sql, null, false, __FILE__, __FUNCTION__, __LINE__);
  if($affectedRows > 0)
  {
    $msg .= 'Please create the bots personality.';
  }
  else {
    $msg .= 'Unable to create the bots personality.';
  }
  return $msg;
}

  /**
   * Changes the current chatbot
   *
   *
   * @return void
   */
  function changeBot() {
  global $dbConn, $msg, $bot_id, $post_vars;
  $botId = (isset($post_vars['bot_id'])) ? $post_vars['bot_id'] : $bot_id;
  
  if($post_vars['bot_id']!="new") {
    $sql = "SELECT * FROM `bots` WHERE bot_id = '$botId'";
    $row = db_fetch($sql, null, __FILE__, __FUNCTION__, __LINE__);
    $count = count($row);
    if($count > 0) {
      $_SESSION['poadmin']['format'] = $row['format'];
      $_SESSION['poadmin']['bot_id'] = $row['bot_id'];
      $_SESSION['poadmin']['bot_name'] = $row['bot_name'];
    }
    else {
      $_SESSION['poadmin']['bot_id']="new";
      $_SESSION['poadmin']['bot_name']='<b class="red">unnamed bot</b>';
    }
  }
  else {
      $_SESSION['poadmin']['bot_name']='<b class="red">unnamed bot</b>';
      $_SESSION['poadmin']['bot_id']="new";
    }
    header("Location: index.php?page=select_bots");
}


  /**
   * Returns an HTML form for selecting a chatbot from the database
   *
   *
   * @return string
   */
  function getChangeList() {
  global $dbConn, $template;
  $bot_id = (isset($_SESSION['poadmin']['bot_id'])) ? $_SESSION['poadmin']['bot_id'] : 0;
  $botId = $bot_id;
  $sql = "SELECT * FROM `bots` ORDER BY bot_name";
  $result = db_fetchAll($sql, null, __FILE__, __FUNCTION__, __LINE__);
  $options = '                <option value="new">Add New Bot</option>' . "\n";
  foreach ($result as $row) {
    $options .= "<!-- bot ID = {$row['bot_id']}, $botId -->\n";
    if($botId == $row['bot_id']) {
      $sel = ' selected="selected"';
    }
    else {
      $sel= '';
    }
    $bot_id = $row['bot_id'];
    $bot_name = $row['bot_name'];
    $options .= "                <option value=\"$bot_id\"$sel>$bot_name</option>\n";
  }
  $options = rtrim($options);
  $form = $template->getSection('ChangeBot');
  $form = str_replace('[options]', $options, $form);
  return $form;
}
