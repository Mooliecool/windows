<?php

  /***************************************
    * http://www.program-o.com
    * PROGRAM O
    * Version: 2.5.3
    * FILE: parse_aiml_as_xml.php
    * AUTHOR: Elizabeth Perreau and Dave Morton
    * DATE: FEB 01 2016
    * DETAILS: Handles the parsing of AIML code as XML
    ***************************************/

  /**
   * This function starts the process of recursively parsing the AIML template as XML, converting it to text
   *
   * @param  array $convoArr - the existing conversation array
   * @return array $convoArr
   **/
  function parse_aiml_as_XML($convoArr)
  {
    global $botsay, $error_response;
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Parsing the AIML template as XML", 2);
    $template = add_text_tags($convoArr['aiml']['template']);
    try
    {
      $aimlTemplate = new SimpleXMLElement($template, LIBXML_NOCDATA);
    }
    catch (exception $e)
    {
      trigger_error("There was a problem parsing the template as XML. Template value:\n$template", E_USER_WARNING);
      $aimlTemplate = new SimpleXMLElement("<text>$error_response</text>", LIBXML_NOCDATA);
    }
    $responseArray = parseTemplateRecursive($convoArr, $aimlTemplate);
    $botsay = trim(implode_recursive(' ', $responseArray, __FILE__, __FUNCTION__, __LINE__));
    $botsay = str_replace(' .', '.', $botsay);
    $botsay = str_replace('  ', ' ', $botsay);
    $botsay = str_replace(' ?', '?', $botsay);
    $botsay = str_replace(' ,', ',', $botsay);
    $botsay = str_replace(' s ', 's ', $botsay);
    $convoArr['aiml']['parsed_template'] = $botsay;
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Completed parsing the template. The bot will say: $botsay", 4);
    return $convoArr;
  }

  /**
   * Wraps mixed content XML with <text></text> tags, allowing full use of PHP's SimpleXML functions
   *
   * @param $input
   * @return string
   */
  function add_text_tags($input)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Adding some TEXT tags into the template, just because I can...', 2);
    /*
      Since we're going to parse the template's contents as XML, we need to prepare it first
      by transforming it into valid XML

      First, wrap the template in TEMPLATE tags, to give the XML a "root" element:
    */
    $template = "<template>$input</template>";
    /*
      SimpleXML can't deal with "mixed" content, so any "loose" text is wrapped in a <text> tag.
      The process will sometimes add extra <text> tags, so part of the process below deals with that.
    */
    $textTagsToRemove = array('<text></text>' => '', '<text> </text>' => '', '<say>' => '', '</say>' => '',
    );
    // Remove any spaces immediately between the XML tags
    $template = preg_replace('~>\s*?<~', '><', $template);
    $textTagSearch = array_keys($textTagsToRemove);
    $textTagReplace = array_values($textTagsToRemove);
    // Remove CRLF
    $template = str_replace("\r\n", '', $template);
    // Remove newline
    $template = str_replace("\n", '', $template);
    // Throw <text> tags around everything that lies between existing tags
    $template = preg_replace('~>(.*?)<~', "><text>$1</text><", $template);
    // Remove any "extra" <text> tags that may have been generated
    $template = str_replace($textTagSearch, $textTagReplace, $template);
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Returning template:\n$template", 4);
    return $template;
  }

  /**
   * Implodes a nested array into a single string recursively
   *
   * @param string $glue
   * @param array  $input
   * @param string $file
   * @param string $function
   * @param string $line
   * @return string
   */
  function implode_recursive($glue, $input, $file = 'unknown', $function = 'unknown', $line = 'unknown')
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Imploding an array into a string. (recursively, if necessary)', 2);
    #runDebug(__FILE__, __FUNCTION__, __LINE__, "This function was called from $file, function $function at line $line.", 4);
    if(empty($input)) return '';
    if (!is_array($input) and !is_string($input))
    {
      $varType = gettype($input);
      trigger_error("Input not array! Input is of type $varType. Error originated in $file, function $function, line $line. Input = " . print_r($input, true));
      return $input;
    }
    elseif (is_string($input)) return $input;
    runDebug(__FILE__, __FUNCTION__, __LINE__,'The variable $input is of type ' . gettype($input), 4);
    foreach ($input as $index => $element)
    {
      if (empty ($element))
        continue;
      if (is_array($element))
      {
        $input[$index] = implode_recursive($glue, $element, __FILE__, __FUNCTION__, __LINE__);
      }
    }
    switch (gettype($input))
    {
      case 'array':
      $out = implode($glue, $input);
      break;
      case 'string':
      $out = $input;
      break;
      default:
      runDebug(__FILE__, __FUNCTION__, __LINE__,'input type: ' . gettype($input), 4);
      $out = (string) $input;
    }
    $out = str_replace('  ', ' ', $out);
    if ($function != 'implode_recursive') runDebug(__FILE__, __FUNCTION__, __LINE__, "Imploding complete. Returning '$out'", 4);
    return ltrim($out);
  }

  /**
   * Parses a SimpleXMLelement object into a string
   *
   * @param array            $convoArr
   * @param SimpleXMLElement $element
   * @param int              $level
   * @return string
   */
  function parseTemplateRecursive(&$convoArr, SimpleXMLElement $element, $level = 0)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Recursively parsing the AIML template.', 2);
    $HTML_tags = array('a', 'abbr', 'acronym', 'address', 'applet', 'area', 'b', 'bdo', 'big', 'blockquote', 'br', 'button', 'caption', 'center', 'cite', 'code', 'col', 'colgroup', 'dd', 'del', 'dfn', 'dir', 'div', 'dl', 'dt', 'em', 'fieldset', 'font', 'form', 'h1', 'h2', 'h3', 'h4', 'h5', 'h6', 'hr', 'i', 'iframe', 'img', 'ins', 'kbd', 'label', 'legend', 'ol', 'object', 's', 'script', 'small', 'span', 'strike', 'strong', 'sub', 'sup', 'table', 'tbody', 'td', 'textarea', 'tfoot', 'th', 'thead', 'tr', 'tt', 'u', 'ul');
    $doNotParseChildren = array('li');
    $response = array();
    $parentName = strtolower($element->getName());
    $elementCount = count($element);
    $children = ($elementCount > 0) ? $element->children() : $element;
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Processing element $parentName at level $level. element XML = " . $element->asXML(), 4);
    $func = 'parse_' . $parentName . '_tag';
    if (in_array($parentName, $HTML_tags)) {
      $func = 'parse_html_tag';
    }
    if (function_exists($func))
    {
      runDebug(__FILE__, __FUNCTION__, __LINE__,"Function $func does exist. Processing now.", 4);
      if (!in_array(strtolower($parentName), $doNotParseChildren))
      {
        runDebug(__FILE__, __FUNCTION__, __LINE__, "Passing element $parentName to the $func function", 4);
        $retVal = $func($convoArr, $element, $parentName, $level);
        $retVal = (is_array($retVal)) ? $retVal = implode_recursive(' ', $retVal, __FILE__, __FUNCTION__, __LINE__) : $retVal;
        runDebug(__FILE__, __FUNCTION__, __LINE__, "Adding '$retVal' to the response array. tag name is $parentName", 4);
        $response[] = $retVal;
        return $response;
      }
    }
    else
    {
      runDebug(__FILE__, __FUNCTION__, __LINE__,"function $func does not exist. Parsing tag as text.", 4);
      $retVal = $element;
    }
    $value = trim((string) $retVal);
    $tmpResponse = ($level <= 1 and ($parentName != 'think') and (!in_array($parentName, $doNotParseChildren))) ? $value : '';
    if (count($children) > 0 and is_object($retVal))
    {
      foreach ($children as $child)
      {
        $childName = $child->getName();
        if (in_array(strtolower($childName), $doNotParseChildren)) continue;
        $tmpResponse = parseTemplateRecursive($convoArr, $child, $level + 1);
        $tmpResponse = implode_recursive(' ', $tmpResponse, __FILE__, __FUNCTION__, __LINE__);
        $tmpResponse = ($childName == 'think') ? '' : $tmpResponse;
        runDebug(__FILE__, __FUNCTION__, __LINE__, "Adding '$tmpResponse' to the response array. tag name is $parentName.", 4);
        $response[] = $tmpResponse;
      }
    }
    return $response;
  }

  /**
   * Converts an XML <text> tag into a string
   *
   * @param array $convoArr
   * @param SimpleXMLElement $element
   * @param string $parentName
   * @param int $level
   * @return string
   */
  function parse_text_tag($convoArr, $element, $parentName, $level)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Parsing a TEXT tag.', 2);
    return (string) $element;
  }

  /**
   * Parses the AIML <star> tag
   *
   * @param array $convoArr
   * @param SimpleXMLElement $element
   * @param string $parentName
   * @param int $level
   * @return array
   */
  function parse_star_tag($convoArr, $element, $parentName, $level)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Parsing a STAR tag.', 2);
    $response = array();
    runDebug(__FILE__, __FUNCTION__, __LINE__, "parse_star_tag called from the $parentName tag at level $level. element = " . $element->asXML(), 4);
    $attributes = $element->attributes();
    if (count($attributes) != 0)
    {
      $index = $element->attributes()->index;
    }
    else $index = 1;
    runDebug(__FILE__, __FUNCTION__, __LINE__, "star index = $index.", 4);
    $star = $convoArr['star'][(int) $index];
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Adding '$star' to the response array.", 4);
    $response[] = $star;
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Index value = $index, star value = $star", 4);
    return $response;
  }

  /**
   * Parses the AIML <thatstar> tag
   *
   * @param array $convoArr
   * @param SimpleXMLElement $element
   * @param string $parentName
   * @param int $level
   * @return array
   */
  function parse_thatstar_tag($convoArr, $element, $parentName, $level)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Parsing a THATSTAR tag.', 2);
    $response = array();
    runDebug(__FILE__, __FUNCTION__, __LINE__, "parse_thatstar_tag called from the $parentName tag at level $level. element = " . $element->asXML(), 4);
    $attributes = $element->attributes();
    if (count($attributes) != 0)
    {
      $index = $element->attributes()->index;
    }
    else $index = 1;
    runDebug(__FILE__, __FUNCTION__, __LINE__, "thatstar index = $index.", 4);
    $star = $convoArr['that_star'][(int) $index];
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Adding '$star' to the response array.", 4);
    $response[] = $star;
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Index value = $index, thatstar value = $star", 4);
    return $response;
  }

  /**
   * Parses the AIML <date> tag
   *
   * @param array $convoArr
   * @param SimpleXMLElement $element
   * @param string $parentName
   * @param int $level
   * @return string
   */
  function parse_date_tag($convoArr, $element, $parentName, $level)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Parsing a DATE tag.', 2);
    global $time_zone_locale;
    $tz_list = array(
        '-12'=>'Pacific/Kwajalein',
        '-11'=>'Pacific/Samoa',
        '-10'=>'Pacific/Honolulu',
        '-9'=>'America/Juneau',
        '-8'=>'America/Los_Angeles',
        '-7'=>'America/Denver',
        '-6'=>'America/Mexico_City',
        '-5'=>'America/New_York',
        '-4'=>'America/Caracas',
        '-3.5'=>'America/St_Johns',
        '-3'=>'America/Argentina/Buenos_Aires',
        '-2'=>'Atlantic/Azores',
        '-1'=>'Atlantic/Azores',
        '0'=>'Europe/London',
        '1'=>'Europe/Paris',
        '2'=>'Europe/Helsinki',
        '3'=>'Europe/Moscow',
        '3.5'=>'Asia/Tehran',
        '4'=>'Asia/Baku',
        '4.5'=>'Asia/Kabul',
        '5'=>'Asia/Karachi',
        '5.5'=>'Asia/Calcutta',
        '6'=>'Asia/Colombo',
        '7'=>'Asia/Bangkok',
        '8'=>'Asia/Singapore',
        '9'=>'Asia/Tokyo',
        '9.5'=>'Australia/Darwin',
        '10'=>'Pacific/Guam',
        '11'=>'Asia/Magadan',
        '12'=>'Asia/Kamchatka'
    );
    $cur_timezone = date_default_timezone_get();
    $cur_locale = setlocale(LC_ALL, '');
    #$cur_locale = setlocale(LC_ALL, 'en_US');
    $format = $element->attributes()->format;
    $locale = $element->attributes()->locale;
    $tz = $element->attributes()->timezone;
    $format = (string) $format;
    $format = (!empty($format)) ? $format : '%c';
    $locale = (string)$locale . '.UTF8';
    if (!empty($locale)) setlocale(LC_ALL, $locale);
    $tz = (string) $tz;
    $tz = (!empty($tz)) ? $tz : $cur_timezone;
    $tz = (!is_numeric($tz)) ? $tz : $tz_list[$tz];
    date_default_timezone_set($tz);
    #$response = "$tz - " . strftime($format);
    $response = strftime($format);
    #$response = $cur_locale;
    date_default_timezone_set($cur_timezone);
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Date tag parsed. Returning $response", 4);
    return $response;
  }

  /**
   * Parses the AIML <random> tag
   *
   * @param array $convoArr
   * @param SimpleXMLElement $element
   * @param string $parentName
   * @param int $level
   * @return string
   */
  function parse_random_tag($convoArr, $element, $parentName, $level)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Parsing a RANDOM tag, or doing some stargazing, or fomenting chaos, or...', 2);
    $liArray = $element->xpath('li');
    runDebug(__FILE__, __FUNCTION__, __LINE__,"Pick array:\n" . print_r($liArray, true), 4);
    $pick = array_rand($liArray);
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Picking option #$pick from random tag.\n", 4);
    $response = parseTemplateRecursive($convoArr, $liArray[$pick], $level + 1);
    $response = implode_recursive(' ', $response, __FILE__, __FUNCTION__, __LINE__);
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Chose Random Response of '$response' for output.", 4);
    return $response;
  }

  /**
   * Parses the AIML <get> tag, obtaining it's value from the database
   *
   * @param array $convoArr
   * @param SimpleXMLElement $element
   * @param string $parentName
   * @param int $level
   * @return string
   */
  function parse_get_tag($convoArr, $element, $parentName, $level)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Parsing a GET tag. Oh, and getting a sandwich while I\'m at it.', 2);
    global $dbConn, $dbn, $remember_up_to;
    $response = '';
    $bot_id = $convoArr['conversation']['bot_id'];
    $user_id = $convoArr['conversation']['user_id'];
    $var_name = $element->attributes()->name;
    $var_name = ($var_name == '*') ? $convoArr['star'][1] : $var_name;
    for ($n = 2; $n <= $remember_up_to; $n++) # index multiple star values
    {
      $var_name = ($var_name == "*$n") ? $convoArr['star'][$n] : $var_name;
    }
    if (empty ($var_name))
      $response = 'undefined';
    if (empty ($response))
    {
     	$sql = "select `value` from `$dbn`.`client_properties` where `user_id` = $user_id and `bot_id` = $bot_id and `name` = '$var_name';";
	runDebug(__FILE__, __FUNCTION__, __LINE__, "Checking the DB for $var_name - sql:\n$sql", 3);
    $row = db_fetch($sql, null, __FILE__, __FUNCTION__, __LINE__);
	if (($row) and (count($row) > 0)) {
		$response = $row['value'];
	}
	else {
		$response = 'undefined';
	}

    }
    runDebug(__FILE__, __FUNCTION__, __LINE__, "The value for $var_name is $response.", 4);
    return $response;
  }

  /**
   * Parses the AIML <set> tag, storing it's value in the database
   *
   * @param array $convoArr
   * @param SimpleXMLElement $element
   * @param string $parentName
   * @param int $level
   * @return string
   */
  function parse_set_tag(&$convoArr, $element, $parentName, $level)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Parsing the SET tag.', 2);
    global $dbConn, $dbn, $user_name, $remember_up_to;
    $var_value = tag_to_string($convoArr, $element, $parentName, $level, 'element');
    $bot_id = $convoArr['conversation']['bot_id'];
    $user_id = $convoArr['conversation']['user_id'];
    $var_name = (string)$element->attributes()->name;
    $var_name = ($var_name == '*') ? $convoArr['star'][1] : $var_name;
    for ($n = 2; $n <= $remember_up_to; $n++) # index multiple star values
    {
      $var_name = ($var_name == "*$n") ? $convoArr['star'][$n] : $var_name;
    }
    $vn_type = gettype($var_name);
    runDebug(__FILE__, __FUNCTION__, __LINE__, "var_name = $var_name and is type: $vn_type", 4);
    if ($var_name == 'name')
    {
      $convoArr['client_properties'][$var_name] = $var_value;
      $user_name = $var_value;
      $escaped_var_value = $var_value;
      $sql = "UPDATE `$dbn`.`users` set `user_name` = '$escaped_var_value' where `id` = $user_id;";
      runDebug(__FILE__, __FUNCTION__, __LINE__, "Updating user name in the DB. SQL:\n$sql", 3);

      $sth = $dbConn->prepare($sql);
      $sth->execute();

      $numRows = $sth->rowCount();
      $sql = "select `user_name` from `$dbn`.`users` where `id` = $user_id limit 1;";
      runDebug(__FILE__, __FUNCTION__, __LINE__, "Checking the users table to see if the value has changed. - SQL:\n$sql", 3);
      $row = db_fetch($sql, null, __FILE__, __FUNCTION__, __LINE__);
      $rowCount = count($row);
      if ($rowCount != 0)
      {
        $tmp_name = $row['user_name'];
        runDebug(__FILE__, __FUNCTION__, __LINE__, "The value for the user's name is $tmp_name.", 4);
      }

    }
    else $convoArr['client_properties'][$var_name] = $var_value;
    $lc_var_name = (IS_MB_ENABLED) ? mb_strtolower($var_name) : strtolower($var_name);
    if ($lc_var_name == 'topic') $convoArr['topic'][1] = $var_value;
    $sql = "select `value` from `$dbn`.`client_properties` where `user_id` = $user_id and `bot_id` = $bot_id and `name` = '$var_name';";
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Checking the client_properties table for the value of $var_name. - SQL:\n$sql", 3);
    $result = db_fetchAll($sql, null, __FILE__, __FUNCTION__, __LINE__);
    $rowCount = count($result);
    /** @noinspection PhpSillyAssignmentInspection */
    $var_name = $var_name;
    $var_name = str_replace("'", "\'", $var_name);
    /** @noinspection PhpSillyAssignmentInspection */
    $var_value = $var_value;
    $var_value = str_replace("'", "\'", $var_value);
    if ($rowCount == 0)
    {
      $sql = "insert into `$dbn`.`client_properties` (`id`, `user_id`, `bot_id`, `name`, `value`)
      values (NULL, $user_id, $bot_id, '$var_name', '$var_value');";
      runDebug(__FILE__, __FUNCTION__, __LINE__, "No value found for $var_name. Inserting $var_value into the table.", 4);
    }
    else
    {
      $sql = "update `$dbn`.`client_properties` set `value` = '$var_value' where `user_id` = $user_id and `bot_id` = $bot_id and `name` = '$var_name';";
      runDebug(__FILE__, __FUNCTION__, __LINE__, "Value found for $var_name. Updating the table to  $var_value.", 4);
    }
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Saving to DB - SQL:\n$sql", 3);

    $sth = $dbConn->prepare($sql);
    $sth->execute();

    $rowCount = $sth->rowCount();
    $response = $var_value;
    $convoArr['client_properties'][$var_name] = $var_value;
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Value for $var_name has ben set. Returning $var_value.", 4);
    $convoArr['client_properties']['test']='passed';
    return $response;
  }

  /**
   * Parses the AIML <think> tag, suppressing output by returning an empty string
   *
   * @param array $convoArr
   * @param SimpleXMLElement $element
   * @param string $parentName
   * @param int $level
   * @return string
   */
  function parse_think_tag(&$convoArr, $element, $parentName, $level)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'I\'m considering parsing a THINK tag.', 2);
    /** @noinspection PhpUnusedLocalVariableInspection */
    $response_string = tag_to_string($convoArr, $element, $parentName, $level, 'element');
    return '';
  }

  /**
   * Parses the AIML <bot> tag, obtaining it's value from the database
   *
   * @param array $convoArr
   * @param SimpleXMLElement $element
   * @return string
   */
  function parse_bot_tag($convoArr, $element)
  {
    global $remember_up_to;
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Parsing a BOT tag.', 2);
    $attributeName = (string)$element->attributes()->name;
    $attributeName = ($attributeName == '*') ? $convoArr['star'][1] : $attributeName;
    for ($n = 2; $n <= $remember_up_to; $n++) # index multiple star values
    {
      $attributeName = ($attributeName == "*$n") ? $convoArr['star'][$n] : $attributeName;
    }
    $response = (!empty ($convoArr['bot_properties'][$attributeName])) ? $convoArr['bot_properties'][$attributeName] : 'undefined';
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Returning bot property $attributeName. Value = $response", 4);
    return $response;
  }

  /**
   * Parses the AIML <id> tag, returning the current conversation ID
   *
   * @param array $convoArr
   * @param SimpleXMLElement $element
   * @param string $parentName
   * @param int $level
   * @return string
   */
  function parse_id_tag($convoArr, $element, $parentName, $level)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Parsing an ID tag.', 2);
    return $convoArr['conversation']['convo_id'];
  }

  /**
   * Parses the AIML <version> tag, returning the current version of Program O
   *
   * @param array $convoArr
   * @param SimpleXMLElement $element
   * @param string $parentName
   * @param int $level
   * @return string
   */
  function parse_version_tag($convoArr, $element, $parentName, $level)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Parsing a VERSION tag.', 2);
    return 'Program O version ' . VERSION;
  }

  /**
   * Parses the AIML <uppercase> tag
   *
   * @param array $convoArr
   * @param SimpleXMLElement $element
   * @param string $parentName
   * @param int $level
   * @return string
   */
  function parse_uppercase_tag($convoArr, $element, $parentName, $level)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'PARSING AN UPPERCASE TAG.', 2);
    $response_string = tag_to_string($convoArr, $element, $parentName, $level, 'element');
    $response_string = (IS_MB_ENABLED) ? mb_strtoupper($response_string) : strtoupper($response_string);
    return ltrim($response_string, ' ');
  }

  /**
   * Parses the AIML <lowercase> tag
   *
   * @param array $convoArr
   * @param SimpleXMLElement $element
   * @param string $parentName
   * @param int $level
   * @return string
   */
  function parse_lowercase_tag($convoArr, $element, $parentName, $level)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'parsing a lowercase tag.', 2);
    $response = tag_to_string($convoArr, $element, $parentName, $level, 'element');
    $response_string = implode_recursive(' ', $response, __FILE__, __FUNCTION__, __LINE__);
    return ltrim(strtolower($response_string), ' ');
  }

  /**
   * Parses the AIML <sentence> tag
   *
   * @param array $convoArr
   * @param SimpleXMLElement $element
   * @param string $parentName
   * @param int $level
   * @return string
   */
  function parse_sentence_tag($convoArr, $element, $parentName, $level)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Parsing a SENTENCE tag.', 2);
    $response_string = tag_to_string($convoArr, $element, $parentName, $level, 'element');
    if (IS_MB_ENABLED)
    {
      $response_string = mb_strtolower($response_string);
      $response = mb_strtoupper(mb_substr($response_string, 0, 1)) . mb_substr($response_string, 1);
    }
    else $response = ucfirst(strtolower($response_string));
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Response string was: $response_string. Transformed to $response.", 4);
    return $response;
  }

  /**
   * Parses the AIML <formal> tag
   *
   * @param array $convoArr
   * @param SimpleXMLElement $element
   * @param string $parentName
   * @param int $level
   * @return string
   */
  function parse_formal_tag($convoArr, $element, $parentName, $level)
  {
    global $charset;
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Parsing A Formal Tag.', 2);
    $response_string = tag_to_string($convoArr, $element, $parentName, $level, 'element');
    $response = (IS_MB_ENABLED) ? mb_convert_case($response_string, MB_CASE_TITLE, $charset) : ucwords(strtolower($response_string));
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Response string was: $response_string. Transformed to $response.", 4);
    return $response;
  }

  /**
   * Parses the AIML <srai> tag
   *
   * @param array $convoArr
   * @param SimpleXMLElement $element
   * @param string $parentName
   * @param int $level
   * @return string
   */
  function parse_srai_tag($convoArr, $element, $parentName, $level)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Parsing an SRAI tag.', 2);
    $srai = tag_to_string($convoArr, $element, $parentName, $level, 'element');
    $response = run_srai($convoArr, $srai);
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Finished parsing SRAI tag', 4);
    $response_string = implode_recursive(' ', $response, __FILE__, __FUNCTION__, __LINE__);
    return $response_string;
  }

  /**
   * Parses the AIML <sr> tag
   *
   * @param array $convoArr
   * @param SimpleXMLElement $element
   * @param string $parentName
   * @param int $level
   * @return string
   */
  function parse_sr_tag($convoArr, $element, $parentName, $level)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Parsing an SR tag.', 4);
    $response = run_srai($convoArr, $convoArr['star'][1]);
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Finished parsing SR tag', 4);
    $response_string = implode_recursive(' ', $response, __FILE__, __FUNCTION__, __LINE__);
    return $response_string;
  }

  /**
   * Parses the AIML <condition> tag
   *
   * @param array $convoArr
   * @param SimpleXMLElement $element
   * @param string $parentName
   * @param int $level
   * @return array|string
   */
  function parse_condition_tag($convoArr, $element, $parentName, $level)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Parsing a CONDITION tag.', 2);
    global $error_response;
    $response = array();
    $attributes = (array)$element->attributes();
    $attributesArray = (isset($attributes['@attributes'])) ? $attributes['@attributes'] : array();
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Element attributes:' . print_r($attributesArray, true), 4);
    $attribute_count = count($attributesArray);
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Element attribute count = $attribute_count", 4);
    if ($attribute_count == 0) // Bare condition tag
    {
      runDebug(__FILE__, __FUNCTION__, __LINE__, 'Parsing a CONDITION tag with no attributes. XML = ' . $element->asXML(), 4);
      $liNamePath = 'li[@name]';
      $condition_xPath = '';
      $exclude = array();
      $choices = $element->xpath($liNamePath);
      foreach ($choices as $choice)
      {
        $choice_name = (string)$choice['name'];
        if (in_array($choice_name, $exclude)) continue;
        $exclude[] = $choice_name;
        runDebug(__FILE__, __FUNCTION__, __LINE__, 'Client properties = ' . print_r($convoArr['client_properties'], true), 4);
        $choice_value = trim(get_client_property($convoArr, $choice_name));
        $condition_xPath .= "li[@name=\"$choice_name\"][@value=\"$choice_value\"]|";
      }
      $condition_xPath .= 'li[not(@*)]';
      runDebug(__FILE__, __FUNCTION__, __LINE__, "xpath search = $condition_xPath", 4);
      $pick_search = $element->xpath($condition_xPath);
      runDebug(__FILE__, __FUNCTION__, __LINE__, 'Pick array = ' . print_r($pick_search, true), 4);
      $pick_count = count($pick_search);
      runDebug(__FILE__, __FUNCTION__, __LINE__, "Pick count = $pick_count.", 4);
      $pick = $pick_search[0];
    }
    elseif (array_key_exists('value', $attributesArray) or array_key_exists('contains', $attributesArray) or array_key_exists('exists', $attributesArray)) // condition tag with either VALUE, CONTAINS or EXISTS attributes
    {
      runDebug(__FILE__, __FUNCTION__, __LINE__, 'Parsing a CONDITION tag with 2 attributes.', 4);
      $condition_name = (string)$element['name'];
      $test_value = trim(get_client_property($convoArr, $condition_name));
      switch (true)
      {
        case (isset($element['value'])):
          $condition_value = (string)$element['value'];
          break;
        case (isset($element['contains'])):
          $condition_value = (string)$element['contains'];
          break;
        case (isset($element['exists'])):
          $condition_value = (string)$element['exists'];
          break;
        default:
          runDebug(__FILE__, __FUNCTION__, __LINE__, 'Something went wrong with parsing the CONDITION tag. Returning the error response.', 1);
          return $error_response;
      }
      $pick = ($condition_value == $test_value) ? $element : '';
    }
    elseif (array_key_exists('name', $attributesArray)) // this ~SHOULD~ just trigger if the NAME value is present, and ~NOT~ NAME and (VALUE|CONTAINS|EXISTS)
    {
      runDebug(__FILE__, __FUNCTION__, __LINE__, 'Parsing a CONDITION tag with only the NAME attribute', 4);
      $condition_name = (string)$element['name'];
      $test_value = trim(get_client_property($convoArr, $condition_name));
      runDebug(__FILE__, __FUNCTION__, __LINE__, "Looking for test value '$test_value'", 4);
      $path = "li[@value]|li[not(@*)]";
      runDebug(__FILE__, __FUNCTION__, __LINE__, "search string = $path", 4);
      $choice = $element->xpath($path);
      runDebug(__FILE__, __FUNCTION__, __LINE__,'element = ' . print_r($element, true), 4);
      runDebug(__FILE__, __FUNCTION__, __LINE__,'Choices = ' . print_r($choice, true), 4);
      if (count($choice) != 0)
      {
        $test_value = rtrim($test_value);
        runDebug(__FILE__, __FUNCTION__, __LINE__,'parent XML = ' . $element->asXML(), 4);
        foreach ($choice as $pick)
        {
          runDebug(__FILE__, __FUNCTION__, __LINE__,'Current pick = ' . print_r($pick, true), 4);
          $attr = $pick->attributes();
          runDebug(__FILE__, __FUNCTION__, __LINE__,'Current pick attributes = ' . print_r($attr, true), 4);
          $testVarValue = (isset($attr['value'])) ? (string)$attr['value'] : '';
          runDebug(__FILE__, __FUNCTION__, __LINE__,"Pick Value = '$testVarValue'", 4);
          runDebug(__FILE__, __FUNCTION__, __LINE__,"Checking to see if $testVarValue (condition value) is equal to $test_value (Client Property).", 4);
          if (_strtolower($testVarValue) == _strtolower($test_value))
          {
            runDebug(__FILE__, __FUNCTION__, __LINE__,'Pick XML = ' . $pick->asXML(), 4);
            break;
          }
        }
        runDebug(__FILE__, __FUNCTION__, __LINE__, 'Found a match. Pick = ' . print_r($pick, true), 4);
      }
      else
      {
        $path = "li[@value=\"$test_value\"]|li[not(@*)]";
        runDebug(__FILE__, __FUNCTION__, __LINE__, "search string = $path", 4);
        $choice = $element->xpath($path);
        $pick = $choice[0];
        runDebug(__FILE__, __FUNCTION__, __LINE__, 'Found a match. Pick = ' . print_r($pick, true), 4);
      }
    }
    else // nothing matches
    {
      runDebug(__FILE__, __FUNCTION__, __LINE__, 'No matches found. Returning default error response. this is probably because of poorly written AIML code.', 1);
      return $error_response;
    }
    $children = (is_object($pick)) ? $pick->children() : null;
    if (!empty ($children))
    {
      foreach ($children as $child)
      {
        $response[] = parseTemplateRecursive($convoArr, $child, $level + 1);
      }
      runDebug(__FILE__, __FUNCTION__, __LINE__, "Response = " . print_r($response, true), 4);
    }
    else
    {
      $response[] = (string) $pick;
    }
    $response_string = implode_recursive(' ', $response, __FILE__, __FUNCTION__, __LINE__);
    return $response_string;
  }

  /**
   * Parses the AIML <person> tag
   *
   * @param array $convoArr
   * @param SimpleXMLElement $element
   * @param string $parentName
   * @param int $level
   * @return string
   */
  function parse_person_tag($convoArr, $element, $parentName, $level)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Parsing a PERSON tag.', 2);
    $response_string = tag_to_string($convoArr, $element, $parentName, $level, 'star');
    $response = swapPerson($convoArr, 3, $response_string);
    return $response;
  }

  /**
   * Parses the AIML <person2> tag
   *
   * @param array $convoArr
   * @param SimpleXMLElement $element
   * @param string $parentName
   * @param int $level
   * @return string
   */
  function parse_person2_tag($convoArr, $element, $parentName, $level)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Parsing a PERSON2 tag.', 2);
    $response_string = tag_to_string($convoArr, $element, $parentName, $level, 'star');
    $response = swapPerson($convoArr, 2, $response_string);
    return $response;
  }

  /**
   * Parses any HTML tags that are not also AIML tags
   *
   * @param array $convoArr
   * @param SimpleXMLElement $element
   * @param string $parentName
   * @param int $level
   * @return string
   */
  function parse_html_tag($convoArr, $element, $parentName, $level)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Parsing a generic HTML tag.', 2);
    $response_string = $element->asXML();
    $response_string = str_replace('<text>', '', $response_string);
    $response_string = str_replace('</text>', '', $response_string);
    $star = (isset($convoArr['star'][1])) ? $convoArr['star'][1] : '';
    if ($star != '') $response_string = str_replace('<star/>', $star, $response_string);
    return $response_string;
  }

  /**
   * Parses the AIML <gender> tag
   *
   * @param array $convoArr
   * @param SimpleXMLElement $element
   * @param string $parentName
   * @param int $level
   * @return string
   */
  function parse_gender_tag($convoArr, $element, $parentName, $level)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Giving part of the response a sex change!', 2);
    $response_string = ' ' . tag_to_string($convoArr, $element, $parentName, $level, 'star');
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Original response string = '$response_string'", 4);
    $nounList = $convoArr['nounList'];
    foreach ($nounList as $noun)
    {
      // This fixes (most) possessives
      $response_string = str_replace(" his $noun ", " x_her $noun ", $response_string);
    }
    $male2tmp = array('he ' => ' x_she ', ' his ' => ' x_hers ', ' him ' => ' x_her ', ' He ' => ' x_She ', ' His ' => ' x_Hers ', ' Him ' => ' x_Her ', 'he!' => ' x_she!', ' his!' => ' x_hers!', ' him!' => ' x_her!', ' He!' => ' x_She!', ' His!' => ' x_Hers!', ' Him!' => ' x_Her!', 'he,' => ' x_she,', ' his,' => ' x_hers,', ' him,' => ' x_her,', ' He,' => ' x_She,', ' His,' => ' x_Hers,', ' Him,' => ' x_Her,', 'he?' => ' x_she?', ' his?' => ' x_hers?', ' him?' => ' x_her?', ' He?' => ' x_She?', ' His?' => ' x_Hers?', ' Him?' => ' x_Her?',);
    $female2male = array(' she ' => ' he ', ' hers ' => ' his ', ' her ' => ' him ', ' She ' => ' He ', ' Hers ' => ' His ', ' Her ' => ' Him ', ' she.' => 'he.', ' hers.' => ' his.', ' her.' => ' him.', ' She.' => ' He.', ' Hers.' => ' His.', ' Her.' => ' Him.', ' she,' => 'he,', ' hers,' => ' his,', ' her,' => ' him,', ' She,' => ' He,', ' Hers,' => ' His,', ' Her,' => ' Him,', ' she!' => 'he!', ' hers!' => ' his!', ' her!' => ' him!', ' She!' => ' He!', ' Hers!' => ' His!', ' Her!' => ' Him!', ' she?' => 'he?', ' hers?' => ' his?', ' her?' => ' him?', ' She?' => ' He?', ' Hers?' => ' His?', ' Her?' => ' Him?',);
    $tmp2male = array(' x_he' => ' he', ' x_she' => ' she', ' x_He' => ' He', ' x_She' => ' She',);
    $m2tSearch = array_keys($male2tmp);
    $m2tReplace = array_values($male2tmp);
    $response_string = str_replace($m2tSearch, $m2tReplace, $response_string);
    $f2mSearch = array_keys($female2male);
    $f2mReplace = array_values($female2male);
    $response_string = str_replace($f2mSearch, $f2mReplace, $response_string);
    $t2mSearch = array_keys($tmp2male);
    $t2mReplace = array_values($tmp2male);
    $response_string = str_replace($t2mSearch, $t2mReplace, $response_string);
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Transformed response string = '$response_string'", 4);
    return $response_string;
  }

  /**
   * Parses the AIML <that> tag
   *
   * @param array $convoArr
   * @param SimpleXMLElement $element
   * @param string $parentName
   * @param int $level
   * @return string
   */
  function parse_that_tag($convoArr, $element, $parentName, $level)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Parsing a THAT tag. How awesome is that?', 2);

    if(!empty($element))
    {
      $attributes = $element->attributes();
      runDebug(__FILE__, __FUNCTION__, __LINE__, 'element attributes = ' . print_r($attributes, true), 2);
      $index = $attributes['index'];
      runDebug(__FILE__, __FUNCTION__, __LINE__, 'element attribute index = ' . $index, 2);
      $index = (!empty ($index)) ? $index : 1;
      if ($index == intval($index))
      {
        $response = $convoArr['that'][(int)$index];
        //$index .= ',1';
      }
      if (strstr($index, ',') !== false)
      {
        list($index1, $index2) = explode(',', $index, 2);
        $index1 = intval($index1);
        $index2 = intval($index2);
        $thatArray = $convoArr['that'];
        runDebug(__FILE__, __FUNCTION__, __LINE__,'THAT array = ' . print_r($thatArray, true), 2);
        runDebug(__FILE__, __FUNCTION__, __LINE__, 'index1 = ' . $index1, 2);

        if (!empty($convoArr['that'][$index1][$index2])) $response = $convoArr['that'][$index1][$index2];
        else $response = '';
      }

      $response_string = implode_recursive(' ', $response, __FILE__, __FUNCTION__, __LINE__);
    }
    else $response_string = implode_recursive(' ', $convoArr['that'][1], __FILE__, __FUNCTION__, __LINE__);

    return $response_string;
  }

  /**
   * Parses the AIML <input> tag
   *
   * @param array $convoArr
   * @param SimpleXMLElement $element
   * @param string $parentName
   * @param int $level
   * @return string
   */
  function parse_input_tag($convoArr, $element, $parentName, $level)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Parsing an INPUT tag.', 2);
    //$element = $element->input;
    $input_index = (string)$element['index'];
    $input_index = (!empty ($input_index)) ? $input_index : 1;
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Parsing the INPUT tag. Index = $input_index.", 4);
    $response_string = $convoArr['input'][$input_index];
    return $response_string;
  }

  /**
   * Parses the AIML <system> tag, Executing system calls and returning the results
   *
   * @param array $convoArr
   * @param SimpleXMLElement $element
   * @param string $parentName
   * @param int $level
   * @return string
   */
  function parse_system_tag($convoArr, $element, $parentName, $level)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Parsing a SYSTEM tag (May God have mercy on us all).', 2);
    $system_call = tag_to_string($convoArr, $element, $parentName, $level, 'element');
    $response_string = shell_exec($system_call);
    return $response_string;
  }

  /**
   * Parses the 'extended' AIML <learn> tag
   *
   * @param array $convoArr
   * @param SimpleXMLElement $element
   * @param string $parentName
   * @param int $level
   * @return string
   */
  function parse_learn_tag($convoArr, $element, $parentName, $level)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Parsing a LEARN tag.', 2);
    global $dbn, $dbConn;
    $bot_id = $convoArr['conversation']['bot_id'];
    $user_id = $convoArr['conversation']['user_id'];
    $sqlTemplate = "insert into `$dbn`.`aiml_userdefined` (`id`, `bot_id`, `aiml`, `pattern`, `thatpattern`, `template`, `user_id`)
values (NULL, $bot_id, '[aiml]', '[pattern]', '[that]', '[template]', '$user_id');";
    $sql = '';
    $failure = false;
    $remove = array('<text>', '</text>');
    $fileName = (string) $element['filename'];
    if (empty($fileName)) // enclosed text is the category to add to the DB
    {
      $category = $element->category;

      $pattern = parseTemplateRecursive($convoArr, $category->pattern, $level + 1);
      $pattern = implode_recursive(' ', $pattern, __FILE__, __FUNCTION__, __LINE__);
      $pattern = (IS_MB_ENABLED) ? mb_strtoupper($pattern) : strtoupper($pattern);

      $thatpattern = (string)$category->that;

      $parsed_template = parseTemplateRecursive($convoArr, $category->template, $level + 1);
      $parsed_template = implode_recursive(' ', $parsed_template, __FILE__, __FUNCTION__, __LINE__);
      runDebug(__FILE__, __FUNCTION__, __LINE__, 'Parsed LEARN template: ' . $parsed_template, 2);

      $catXML = new SimpleXMLElement('<category/>');
      $catXML->addChild('pattern', $pattern);
      if (!empty($thatpattern))
        $catXML->addChild('that', $thatpattern);
      $catXML->addChild('template', $parsed_template);
      $category = $catXML->asXML();
      $category = trim(str_replace('<?xml version="1.0"?>', '', $category));
      $sqlAdd = str_replace('[aiml]', $category, $sqlTemplate);
      $sqlAdd = str_replace('[pattern]', $pattern, $sqlAdd);
      $sqlAdd = str_replace('[that]', $thatpattern, $sqlAdd);
      $sqlAdd = str_replace('[template]', $parsed_template, $sqlAdd);
      $sql .= $sqlAdd;

      $sth = $dbConn->prepare($sql);
      $sth->execute();

    }
    else
    {
      $uploaded_file = _UPLOAD_PATH_ . $fileName;
      if (!file_exists($uploaded_file)) $failure = "File $fileName does not exist in the upload path!";
      else
      {
        $aiml = simplexml_load_file($uploaded_file);
        if (!$aiml) $failure = "Could not parse file $uploaded_file as XML!";
        else
        {
          foreach ($aiml->topic as $topic)
          {
            $topicName = (string)$topic['name'];
            foreach ($topic as $category)
            {
              $catXML  = $category->asXML();
              $pattern = $category->pattern->asXML();
              $thatpattern = $category->that->asXML();
              $template = $category->template->asXML();

              $sqlAdd = str_replace('[aiml]', $catXML, $sqlTemplate);
              $sqlAdd = str_replace('[pattern]', $pattern, $sqlAdd);
              $sqlAdd = str_replace('[that]', $thatpattern, $sqlAdd);
              $sqlAdd = str_replace('[template]', $template, $sqlAdd);
              $sqlAdd = str_replace('[topic]', $topicName, $sqlAdd);
              $sqlAdd = str_replace('[fileName]', $fileName, $sqlAdd);
              $sql .= $sqlAdd;
            }
          }
        }
      }
    }

    if ($failure) {
      trigger_error($failure);
    }
    else {
      $sql = str_replace($remove, '', $sql);
      runDebug(__FILE__, __FUNCTION__, __LINE__, "Adding AIML to the DB. SQL:\n$sql", 3);
    }

    return '';
  }

  /**
   * Parses the 'extended' AIML <eval> tag
   *
   * @param array $convoArr
   * @param SimpleXMLElement $element
   * @param string $parentName
   * @param int $level
   * @return array|string
   */
  function parse_eval_tag($convoArr, $element, $parentName, $level)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Parsing an EVAL tag.', 2);
    $response_string = tag_to_string($convoArr, $element, $parentName, $level, 'element');
    // do something here
    return $response_string;
  }

  /**
   * Converts the contents of the AIML tag to a string.
   *
   * @param array            $convoArr
   * @param SimpleXMLElement $element
   * @param string           $parentName
   * @param int              $level
   * @param string           $type
   * @return string
   */
  function tag_to_string(&$convoArr, $element, $parentName, $level, $type = 'element')
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, "converting the $parentName tag into text.", 2);
    $response = array();
    $children = $element->children();
    if (!empty ($children))
    {
      foreach ($children as $child)
      {
        $response[] = parseTemplateRecursive($convoArr, $child, $level + 1);
      }
    }
    else
    {
      switch ($type)
      {
        case 'element':
        $response[] = (string) $element;
        break;
        default:
        $response[] = $convoArr['star'][1];
      }
    }
    $response_string = implode_recursive(' ', $response, __FILE__, __FUNCTION__, __LINE__);
    // do something here
    return $response_string;
  }
