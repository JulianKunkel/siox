<?php

require_once("include/DB.php"); 
require_once("include/Activity.php"); 
require_once("include/Program.php"); 
require_once("include/Stats.php");
$site_title = "Comparison of Energy Statistics";
$site_description = "";
require_once("header.php"); 

?>

<h1>Statistic Comparison</h1>

<?php 

$progids = $_POST['progs'];

if (empty($progids))
	die("Error: No program selected.");

$proglist  = array();
$startvals = array();
$stopvals  = array();
foreach ($progids as $pid) {
	$p = Program::get($pid); 
	$startvals[] = $p->times['start'];
	$stopvals[]  = $p->times['stop'];
	$proglist[]  = $p;
}
$startvals_ser = serialize($startvals);
$stopvals_ser = serialize($stopvals);

$startvals_enc = urlencode($startvals_ser);
$stopvals_enc = urlencode($stopvals_ser);

$statids = $_POST['stats'];
if (empty($statids))
	die("Error: No statistics selected.");

$stats = Stats::get_list($statids); 
?>

<?php foreach ($stats as $s): ?>

<h2><?=$s->childname?></h2>

<img src="plot_comp.php?x=timestamp&amp;y=<?=$s->childobjectid?>&amp;start=<?=$startvals_enc?>&amp;stop=<?=$stopvals_enc?>" />

<?php endforeach ?>
 
<?php require_once("footer.php"); ?>

