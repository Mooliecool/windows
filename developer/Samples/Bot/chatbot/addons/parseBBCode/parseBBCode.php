<?php
// Program O Addon - Parse BB_Code
  /**
   * Function parseEmotes
   *
   * * @param $msg
   * @return mixed
   */
  function parseEmotes($msg) {
    $emotesFile = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'emotes.dat';
    $emotes_URL = _BASE_URL_ . 'chatbot/addons/parseBBCode/images/';                                           // Define the location of the emote image
    $smilieArray = file($emotesFile);
    rsort($smilieArray);
    foreach ($smilieArray as $line) {                                   // Iterate through the list
      $line = rtrim($line);                                             // Trim off excess line feeds and whitespace
      list($symbol, $fname, $width, $height, $alt) = explode(', ',$line); // Seperate the various values from the list
      $alt = rtrim($alt);                                               // Just to make sure the line is clean
      //$alt = str_replace('<', '&lt;', $alt);                            // There has to be an oddball, doesn't there?
      $tmpAlt = substr($alt,0,1) . '&#' . ord(substr($alt,1,1)) . substr($alt,2);
      $ds = DIRECTORY_SEPARATOR;
      if (strpos($msg,$symbol) !== false) {                             // If it finds an emoticon symbol, then process it
        $emot = <<<endLine
<img src="$emotes_URL$fname" width="$width" height="$height" alt="$tmpAlt" title="$tmpAlt" />

endLine;
        //$x = saveFile('txt/image.txt', $emot, true);
        $msg = str_replace($symbol, $emot, $msg);                       // swap out the found symbol for it's emote image
      }
    }
    return $msg;                                                        // Send back the processed image
  }
// end function parseEmotes

  /**
   * Function parseURLs
   *
   * * @param $msg
   * @return mixed
   */
  function parseURLs($msg) {
    if (strpos($msg, '[url') === false) return $msg;
    $replace = array('<a href="$1" target="_blank">$2</a>', '<a href="$1" target="_blank">$1</a>');
    $rules = array(
      '~\[url=(.*?)\](.*?)\[\/url\]~i',
      '~\[url\](.*?)\[\/url\]~i'
    );
    $msg = preg_replace($rules, $replace, $msg);
    return $msg;
  }

  /**
   * Function parseLinks
   *
   * * @param $msg
   * @return mixed
   */
  function parseLinks($msg) {
    if (strpos($msg, '[link') === false) return $msg;
    $replace = array('<a href="$1" target="_blank">$2</a>', '<a href="$1" target="_blank">$1</a>');
    $rules = array(
      '~\[link=(.*?)\](.*?)\[\/link\]~i',
      '~\[link\](.*?)\[\/link\]~i'
    );
    $msg = preg_replace($rules, $replace, $msg);
    return $msg;
  }

  /**
   * Function parseImages
   *
   * * @param $msg
   * @return mixed
   */
  function parseImages($msg) {
    $oldMsg = $msg;
    if (strpos($msg, '[img]') === false) return $msg;
    if (!isset($_SESSION['imageFilter'])) $_SESSION['imageFilter'] = 'false';
    $replace = '<br><img src="$1" width="150" onclick="window.open(this.src)" class="userImg"/>';
    $rules = '~\[img](.*?)\[\/img\]~i';
    $msg = preg_replace($rules, $replace, $msg);
    return $msg;
  }

  /**
   * Function parseColors
   *
   * * @param $msg
   * @return mixed
   */
  function parseColors($msg) {
    $test = $msg;
    if (preg_match('~\[color=(.*?)\]~', $test, $matches))
    {
      $openTag = $matches[0];
      $tagColor = $matches[1];
      $closeTag = '[/color]';
      $msg = str_replace($openTag, "<span style='color:$tagColor'>", $msg);
      $msg = str_ireplace($closeTag, '</span>', $msg);
    }
    if (preg_match('~\[(\#(?:[0-9]|[A-F]|[a-f]){6})\]~', $test, $matches))
    {
      $openTag = $matches[0];
      $closeTag = str_replace('[#', '[/#', $openTag);
      $tagColor = $matches[1];
      $msg = str_replace($openTag, "<span style='color:$tagColor'>", $msg);
      $msg = str_replace($closeTag, '</span>', $msg);
    }
    if (preg_match('~\[(\#(?:[0-9]|[A-F]|[a-f]){3})\]~', $test, $matches))
    {
      #file_put_contents(_ADDONS_PATH_ . 'spell_checker/matches.txt', print_r($matches, true), FILE_APPEND);
      $openTag = $matches[0];
      $tagColor = $matches[1];
      $tagColor = str_replace(']', '', $tagColor);
      $msg = str_replace($openTag, "<span style='color:$tagColor'>", $msg);
      $msg = str_replace($closeTag, '</span>', $msg);
    }
    $colorFile = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'colors.dat';
    $colorList = file($colorFile);
      $closeSpan = '</span>';
    foreach($colorList as $colorRow) {
      list($color, $css) = explode(', ', $colorRow);
      $openSymbol = "[$color]";
      $closeSymbol = "[/$color]";
      $openSpan = "<span style='color:$css'>";
      $msg = str_ireplace($openSymbol, $openSpan, $msg);
      $msg = str_ireplace($closeSymbol, $closeSpan, $msg);
    }
    return $msg;
  }

  /**
   * Function parseFormatting
   *
   * * @param $msg
   * @return mixed
   */
  function parseFormatting ($msg) {
    $search  = array('[b]', '[/b]', '[u]', '[/u]', '[i]', '[/i]', '[s]', '[/s]');
    $replace = array('<b>', '</b>', '<span class=\'underline\'>', '</span>', '<i>', '</i>', '<s>', '</s>');
    $msg = str_ireplace($search, $replace, $msg);
    return $msg;
  }

  /**
   * Function parseInput
   *
   * * @param $msg
   * @return mixed
   */
  function parseInput ($msg) {
    runDebug(__FILE__, __FUNCTION__, __LINE__,"Pre-parsing input. Setting Timestamp. msg = |$msg|", 4);
    $smilieArray = file(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'inputEmotes.dat');
    rsort($smilieArray);
    $out = str_replace($smilieArray, 'emoteshown', $msg);
                                                                      // Edit the input to deal with multiple punctuation marks
    $emSearch = '/^\!+/';                                             // Exclamation mark search string
    $out = preg_replace($emSearch, 'emonly', $out);                   //
    $qmSearch = '/^\?+/';                                             // Question mark search string
    $out = preg_replace($qmSearch, 'qmonly', $out);                   //
    $periodSearch = '/^\.+/';                                         // Period search string
    $out = preg_replace($periodSearch, 'periodonly', $out);           //
    runDebug(__FILE__, __FUNCTION__, __LINE__,"msg now = |$out|", 4);
    return $out;                                                      // Send back the processed image
  }

  /**
   * Function checkForParsing
   *
   * * @param $convoArr
   * @return mixed
   */
  function checkForParsing($convoArr) {
    $message = $convoArr['send_to_user'];
    $oldMsg = $message;
    $fAr = array('~\[b\]~', '~\[i\]~', '~\[u\]~', '~\[s\]~');
    $message = parseEmotes($message);
    $message = (strpos($message, '[url') !== false) ? parseURLs($message) : $message;
    $message = (strpos($message, '[link') !== false) ? parseLinks($message) : $message;
    $message = (strpos($message, '[img]') !== false) ? parseImages($message) : $message;
    $message = parseColors($message);
    $message = parseFormatting($message);
    if ($message != $oldMsg) $convoArr['send_to_user'] = $message;
    return $convoArr;
  }
//end function checkForParsing


?>
