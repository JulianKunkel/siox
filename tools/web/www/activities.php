<?php 
require_once("include/DB.php"); 
require_once("include/Activity.php"); 
require_once("include/Error.php");
require_once("include/Program.php"); 
$site_title = "Activity overview";
$site_description = "Overview of the stored activities";
require_once("header.php"); 

$nid  = $_GET['nid'];
$pid  = $_GET['pid'];
$time = $_GET['time'];

?>

<h1>Activity Overview</h1>

<?php 

$page_size = 200; 
$total_activities = Activity::get_count($nid, $pid, $time); 
$total_pages = max(ceil($total_activities / $page_size), 1); 
$current_page = isset($_GET['page']) ? $_GET['page'] : $total_pages; 
$activities = Activity::get_list($nid, $pid, $time, $current_page, $page_size); 

$pnum = $_GET['pnum']; 
$p = Program::get($pnum); 

?>

<form name="purge_frm" method="post" action="purge.php" style="float: left">
	<input type="submit" value="Purge database" onclick="return confirm('You are about to delete all data. Are you sure?')" />
</form>
<form name="runs_frm" method="post" action="index.php" style="float: left;">
	<input type="submit" value="Execution Overview" />
</form>
<form name="stats_frm" method="post" action="statistics.php?start=<?=$p->times['start']?>&amp;stop=<?=$p->times['stop']?>&amp;nid=<?=$nid?>&amp;pid=<?=$pid?>&amp;time=<?=$time?>&amp;pnum=<?=$pnum?>" style="float: left;">
	<input type="submit" value="Time frame statistics" />
</form>


<br /><br />

<h2>Program information</h2>

<table cellspacing="0" cellpadding="0">
	<tr class="even"><th>Program number</th><td><?=$pnum?></td></tr>
	<tr class="odd"><th>Total activities</th><td><?=$total_activities?></td></tr>
	<tr class="odd"><th>Node (nid, pid, time)</th><td><?="$p->node ($p->nid, $p->pid, $p->time)"?></td></tr>
</table>

<h2>Execution Context</h2>

<table cellspacing="0" cellpadding="0">
<?php $i = 0; ?>
<?php foreach ($p->attributes as $k => $v): ?>
	<tr class="<?=$i++ % 2 == 0 ? "even" : "odd"; ?>">
		<th style="text-align: left;"><?=$k?></th><td><?=str_replace(" ", "<br />", $v)?></td>
	</tr>
<?php endforeach ?>
</table>

<h2 id="act_list">Activity List</h2>

<table id="big_list" cellspacing="0" cellpadding="0">
<thead>
	<tr>
		<th colspan="2" style="text-align: left"><a href="?nid=<?=$nid?>&amp;pid=<?=$pid?>&amp;time=<?=$time?>&amp;page=1&amp;pnum=<?=$pnum?>#act_list">↶ first</a>&nbsp;&nbsp;<a href="?nid=<?=$nid?>&amp;pid=<?=$pid?>&amp;time=<?=$time?>&amp;page=<?=$current_page == 1 ? 1 : $current_page-1?>&amp;pnum=<?=$pnum?>#act_list">← previous</a></th>
		<th colspan="2" style="text-align: center"><?=$current_page?> / <?=$total_pages?></th>
		<th colspan="2" style="text-align: right"><a href="?nid=<?=$nid?>&amp;pid=<?=$pid?>&amp;time=<?=$time?>&amp;page=<?=$current_page == $total_pages ? $current_page : $current_page+1?>&amp;pnum=<?=$pnum?>#act_list">next →</a>&nbsp;&nbsp;<a href="?nid=<?=$nid?>&amp;pid=<?=$pid?>&amp;time=<?=$time?>&amp;page=<?=$total_pages?>&amp;pnum=<?=$pnum?>#act_list">last ↷</a></th>
	</tr>
<tr>
		<th>#</th>
		<th>Function</th>
		<th>Time start</th>
		<th>Time stop</th>
		<th>Duration [µs]</th>
		<th>Error code</th>
	</tr>
</thead>
<tbody>
<?php if (count($activities) == 0): ?>
	<tr class="even">
		<td colspan="6" style="text-align: center;">The activity database is empty.</td>
	</tr>
<?php endif ?>
<?php $i = 0; ?>
<?php foreach ($activities as $a): ?>
	<tr class="<?=$i++ % 2 == 0 ? "even" : "odd";?> <?=$a->error_value != 0 ? "error" : ""?>" onclick="window.location='activity.php?unique_id=<?=$a->unique_id?>&amp;pnum=<?=$pnum?>'">
		<td><?=$a->unique_id?></td>
		<td><?=$a->name?></td>
		<td><?=date("d.m.Y H:i:s", floor($a->time_start / 1000000000)).".<b>".($a->time_start % 1000000000)."</b>"?></td>
		<td><?=date("d.m.Y H:i:s", floor($a->time_stop / 1000000000)).".<b>".($a->time_stop % 1000000000)."</b>"?></td>
		<td align="center"><?=round(($a->time_stop - $a->time_start) / 1000, 3)?></td>
		<td><?=$a->error_value != 0 ? $erno[$a->error_value] : ""?></td>
	</tr>
<?php endforeach ?>
</tbody>
<tfoot>
	<tr>
		<th colspan="2" style="text-align: left"><a href="?nid=<?=$nid?>&amp;pid=<?=$pid?>&amp;time=<?=$time?>&amp;page=1&amp;pnum=<?=$pnum?>#act_list">↶ first</a>&nbsp;&nbsp;<a href="?nid=<?=$nid?>&amp;pid=<?=$pid?>&amp;time=<?=$time?>&amp;page=<?=$current_page == 1 ? 1 : $current_page-1?>&amp;pnum=<?=$pnum?>#act_list">← previous</a></th>
		<th colspan="2" style="text-align: center"><?=$current_page?> / <?=$total_pages?></th>
		<th colspan="2" style="text-align: right"><a href="?nid=<?=$nid?>&amp;pid=<?=$pid?>&amp;time=<?=$time?>&amp;page=<?=$current_page == $total_pages ? $current_page : $current_page+1?>&amp;pnum=<?=$pnum?>#act_list">next →</a>&nbsp;&nbsp;<a href="?nid=<?=$nid?>&amp;pid=<?=$pid?>&amp;time=<?=$time?>&amp;page=<?=$total_pages?>&amp;pnum=<?=$pnum?>#act_list">last ↷</a></th>
	</tr>
</tfoot>
</table>

<?php require_once("footer.php"); ?>
