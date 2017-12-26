<?php

  /**
   * The Template class creates a Template object from a supplied filename which can then
   * be used to return template sections; strings of HTML code used to display data within
   * the script.
   *
   * @author Dave Morton
   */
  class Template {

    /**
     * Generates a template object, based on the file name passed to the class
     *
     * @param $file
     * @return \Template
     */
    public function __construct($file) {
      if (!file_exists($file)) return $this->throwError("File $file does not exist!");
      $this->file = $file;
      $this->rawTemplate = file_get_contents($file, true);
      $this->sectionStart = '<!-- Section [section] Start -->';
      $this->sectionEnd = '<!-- Section [section] End -->';
      return $this;
    }

    /**
     * Returns a section of the template file, based on the section name, and whether or not
     * to return anything if the section's start tag is not found (handy for getting the first
     * section of the file)
     *
     * @param $sectionName
     * @param bool $notFoundReturn
     * @return string
     */
    public function getSection($sectionName, $notFoundReturn = false) {
      $sectionStart = $this->sectionStart;
      $sectionEnd = $this->sectionEnd;
      $rawTemplate = $this->rawTemplate;
      $start = str_replace('[section]', $sectionName, $sectionStart);
      $sectionStartLen = strlen($start);
      $end   = str_replace('[section]', $sectionName, $sectionEnd);
      $startPos = strpos($rawTemplate, $start, 0);
      if ($startPos === false) {
        if ($notFoundReturn) {
          return "\n";
        }
        else $startPos = 0;
      }
      else $startPos += $sectionStartLen;
      $endPos = strpos($rawTemplate, $end, $startPos) - 1;
      $sectionLen = $endPos - $startPos;
      $out = substr($rawTemplate, $startPos, $sectionLen);
      return trim($out);
    }

    /**
     * exits the script with the error message passed
     * @todo: exit() is a bad idea - a more user-friendly solution needs to be implemented.
     *
     * @param $message
     * @return void
     */
    protected function throwError($message) {
      exit($message);
    }
  }
?>