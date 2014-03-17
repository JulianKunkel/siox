<?php 

require_once("include/DB.php"); 
require_once("include/Stats.php"); 
require_once("include/GNUPlot.php"); 

if (!isset($_GET["x"]) || !isset($_GET["y"]))
	die("Undefined (x,y) axes.");

$x = $_GET["x"];
$y = $_GET["y"];
$start = $_GET["start"];
$stop  = $_GET["stop"];

$tmp_dir = "/var/www/siox/tmp";
$data_file = Stats::store_data($x, $y, $tmp_dir, $start, $stop);
$plot = new GNUPlot($tmp_dir);
$plot->set("set xlabel \"Time\"");
$plot->set("set grid ytics");
$plot->set("set border 3");
$plot->set("set xtics rotate by -45");
$plot->set("set key horizontal outside bottom");

$plot_cmd = "'$data_file' u 1:2 w points t \"Title\"";

$plot->set("plot $plot_cmd");
$plot->render();

Stats::destroy_data($data_file);

?>
