<?php

  /***************************************
  * http://www.program-o.com
  * PROGRAM O
  * Version: 2.5.3
  * FILE: chatbot/core/conversation/display_conversation.php
  * AUTHOR: Elizabeth Perreau and Dave Morton
  * DATE: MAY 17TH 2014
  * DETAILS: this file contains the functions to handle the return of the conversation lines back to the user
  ***************************************/
  /**
  * function get_conversation_to_display()
  * This function gets the conversation from the db to display/return to the user
  * @link http://blog.program-o.com/?p=1223
  * @param  array $convoArr - the conversation array
  * @return array $orderedRows - a list of conversation line
  **/
  function get_conversation_to_display($convoArr)
  {
    global $dbConn, $dbn, $bot_name, $unknown_user;
    $user_id = $convoArr['conversation']['user_id'];
    $bot_id = $convoArr['conversation']['bot_id'];
    $user_name = $convoArr['conversation']['user_name'];
    $user_name = (!empty ($user_name)) ? $user_name : $unknown_user;
    $convoArr['conversation']['bot_name'] = $bot_name;
    if (empty ($bot_name))
    {
      $sql = "select `bot_name` from `bots` where `bot_id` = $bot_id limit 1;";
      $row = db_fetch($sql, null, __FILE__, __FUNCTION__, __LINE__);
      $bot_name = $row['bot_name'];
    }
    if ($convoArr['conversation']['conversation_lines'] != 0)
    {
      $limit = " LIMIT " . $convoArr['conversation']['conversation_lines'];
    }
    else
    {
      $limit = "";
    }
    
    $sql = "SELECT * FROM `$dbn`.`conversation_log`
        WHERE
        `user_id` = '" . $convoArr['conversation']['user_id'] . "'
        AND `bot_id` = '" . $convoArr['conversation']['bot_id'] . "'
        AND `convo_id` = '" . $convoArr['conversation']['convo_id'] . "'
        ORDER BY id DESC $limit ";
    runDebug(__FILE__, __FUNCTION__, __LINE__, "get_conversation SQL: $sql", 3);
    $result = db_fetchAll($sql, null, __FILE__, __FUNCTION__, __LINE__);
    if (count($result) > 0)
    {
      foreach ($result as $row)
      {
        $allrows[] = $row;
      }
      $orderedRows = array_reverse($allrows, false);
    }
    else
    {
      $orderedRows = array('id' => NULL, 'input' => "", 'response' => "", 'user_id' => $convoArr['conversation']['user_id'], 'bot_id' => $convoArr['conversation']['bot_id'], 'timestamp' => "");
    }
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Found '" . count($result) . "' lines of conversation", 2);
    
    return $orderedRows;
  }

  /**
  * function get_conversation()
  * This function gets the conversation format
  * @link http://blog.program-o.com/?p=1225
  * @param  array $convoArr - the conversation array
  * @return array $convoArr
  **/
  function get_conversation($convoArr)
  {
    $conversation = get_conversation_to_display($convoArr);
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Processing conversation as " . $convoArr['conversation']['format'], 4);
    switch ($convoArr['conversation']['format'])
    {
      case "html" :
        $convoArr = get_html($convoArr, $conversation);
        break;
      case "json" :
        $convoArr = get_json($convoArr, $conversation);
        break;
      case "xml" :
        $convoArr = get_xml($convoArr, $conversation);
        break;
    }
    return $convoArr;
  }

  /**
  * function get_html()
  * This function formats the response as html
  * @link http://blog.program-o.com/?p=1227
  * @param  array $convoArr - the conversation array
  * @param  array $conversation - the conversation lines to format
  * @return array $convoArr
  **/
  function get_html($convoArr, $conversation)
  {
    $show = "";
    $user_name = $convoArr['conversation']['user_name'];
    $bot_name = $convoArr['conversation']['bot_name'];
    foreach ($conversation as $index => $conversation_subarray)
    {
      $show .= "<div class=\"usersay\">$user_name: " . stripslashes($conversation_subarray['input']) . "</div>";
      $show .= "<div class=\"botsay\">$bot_name: " . stripslashes($conversation_subarray['response']) . "</div>";
    }
    $convoArr['send_to_user'] = $show;
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Returning HTML", 4);
    return $convoArr;
  }

  /**
  * function get_json()
  * This function formats the response as json
  * @link http://blog.program-o.com/?p=1229
  * @param  array $convoArr - the conversation array
  * @param  array $conversation - the conversation lines to format
  * @return array $convoArr
  **/
  function get_json($convoArr, $conversation)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Outputting response as JSON', 2);
    $show_json = array();
    $i = 0;
    foreach ($conversation as $index => $conversation_subarray)
    {
      $show_json['convo_id'] = $convoArr['conversation']['convo_id'];
      $show_json['usersay'] = stripslashes($conversation_subarray['input']);
      $show_json['botsay'] = stripslashes($conversation_subarray['response']);
      $i++;
    }
    $convoArr['send_to_user'] = json_encode($show_json);
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Returning JSON string: " . $convoArr['send_to_user'], 4);
    return $convoArr;
  }

  /**
  * function get_xml()
  * This function formats the response as xml
  * @link http://blog.program-o.com/?p=1238
  * @param  array $convoArr - the conversation array
  * @param  array $conversation - the conversation lines to format
  * @return array $convoArr
  **/
  function get_xml($convoArr, $conversation)
  {
    $addTags = array('bot_id','bot_name','user_id','user_name');
    $program_o = new SimpleXMLElement('<program_o/>');
    $program_o->addChild('version',VERSION);
    $program_o->addChild('status');
    $status = $program_o->status;
    $status->addChild('success', true);
    foreach ($addTags as $tag_name)
    {
      $tmpVal = $convoArr['conversation'][$tag_name];
      $program_o->addChild($tag_name, $tmpVal);
    }
    $program_o->addChild('chat');
    $chat = $program_o->chat;
    foreach ($conversation as $index => $conversation_subarray)
    {
      if (empty($conversation_subarray)) continue;
      $line = $chat->addChild('line');
      $line->addChild('input', $conversation_subarray['input']);
      $line->addChild('response', $conversation_subarray['response']);
    }
    $responseXML = $program_o->asXML();
    libxml_use_internal_errors(true);
    $dom = new DOMDocument('1.0');
    $dom->preserveWhiteSpace = true;
    $dom->formatOutput = true;
    $dom->loadXML($responseXML);
    $send_to_user = $dom->saveXML();
    $convoArr['send_to_user'] = $send_to_user;
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Returning XML", 4);
    return $convoArr;
  }

  /**
   * function display_conversation()
   * Displays the output of the conversation if the current format is XML or JSON and updated referenced $display if html
   *
   * @link  http://blog.program-o.com/?p=1233
   * @param (array) $convoArr
   * @return void (void) [return value]
   */
  function display_conversation($convoArr)
  {
    $display = $convoArr['send_to_user'];
    $format =(isset($convoArr['conversation']['format'])) ? strtolower(trim($convoArr['conversation']['format'])) : 'html';
    switch ($format) {
      case 'html' :
        $display = str_ireplace('<![CDATA[', '', $display);
        $display = str_replace(']]>', '', $display);
        echo trim($display);
        break;
      case 'xml' :
        header("Content-type: text/xml; charset=utf-8");
        echo trim($display);
        break;
      case 'json' :
        header("Content-type: text/plain; charset=utf-8");
        echo trim($display);
        break;
      default :
    }
  }