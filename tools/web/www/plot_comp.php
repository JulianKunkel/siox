<?php 

require_once("include/DB.php"); 
require_once("include/Stats.php"); 
require_once("include/GNUPlot.php"); 

if (!isset($_GET["x"]) || !isset($_GET["y"]))
	die("Undefined (x,y) axes.");

$x = $_GET["x"];
$y = $_GET["y"];

$startvals_enc = $_GET["start"];
$stopvals_enc  = $_GET["stop"];
$labels_enc    = $_GET["label"];

$startvals_ser = urldecode($startvals_enc);
$stopvals_ser  = urldecode($stopvals_enc);
$labels_ser    = urldecode($labels_enc);

$startvals = unserialize($startvals_ser);
$stopvals  = unserialize($stopvals_ser);
$labels    = unserialize($labels_ser);

$tmp_dir = "/var/www/siox/tmp";
$data_files = array();
for ($i = 0; $i < count($startvals); $i++) {
	$data_files[] = Stats::store_data($x, $y, $tmp_dir, $startvals[$i], $stopvals[$i]);
}
$plot = new GNUPlot($tmp_dir);
$plot->set("set xlabel \"Time\"");
$plot->set("set grid ytics");
$plot->set("set border 3");
$plot->set("set xtics rotate by -45");
//$plot->set("set key horizontal outside bottom");

$plot_cmd = array();
for ($i = 0; $i < count($data_files); $i++) {
	$file  = $data_files[$i];
	$label = $labels[$i];
	$plot_cmd[] = "'$file' u 1:($2 < 4000 ? $2 : 1/0) w points t '$label'";
}

$plot_cmd = implode(', ', $plot_cmd);
$plot->set("plot $plot_cmd");
$plot->render();

foreach ($data_files as $f)
	Stats::destroy_data($f);

?>
