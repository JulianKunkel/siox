<?php 

require_once("include/DB.php"); 
require_once("include/Stats.php"); 
require_once("include/GNUPlot.php"); 

/*
$x = isset($_GET["x"]) ? $_GET["x"] : 'elapsed_secs';
$y = isset($_GET["y"]) ? $_GET["y"] : 'rd_bw';
$c = isset($_GET["c"]) ? $_GET["c"] : '0';
$o = isset($_GET["o"]) ? $_GET["o"] : '0';
$z = isset($_GET["z"]) ? $_GET["z"] : '1000';
$cond = isset($_GET["cond"]) ? base64_decode($_GET["cond"]) : '';

$tmp_dir = $conf["plot_tmp_dir"];

$data_files = array();

foreach (Stats::lun_values($c) as $lun) 
	$data_files["LUN $lun"] = Stats::store_metric($x, $y, $c, $lun, $o, $z, $tmp_dir, $cond);

$plot = new GNUPlot($tmp_dir);
$plot->set("set title \"Controller $c\"");
$plot->set("set xlabel \"$x\"");
$plot->set("set ylabel \"$y\"");
$plot->set("set grid ytics");
$plot->set("set border 3");
$plot->set("set xtics rotate by -45");
$plot->set("set key horizontal outside bottom");

$plot_cmds = array();
foreach ($data_files as $title => $data_file) {
	$plot_cmds[] = "\"$data_file\" u 1:2 w points t \"$title\"";
}
$plot->set("plot ".implode(', ', $plot_cmds));
$plot->render();

foreach ($data_files as $data_file)
	Stats::destroy_metric($data_file);
*/
?>
