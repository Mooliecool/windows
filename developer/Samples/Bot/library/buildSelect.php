<?php

  /**
   * Builds HTML  selectbox options from an array of values and labels
   *
   * @AUTHOR   Elizabeth Perreau and Dave Morton
   *
   * @param array  $optionList
   * @param array  $selList
   * @param string $selectText
   * @param bool   $useSelDefault
   * @param bool   $useOnlyValues
   * @param int    $space
   * @return string
   */
  function buildSelect($optionList, $selList = array(), $selectText = '', $useSelDefault = false, $useOnlyValues = false, $space = 2)
  {
    $usd = ($useSelDefault) ? 1 : 0;
    $uov = ($useOnlyValues) ? 1 : 0;
    $out = <<<endDebug
<!-- DEBUG:
selList = $selList
SelectText = $selectText
useSelDefault = $usd
useOnlyValues = $uov
space = $space
-->

endDebug;
      $out = '';
    $padding = str_pad(" ", $space);
    if ($selectText != "")
    {
      $selDefault = (!$useSelDefault) ? '' : ' selected="selected"';
      $out .= "$padding<option $selDefault>$selectText</option>\n";
    }
    foreach ($optionList as $key => $value)
    {
      $sel = (in_array($key, $selList)) ? ' selected="selected"' : '';
      $key = ($useOnlyValues) ? $value : $key;
      $out .= "$padding<option$sel value='$key'>$value</option>\n";
    }
    return rtrim($out);
  }
?>
