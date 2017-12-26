<?PHP
  /***************************************
    * http://www.program-o.com
    * PROGRAM O
    * Version: 2.5.3
    * FILE: botpersonality.php
    * AUTHOR: Elizabeth Perreau and Dave Morton
    * DATE: 05-26-2014
    * DETAILS: Displays predicate values for the current chatbot
    ***************************************/
  # set template section defaults
  # Build page sections
  # ordered here in the order that the page is constructed
  $post_vars = filter_input_array(INPUT_POST);
  $bot_name = (isset ($_SESSION['poadmin']['bot_name'])) ? $_SESSION['poadmin']['bot_name'] : 'unknown';
  $func = (isset ($post_vars['func'])) ? $post_vars['func'] : 'getBot';
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
  $pageTitle = 'My-Program O - Bot Personality';
  $mainContent = "main content";
  switch ($func)
  {
    case 'updateBot' :
    case 'addBotPersonality' :
      $msg = $func();
      $mainContent = getBot();
      break;
    default :
      $mainContent = $func();
  }
  $mainTitle = 'Bot Personality Settings for ' . $bot_name;

  /**
   * Function getBot
   *
   *
   * @return string
   */
  function getBot()
  {
    $formCell =
'                <td>
                   <label for="[row_label]">
                     <span class="label">[row_label]:</span>
                     <span class="formw">
                       <input name="[row_label]" id="[row_label]" value="[row_value]" />
                     </span>
                   </label>
                 </td>
';
    $blankCell =
'                <td style="text-align: center">
                   <label for="newEntryName[cid]">
                     <span class="label">
                       New Entry Name: <input name="newEntryName[cid]" id="newEntryName[cid]" style="width: 98%" />
                     </span>
                   </label>&nbsp;
                   <label for="newEntryValue[cid]" style="float: left; padding-left: 3px;">
                     <span class="formw">New Entry Value: </span>
                     <input name="newEntryValue[cid]" id="newEntryValue[cid]" />
                   </label>
                 </td>
';
    $startDiv = '      <td>' . "\n        ";
    $endDiv = "\n      </td>\n      <br />\n";
    $inputs = "";
    $row_class = 'row fm-opt';
    $bot_name = $_SESSION['poadmin']['bot_name'];
    $bot_id = (isset ($_SESSION['poadmin']['bot_id'])) ? $_SESSION['poadmin']['bot_id'] : 0;
    $bot_id = ($bot_id == 'new') ? 0 : $bot_id;
    //get the current bot's personality table from the db
    $sql = "SELECT * FROM `botpersonality` where  `bot_id` = $bot_id";
    $rows = db_fetchAll($sql, null, __FILE__, __FUNCTION__, __LINE__);
      $rowCount = count($rows);
      if ($rowCount > 0)
      {
        $left = true;
        $colCount = 0;
        foreach ($rows as $row)
        {
          $rid = $row['id'];
          $label = $row['name'];
          $value = stripslashes_deep($row['value']);
          $tmpRow = str_replace('[row_class]', $row_class, $formCell);
          $tmpRow = str_replace('[row_id]', $rid, $tmpRow);
          $tmpRow = str_replace('[row_label]', $label, $tmpRow);
          $tmpRow = str_replace('[row_value]', $value, $tmpRow);
          $inputs .= $tmpRow;
          $colCount++;
          if ($colCount >= 3)
          {
            $inputs .= '              </tr>
              <tr>' . PHP_EOL;
            $colCount = 0;
          }
        }
        $inputs .= "<!-- colCount = $colCount -->\n";
        if (($colCount > 0) and ($colCount < 3))
        {
          for ($n = 0; $n < (3 - $colCount); $n++)
          {
            $addCell = str_replace('[cid]', "[$n]", $blankCell);
            $inputs .= $addCell;
          }
        }
        $action = 'Update Data';
        $func = 'updateBot';
      }
      else
      {
        $inputs = newForm();
        $action = 'Add New Data';
        $func = 'addBotPersonality';
      }
    if (empty ($func))
      $func = 'getBot';
    $form = <<<endForm2
          <form name="botpersonality" action="index.php?page=botpersonality" method="post">
            <table class="botForm">
              <tr>
$inputs
              </tr>
              <tr>
                <td colspan="3">
                  <input type="hidden" id="bot_id" name="bot_id" value="$bot_id">
                  <input type="hidden" id="func" name="func" value="$func">
                  <input type="submit" name="action" id="action" value="$action">
                </td>
              </tr>
            </table>
          </form>
  <!-- fieldset>
  </fieldset -->
endForm2;
      return $form;
  }

  /**
   * Function stripslashes_deep
   *
   * * @param $value
   * @return string
   */
  function stripslashes_deep($value)
  {
    $newValue = stripslashes($value);
    while ($newValue != $value)
    {
      $value = $newValue;
      $newValue = stripslashes($value);
    }
    return $newValue;
  }

  /**
   * Function updateBot
   *
   *
   * @return string
   */
  function updateBot()
  {
    global $bot_id, $bot_name, $post_vars;
    $msg = "";
    if (!empty ($post_vars['newEntryName']))
    {
      $newEntryNames = $post_vars['newEntryName'];
      $newEntryValues = $post_vars['newEntryValue'];
      $sql = "Insert into `botpersonality` (`id`, `bot_id`, `name`, `value`) values (null, $bot_id, :name, :value);";
      $params = array();
      foreach ($newEntryNames as $index => $key)
      {
        $value = $newEntryValues[$index];
        if (empty ($value)) continue;
        $params[] = array(':name' => $key, ':value' => $value);
      }
      $rowsAffected = db_write($sql, $params, true, __FILE__, __FUNCTION__, __LINE__);
      if ($rowsAffected > 0)
      {
        $msg = (empty ($msg)) ? "Bot personality added. \n" : $msg;
      }
      else
      {
        $msg = 'Error updating bot personality.';
      }
    }
    $sql = "SELECT * FROM `botpersonality` where `bot_id` = $bot_id;";
    $result = db_fetchAll($sql, null, __FILE__, __FUNCTION__, __LINE__);
    $rows = array();
    $insertParams = array();
    $updateParams = array();
    foreach ($result as $row)
    {
      $name = $row['name'];
      $value = $row['value'];
      $rows[$name] = array('id' => $row['id'], 'value' => $value);
    }
    $insertSQL = "Insert into `botpersonality` (`id`, `bot_id`, `name`, `value`) values (null, $bot_id, :name, :value);";
    $updateSQL = "update `botpersonality` set `value` = :value where `id` = :id;";
    $exclude = array('bot_id', 'func', 'action', 'newEntryName', 'newEntryValue');
    $values = '';
    foreach ($post_vars as $key => $value)
    {
      if (in_array($key, $exclude)) continue;
      if (!isset($rows[$key]))
      {
        $insertParams[] = array(':name' => $key, ':value' => $value);
      }
      else
      {
        $oldValue = $rows[$key]['value'];
        if ($value != $oldValue)
        {
          $curId = $rows[$key]['id'];
          $updateParams[] = array(':value' => $value, ':id' => $curId);
        }
      }
    }
    if (empty($insertParams) && empty($updateParams)) return 'No changes found.';
    $affectedRows = (!empty($updateParams)) ? db_write($updateSQL, $updateParams, true, __FILE__, __FUNCTION__, __LINE__) : 0;
    $affectedRows += (!empty($updateParams)) ? db_write($insertSQL, $insertParams, true, __FILE__, __FUNCTION__, __LINE__) : 0;
    if ($affectedRows > 0) $msg = 'Bot Personality Updated.';
    else $msg = "Something went wrong! Affected rows = $affectedRows.";
    return $msg;
  }

  /**
   * Function addBotPersonality
   *
   *
   * @return string
   */
  function addBotPersonality()
  {
    global $post_vars;
    $bot_id = $post_vars['bot_id'];
    $sql = "Insert into `botpersonality` (`id`, `bot_id`, `name`, `value`) values (null, $bot_id, :name, :value);";
    $msg = "";
    $params = array();
    $newEntryNames = (isset ($post_vars['newEntryName'])) ? $post_vars['newEntryName'] : '';
    $newEntryValues = (isset ($post_vars['newEntryValue'])) ? $post_vars['newEntryValue'] : '';
    if (!empty ($newEntryNames))
    {
      if (is_string($newEntryNames))
      {
        $newEntryNames = array(0 => $newEntryNames);
      }
      foreach ($newEntryNames as $index => $key)
      {
        $value = trim($newEntryValues[$index]);
        if (!empty ($value))
        {
          $params[] = array(':name' => $key, ':value' => $value);
        }
      }
    }
    $skipKeys = array('bot_id', 'action', 'func', 'newEntryName', 'newEntryValue');
    $sqlParams = array();
    foreach ($post_vars as $key => $value)
    {
      if (in_array($key, $skipKeys)) continue;
        if (is_array($value))
        {
          foreach ($value as $index => $fieldValue)
          {
            $field = $key[$fieldValue];
            $fieldValue = trim($fieldValue);
            $params[] = array(':name' => $field, ':value' => $fieldValue);
          }
          continue;
        }
        else
        {
          $value = trim($value);
          $params[] = array(':name' => $key, ':value' => $value);
        }
    }
    $rowsAffected = db_write($sql, $params, true, __FILE__, __FUNCTION__, __LINE__);
    if ($rowsAffected > 0)
    {
      $msg = (empty ($msg)) ? "Bot personality added. \n" : $msg;
    }
    else
    {
      $msg = 'Error updating bot personality.';
    }
    return $msg;
  }

  /**
   * Function newForm
   *
   *
   * @return string
   */
  function newForm()
  {
    $out = '                <table class="botForm">
                  <tr>
';
    $rowTemplate =
    '                    <td><label for="[field]"><span class="label">[uc_field]:</span></label> <span class="formw"><input name="[field]" id="[field]" value="" /></span></td>
';
    $tr = '                  </tr>
                  <tr>
';
    $blankTD = '                    <td>&nbsp;</td>
';
    $lastBit =
    '                  </tr>
                  <tr>
                    <td style="text-align: center"><label for="newEntryName[0]"><span class="label">New Entry Name: <input name="newEntryName[0]" id="newEntryName[0]" style="width: 98%" /></label></span>&nbsp;<span class="formw"><label for="newEntryValue[0]" style="float: left; padding-left: 3px;">New Entry Value: </label><input name="newEntryValue[0]" id="newEntryValue[0]" /></span></td>
                    <td style="text-align: center"><label for="newEntryName[1]"><span class="label">New Entry Name: <input name="newEntryName[1]" id="newEntryName[1]" style="width: 98%" /></label></span>&nbsp;<span class="formw"><label for="newEntryValue[1]" style="float: left; padding-left: 3px;">New Entry Value: </label><input name="newEntryValue[1]" id="newEntryValue[1]" /></span></td>
                    <td style="text-align: center"><label for="newEntryName[2]"><span class="label">New Entry Name: <input name="newEntryName[2]" id="newEntryName[2]" style="width: 98%" /></label></span>&nbsp;<span class="formw"><label for="newEntryValue[2]" style="float: left; padding-left: 3px;">New Entry Value: </label><input name="newEntryValue[2]" id="newEntryValue[2]" /></span></td>
                  </tr>
                </table>
';
    $fields = file(_CONF_PATH_ . 'default_botpersonality_fields.dat');
    $count = 0;
    foreach ($fields as $field)
    {
      $count++;
      $field = trim($field);
      $tmpRow = str_replace('[field]', $field, $rowTemplate);
      $tmpRow = str_replace('[uc_field]', ucfirst($field), $tmpRow);
      $out .= $tmpRow;
      if ($count % 3 == 0)
        $out .= $tr;
    }
    switch ($count % 3)
    {
      case 1 :
        $out .= $blankTD;
        break;
      case 2 :
        $out .= $blankTD . $blankTD;
    }
    $out .= $lastBit;
    return $out;
  }
