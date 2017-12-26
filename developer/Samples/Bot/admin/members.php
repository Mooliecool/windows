<?php
  /***************************************
    * http://www.program-o.com
    * PROGRAM O
    * Version: 2.5.3
    * FILE: members.php
    * AUTHOR: Elizabeth Perreau and Dave Morton
    * DATE: 12-12-2014
    * DETAILS: Administers member (admin) accounts
    ***************************************/
  ini_set('memory_limit','128M');
  ini_set('max_execution_time','0');
  $post_vars = filter_input_array(INPUT_POST);

  $user_name = '';
  $action = (isset($post_vars['action'])) ? ucfirst(strtolower($post_vars['action'])) : 'Add';
  if (!empty($post_vars)) {
    $msg = save($action);
  }


  $id = (isset($post_vars['id']) and $action != 'Add') ? $post_vars['id'] : getNextID();
  $id = ($id <= 0) ? getNextID() : $id;
  if (isset($post_vars['memberSelect'])) {
    $id = $post_vars['memberSelect'];
    $data = getMemberData($post_vars['memberSelect']);
    $id = $data['id'];
    $user_name = $data['user_name'];
  }
  $upperScripts = $template->getSection('UpperScripts');
  $XmlEntities = array(
    '&amp;'  => '&',
    '&lt;'   => '<',
    '&gt;'   => '>',
    '&apos;' => '\'',
    '&quot;' => '"',
  );

  $AdminsOpts   = getAdminsOpts();

  $membersForm       = $template->getSection('MembersForm');
  $members_list_form = $template->getSection('MembersListForm');
  $showHelp          = $template->getSection('MembersShowHelp');
  $topNav            = $template->getSection('TopNav');
  $leftNav           = $template->getSection('LeftNav');
  $main              = $template->getSection('Main');
  $navHeader         = $template->getSection('NavHeader');
  $FooterInfo        = getFooter();
  $errMsgClass       = (!empty($msg)) ? "ShowError" : "HideError";
  $errMsgStyle       = $template->getSection($errMsgClass);
  $noLeftNav         = '';
  $noTopNav          = '';
  $noRightNav        = $template->getSection('NoRightNav');
  $headerTitle       = 'Actions:';
  $pageTitle         = 'My-Program O - Admin Accounts';
  $mainContent       = $template->getSection('MembersMain');
  $mainTitle         = "Modify Admin Account Data [helpLink]";

  $members_list_form = str_replace('[adminList]', $AdminsOpts, $members_list_form);
  $mainContent       = str_replace('[showHelp]', $showHelp, $mainContent);
  $mainContent       = str_replace('[members_content]', $membersForm, $mainContent);
  $mainContent       = str_replace('[members_list_form]', $members_list_form, $mainContent);
  $mainContent       = str_replace('[user_name]', $user_name, $mainContent);
  $mainContent       = str_replace('[action]', $action, $mainContent);
  $mainContent       = str_replace('[id]', $id, $mainContent);
  $mainTitle         = str_replace('[helpLink]', $template->getSection('HelpLink'), $mainTitle);


  /**
   * Function save
   *
   * * @param $action
   * @return string
   */
  function save($action) {
    global $dbConn, $dbn, $action, $post_vars;
    if (isset($post_vars['memberSelect'])) {
      $id = $post_vars['memberSelect'];
    }
    else {
      if (!isset($post_vars['user_name']) or !isset($post_vars['password']) or !isset($post_vars['passwordConfirm'])) return 'You left something out!';
      $id = $post_vars['id'];
      $user_name = $post_vars['user_name'];
      $password1 = $post_vars['password'];
      $password2 = $post_vars['passwordConfirm'];
      $password = md5($password1);
      if ($action != 'Delete' and ($password1 != $password2)) return 'The passwords don\'t match!';
    }
    switch ($action) {
      case 'Add':
      $ip = $_SERVER['REMOTE_ADDR'];
      $sql = "insert into myprogramo (id, user_name, password, last_ip, last_login) values (null, :user_name, :password,:ip, CURRENT_TIMESTAMP);";
      $params = array(
        ':user_name' => $user_name,
        ':password'  => $password,
        ':ip'        => $ip
      );
      $out = "Account for $user_name successfully added!";
      break;
      case 'Delete':
      $action = 'Add';
      $sql = "DELETE FROM `$dbn`.`myprogramo` WHERE `myprogramo`.`id` = :id LIMIT 1";
      $params = array(':id' => $id);
      $out = "Account for $user_name successfully deleted!";
      break;
      case 'Edit':
      $action = 'Add';
      $sql = "update myprogramo set user_name = :user_name, password = :password where id = :id;";
      $params = array(
        ':user_name' => $user_name,
        ':password'  => $password,
        ':id'        => $id
      );

      $out = "Account for $user_name successfully updated!";
      break;
      default:
      $action = 'Edit';
      $sql = '';
      $out = '';
    }
    if (!empty($sql))
    {
      //save_file(_LOG_PATH_ . 'memberSQL.txt', $sql);
      $affectedRows = db_write($sql, $params, false, __FILE__, __FUNCTION__, __LINE__);

      //
    }
    return $out;
  }


  /**
   * Function getAdminsOpts
   *
   *
   * @return string
   */
  function getAdminsOpts() {
    global $dbn, $dbConn;
    $out = "                  <!-- Start List of Current Admin Accounts -->\n";
    $optionTemplate = "                  <option value=\"[val]\">[key]</option>\n";
    $sql = 'SELECT id, user_name FROM myprogramo order by user_name;';
    $result = db_fetchAll($sql, null, __FILE__, __FUNCTION__, __LINE__);
    foreach ($result as $row) {
      $user_name = $row['user_name'];
      $id = $row['id'];
      $curOption = str_replace('[key]', $row['user_name'], $optionTemplate);
      $curOption = str_replace('[val]', $row['id'], $curOption);
      $out .= $curOption;
    }
    
    $out .= "                  <!-- End List of Current Admin Accounts -->\n";
    return $out;
  }

  /**
   * Function getMemberData
   *
   * * @param $id
   * @return void
   */
  function getMemberData($id) {
    if ($id <= 0) return;
    global $user_name, $dbConn;
    $sql = "select id, user_name from myprogramo where id = $id limit 1;";
    $row = db_fetch($sql, null, __FILE__, __FUNCTION__, __LINE__);
    return $row;
  }

  /**
   * Function getNextID
   *
   *
   * @return int
   */
  function getNextID() {
    global $dbConn;
    $sql = "select id from myprogramo order by id desc limit 1;";
    $row = db_fetch($sql, null, __FILE__, __FUNCTION__, __LINE__);
    return $row['id'] + 1;
  }

?>