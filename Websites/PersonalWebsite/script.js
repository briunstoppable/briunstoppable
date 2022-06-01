/**
 *
 * 3D CSS3 Book Generator with jQuery
 *
 * Licensed under the MIT license.
 * http://www.opensource.org/licenses/mit-license.php
 *
 * Copyright 2012, Script Tutorials
 * http://www.script-tutorials.com/
 */

// Goto page function
function gotoPage(i) {
    $('.page').eq(i).removeClass('active');
    if ((i-1) >= 0) {
        $('.page').eq(i-1).addClass('active');
    }
}

// Wrap everything as a jQuery plugin
(function($){
    $.fn.extend({
        EbookTransformer: function(options) {
            var defaults = {
                height: 400
            };
            var options = $.extend(defaults, options);

            // The book object
            var objBook = $(this);

            // Inner variables
            var vPages = new Array();
            var vSides = new Array();
            var vSubObj = new Array();
            var iTmpHeight = 0;

            // initialization function
            init = function() {
                // Walk through all the objects of the book, and prepare Sides (for Pages)
                objBook.children().each(function(i){
                    if (iTmpHeight + this.clientHeight > options.height && vSubObj.length) {
                        vSides.push(vSubObj);
                        vSubObj = new Array();
                        iTmpHeight = 0;
                    }

                    iTmpHeight += this.clientHeight;
                    vSubObj.push(this);
                });

                if (iTmpHeight > 0) {
                    vSides.push(vSubObj);
                }
                $(vSides).wrap('<div class="side"></div>');

                // Prepare Pages (each Page consists of 2 Sides)
                var iPage = 1;
                var vCouples = Array();
                objBook.children().each(function(i){
                    // Add Next and Prev buttons
                    if (vCouples.length == 0) {
                        $(this).append('<button onclick="gotoPage('+iPage+')">Next page</button>');
                    }
                    if (vCouples.length == 1) {
                        $(this).append('<button onclick="gotoPage('+(iPage-1)+')">Previous page</button>');
                    }
                    vCouples.push(this);

                    if (vCouples.length == 2) {
                        vPages.push(vCouples);
                        vCouples = new Array();
                        iPage++;
                    }
                });
                if (vCouples.length == 1) {
                    vCouples.push($('<div class="side"><h2>The end</h2><img src="backcover.JPG" alt="Northern Lights" width="300" height="200"><button onclick="gotoPage('+(iPage-1)+')">Previous page</button></div>')[0]);
                    vPages.push(vCouples);
                }
                $(vPages).wrap('<div class="page"></div>');

                // Add extra CSS for the pages
                var sExtraCSS = '';
                objBook.children().each(function(i){
                    //alert(i); // 0 .. 2
                    sExtraCSS += ''+
                    '.page:nth-child('+(i+1)+') {'+
                    '-moz-transform: translate3d(0px, 0px, -'+i+'px);'+
                    '-webkit-transform: translate3d(0px, 0px, -'+i+'px);'+
                    '}'+
                    '.active:nth-child('+(i+1)+') {'+
                    '-moz-transform: rotateY(-179deg) translate3d(0px, 0px, -'+i+'px);'+
                    '-webkit-transform: rotateY(-179deg) translate3d(0px, 0px, -'+i+'px);'+
                    '}';
                });
                $('.book').append('<style>'+sExtraCSS+'</style>');
            };

            // initialization
            init();
        }
    });
})(jQuery);

// Window onload
jQuery(window).load(function() {
    $('.book').EbookTransformer({height: 480});
});
