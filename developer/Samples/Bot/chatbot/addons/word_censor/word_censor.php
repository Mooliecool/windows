<?php

  /***************************************
  * www.program-o.com
  * PROGRAM O
  * Version: 2.5.3
  * FILE: word_censor/word_censor.php
  * AUTHOR: Elizabeth Perreau and Dave Morton
  * DATE: MAY 17TH 2014
  * DETAILS: this file contains the addon library to censor output before they are output to user
  *             the swear words are encoded in the session array to protect little eyes
  ***************************************/
  /**
   * function run_censor()
   * A function to run the censorship of words
   * if the censor session array is not set this will set it
   *
   * @param  array $convoArr - the conversation array
   * @return array $convoArr (censored)
   */
  function run_censor($convoArr)
  {
    if (!isset ($_SESSION['pgo_word_censor']))
    {
      initialise_censor($convoArr['conversation']['bot_id']);
    }
    $convoArr['send_to_user'] = censor_words($convoArr['send_to_user']);
    return $convoArr;
  }

  /**
  * function intialise_censor()
  * A function to build session array containing the words from the censor list in the db
  **/
  function initialise_censor($bot_id)
  {
    global $dbConn, $dbn; //set in global config file
    $_SESSION['pgo_word_censor'] = array();
    $sql = "SELECT * FROM `$dbn`.`wordcensor` WHERE `bot_exclude` NOT LIKE '%[$bot_id]%'";
    $result = db_fetchAll($sql, null, __FILE__, __FUNCTION__, __LINE__);
    foreach ($result as $row)
    {
      $index = $row['word_to_censor'];
      $value = $row['replace_with'];
      $_SESSION['pgo_word_censor'][$index] = $value;
    }
    
  }

  /**
   * function censor_words()
   * A function to censor words before outputting them to screen
   *
   * @param  string $output - the string we wish to censor
   * @return mixed|string $coutput (censored)
   */
  function censor_words($output)
  {
    foreach ($_SESSION['pgo_word_censor'] as $find => $replace)
    {
      $output = preg_replace("/\b$find\b/i", $replace, $output);
    }
    return $output;
  }

?>