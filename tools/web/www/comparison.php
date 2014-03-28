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
$labels    = array();
foreach ($progids as $pid) {
	$p = Program::get($pid); 
	$startvals[] = $p->times['start'];
	$stopvals[]  = $p->times['stop'];
	$labels[]    = $p->childobjectid;
	$proglist[]  = $p;
}
$startvals_ser = serialize($startvals);
$stopvals_ser  = serialize($stopvals);
$labels_ser    = serialize($labels);
$startvals_enc = urlencode($startvals_ser);
$stopvals_enc  = urlencode($stopvals_ser);
$labels_enc    = urlencode($labels_ser);

$statids = $_POST['stats'];
if (empty($statids))
	die("Error: No statistics selected.");

$stats = Stats::get_list($statids); 
$cumstats = Stats::get_list(array('energy/'));

?>

<form name="comp_frm" method="post" action="">
	<input type="submit" value="Program overview" onclick="return action='index.php'" />
</form>

<h2>Comparing the following program runs:</h2>

<?php foreach ($proglist as $p): ?>
<b>[<?=$p->childobjectid?>]:</b> <?=$p->attributes['description/commandLine'];?><br />
<?php endforeach ?>

<h2>Summaries</h2>

<table>
	<tr>
		<th>ID</th>
		<th>Duration</th>
<?php foreach ($cumstats as $cs):?>
		<th><?=$cs->childname?></th>
<?php endforeach ?>
	</tr>
<?php foreach ($proglist as $p): ?>
	<tr>
		<td><?=$p->childobjectid?></td>
		<td><?=round(($p->times["stop"] - $p->times["start"]) / 1000000000, 3)?> s</td>
<?php foreach ($cumstats as $cs): ?>
		<td><?=Stats::get_cumulative($cs->childobjectid, $p->times['start'], $p->times['stop'])?></td>
<?php endforeach ?>		
	</tr>
<?php endforeach ?>
</table>


<?php foreach ($stats as $s): ?>

<h2><?=$s->childname?></h2>

<img src="plot_comp.php?x=timestamp&amp;y=<?=$s->childobjectid?>&amp;start=<?=$startvals_enc?>&amp;stop=<?=$stopvals_enc?>&amp;label=<?=$labels_enc?>" />

<?php endforeach ?>
 
<?php require_once("footer.php"); ?>

