<?php
  /***************************************
    * http://www.program-o.com
    * PROGRAM O
    * Version: 2.5.3
    * FILE: download.php
    * AUTHOR: Elizabeth Perreau and Dave Morton
    * DATE: 12-08-2014
    * DETAILS: Provides the ability to download a chatbot's AIML filesm either in AIML or SQL format.
    ***************************************/

  $content = '';
  $status = '';
  $bot_id = ($bot_id == 'new') ? 0 : $bot_id;
  $referer = filter_input(INPUT_SERVER, 'HTTP_REFERER', FILTER_SANITIZE_URL);
  $upperScripts = $template->getSection('UpperScripts');
  $post_vars = filter_input_array(INPUT_POST);
  $get_vars = filter_input_array(INPUT_GET);
  if (isset($post_vars))
  {
    $type = $post_vars['type'];
    $zipFilename = "$bot_name.$type.zip";
    if (!isset($post_vars['filenames']))
    {
      $msg .= 'No files were selected for download. Please select at least one file.';
    }
    else
    {
      $fileNames = $post_vars['filenames'];
      unlink(_ADMIN_PATH_ . "downloads/$zipFilename"); // clear out any old zip files to prepare for the new one.
      $zip = new ZipArchive();
      $success = $zip->open(_ADMIN_PATH_ . "/downloads/$zipFilename", ZipArchive::CREATE);
      if ($success === true)
      {
        foreach ($fileNames as $filename)
        {
          $curZipContent = ($type == 'SQL') ? getSQLByFileName($filename) : getAIMLByFileName($filename);
          $filename = ($type == 'SQL') ? str_replace('.aiml', '.sql', $filename) : $filename;
          $zip->addFromString($filename, $curZipContent);
        }
        $zip->close();
        $_SESSION['send_file'] = $zipFilename;
        $_SESSION['referer'] = $referer;
        header("Refresh: 5; url=file.php");
        $msg .= "The file $zipFilename is being processed. If the download doesn't start within a few seconds, please click <a href=\"file.php\">here</a>.\n";
      }
    }
  }
  $content .= renderMain();
  $showHelp = $template->getSection('DownloadShowHelp');
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
  $pageTitle = "My-Program O - Download AIML files";
  $mainContent = $content;
  $mainTitle = "Download AIML files for the bot named  $bot_name [helpLink]";
  $mainContent = str_replace('[showHelp]', $showHelp, $mainContent);
  $mainContent = str_replace('[status]', $status, $mainContent);
  $mainTitle = str_replace('[helpLink]', $template->getSection('HelpLink'), $mainTitle);

  /**
   * Function getAIMLByFileName
   *
   * * @param $filename
   * @return string
   */
  function getAIMLByFileName($filename)
  {
    if ($filename == 'null') return "You need to select a file to download.";
    global $dbn, $botmaster_name, $charset, $dbConn;
    $bmnLen = strlen($botmaster_name) - 2;
    $bmnSearch = str_pad('[bm_name]', $bmnLen);
    $categoryTemplate =
    '<category><pattern>[pattern]</pattern>[that]<template>[template]</template></category>';
    $cleanedFilename = $filename;
    $fileNameSearch = '[fileName]';
    $cfnLen = strlen($cleanedFilename);
    $fileNameSearch = str_pad($fileNameSearch, $cfnLen);
    $topicArray = array();
    $curPath = dirname(__FILE__);
    chdir($curPath);
    $fileContent = file_get_contents('./AIML_Header.dat');
    $fileContent = str_replace('[year]', date('Y'), $fileContent);
    $fileContent = str_replace('[charset]', $charset, $fileContent);
    $fileContent = str_replace($bmnSearch, $botmaster_name, $fileContent);
    $curDate = date('m-d-Y', time());
    $cdLen = strlen($curDate);
    $curDateSearch = str_pad('[curDate]', $cdLen);
    $fileContent = str_replace($curDateSearch, $curDate, $fileContent);
    $fileContent = str_replace($fileNameSearch, $cleanedFilename, $fileContent);
    $sql = "select distinct topic from aiml where filename like '$cleanedFilename';";
    $result = db_fetchAll($sql, null, __FILE__, __FUNCTION__, __LINE__);
    foreach ($result as $row)
    {
      $topicArray[] = $row['topic'];
    }
    foreach ($topicArray as $topic)
    {
      if (!empty ($topic))
        $fileContent .= "<topic name=\"$topic\">\n";
      $sql =
      "select pattern, thatpattern, template from aiml where topic like '$topic' and filename like '$cleanedFilename';";
      $result = db_fetchAll($sql, null, __FILE__, __FUNCTION__, __LINE__);
      foreach ($result as $row)
      {
        $pattern = (IS_MB_ENABLED) ? mb_strtoupper($row['pattern']) : strtoupper($row['pattern']);
        $template = str_replace("\r\n", '', $row['template']);
        $template = str_replace("\n", '', $row['template']);
        $newLine = str_replace('[pattern]', $pattern, $categoryTemplate);
        $newLine = str_replace('[template]', $template, $newLine);
        $that = (!empty ($row['thatpattern'])) ? '<that>' . $row['thatpattern'] .
                '</that>' : '';
        $newLine = str_replace('[that]', $that, $newLine);
        $fileContent .= "$newLine\n";
      }
      if (!empty ($topic))
        $fileContent .= "</topic>\n";
    }
    $fileContent .= "\r\n</aiml>\r\n";
    $dom = new DOMDocument();
    $dom->preserveWhiteSpace = false;
    $dom->formatOutput = true;
    $dom->loadXML(trim($fileContent));
    $fileContent = $dom->saveXML();
    $outFile = ltrim($fileContent, "\n\r\n");
    $outFile = mb_convert_encoding($outFile, 'UTF-8');
    return $outFile;
  }

  /**
   * Function getSQLByFileName
   *
   * * @param $filename
   * @return string
   */
  function getSQLByFileName($filename)
  {
    global $dbn, $botmaster_name, $dbh, $dbConn;
    $curPath = dirname(__FILE__);
    chdir($curPath);
    $dbFilename = $filename;
    $filename = str_ireplace('.aiml', '.sql', $filename);
    $categoryTemplate =
    "    ([id],[bot_id],'[aiml]','[pattern]','[thatpattern]','[template]','[topic]','[filename]'),";
    $phpVer = phpversion();
    $cleanedFilename = $dbFilename;
    $topicArray = array();
    $sql = "select * from aiml where filename like '$cleanedFilename' order by id asc;";
    $fileContent = file_get_contents('SQL_Header.dat');
    $fileContent = str_replace('[botmaster_name]', $botmaster_name, $fileContent);
    $fileContent = str_replace('[host]', $dbh, $fileContent);
    $fileContent = str_replace('[dbn]', $dbn, $fileContent);
    $fileContent = str_replace('[sql]', $sql, $fileContent);
    $fileContent = str_replace('[phpVer]', $phpVer, $fileContent);
    $curDate = date('m-d-Y h:j:s A', time());
    $fileContent = str_replace('[curDate]', $curDate, $fileContent);
    $fileContent = str_replace('[fileName]', $cleanedFilename, $fileContent);
    $result = db_fetchAll($sql, null, __FILE__, __FUNCTION__, __LINE__);
    foreach ($result as $row)
    {
      $aiml = str_replace("\r\n", '', $row['aiml']);
      $aiml = str_replace("\n", '', $aiml);
      $template = str_replace("\r\n", '', $row['template']);
      $template = str_replace("\n", '', $template);
      //$newLine = str_replace('[id]', $row['id'], $categoryTemplate);
      $newLine = str_replace('[id]', 'null', $categoryTemplate);
      $newLine = str_replace('[bot_id]', $row['bot_id'], $newLine);
      $newLine = str_replace('[aiml]', $aiml, $newLine);
      $newLine = str_replace('[pattern]', $row['pattern'], $newLine);
      $newLine = str_replace('[thatpattern]', $row['thatpattern'], $newLine);
      $newLine = str_replace('[template]', $template, $newLine);
      $newLine = str_replace('[topic]', $row['topic'], $newLine);
      $newLine = str_replace('[filename]', $row['filename'], $newLine);
      $fileContent .= "$newLine\r\n";
    }
    $fileContent = trim($fileContent, ",\r\n");
    $fileContent .= "\n";
    return $fileContent;
  }

  /**
   * Function getCheckboxes
   *
   *
   * @return string
   */
  function getCheckboxes()
  {
    global $bot_id, $bot_name, $msg;
    $sql = "SELECT DISTINCT filename FROM `aiml` where `bot_id` = $bot_id order by `filename`;";
    $result = db_fetchAll($sql, null, __FILE__, __FUNCTION__, __LINE__);
    if (count($result) == 0)
    {
      $msg = "The chatbot '$bot_name' has no AIML categories to download. Please select another bot.";
      return false;
    }
    $out = "";
    $checkboxTemplate = <<<endRow
                    <div class="cbCell">
                      <input id="[file_name_id]" name="filenames[]" type="checkbox" class="cbFiles" value="[file_name]">
                      <label for="[file_name_id]">&nbsp;[file_name]</label>
                    </div>
endRow;
    $rowCount = 0;
    foreach ($result as $row)
    {
      if (empty ($row['filename'])){
        $row['filename'] = 'unnamed_AIML.aiml';
      }
      $file_name = $row['filename'];
      $file_name_id = str_replace('.', '_', $file_name);
      $curCheckbox = str_replace('[file_name]', $file_name, $checkboxTemplate);
      $curCheckbox = str_replace('[file_name_id]', $file_name_id, $curCheckbox);
      $out .= $curCheckbox;
      $rowCount++;
    }
    return rtrim($out);
  }

  /**
   * Function renderMain
   *
   *
   * @return string
   */
  function renderMain()
  {
    global $msg, $template;
    $file_checkboxes = getCheckboxes();
    if ($file_checkboxes === false) return "<div class=\"bold red center\">$msg</div><br>\n";
    $content = $template->getSection('DownloadForm');
    $content = str_replace('[file_checkboxes]', $file_checkboxes, $content);
    return $content;
  }

