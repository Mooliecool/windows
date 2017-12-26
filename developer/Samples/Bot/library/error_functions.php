<?php

  /***************************************
  * http://www.program-o.com
  * PROGRAM O
  * Version: 2.5.3
  * FILE: library/error_functions.php
  * AUTHOR: Elizabeth Perreau and Dave Morton
  * DATE: MAY 17TH 2014
  * DETAILS: common library of debugging functions
  ***************************************/

  /**
  * function myErrorHandler()
  * Process PHP errors
  * @param string $errno - the severity of the error
  * @param  string $errstr - the file the error came from
  * @param  string $errfile - the file the error came from
  * @param  string $errline - the line of code
  **/
  function myErrorHandler($errno, $errstr, $errfile, $errline)
  {
    switch ($errno)
    {
      case E_NOTICE :
      case E_USER_NOTICE :
        $errors = 'Notice';
        break;
        case E_DEPRECATED :
      case E_USER_DEPRECATED :
        $errors = 'Deprecated';
        break;
      case E_WARNING :
      case E_USER_WARNING :
        $errors = 'Warning';
        break;
      case E_ERROR :
      case E_USER_ERROR :
        $errors = 'Fatal Error';
        break;
      default :
        $errors = 'Unknown';
        break;
    }
    $info = "PHP ERROR [$errors] -$errstr in $errfile on Line $errline";
    //a little hack to hide 'deprecated' errors of which there may be ~MORE~ than a few, depending on PHP version
    if (!stripos($errstr, 'deprecated'))
    {
      runDebug($errfile, '', $errline, $info, 1);
    }
    $current_DateTime = date('m/d/Y H:i:s');
    save_file(_LOG_PATH_ . 'error.log', "$current_DateTime - $info\r\n", true);
  }

  /**
   * function runDebug()
   * Building to a global debug array
   *
   * @param  string $fileName     - the file the error came from
   * @param  string $functionName - the function that triggered the error
   * @param  string $line         - the line of code
   * @param  string $info         - the message to display
   * @param int     $level
   */
  function runDebug($fileName, $functionName, $line, $info, $level = 0)
  {
    global $debugArr, $srai_iterations, $quickdebug, $writetotemp, $convoArr, $last_timestamp, $debug_level;
    $debug_level = (isset($convoArr['conversation']['debug_level'])) ? $convoArr['conversation']['debug_level'] : $debug_level;
    if (empty ($functionName)) $functionName = 'Called outside of function';
    //only log the debug info if the info level is equal to or less than the chosen level
    if (($level <= $debug_level))// && ($level != 0) && ($debug_level != 0)
    {
      // Set elapsed time from last debug call and update last timestamp
      if ($quickdebug == 1)
      {
        outputDebug($fileName, $functionName, $line, $info);
      }
      $current_timestamp = microtime(true);
      $usecN = round($current_timestamp - floor($current_timestamp), 5);
      //Lose the decimal point and everything to the left of it
      $usecD = str_replace('0.', '', $usecN);
      $et = $current_timestamp - $last_timestamp;
      $elapsed_time = number_format($et * 1000, 3);
      $last_timestamp = $current_timestamp;
      //build timestamp index for the debug array
      $index = date('d-m-Y H:i:s.') . $usecD . "[$level][$debug_level] - Elapsed time: $elapsed_time milliseconds";

      //If there's already an index in the debug array with the same value, just add a space, to make a new, unique index that is visually identical.
      while (array_key_exists($index, $debugArr))
      {
        $index .= ' ';
      }
      //mem_tracer($fileName, $functionName, $line); # only uncomment this to trace memory leaks!
      //add to array
      $debugArr[$index]['fileName'] = basename($fileName);
      $debugArr[$index]['functionName'] = $functionName;
      $debugArr[$index]['line'] = $line;
      $debugArr[$index]['info'] = $info;
      if ($srai_iterations < 1)
      {
        $sr_it = 0;
      }
      else
      {
        $sr_it = $srai_iterations;
      }
      $debugArr[$index]['srai_iteration'] = $sr_it;
      //if we are logging to file then build a log file. This will be overwriten if the program completes
      if ($writetotemp == 1)
      {
        writefile_debug(implode("\n",$debugArr), $convoArr);
      }
    }
  }

  /**
  * function handleDebug()
  * Handle the debug array at the end of the process
  * @param  array $convoArr - conversation arrau
  * @return array $convoArr;
  **/
  function handleDebug($convoArr, $et)
  {
    global $debugArr, $debug_level, $debug_mode;
    $debug_level = (isset($convoArr['conversation']['debug_level'])) ? $convoArr['conversation']['debug_level'] : $debug_level;
    $debug_mode = (isset($convoArr['conversation']['debugmode'])) ? $convoArr['conversation']['debugmode'] : $debug_mode;
    $convoArr['debug'] = $debugArr;
    $log = '';
    foreach ($debugArr as $time => $subArray)
    {
      $log .= $time . '[NEWLINE]';
      foreach ($subArray as $index => $value)
      {
        if (($index == 'fileName') || ($index == 'functionName') || ($index == 'line'))
        {
          $log .= "[$value]";
        }
        elseif ($index == 'info')
        {
          $log .= "[NEWLINE]$value [NEWLINE]-----------------------[NEWLINE]";
        }
      }
    }
    $log = rtrim($log);
    if ($debug_level == 4)
    {
    //show the full array
      $showArr = $convoArr;
      unset ($showArr['debug']);
    }
    else
    {
    //show a reduced array
      $showArr = reduceConvoArr($convoArr);
    }
    if ($debug_level != 0)
    {
      //$log .= '[NEWLINE]-----------------------[NEWLINE]';
      $log .= "Debug Level: $debug_level";
      $log .= '[NEWLINE]-----------------------[NEWLINE]';
      $log .= "Debug Mode: $debug_mode";
      $log .= '[NEWLINE]-----------------------[NEWLINE]';
      $log .= 'CONVERSATION ARRAY';
      $log .= '[NEWLINE]-----------------------[NEWLINE]';
      $log .= str_replace("\n", PHP_EOL, print_r($showArr, true));
      $log .= '[NEWLINE]-----------------------[NEWLINE]';
      $log .= "Total execution time: $et Milliseconds. Goodbye.";
    }
    switch ($debug_mode)
    {
      case 0 :
        //show in source code
        $log = str_replace('[NEWLINE]', "\n", $log);
        display_on_page(0, $log);
        break;
      case 1 :
        //write to log file
        $log = str_replace('[NEWLINE]', PHP_EOL, $log);
        writefile_debug($log, $convoArr);
        break;
      case 2 :
        //show in webpage
        $log = str_replace('[NEWLINE]', '<br/>', $log);
        display_on_page(1, $log);
        break;
      case 3 :
        //email to user
        $log = str_replace('[NEWLINE]', "\r\n", $log);
        email_debug($convoArr['conversation']['debugemail'], $log);
        break;
    }
    return $convoArr;
  }

  /** reduceConvoArr()
   *  A small function to create a smaller convoArr just for debuggin!
   *
   * @param array $convoArr - the big array to be reduced
   * @return array
   */
  function reduceConvoArr($convoArr)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Reducing the conversation array.', 0);
    $showConvoArr = array();
    $showConvoArr['conversation'] = (isset($convoArr['conversation'])) ? $convoArr['conversation'] : '';
    $showConvoArr['aiml'] = (isset($convoArr['aiml'])) ? $convoArr['aiml'] : '';
    $showConvoArr['topic'][1] = (isset($convoArr['topic'][1])) ? $convoArr['topic'][1] : '';
    $showConvoArr['that'][1] = (isset($convoArr['that'][1])) ? $convoArr['that'][1] : '';
    if (isset($convoArr['star']))
    {
      foreach ($convoArr['star'] as $index => $star)
      {
        if (!empty ($star))
          $showConvoArr['star'][$index] = $star;
      }
    }
    $showConvoArr['input'][1] = (isset($convoArr['input'][1])) ? $convoArr['input'][1] : '';
    $showConvoArr['stack']['top'] = (isset($convoArr['stack']['top'])) ? $convoArr['stack']['top'] : '';
    $showConvoArr['stack']['last'] = (isset($convoArr['stack']['last'])) ? $convoArr['stack']['last'] : '';
    $showConvoArr['client_properties'] = (isset($convoArr['client_properties'])) ? $convoArr['client_properties'] : '';
    $showConvoArr['aiml']['user_raw'] = (isset($convoArr['aiml']['user_raw'])) ? $convoArr['aiml']['user_raw'] : '';
    $showConvoArr['aiml']['lookingfor'] = (isset($convoArr['aiml']['lookingfor'])) ? $convoArr['aiml']['lookingfor'] : '';
    $showConvoArr['aiml']['pattern'] = (isset($convoArr['aiml']['pattern'])) ? $convoArr['aiml']['pattern'] : '';
    $showConvoArr['aiml']['thatpattern'] = (isset($convoArr['aiml']['thatpattern'])) ? $convoArr['aiml']['thatpattern'] : '';
    $showConvoArr['aiml']['topic'] = (isset($convoArr['aiml']['topic'])) ? $convoArr['aiml']['topic'] : '';
    $showConvoArr['aiml']['score'] = (isset($convoArr['aiml']['score'])) ? $convoArr['aiml']['score'] : '';
    $showConvoArr['aiml']['aiml_id'] = (isset($convoArr['aiml']['aiml_id'])) ? $convoArr['aiml']['aiml_id'] : '';
    $showConvoArr['aiml']['parsed_template'] = (isset($convoArr['aiml']['parsed_template'])) ? $convoArr['aiml']['parsed_template'] : '';
    $showConvoArr['user_say'][1] = (isset($convoArr['aiml']['parsed_template'])) ? $convoArr['user_say'][1] : '';
    $showConvoArr['that_raw'][1] = (isset($convoArr['that_raw'][1])) ? $convoArr['that_raw'][1] : '';
    $showConvoArr['parsed_template'][1] = (isset($convoArr['parsed_template'][1])) ? $convoArr['parsed_template'][1] : '';
    return $showConvoArr;
  }

  /**
   * function writefile_debug()
   * Handles the debug when written to a file
   *
   * @param  string $log - the data to write
   * @internal param string $myFile - the name of the file which is also the convo id
   */
  function writefile_debug($log)
  {
    global $new_convo_id, $old_convo_id;
    $session_id = ($new_convo_id === false) ? session_id() : $new_convo_id;
    $myFile = _DEBUG_PATH_ . $session_id . '.txt';
    if (!IS_WINDOWS)
    {
      $log = str_replace("\n", "\r\n", $log);
      $log = str_replace("\r\r", "\r", $log);
    }
    file_put_contents($myFile, $log);
  }

  /**
  * function display_on_page()
  * Handles the debug when it is displayed on the webpage either in the source or on the page
  * @param  int $show_on_page - 0=show in source 1=output to user
  * @param  string $log - the data to show
  **/
  function display_on_page($show_on_page, $log)
  {
    if ($show_on_page == 0)
    {
      echo '<!--<pre>';
      print_r($log);
      echo '</pre>-->';
    }
    else
    {
      echo '<pre>';
      print_r($log);
      echo '</pre>';
    }
  }

  /**
  * function email_debug()
  * Handles the debug when it is emailed to the botmaster
  * @param  string $email - email address
  * @param  string $log - the data to send
  **/
  function email_debug($email, $log)
  {
    $to = $email;
    $subject = 'Debug Data';
    $message = $log;
    $headers = 'From: ' . $email . "\r\nReply-To: $email \r\n" . 'X-Mailer: PHP/' . phpversion();
    mail($to, $subject, $message, $headers);
  }

  /**
  * function outputDebug()
  * Used in the install/upgrade files will display it straightaway
  * @param  string $fileName - the file the error came from
  * @param  string $functionName - the function that triggered the error
  * @param  string $line - the line of code
  * @param  string $info - the message to display
  **/
  function outputDebug($fileName, $functionName, $line, $info)
  {
    global $srai_iterations;
    list($usec, $sec) = explode(' ', microtime());
    //build timestamp index for the debug array
    $string = ((float) $usec + (float) $sec);
    $string2 = explode('.', $string);
    $index = date('d-m-Y H:i:s', $string2[0]) . ':' . $string2[1];
    if ($srai_iterations < 1)
    {
      $sr_it = 0;
    }
    else
    {
      $sr_it = $srai_iterations;
    }
    //add to array
    print '<br/>----------------------------------------------------';
    print '<br/>' . $index . ': ' . $fileName;
    print '<br/>' . $index . ': ' . $functionName;
    print '<br/>' . $index . ': ' . $line;
    print '<br/>' . $index . ': ' . $info;
    print '<br/>' . $index . ': srai:' . $sr_it;
    print '<br/>----------------------------------------------------';
  }

  /**
   * Function save_file
   *
   * * @param $file
   * @param      $content
   * @param bool $append
   * @return int
   * @throws Exception
   */
  function save_file($file, $content, $append = false)
  {
    if (function_exists('file_put_contents'))
    {
      ($append) ? $x = file_put_contents($file, $content, FILE_APPEND) : $x = file_put_contents($file, $content);
    }
    else
    {
      $fileMode = ($append === true) ? 'a' : 'w';
      if (($fh = fopen($file, $fileMode)) === false) throw new Exception('Can\'t open the file!');
      $cLen = strlen($content);

      fwrite($fh, $content, $cLen);
      fclose($fh);
    }
    return 1;
  }

  /**
   * Function mem_tracer
   *
   * * @param $file
   * @param $function
   * @param $line
   * @return void
   */
  function mem_tracer($file, $function, $line)
  {
    $file = str_replace(_BOTCORE_PATH_ . 'aiml' . DIRECTORY_SEPARATOR, '', $file);
    $mem_state = number_format(memory_get_usage(true));
    $trace_file = _DEBUG_PATH_ . session_id() . '.mem_trace.txt';
    if (!file_exists($trace_file)) /** @noinspection PhpUndefinedConstantInspection */
      save_file($trace_file, 'PHP memory limit = ' . ini_get('memory_limit') . "\nBase folder = " . _BASE_PATH_ . "\n");
    $append = true;
    $content = "$file.$function.$line: Memory used = $mem_state bytes\r\n";
    save_file($trace_file, $content, $append);
  }

?>
