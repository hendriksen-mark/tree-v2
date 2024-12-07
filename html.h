String Get_http_content(){
    String http_content = "<!doctype html>";
    http_content += "<html>";

    http_content += "<head>";
    http_content += "<meta charset=\"utf-8\">";
    http_content += "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">";
    http_content += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
    http_content += "<title>Tree v2 by Evil Genius Labs</title>";

    //http_content += "<!-- request CSS from internet CDN -->";
    http_content += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\">";
    http_content += "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/jquery-minicolors/2.2.4/jquery.minicolors.min.css\"/>";

    //http_content += "<!-- request CSS from the ESP8266 web server -->";
    //http_content += "<!-- <link rel=\"stylesheet\" href=\"https://raw.githubusercontent.com/hendriksen-mark/tree-v2/refs/heads/master/data/js/bootstrap.min.js\"> -->";
    //http_content += "<!-- <link rel=\"stylesheet\" href=\"https://raw.githubusercontent.com/hendriksen-mark/tree-v2/refs/heads/master/data/js/jquery.minicolors.min.js\"> -->";

    http_content += "<link rel=\"stylesheet\" href=\"https://raw.githubusercontent.com/hendriksen-mark/tree-v2/refs/heads/master/data/css/styles.css\">";

    http_content += "<link rel=\"icon\" href=\"https://github.com/hendriksen-mark/tree-v2/blob/master/data/images/atom196.png?raw=true\">";
    http_content += "</head>";

    http_content += "<body>";

    http_content += "<nav class=\"navbar navbar-default navbar-static-top\" id=\"top\" role=\"banner\">";
    http_content += "<div class=\"container\">";
    http_content += "<div class=\"navbar-header\">";
    http_content += "<button type=\"button\" class=\"navbar-toggle collapsed\" data-toggle=\"collapse\" data-target=\"#navbar-collapse-1\" aria-expanded=\"false\">";
    http_content += "<span class=\"sr-only\">Toggle navigation</span>";
    http_content += "<span class=\"icon-bar\"></span>";
    http_content += "<span class=\"icon-bar\"></span>";
    http_content += "<span class=\"icon-bar\"></span>";
    http_content += "</button>";
    http_content += "<a class=\"navbar-brand\" href=\"https://www.evilgeniuslabs.org\" target=\"_blank\"><img src=\"https://evilgeniuslabs.org/images/atom.svg\" style=\"width: 24px; height: 24px;\" /></a>";
    http_content += "<a class=\"navbar-brand\" href=\"https://www.evilgeniuslabs.org\" target=\"_blank\">Evil Genius Labs</a>";
    http_content += "</div>";
    http_content += "<div class=\"collapse navbar-collapse\" id=\"navbar-collapse-1\">";
    http_content += "<ul class=\"nav navbar-nav\">";
    http_content += "<li class=\"active\"><a href=\"/\">Tree v2 <span class=\"sr-only\">(current)</span></a></li>";
    http_content += "<li><a href=\"https://raw.githubusercontent.com/hendriksen-mark/tree-v2/refs/heads/master/data/simple.htm\" target=\"_blank\" title=\"Simple Mode\">Simple</a></li>";
    http_content += "<li><a href=\"https://raw.githubusercontent.com/hendriksen-mark/tree-v2/refs/heads/master/data/edit.htm\" target=\"_blank\" title=\"Edit Files\">Files</a></li>";
    http_content += "<li><a href=\"/update\" target=\"_blank\" title=\"Update Firmware\">Firmware</a></li>";
    http_content += "</ul>";
    http_content += "<ul class=\"nav navbar-nav navbar-right\">";
    http_content += "<li>";
    http_content += "<a href=\"https://github.com/evilgeniuslabs/tree-v2\" title=\"Tree v2 on GitHub\" target=\"_blank\">";
    http_content += "<img style=\"height: 16px;\" src=\"https://github.com/favicon.ico\">";
    http_content += "</a>";
    http_content += "</li>";
    http_content += "</ul>";
    http_content += "</div>";
    http_content += "</div>";
    http_content += "</nav>";

    http_content += "<div id=\"container\" class=\"container\">";

    http_content += "<form class=\"form-horizontal\" id=\"form\">";
    //http_content += "<!-- <div class=\"form-group\">";
    //http_content += "<div class=\"col-sm-1 col-sm-offset-2\">";
    //http_content += "<a href=\"/\" class=\"btn btn-default\" aria-label=\"Refresh\" title=\"Refresh\">";
    //http_content += "<span class=\"glyphicon glyphicon-refresh\" id=\"btnRefresh\"></span>";
    //http_content += "</a>";
    //http_content += "</div>";
    //http_content += "<div class=\"col-sm-9\">";
    //http_content += "<p id=\"status\" class=\"form-control-static\">Loading, please wait...</p>";
    //http_content += "</div>";
    //http_content += "</div> -->";
    http_content += "</form>";

    http_content += "</div>";

    http_content += "<div id=\"templates\" style=\"display: none\">";

    http_content += "<div id=\"sectionTemplate\" class=\"form-group\">";
    http_content += "<div class=\"col-sm-12\">";
    http_content += "<hr style=\"margin-bottom: 5px;margin-top: 5px;\" />";
    http_content += "</div>";
    http_content += "</div>";

    http_content += "<div id=\"numberTemplate\" class=\"form-group\">";
    http_content += "<label class=\"col-sm-2 control-label\"></label>";
    http_content += "<div class=\"col-sm-2\">";
    http_content += "<input class=\"form-control input\" type=\"number\" step=\"1\" min=\"0\" max=\"255\" />";
    http_content += "</div>";
    http_content += "<div class=\"col-sm-8\">";
    http_content += "<input class=\"form-control slider\" type=\"range\" step=\"1\" min=\"0\" max=\"255\" />";
    http_content += "</div>";
    http_content += "</div>";

    http_content += "<div id=\"booleanTemplate\" class=\"form-group\">";
    http_content += "<label class=\"col-sm-2 control-label\"></label>";
    http_content += "<div class=\"col-sm-10\">";
    http_content += "<div class=\"btn-group\" role=\"group\">";
    http_content += "<button type=\"button\" class=\"btn btn-default\" id=\"btnOn\">On</button>";
    http_content += "<button type=\"button\" class=\"btn btn-default\" id=\"btnOff\">Off</button>";
    http_content += "</div>";
    http_content += "</div>";
    http_content += "</div>";

    http_content += "<div id=\"selectTemplate\" class=\"form-group\">";
    http_content += "<label class=\"col-sm-2 control-label\"></label>";
    http_content += "<div class=\"col-sm-8\">";
    http_content += "<select class=\"form-control\"></select>";
    http_content += "</div>";
    http_content += "<div class=\"col-sm-2\">";
    http_content += "<div class=\"btn-group\" role=\"group\" aria-label=\"...\">";
    http_content += "<button type=\"button\" class=\"btn btn-default btn-previous\"";
    http_content += "aria-label=\"Previous\" title=\"Previous\">";
    http_content += "<span class=\"glyphicon glyphicon-chevron-left\"></span>";
    http_content += "</button>";
    http_content += "<button type=\"button\" class=\"btn btn-default btn-next\"";
    http_content += "aria-label=\"Next\" title=\"Next\">";
    http_content += "<span class=\"glyphicon glyphicon-chevron-right\"></span>";
    http_content += "</button>";
    http_content += "</div>";
    http_content += "</div>";
    http_content += "</div>";

    http_content += "<div id=\"colorPaletteTemplate\" class=\"form-group\">";
    http_content += "<label class=\"col-sm-2 control-label color-label\"></label>";
    http_content += "<div class=\"col-sm-10\">";
    http_content += "<div class=\"btn-group btn-group-justified\" role=\"group\">";
    http_content += "<div class=\"btn-group\" role=\"group\">";
    http_content += "<button type=\"button\" class=\"btn btn-default btn-color\" style=\"background: #FF0000;\" title=\"Red\">&nbsp;</button>";
    http_content += "</div>";
    http_content += "<div class=\"btn-group\" role=\"group\">";
    http_content += "<button type=\"button\" class=\"btn btn-default btn-color\" style=\"background: #FF8000;\" title=\"Orange\">&nbsp;</button>";
    http_content += "</div>";
    http_content += "<div class=\"btn-group\" role=\"group\">";
    http_content += "<button type=\"button\" class=\"btn btn-default btn-color\" style=\"background: #FFFF00;\" title=\"Yellow\">&nbsp;</button>";
    http_content += "</div>";
    http_content += "<div class=\"btn-group\" role=\"group\">";
    http_content += "<button type=\"button\" class=\"btn btn-default btn-color\" style=\"background: #80FF00;\" title=\"Chartreuse\">&nbsp;</button>";
    http_content += "</div>";
    http_content += "<div class=\"btn-group\" role=\"group\">";
    http_content += "<button type=\"button\" class=\"btn btn-default btn-color\" style=\"background: #00FF00;\" title=\"Green\">&nbsp;</button>";
    http_content += "</div>";
    http_content += "<div class=\"btn-group\" role=\"group\">";
    http_content += "<button type=\"button\" class=\"btn btn-default btn-color\" style=\"background: #00FF80;\" title=\"Spring Green\">&nbsp;</button>";
    http_content += "</div>";
    http_content += "<div class=\"btn-group\" role=\"group\">";
    http_content += "<button type=\"button\" class=\"btn btn-default btn-color\" style=\"background: #00FFFF;\" title=\"Cyan\">&nbsp;</button>";
    http_content += "</div>";
    http_content += "<div class=\"btn-group\" role=\"group\">";
    http_content += "<button type=\"button\" class=\"btn btn-default btn-color\" style=\"background: #0080FF;\" title=\"Azure\">&nbsp;</button>";
    http_content += "</div>";
    http_content += "<div class=\"btn-group\" role=\"group\">";
    http_content += "<button type=\"button\" class=\"btn btn-default btn-color\" style=\"background: #0000FF;\" title=\"Blue\">&nbsp;</button>";
    http_content += "</div>";
    http_content += "<div class=\"btn-group\" role=\"group\">";
    http_content += "<button type=\"button\" class=\"btn btn-default btn-color\" style=\"background: #8000FF;\" title=\"Violet\">&nbsp;</button>";
    http_content += "</div>";
    http_content += "<div class=\"btn-group\" role=\"group\">";
    http_content += "<button type=\"button\" class=\"btn btn-default btn-color\" style=\"background: #FF00FF;\" title=\"Magenta\">&nbsp;</button>";
    http_content += "</div>";
    http_content += "<div class=\"btn-group\" role=\"group\">";
    http_content += "<button type=\"button\" class=\"btn btn-default btn-color\" style=\"background: #FF0080;\" title=\"Rose\">&nbsp;</button>";
    http_content += "</div>";
    http_content += "<div class=\"btn-group\" role=\"group\">";
    http_content += "<button type=\"button\" class=\"btn btn-default btn-color\" style=\"background: #FFFFFF;\" title=\"White\">&nbsp;</button>";
    http_content += "</div>";
    http_content += "</div>";
    http_content += "</div>";
    http_content += "</div>";

    http_content += "<div id=\"colorTemplate\">";
    http_content += "<div class=\"form-group\">";
    //http_content += "<!-- <label class=\"col-sm-2 control-label color-label\"></label> -->";
    http_content += "<div class=\"col-sm-12 col-sm-offset-2\">";
    http_content += "<input type=\"text\" class=\"form-control minicolors\" />";
    http_content += "</div>";
    http_content += "</div>";
    http_content += "<div class=\"form-group\">";
    http_content += "<label class=\"col-sm-2 control-label\">Red</label>";
    http_content += "<div class=\"col-sm-2\">";
    http_content += "<input class=\"form-control color-red-input\" type=\"number\" step=\"1\" min=\"0\" max=\"255\" />";
    http_content += "</div>";
    http_content += "<div class=\"col-sm-8\">";
    http_content += "<input class=\"form-control color-red-slider\" type=\"range\" step=\"1\" min=\"0\" max=\"255\" />";
    http_content += "</div>";
    http_content += "</div>";
    http_content += "<div class=\"form-group\">";
    http_content += "<label class=\"col-sm-2 control-label\">Green</label>";
    http_content += "<div class=\"col-sm-2\">";
    http_content += "<input class=\"form-control color-green-input\" type=\"number\" step=\"1\" min=\"0\" max=\"255\" />";
    http_content += "</div>";
    http_content += "<div class=\"col-sm-8\">";
    http_content += "<input class=\"form-control color-green-slider\" type=\"range\" step=\"1\" min=\"0\" max=\"255\" />";
    http_content += "</div>";
    http_content += "</div>";
    http_content += "<div class=\"form-group\">";
    http_content += "<label class=\"col-sm-2 control-label\">Blue</label>";
    http_content += "<div class=\"col-sm-2\">";
    http_content += "<input class=\"form-control color-blue-input\" type=\"number\" step=\"1\" min=\"0\" max=\"255\" />";
    http_content += "</div>";
    http_content += "<div class=\"col-sm-8\">";
    http_content += "<input class=\"form-control color-blue-slider\" type=\"range\" step=\"1\" min=\"0\" max=\"255\" />";
    http_content += "</div>";
    http_content += "</div>";
    http_content += "</div>";

    http_content += "</div>";

    http_content += "<nav class=\"navbar navbar-default navbar-fixed-bottom\">";
    http_content += "<div class=\"container\">";
    http_content += "<div class=\"navbar-header\">";
    http_content += "<button type=\"button\" class=\"navbar-toggle collapsed\" data-toggle=\"collapse\" data-target=\"#navbar-collapse-2\" aria-expanded=\"false\">";
    http_content += "<span class=\"sr-only\">Toggle navigation</span>";
    http_content += "<span class=\"icon-bar\"></span>";
    http_content += "<span class=\"icon-bar\"></span>";
    http_content += "<span class=\"icon-bar\"></span>";
    http_content += "</button>";
    http_content += "</div>";
    http_content += "<div class=\"collapse navbar-collapse\" id=\"navbar-collapse-2\">";
    http_content += "<ul class=\"nav navbar-nav\">";
    http_content += "<li>";
    http_content += "<a href=\"/\" aria-label=\"Refresh\" title=\"Refresh\">";
    http_content += "<span class=\"glyphicon glyphicon-refresh\" id=\"btnRefresh\"></span>";
    http_content += "</a>";
    http_content += "</li>";
    http_content += "<li><p class=\"navbar-text\" id=\"status\">Loading, please wait...</p></li>";
    http_content += "</ul>";
    http_content += "</div>";
    http_content += "</div>";
    http_content += "</nav>";

    //http_content += "<!-- request js from internet CDN -->";
    http_content += "<script src=\"https://code.jquery.com/jquery-3.1.1.min.js\"></script>";
    http_content += "<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\"></script>";
    http_content += "<script src=\"https://cdnjs.cloudflare.com/ajax/libs/jquery-minicolors/2.2.4/jquery.minicolors.min.js\"></script>";
    //http_content += "<!-- <script src=\"https://cdnjs.cloudflare.com/ajax/libs/reconnecting-websocket/1.0.0/reconnecting-websocket.min.js\"></script> -->";

    //http_content += "<!-- request js from the ESP8266 web server -->";
    //http_content += "<!-- <script src=\"js/jquery-3.1.1.min.js\"></script> -->";
    //http_content += "<!-- <script src=\"js/bootstrap.min.js\"></script> -->";
    //http_content += "<!-- <script src=\"js/jquery.minicolors.min.js\"></script> -->";
    //http_content += "<!-- <script src=\"js/r-websocket.min.js\"></script> -->";

    http_content += "<script src=\"https://raw.githubusercontent.com/hendriksen-mark/tree-v2/refs/heads/master/data/js/app.js\"></script>";

    http_content += "</body>";

    http_content += "</html>";
    return http_content;
};