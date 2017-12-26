<?php

  /***************************************
  * http://www.program-o.com
  * PROGRAM O
  * Version: 2.5.3
  * FILE: help.php
  * AUTHOR: Elizabeth Perreau and Dave Morton
  * DATE: 13-11-2014
  * DETAILS: Provides halp information for the Program O install script
  ***************************************/
  ini_set("display_errors", false);
  ini_set("log_errors", true);
  ini_set("error_log", "../logs/error.log");
  define('SECTION_START', '<!-- Section [section] Start -->'); # search params for start and end of sections
  define('SECTION_END', '<!-- Section [section] End -->'); # search params for start and end of sections
  $template = file_get_contents('help.tpl.htm');
  $content = help_getSection('HelpPage', $template, false);
  $helpContent = help_getSection('HelpMain', $template);
  $content = str_replace('[helpContent]', $helpContent, $content);
  echo ($content);

  /**
  * Function help_getSection
  *
  * * @param $sectionName
  * @param      $page_template
  * @param bool $notFoundReturn
  * @return string
  */
  function help_getSection($sectionName, $page_template, $notFoundReturn = true)
  {
    $sectionStart = str_replace('[section]', $sectionName, SECTION_START);
    $sectionStartLen = strlen($sectionStart);
    $sectionEnd = str_replace('[section]', $sectionName, SECTION_END);
    $startPos = strpos($page_template, $sectionStart, 0);
    if ($startPos === false)
    {
      if ($notFoundReturn)
      {
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
