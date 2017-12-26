<?php
  /***************************************
    * http://www.program-o.com
    * PROGRAM O
    * Version: 2.5.3
    * FILE: demochat.php
    * AUTHOR: Elizabeth Perreau and Dave Morton
    * DATE: 12-12-2014
    * DETAILS: Displays a demo chat page for the currently selected chatbot
    ***************************************/
  $bot_id = ($bot_id == 'new') ? 0 : $bot_id;
  $upperScripts  = '';
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
  $pageTitle     = 'My-Program O - Chat Demo';
  $mainContent   = 'This will eventually be the page for the chat demo.';
  $mainContent   = showChatFrame();
  $mainTitle     = 'Chat Demo';

  /**
   * Function showChatFrame
   *
   *
   * @return mixed|string
   */
  function showChatFrame() {
    global $template, $bot_name, $bot_id, $dbConn;
    $qs = '?bot_id=' . $bot_id;
    $sql = "select `format` from `bots` where `bot_id` = $bot_id limit 1;";
    $row = db_fetch($sql, null, __FILE__, __FUNCTION__, __LINE__);
    $format = strtolower($row['format']);
    switch ($format) {
      case "html":
        $url = '../gui/plain/';
        break;
      case "json":
        $url = '../gui/jquery/';
        break;
      case "xml":
        $url = '../gui/xml/';
        break;
      default:
        $url = '../gui/plain/';
    }
    $url .= $qs;
    $out = $template->getSection('ChatDemo');
    $out = str_replace('[pageSource]', $url, $out);
    $out = str_replace('[format]', strtoupper($format), $out);
    return $out;
  }
?>
