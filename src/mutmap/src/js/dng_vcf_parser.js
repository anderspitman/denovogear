// eslint exceptions
//
/* global vcf */
/* exported vcfParser */

var vcfParser = (function() {
  "use strict";

  function parseVCFText(text) {
    var parsed = vcf.parser()(text);
    return parsed;
  }

  return {
    parseVCFText: parseVCFText
  };

}());