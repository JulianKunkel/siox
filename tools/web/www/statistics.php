<?php
require_once("include/DB.php"); 
require_once("include/Activity.php"); 
require_once("include/Program.php"); 
require_once("include/Stats.php"); 
$site_title = "Statistics overview";
$site_description = "Overview of the stored statistics.";
require_once("header.php"); 

//if (!isset($_GET['start']) || !isset($_GET['stop']))
//	die("Undefined time frame.");

$start = $_GET['start'];
$stop  = $_GET['stop'];

$start = 0;
$stop  = 0;

$nid  = $_GET['nid'];
$pid  = $_GET['pid'];
$time = $_GET['time'];
$pnum = $_GET['pnum'];

$stats_list = Stats::get_list();
?>

<h1>Statistics</h1>

<form name="runs_frm" method="post" action="index.php" style="float: left;">
	<input type="submit" value="Execution Overview" />
</form>
<form name="acts_frm" method="post" action="activities.php?nid=<?php echo $nid?>&amp;pid=<?php echo $pid?>&amp;time=<?php echo $time?>&amp;pnum=<?php echo $pnum?>" style="float: left">
	<input type="submit" value="Activities Overview" />
</form>

<br /><br/>

<?php foreach ($stats_list as $stat): ?>

<h2><?php echo $stat->childname?></h2>

<img src="plot.php?x=timestamp&amp;y=<?php echo $stat->childobjectid?>&amp;start=<?php echo $start?>&amp;stop=<?php echo $stop?>" />

<?php endforeach ?>

<br /><br />

<?php require_once("footer.php"); ?>
