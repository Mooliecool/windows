<?php
  $msg = '';
  $upperScripts = <<<endScript

    <script type="text/javascript" src="scripts/tablesorter.min.js"></script>
    <script type="text/javascript">
<!--
      var state = 'hidden';
      function showhide(layer_ref) {
        if (state == 'visible') {
          state = 'hidden';
        }
        else {
          state = 'visible';
        }
        if (document.all) { //IS IE 4 or 5 (or 6 beta)
          eval( "document.all." + layer_ref + ".style.visibility = state");
        }
        if (document.layers) { //IS NETSCAPE 4 or below
          document.layers[layer_ref].visibility = state;
        }
        if (document.getElementById && !document.all) {
          maxwell_smart = document.getElementById(layer_ref);
          maxwell_smart.style.visibility = state;
        }
      }
//-->
    </script>
endScript;
    $post_vars = filter_input_array(INPUT_POST);
  $get_vars = filter_input_array(INPUT_GET);
  $request_vars = (array) $post_vars + (array) $get_vars;
  $group = (isset ($request_vars['group'])) ? $request_vars['group'] : 1;
  $content = $template->getSection('SearchWordCensorForm');
  $wc_action = isset ($request_vars['action']) ? strtolower($request_vars['action']) : '';
  $wc_id = isset ($request_vars['censor_id']) ? $request_vars['censor_id'] : - 1;
  if (!empty ($wc_action))
  {
    switch ($wc_action)
    {
      case 'search' :
        $content .= runWordCensorSearch();
        $content .= wordCensorForm();
        break;
      case 'update' :
        $x = updateWordCensor();
        $content .= wordCensorForm();
        break;
      case 'delete' :
        $content .= ($wc_id >= 0) ? delWordCensor($wc_id) . wordCensorForm() :
                    wordCensorForm();
        break;
      case 'edit' :
        $content .= ($wc_id >= 0) ? editWordCensorForm($wc_id) : wordCensorForm();
        break;
      case 'add' :
        $x = insertWordCensor();
        $content .= wordCensorForm();
        break;
      default :
        $content .= wordCensorForm();
    }
  }
  else
  {
    $content .= wordCensorForm();
  }
  $content = str_replace('[group]', $group, $content);
  $topNav = $template->getSection('TopNav');
  $leftNav = $template->getSection('LeftNav');
  $main = $template->getSection('Main');
  
  $navHeader = $template->getSection('NavHeader');
  $rightNav = $template->getSection('RightNav');
  
  $rightNavLinks = getWordCensorWords();
  $FooterInfo = getFooter();
  $errMsgClass = (!empty ($msg)) ? "ShowError" : "HideError";
  $errMsgStyle = $template->getSection($errMsgClass);
  $noLeftNav = '';
  $noTopNav = '';
  $noRightNav = '';
  $headerTitle = 'Actions:';
  $pageTitle = 'My-Program O - Word Censor Editor';
  $mainContent = $content;
  $mainTitle = 'Word Censor Editor';
  $mainContent = str_replace('[wordCensorForm]', wordCensorForm(), $mainContent);
  $rightNav = str_replace('[rightNavLinks]', $rightNavLinks, $rightNav);
  $rightNav = str_replace('[navHeader]', $navHeader, $rightNav);
  $rightNav = str_replace('[headerTitle]', wcPaginate(), $rightNav);

  /**
   * Function wcPaginate
   *
   *
   * @return string
   */
  function wcPaginate()
  {
    global $dbConn, $request_vars;
    $sql = "select count(*) from `wordcensor` where 1";
    $row = db_fetch($sql, null, __FILE__, __FUNCTION__, __LINE__);
    $rowCount = $row['count(*)'];
    $lastPage = intval($rowCount / 50);
    $remainder = ($rowCount / 50) - $lastPage;
    if ($remainder > 0)
      $lastPage++;
    $out = "Censored Words<br />\n50 words per page:<br />\n";
    $link =
    " - <a class=\"paginate\" href=\"index.php?page=wordcensor&amp;group=[group]\">[label]</a>";
    $curStart = (isset ($request_vars['group'])) ? $request_vars['group'] : 1;
    $firstPage = 1;
    $prev = ($curStart > ($firstPage + 1)) ? $curStart - 1 : - 1;
    $next = ($lastPage > ($curStart + 1)) ? $curStart + 1 : - 1;
    $firstLink = ($firstPage != $curStart) ? str_replace('[group]', '1', $link) : '';
    $prevLink = ($prev > 0) ? str_replace('[group]', $prev, $link) : '';
    $curLink = "- $curStart ";
    if (empty ($firstLink) and empty ($prevLink))
      $curLink = $curStart;
    $nextLink = ($next > 0) ? str_replace('[group]', $next, $link) : '';
    $lastLink = ($lastPage > $curStart) ? str_replace('[group]', $lastPage, $link) : '';
    $firstLink = str_replace('[label]', 'first', $firstLink);
    $prevLink = str_replace('[label]', '&lt;&lt;', $prevLink);
    $nextLink = str_replace('[label]', '&gt;&gt;', $nextLink);
    $lastLink = str_replace('[label]', 'last', $lastLink);
    $out .= ltrim("$firstLink\n$prevLink\n$curLink\n$nextLink\n$lastLink", " - ");
    return $out;
  }

  /**
   * Function getWordCensorWords
   *
   *
   * @return string
   */
  function getWordCensorWords()
  {
    global $dbConn, $template, $request_vars;
    $group = (isset ($request_vars['group'])) ? $request_vars['group'] : 1;
    $_SESSION['poadmin']['group'] = $group;
    $startEntry = ($group - 1) * 50;
    $startEntry = ($startEntry < 0) ? 0 : $startEntry;
    $end = $group + 50;
    $_SESSION['poadmin']['page_start'] = $group;
    $curID = (isset ($request_vars['id'])) ? $request_vars['id'] : - 1;
    $sql =
    "select `censor_id`,`word_to_censor` from `wordcensor` where 1 order by abs(`censor_id`) asc limit $startEntry, 50;";
    $baseLink = $template->getSection('NavLink');
    $links = '      <div class="userlist">' . "\n";
    $count = 0;
    $result = db_fetchAll($sql, null, __FILE__, __FUNCTION__, __LINE__);
    foreach ($result as $row)
    {
      $linkId = $row['censor_id'];
      $linkClass = ($linkId == $curID) ? 'selected' : 'noClass';
      $word_to_censor = $row['word_to_censor'];
      $tmpLink = str_replace('[linkClass]', " class=\"$linkClass\"", $baseLink);
      $linkHref =
      " href=\"index.php?page=wordcensor&amp;action=edit&amp;censor_id=$linkId&amp;group=$group#$linkId\" name=\"$linkId\"";
      $tmpLink = str_replace('[linkHref]', $linkHref, $tmpLink);
      $tmpLink = str_replace('[linkOnclick]', '', $tmpLink);
      $tmpLink = str_replace('[linkTitle]',
                            " title=\"Edit spelling replace_with for the word '$word_to_censor'\"",
                            $tmpLink);
      $tmpLink = str_replace('[linkLabel]', $word_to_censor, $tmpLink);
      $links .= "$tmpLink\n";
      $count++;
    }
    $page_count = intval($count / 50);
    $_SESSION['poadmin']['page_count'] = $page_count + (($count / 50) > $page_count) ? 1 :
                                                       0;
    $links .= "\n      </div>\n";
    return $links;
  }

  /**
   * Function wordCensorForm
   *
   *
   * @return mixed|string
   */
  function wordCensorForm()
  {
    global $template, $request_vars;
    $out = $template->getSection('WordCensorForm');
    $group = (isset ($request_vars['group'])) ? $request_vars['group'] : 1;
    $out = str_replace('[group]', $group, $out);
    return $out;
  }

  /**
   * Function insertWordCensor
   *
   *
   * @return string
   */
  function insertWordCensor()
  {
    global $dbConn, $template, $msg, $request_vars;
    $replace_with = trim($request_vars['replace_with']);
    $word_to_censor = trim($request_vars['word_to_censor']);
    if (($replace_with == "") || ($word_to_censor == ""))
    {
      $msg = '        <div id="errMsg">You must enter a spelling mistake and the replace_with.</div>' . "\n";
    }
    else
    {
      $sql = 'INSERT INTO `wordcensor` (`censor_id`, `word_to_censor`, `replace_with`, `bot_exclude`) VALUES
      (NULL, :word_to_censor, :replace_with, "")';
      $params = array(
        ':word_to_censor' => $word_to_censor,
        ':replace_with' => $replace_with,
      );
      $rowsAffected = db_write($sql, $params, false, __FILE__, __FUNCTION__, __LINE__);
      if ($rowsAffected > 0)
      {
        $msg = '<div id="successMsg">Correction added.</div>';
      }
      else
      {
        $msg = '<div id="errMsg">There was a problem editing the replace_with - no changes made.</div>';
      }
    }
    return $msg;
  }

  /**
   * Function delWordCensor
   *
   * * @param $id
   * @return void
   */
  function delWordCensor($id)
  {
    global $dbConn, $template, $msg;
    if ($id == "")
    {
      $msg =
      '<div id="errMsg">There was a problem editing the replace_with - no changes made.</div>';
    }
    else
    {
      $sql = 'DELETE FROM `wordcensor` WHERE `censor_id` = :id LIMIT 1;';
      $params = array(
        ':id' => $id
      );
      $rowsAffected = db_write($sql, $params, false, __FILE__, __FUNCTION__, __LINE__);
      if ($rowsAffected > 0)
      {
        $msg = '<div id="successMsg">Correction deleted.</div>';
      }
      else
      {
        $msg =
        '<div id="errMsg">There was a problem editing the replace_with - no changes made.</div>';
      }
    }
  }

  /**
   * Function runWordCensorSearch
   *
   *
   * @return string
   */
  function runWordCensorSearch()
  {
    global $dbConn, $template, $request_vars;
    $search = trim($request_vars['search']);
    $sql =
    "SELECT * FROM `wordcensor` WHERE `word_to_censor` LIKE '%$search%' OR `replace_with` LIKE '%$search%' LIMIT 50";
    $htmltbl =
    '               <table>
                  <thead>
                    <tr>
                      <th class="sortable">word_to_censor</th>
                      <th class="sortable">Correction</th>
                      <th>Action</th>
                    </tr>
                  </thead>
                <tbody>';
    $result = db_fetchAll($sql, null, __FILE__, __FUNCTION__, __LINE__);
    $i = 0;
    foreach ($result as $row)
    {
      $i++;
      $word_to_censor = (IS_MB_ENABLED) ? mb_strtoupper($row['word_to_censor']) :
                        strtoupper($row['word_to_censor']);
      $replace_with = (IS_MB_ENABLED) ? mb_strtoupper($row['replace_with']) : strtoupper($row
                      ['replace_with']);
      $id = $row['censor_id'];
      $group = round(($id / 50));
      $action =
      "<a href=\"index.php?page=wordcensor&amp;action=edit&amp;censor_id=$id&amp;group=$group#$id\"><img src=\"images/edit.png\" border=0 width=\"15\" height=\"15\" alt=\"Edit this entry\" title=\"Edit this entry\" /></a>
                    <a href=\"index.php?page=wordcensor&amp;action=delete&amp;censor_id=$id&amp;group=$group#$id\" onclick=\"return confirm('Do you really want to delete this entry? You will not be able to undo this!')\";><img src=\"images/del.png\" border=0 width=\"15\" height=\"15\" alt=\"Delete this entry\" title=\"Delete this entry\" /></a>";
      $htmltbl .=
      "<tr valign=top>
                            <td>$word_to_censor</td>
                            <td>$replace_with</td>
                            <td align=center>$action</td>
                        </tr>";
    }
    $htmltbl .= "</tbody></table>";
    if ($i >= 50)
    {
      $msg =
      "Found more than 50 results for '<b>$search</b>', please refine your search further";
    }
    elseif ($i == 0)
    {
      $msg =
      "Found 0 results for '<b>$search</b>'. You can use the form below to add that entry.";
      $htmltbl = "";
    }
    else
    {
      $msg = "Found $i results for '<b>$search</b>'";
    }
    $htmlresults = "<div id=\"pTitle\">$msg</div>" . $htmltbl;
    return $htmlresults;
  }

  /**
   * Function editWordCensorForm
   *
   * * @param $id
   * @return mixed|string
   */
  function editWordCensorForm($id)
  {
    global $dbConn, $template, $request_vars, $dbConn;
    $group = (isset ($request_vars['group'])) ? $request_vars['group'] : 1;
    $form = $template->getSection('EditWordCensorForm');
    $sql = "SELECT * FROM `wordcensor` WHERE `censor_id` = '$id' LIMIT 1";
    $row = db_fetch($sql, null, __FILE__, __FUNCTION__, __LINE__);
    $uc_word_to_censor = (IS_MB_ENABLED) ? mb_strtoupper($row['word_to_censor']) :
                         strtoupper($row['word_to_censor']);
    $uc_replace_with = (IS_MB_ENABLED) ? mb_strtoupper($row['replace_with']) : strtoupper(
                       $row['replace_with']);
    $form = str_replace('[censor_id]', $row['censor_id'], $form);
    $form = str_replace('[word_to_censor]', $uc_word_to_censor, $form);
    $form = str_replace('[replace_with]', $uc_replace_with, $form);
    $form = str_replace('[group]', $group, $form);
    return $form;
  }

  function updateWordCensor()
  {
    global $dbConn, $template, $msg, $request_vars;
    $word_to_censor = trim($request_vars['word_to_censor']);
    $replace_with = trim($request_vars['replace_with']);
    $id = trim($request_vars['censor_id']);
    if (($id == "") || ($word_to_censor == "") || ($replace_with == ""))
    {
      $msg =
      '<div id="errMsg">There was a problem editing the replace_with - no changes made.</div>';
    }
    else
    {
      $sql ='UPDATE `wordcensor` SET `word_to_censor` = :word_to_censor,`replace_with`= :replace_with WHERE `censor_id`= :id LIMIT 1';
      $params = array(
        ':word_to_censor' => $word_to_censor,
        ':replace_with' => $replace_with,
        ':id' => $id
      );
      $result = db_write($sql, $params, false, __FILE__, __FUNCTION__, __LINE__);
      if ($result > 0)
      {
        $msg = '<div id="successMsg">Correction edited.</div>';
      }
      else
      {
        $msg =
        '<div id="errMsg">There was a problem editing the replace_with - no changes made.</div>';
      }
    }
  }
