<?php

  /* * *************************************
  * http://www.program-o.com
  * PROGRAM O
  * Version: 2.5.3
  * FILE: editAiml.php
  * AUTHOR: Elizabeth Perreau and Dave Morton
  * DATE: 05-26-2014
  * DETAILS: Search the AIML table of the DB for desired categories
  * ************************************* */
  $post_vars = filter_input_array(INPUT_POST);
  $get_vars = filter_input_array(INPUT_GET);
  $form_vars = array_merge((array) $post_vars, (array) $get_vars);
  if (isset ($get_vars['action']))
  {
  //load shared files
    $thisFile = __FILE__;
    require_once('../config/global_config.php');
    require_once (_LIB_PATH_ . 'PDO_functions.php');
    require_once (_LIB_PATH_ . 'error_functions.php');
    $session_name = 'PGO_Admin';
    session_name($session_name);
    session_start();
    $bot_id = (isset ($_SESSION['poadmin']['bot_id'])) ? $_SESSION['poadmin']['bot_id'] : 1;
    if (empty ($_SESSION) || !isset ($_SESSION['poadmin']['uid']) || $_SESSION['poadmin']['uid'] == "")
    {
      error_log('Session vars: ' . print_r($_SESSION, true), 3, _LOG_PATH_ . 'session.txt');
      exit (json_encode(array('error' => "No session found")));
    }
    // Open the DB
    $dbConn = db_open();
  }
  if ((isset ($get_vars['action'])) && ($get_vars['action'] == "search"))
  {
    $group = (isset ($get_vars['group'])) ? $get_vars['group'] : 1;
    exit (runSearch());
  }
  elseif ((isset ($get_vars['action'])) && ($get_vars['action'] == "add"))
  {
    exit (insertAIML());
  }
  elseif ((isset ($get_vars['action'])) && ($get_vars['action'] == "update"))
  {
    exit (updateAIML());
  }
  elseif ((isset ($get_vars['action'])) && ($get_vars['action'] == "del") && (isset ($get_vars['id'])) && ($get_vars['id'] != ""))
  {
    exit (delAIML($get_vars['id']));
  }
  else
  {
    $mainContent = $template->getSection('EditAimlPage');
  }

  $topNav = $template->getSection('TopNav');
  $leftNav = $template->getSection('LeftNav');
  $rightNav = $template->getSection('RightNav');
  $main = $template->getSection('Main');
  $navHeader = $template->getSection('NavHeader');
  $FooterInfo = getFooter();
  $errMsgClass = (!empty ($msg)) ? "ShowError" : "HideError";
  $errMsgStyle = $template->getSection($errMsgClass);
  $noLeftNav = '';
  $noTopNav = '';
  $noRightNav = $template->getSection('NoRightNav');
  $headerTitle = 'Actions:';
  $pageTitle = 'My-Program O - Search/Edit AIML';
  $mainTitle = 'Search/Edit AIML';

  /**
  * Function delAIML
  *
  * * @param $id
  * @return string
  */
  function delAIML($id)
  {
    if ($id != "")
    {
      $sql = "DELETE FROM `aiml` WHERE `id` = '$id' LIMIT 1";
      $affectedRows = db_write($sql, null, false, __FILE__, __FUNCTION__, __LINE__);
      if ($affectedRows == 0)
      {
        $msg = 'Error AIML couldn\'t be deleted - no changes made.</div>';
      }
      else
      {
        $msg = 'AIML has been deleted.';
      }
    }
    else
    {
      $msg = 'Error AIML couldn\'t be deleted - no changes made.';
    }
    return $msg;
  }

  /**
  * Function runSearch
  *
  *
  * @return mixed|string
  */
  function runSearch()
  {
    global $bot_id, $form_vars, $dbConn, $group;
    $groupSize = 10;
    $limit = ($group - 1) * $groupSize;
    $limit = ($limit < 0) ? 0 : $limit;
    $search_fields = array('topic', 'filename', 'pattern', 'template', 'thatpattern');
    $searchTerms = array();
    $searchArguments = array($bot_id);
    $searchParams = array($bot_id);
    foreach ($search_fields as $index)
    {
      if (!empty ($form_vars[$index]))
      {
        $searchParams[] = "%{$form_vars[$index]}%";
        array_push($searchArguments, "%" . trim($form_vars[$index]) . "%");
        array_push($searchTerms, "$index like ?");
      }
    }
    $searchTerms = (!empty ($searchTerms)) ? implode(" AND ", $searchTerms) : "TRUE";
    $countSQL = "SELECT count(id) FROM `aiml` WHERE `bot_id` = ? AND ($searchTerms)";
    $count = db_fetch($countSQL, $searchParams, __FILE__, __FUNCTION__, __LINE__);
    $total = $count['count(id)'];
    $limit = ($limit >= $total) ? $total - 1 - (($total - 1) % $groupSize) : $limit;
    $order = isset ($form_vars['sort']) ? $form_vars['sort'] . " " . $form_vars['sortOrder'] : "id";
    $sql = "SELECT id, topic, filename, pattern, template, thatpattern FROM `aiml` " . "WHERE `bot_id` = ? AND ($searchTerms) order by $order limit $limit, $groupSize;";
    $result = db_fetchAll($sql, $searchParams, __FILE__, __FUNCTION__, __LINE__);
    $out = array("results" => $result, "total_records" => $total, "start_index" => 0, "page" => ($limit / $groupSize) + 1, "page_size" => $groupSize);
    return json_encode($out);
  }

  /**
  * Function updateAIML
  *
  *
  * @return string
  */
  function updateAIML()
  {
    global $post_vars, $dbConn;
    $template = trim($post_vars['template']);
    $pattern = (IS_MB_ENABLED) ? mb_strtoupper(trim($post_vars['pattern'])) : strtoupper(trim($post_vars['pattern']));
    $thatpattern = (IS_MB_ENABLED) ? mb_strtoupper(trim($post_vars['thatpattern'])) : strtoupper(trim($post_vars['thatpattern']));
    $topic = (IS_MB_ENABLED) ? mb_strtoupper(trim($post_vars['topic'])) : strtoupper(trim($post_vars['topic']));
    $id = trim($post_vars['id']);
    if (($template == "") || ($pattern == "") || ($id == ""))
    {
      $msg = 'Please make sure you have entered a user input and bot response ';
    }
    else
    {
      $sql = "UPDATE `aiml` SET `pattern`=?,`thatpattern`=?,`template`=?,`topic`=?,`filename`=? WHERE `id`=? LIMIT 1";
      $sth = $dbConn->prepare($sql);
      try
      {
        $sth->execute(array($pattern, $thatpattern, $template, $topic, trim($post_vars['filename']), $id));
      }
      catch (Exception $e)
      {
        header("HTTP/1.0 500 Internal Server Error");
        throw ($e);
      }
      $affectedRows = $sth->rowCount();
      if ($affectedRows > 0)
      {
        $msg = 'AIML Updated.';
      }
      else
      {
        $msg = 'There was an error updating the AIML - no changes made.';
      }
    }
    return $msg;
  }

  /**
  * Function insertAIML
  *
  *
  * @return string
  */
  function insertAIML()
  {
  //db globals
    global $msg, $post_vars, $dbConn, $bot_id;
    $aiml = "<category><pattern>[pattern]</pattern>[thatpattern]<template>[template]</template></category>";
    $aimltemplate = trim($post_vars['template']);
    $pattern = trim($post_vars['pattern']);
    $pattern = (IS_MB_ENABLED) ? mb_strtoupper($pattern) : strtoupper($pattern);
    $thatpattern = trim($post_vars['thatpattern']);
    $thatpattern = (IS_MB_ENABLED) ? mb_strtoupper($thatpattern) : strtoupper($thatpattern);
    $aiml = str_replace('[pattern]', $pattern, $aiml);
    $aiml = (empty ($thatpattern)) ? str_replace('[thatpattern]', "<that>$thatpattern</that>", $aiml) : $aiml;
    $aiml = str_replace('[template]', $aimltemplate, $aiml);
    $topic = trim($post_vars['topic']);
    $topic = (IS_MB_ENABLED) ? mb_strtoupper($topic) : strtoupper($topic);
    if (($pattern == "") || ($aimltemplate == ""))
    {
      $msg = 'You must enter a user input and bot response.';
    }
    else
    {
      $sth = $dbConn->prepare("INSERT INTO `aiml` (`id`,`bot_id`, `aiml`, `pattern`,`thatpattern`,`template`,`topic`,`filename`) " . "VALUES (NULL, ?, ?, ?, ?, ?, ?,'admin_added.aiml')");
      try
      {
        $sth->execute(array($bot_id, $aiml, $pattern, $thatpattern, $aimltemplate, $topic));
      }
      catch (Exception $e)
      {
        header("HTTP/1.0 500 Internal Server Error");
        throw ($e);
      }
      $affectedRows = $sth->rowCount();
      if ($affectedRows > 0)
      {
        $msg = "AIML added.";
      }
      else
      {
        $msg = "There was a problem adding the AIML - no changes made.";
      }
    }
    return $msg;
  }
