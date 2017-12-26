<?php

  /***************************************
  * www.program-o.com
  * PROGRAM O
  * Version: 2.5.3
  * FILE: spell_checker/spell_checker.php
  * AUTHOR: Elizabeth Perreau and Dave Morton
  * DATE: MAY 17TH 2014
  * DETAILS: this file contains the addon library to spell check into before its matched in the database
  ***************************************/

  if (!defined('SPELLCHECK_PATH'))
  {
    $this_folder = dirname( realpath( __FILE__ ) ) . DIRECTORY_SEPARATOR;
    define('SPELLCHECK_PATH', $this_folder);
  }

      if (empty($_SESSION['spellcheck_common_words']))
    {
      $_SESSION['spellcheck_common_words'] = file(SPELLCHECK_PATH.'spellcheck_common_words.dat', FILE_IGNORE_NEW_LINES);
    }

    $spellcheck_common_words = $_SESSION['commonWords'];

  /**
   * function run_spellcheck_say()
   * A function to run the spellchecking of the userinput
   *
   * @param  string $say - The user's input
   * @return string $say (spellchecked)
   */
  function run_spell_checker_say($say)
  {
    global $bot_id;
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Starting function and setting timestamp.', 2);
    $sentence = '';
    $wordArr = explode(' ', $say);
    foreach ($wordArr as $index => $word)
    {
      $sentence .= spell_check($word, $bot_id) . " ";
    }
    return trim($sentence);
  }

  /**
   * function spell_check()
   * Checks the given word against a list of commonly misspelled words, replacing it with a correction, if necessary.
   *
   * @param $word
   * @param $bot_id
   * @internal param $ [type] [variable used]
   * @return mixed|string [type] [return value]
   */
  function spell_check($word, $bot_id)
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, "Preforming a spel chek on $word.", 2);
    global $dbConn, $dbn, $spellcheck_common_words;
    if (strstr($word, "'")) $word = str_replace("'", ' ', $word);
    $test_word = (IS_MB_ENABLED) ? mb_strtolower($word) : strtolower($word);
    if (!isset($_SESSION['spellcheck'])) load_spelling_list();
    if (in_array($test_word, $spellcheck_common_words))
    {
      runDebug(__FILE__, __FUNCTION__, __LINE__, "The word '$word' is a common word. Returning without checking.", 4);
      return $word;
    }
    if (in_array($test_word, array_keys($_SESSION['spellcheck'])))
    {
      $corrected_word = $_SESSION['spellcheck'][$test_word];
      runDebug(__FILE__, __FUNCTION__, __LINE__, "Misspelling found! Replaced $word with $corrected_word.", 4);
    }
    else
    {
      runDebug(__FILE__, __FUNCTION__, __LINE__,'Spelling check passed.', 4);
      $corrected_word = $word;
    }
    if (IS_MB_ENABLED)
    {
      switch ($word){
        case mb_strtolower($word):
        $corrected_word = mb_strtolower($corrected_word);
        break;
        case mb_strtoupper($word):
        $corrected_word = mb_strtoupper($corrected_word);
        break;
        case mb_convert_case($word, MB_CASE_TITLE):
        $corrected_word = mb_convert_case($corrected_word, MB_CASE_TITLE);
        break;
        default:
      }
    }
    else
    {
      switch ($word){
        case strtolower($word):
        $corrected_word = strtolower($corrected_word);
        break;
        case strtoupper($word):
        $corrected_word = strtoupper($corrected_word);
        break;
        case ucfirst($word):
        $corrected_word = ucfirst($corrected_word);
        break;
        default:
      }
    }
  //set in global config file
    return $corrected_word;
  }

  /**
   * function load_spelling_list
   * Gets all missspelled words and their corrections from the DB, loading them into a session variable.
   *
   * @internal param $ (none)
   * @return void (void)
   */
  function load_spelling_list()
  {
    runDebug(__FILE__, __FUNCTION__, __LINE__, 'Loading the spellcheck list from the DB.', 2);
    global $dbConn, $dbn;
    $_SESSION['spellcheck'] = array();
    $sql = "SELECT `missspelling`, `correction` FROM `$dbn`.`spellcheck`;";
    $result = db_fetchAll($sql, null, __FILE__, __FUNCTION__, __LINE__);
    $num_rows = count($result);
    if ($num_rows > 0)
    {
      foreach ($result as $row)
      {
        $missspelling = (IS_MB_ENABLED) ? mb_strtolower($row['missspelling']) : strtolower($row['missspelling']);
        $correction = $row['correction'];
        $_SESSION['spellcheck'][$missspelling] = $correction;
      }
    }
    
  }

?>