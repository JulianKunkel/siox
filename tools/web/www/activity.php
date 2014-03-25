<?php 

require_once "include/DB.php";
require_once "include/Activity.php";
require_once "include/Util.php";
require_once "include/Error.php";

$site_title = "Causal Chain View"; 
$site_description = "Visualization of the causal chain for the activity"; 

require_once "header.php"; 

$pnum = $_GET['pnum'];
$act = Activity::get_activity($_GET["unique_id"]);

?>

<h1>Detail of Activity <?=$_GET["unique_id"]?></h1>

<form name="runs_frm" method="post" action="index.php" style="float: left;">
	<input type="submit" value="Execution Overview" />
</form>
<form name="acts_frm" method="post" action="activities.php?nid=<?=$act->cid_pid_nid?>&amp;pid=<?=$act->cid_pid_pid?>&amp;time=<?=$act->cid_pid_time?>&amp;pnum=<?=$pnum?>" style="float: left">
	<input type="submit" value="Activities Overview" />
</form>

<br /><br />

<h2>Causal Chain</h2>

<img src="causal_chain.php?unique_id=<?=$_GET["unique_id"]?>" />


<h2>Attribute List</h2>

<table cellspacing="0" cellpadding="0">
	<tr>
		<th>name</th><td class="odd"><?=$act->childname?>()</td>
	</tr>
	<tr>
		<th>unique_id</th><td class="even"><?=$act->unique_id?></td>
	</tr>
	<tr>
		<th>ucaid</th><td class="odd"><?=$act->ucaid?></td>
	</tr>
	<tr>
		<th>id</th><td class="even"><?=$act->id?></td>
	</tr>
	<tr>
		<th>thread_id</th><td class="odd"><?=$act->thread_id?></td>
	</tr>
	<tr>
		<th>cid_pid_nid</th><td class="even"><?=$act->cid_pid_nid?></td>
	</tr>
	<tr>
<?php 
$pid_secs  = $act->cid_pid_time;
$pid_days  = floor($pid_secs / (60*60*24));
$pid_secs %= 60*60*24;
$pid_hours = floor($pid_secs / (60*60));
$pid_secs %= 60*60;
$pid_mins  = floor($pid_secs / 60);
$pid_secs %= 60;
?>
		<th>cid_pid_time</th><td class="odd"><?=$pid_days?> d <?=$pid_hours?> h <?=$pid_mins?> m <?=$pid_secs?> s</td>
	</tr>
	<tr>
		<th>cid_id</th><td class="even"><?=$act->cid_id?></td>
	</tr>
	<tr>
		<th>time_start</th><td class="odd"><?=date("d.m.Y H:i:s", floor($act->time_start / 1000000000)).".<b>".($act->time_start % 1000000000)."</b>"?></td>
	</tr>
	<tr>
		<th>time_stop</th><td class="even"><?=date("d.m.Y H:i:s", floor($act->time_stop / 1000000000)).".<b>".($act->time_stop % 1000000000)."</b>"?></td>
	</tr>
	<tr>
		<th>duration</th><td class="odd"><?=round(($act->time_stop - $act->time_start) / 1000, 3)?> µs</td>
	</tr>
	<tr>
		<th>attributes</th><td class="even"><?php foreach ($act->attributes as $k => $v):?><?=$k?> = <?=$v?><br /><?php endforeach?></td>
	</tr>
	<tr>
		<th>remote_calls</th><td class="odd"><?=implode(', ', hyperlink_ids('remote_call.php?unique_id=', $act->remote_calls));?></td>
	</tr>
	<tr>
		<th>parents</th><td class="even"><?=implode(', ', hyperlink_ids('activity.php?unique_id=', $act->parents));?></td>
	</tr>
	<tr>
		<th>error_value</th><td class="odd"><?=$act->error_value != 0 ? $erno[$act->error_value] : "" ?></td>
	</tr>

</table>


<?php require_once "footer.php"; ?>
