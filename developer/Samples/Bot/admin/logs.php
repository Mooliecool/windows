<?php

  /***************************************
  * http://www.program-o.com
  * PROGRAM O
  * Version: 2.5.3
  * FILE: logs.php
  * AUTHOR: Elizabeth Perreau and Dave Morton
  * DATE: 12-12-2014
  * DETAILS: Displays chat logs for the currently selected chatbot
  ***************************************/
  $get_vars = filter_input_array(INPUT_GET);
  $show = (isset ($get_vars['showing'])) ? $get_vars['showing'] : "last 20";
  //showThis($show)
  $show_this = showThis($show);
  $convo = (isset ($get_vars['id'])) ? getuserConvo($get_vars['id'], $show) : "Please select a conversation from the side bar.";
  $user_list = (isset ($get_vars['id'])) ? getuserList($get_vars['id'], $show) : getuserList($_SESSION['poadmin']['bot_id'], $show);
  $bot_name = (isset ($_SESSION['poadmin']['bot_name'])) ? $_SESSION['poadmin']['bot_name'] : 'unknown';
  $upperScripts = $template->getSection('UpperScripts');

  $topNav = $template->getSection('TopNav');
  $leftNav = $template->getSection('LeftNav');
  $main = $template->getSection('Main');
  $FooterInfo = getFooter();
  $errMsgClass = (!empty ($msg)) ? "ShowError" : "HideError";
  $errMsgStyle = $template->getSection($errMsgClass);
  $rightNav = $template->getSection('RightNav');
  $navHeader = $template->getSection('NavHeader');
  $noLeftNav = '';
  $noTopNav = '';
  $noRightNav = '';
  $headerTitle = 'Actions:';
  $pageTitle = 'My-Program O - Chat Logs';
  $mainContent = $template->getSection('ConversationLogs1');
  $mainTitle = 'Chat Logs';
  $rightNav = str_replace('[rightNavLinks]', $show_this . $user_list, $rightNav);
  $rightNav = str_replace('[navHeader]', $navHeader, $rightNav);
  $rightNav = str_replace('[headerTitle]', 'Log Actions:', $rightNav);
  $mainContent = str_replace('[show_this]', '', $mainContent);
  $mainContent = str_replace('[convo]', $convo, $mainContent);
  $mainContent = str_replace('[bot_name]', $bot_name, $mainContent);

  /**
  * Function getUserNames
  *
  *
  * @return array
  */
  function getUserNames()
  {
    global $dbConn;
    $nameList = array();
    $sql = "select `id`, `user_name` from `users` where 1;";
    $result = db_fetchAll($sql, null, __FILE__, __FUNCTION__, __LINE__);
    foreach ($result as $row)
    {
      $nameList[$row['id']] = $row['user_name'];
    }
    return $nameList;
  }

  /**
  * Function getuserList
  *
  * * @param $showing
  * @return string
  */
  function getuserList($showing)
  {
  //db globals
    global $template, $get_vars, $dbConn;
    $nameList = getUserNames();
    $curUserid = (isset ($get_vars['id'])) ? $get_vars['id'] : - 1;
    $bot_id = $_SESSION['poadmin']['bot_id'];
    $linkTag = $template->getSection('NavLink');
    $sql = "SELECT DISTINCT(`user_id`),COUNT(`user_id`) AS TOT FROM `conversation_log`  WHERE bot_id = '$bot_id' AND DATE(`timestamp`) = '[repl_date]' GROUP BY `user_id`, `convo_id` ORDER BY ABS(`user_id`) ASC";
    $showarray = array("last 20", "previous week", "previous 2 weeks", "previous month", "last 6 months", "this year", "previous year", "all years");
    switch ($showing)
    {
      case "today" :
        $repl_date = date("Y-m-d");
        break;
      case "previous week" :
        $repl_date = strtotime("-1 week");
        break;
      case "previous 2 weeks" :
        $repl_date = strtotime("-2 week");
        break;
      case "previous month" :
        $repl_date = strtotime("-1 month");
        break;
      case "previous 6 months" :
        $repl_date = strtotime("-6 month");
        break;
      case "past 12 months" :
        $repl_date = strtotime("-1 year");
        break;
      case "all time" :
        $sql = "SELECT DISTINCT(`user_id`),COUNT(`user_id`) AS TOT FROM `conversation_log`  WHERE  bot_id = '$bot_id' GROUP BY `user_id` ORDER BY ABS(`user_id`) ASC";
        $repl_date = time();
        break;
      default :
        $sql = "SELECT DISTINCT(`user_id`),COUNT(`user_id`) AS TOT FROM `conversation_log`  WHERE  bot_id = '$bot_id' GROUP BY `user_id` ORDER BY ABS(`user_id`) ASC";
        $repl_date = time();
    }
    $sql = str_replace('[repl_date]', $repl_date, $sql);
    $list = <<<endList

      <div class="userlist">
        <ul>

endList;
      $rows = db_fetchAll($sql, null, __FILE__, __FUNCTION__, __LINE__);
    $numRows = count($rows);
    if ($numRows == 0)
      $list .= '          <li>No log entries found</li>';
    foreach ($rows as $row)
    {
      $user_id = $row['user_id'];
      $linkClass = ($user_id == $curUserid) ? 'selected' : 'noClass';
      $userName = @ $nameList[$user_id];
      $TOT = $row['TOT'];
      $tmpLink = str_replace('[linkClass]', " class=\"$linkClass\"", $linkTag);
      $tmpLink = str_replace('[linkOnclick]', '', $tmpLink);
      $tmpLink = str_replace('[linkHref]', "href=\"index.php?page=logs&showing=$showing&id=$user_id#$user_id\" name=\"$user_id\"", $tmpLink);
      $tmpLink = str_replace('[linkTitle]', " title=\"Show entries for user $userName\"", $tmpLink);
      $tmpLink = str_replace('[linkLabel]', "USER:$userName($TOT)", $tmpLink);
      $anchor = "            <a name=\"$user_id\" />\n";
      $anchor = '';
      $list .= "$tmpLink\n$anchor";
    }
    $list .= "\n       </div>\n";
    return $list;
  }

  /**
  * Function showThis
  *
  * * @param string $showing
  * @return string
  */
  function showThis($showing = "last 20")
  {
    $showarray = array("last 20", "today", "previous week", "previous 2 weeks", "previous month", "last 6 months", "past 12 months", "all time");
    $options = "";
    foreach ($showarray as $index => $value)
    {
      if ($value == $showing)
      {
        $sel = " SELECTED=SELECTED";
      }
      else
      {
        $sel = "";
      }
      $options .= "          <option value=\"$value\"$sel>$value</option>\n";
    }
    $form = <<<endForm
        <form name="showthis" method="post" action="index.php?page=logs">
          <select name="showing" id="showing">
$options
          </select>
        <input type="submit" id="submit" name="submit" value="show">
      </form>
endForm;
      return $form;
  }

  /**
  * Function getuserConvo
  *
  * * @param $id
  * @param $showing
  * @return mixed|string
  */
  function getuserConvo($id, $showing)
  {
    global $dbConn;
    $bot_name = (isset ($_SESSION['poadmin']['bot_name'])) ? $_SESSION['poadmin']['bot_name'] : 'Bot';
    $bot_id = (isset ($_SESSION['poadmin']['bot_id'])) ? $_SESSION['poadmin']['bot_id'] : 0;
    $nameList = getUserNames();
    $user_name = $nameList[$id];
    switch ($showing)
    {
      case "today" :
        $sqladd = "AND DATE(`timestamp`) = '" . date('Y-m-d') . "'";
        $title = "Today's ";
        break;
      case "previous week" :
        $lastweek = strtotime("-1 week");
        $sqladd = "AND DATE(`timestamp`) >= '" . $lastweek . "'";
        $title = "Last week's ";
        break;
      case "previous 2 weeks" :
        $lasttwoweek = strtotime("-2 week");
        $sqladd = "AND DATE(`timestamp`) >= '" . $lasttwoweek . "'";
        $title = "Last two week's ";
        break;
      case "previous month" :
        $lastmonth = strtotime("-1 month");
        $sqladd = "AND DATE(`timestamp`) >= '" . $lastmonth . "'";
        $title = "Last month's ";
        break;
      case "previous 6 months" :
        $lastsixmonth = strtotime("-6 month");
        $sqladd = "AND DATE(`timestamp`) >= '" . $lastsixmonth . "'";
        $title = "Last six month's ";
        break;
      case "past 12 months" :
        $lastyear = strtotime("-1 year");
        $sqladd = "AND DATE(`timestamp`) >= '" . $lastyear . "'";
        $title = "Last twelve month's ";
        break;
      case "all time" :
        $sql = "";
        $title = "All ";
        break;
      default :
        $sqladd = "";
        $title = "Last ";
    }
    $lasttimestamp = "";
    $i = 1;
    //get undefined defaults from the db
    $sql = "SELECT *  FROM `conversation_log` WHERE `bot_id` = '$bot_id' AND `user_id` = $id $sqladd ORDER BY `id` ASC";
    $list = "<hr><br/><h4>$title conversations for user: $id</h4>";
    $list .= "<div class=\"convolist\">";
    $result = db_fetchAll($sql, null, __FILE__, __FUNCTION__, __LINE__);
    foreach ($result as $row)
    {
      $thisdate = date("Y-m-d", strtotime($row['timestamp']));
      if ($thisdate != $lasttimestamp)
      {
        if ($i > 1)
        {
          if ($showing == "last 20")
          {
            break;
          }
        }
        $date = date("Y-m-d");
        $list .= "<hr><br/><h4>Conversation#$i $thisdate</h4>";
        $i++;
      }
      $list .= "<br><span style=\"color:DARKBLUE;\">$user_name: " . $row['input'] . "</span>";
      $list .= "<br><span style=\"color:GREEN;\">$bot_name: " . $row['response'] . "</span>";
      $lasttimestamp = $thisdate;
    }
    $list .= "</div>";
    $list = str_ireplace('<script', '&lt;script', $list);
    return $list;
  }

?>
