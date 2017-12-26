<?php
  /***************************************
    * http://www.program-o.com
    * PROGRAM O
    * Version: 2.5.3
    * FILE: stats.php
    * AUTHOR: Elizabeth Perreau and Dave Morton
    * DATE: 12-12-2014
    * DETAILS: Displays chatbot statistics for the currently selected chatbot
    ***************************************/
  $oneday = getStats("today");
  $oneweek = getStats("-1 week");
  $onemonth = getStats("-1 month");
  $sixmonths = getStats("-6 month");
  $oneyear = getStats("1 year ago");
  $alltime = getStats("all");
  $singlelines = getChatLines(1, 1);
  $alines = getChatLines(1, 25);
  $blines = getChatLines(26, 50);
  $clines = getChatLines(51, 100);
  $dlines = getChatLines(101, 1000000);
  $avg = getChatLines("average", 1000000);
  $upperScripts = '';
  $topNav = $template->getSection('TopNav');
  $leftNav = $template->getSection('LeftNav');
  $main = $template->getSection('Main');
  $navHeader = $template->getSection('NavHeader');
  $FooterInfo = getFooter();
  $errMsgClass = (!empty ($msg)) ? "ShowError" : "HideError";
  $errMsgStyle = $template->getSection($errMsgClass);
  $noLeftNav = '';
  $noTopNav = '';
  $noRightNav = $template->getSection('NoRightNav');
  $headerTitle = 'Actions:';
  $pageTitle = 'My-Program O - Bot Stats';
  $mainContent = $template->getSection('StatsPage');
  $mainTitle = 'Bot Statistics for ' . $bot_name;
  $mainContent = str_replace('[oneday]', $oneday, $mainContent);
  $mainContent = str_replace('[oneweek]', $oneweek, $mainContent);
  $mainContent = str_replace('[onemonth]', $onemonth, $mainContent);
  $mainContent = str_replace('[sixmonths]', $sixmonths, $mainContent);
  $mainContent = str_replace('[oneyear]', $oneyear, $mainContent);
  $mainContent = str_replace('[alltime]', $alltime, $mainContent);
  $mainContent = str_replace('[singlelines]', $singlelines, $mainContent);
  $mainContent = str_replace('[alines]', $alines, $mainContent);
  $mainContent = str_replace('[blines]', $blines, $mainContent);
  $mainContent = str_replace('[clines]', $clines, $mainContent);
  $mainContent = str_replace('[dlines]', $dlines, $mainContent);
  $mainContent = str_replace('[avg]', $avg, $mainContent);

  /**
  * Function getStats
  *
  * * @param $interval
  * @return mixed
  */
  function getStats($interval)
  {
    global $bot_id, $dbConn;
    if ($interval != "all")
    {
      $intervaldate = date("Y-m-d", strtotime($interval));
      $sqladd = " AND date(timestamp) >= '$intervaldate'";
    }
    else
    {
      $sqladd = "";
    }
    //get undefined defaults from the db
    $sql = "SELECT count(distinct(`user_id`)) AS TOT FROM `conversation_log` WHERE bot_id = '$bot_id' $sqladd";
    $row = db_fetch($sql, null, __FILE__, __FUNCTION__, __LINE__);
    $res = $row['TOT'];
    return $res;
  }

  /**
  * Function getChatLines
  *
  * * @param $i
  * @param $j
  * @return mixed
  */
  function getChatLines($i, $j)
  {
    global $bot_id, $dbConn;
    $sql = <<<endSQL
SELECT AVG(`chatlines`) AS TOT
				FROM `users`
				INNER JOIN `conversation_log` ON `users`.`id` = `conversation_log`.`user_id`
				WHERE `conversation_log`.`bot_id` = $bot_id AND [endCondition];
endSQL;
	    if ($i == "average")
    {
      $endCondition = '`chatlines` != 0;';
    }
    else
    {
      $endCondition = "(`chatlines` >= $i AND `chatlines` <= $j)";
    }
    $sql = str_replace('[endCondition]', $endCondition, $sql);
    //get undefined defaults from the db
    $row = db_fetch($sql, null, __FILE__, __FUNCTION__, __LINE__);
    $res = $row['TOT'];
    return $res;
  }

?>