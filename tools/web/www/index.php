<?php 
require_once("include/DB.php"); 
require_once("include/Activity.php"); 
require_once("include/Program.php"); 
$site_title = "Program Execution Overview";
$site_description = "Overview of the stored program runs";
require_once("header.php"); 
?>

<h1>Program Execution List</h1>

<?php $page_size = 200; ?>
<?php $total_runs = Program::get_count(); ?>
<?php $total_pages = max(ceil($total_runs / $page_size), 1); ?>
<?php $current_page = isset($_GET['page']) ? $_GET['page'] : $total_pages; ?>
<?php $runs = Program::get_list($current_page, $page_size); ?>

<form name="purge_frm" method="post" action="purge.php">
	<input type="submit" value="Purge database"    onclick="return confirm('You are about to delete all data. Are you sure?')" />
	<input type="submit" value="Topology overview" onclick="return action='topology.php'" />
	<input type="submit" value="Statistics" onclick="return action='statistics.php'" />
</form>

<br /><br />

<?php

	$link_first = "<a href=\"?page=1\">↶ first</a>";
	$link_prev  = "<a href=\"?page=";
	$link_prev .= $current_page == 1 ? 1 : $current_page-1;
	$link_prev .= "\">← previous</a>";
	$link_next  = "<a href=\"?page=";
	$link_next .= $current_page == $total_pages ? $current_page : $current_page+1;
	$link_next .= "\">next →</a>";
	$link_last .= "<a href=\"?page=$total_pages\">last ↷</a>";
?>

<table id="big_list" cellspacing="0" cellpadding="0">
<thead>
	<tr><?php include("table_nav.php"); ?></tr>
	<tr><th>Command</th><th>Start</th><th>Duration</th><th>Node</th><th>User</th></tr>
</thead>
<tbody>
<?php if (count($runs) == 0): ?>
	<tr class="even">
		<td colspan="6" style="text-align: center;">The activity database is empty.</td>
	</tr>
<?php endif ?>
<?php $i = 0; ?>
<?php foreach ($runs as $r): ?>
	<tr class="<?=$i++ % 2 == 0 ? "even" : "odd";?>" onclick="window.location='activities.php?nid=<?=$r->nid?>&amp;pid=<?=$r->pid?>&amp;time=<?=$r->time?>&amp;pnum=<?=$r->childobjectid?>'">
		<td><?=$r->attributes['description/commandLine'];?></td>
		<td><?=date("d.m.Y H:i:s", floor($r->times["start"] / 1000000000)).".".($r->times["start"] % 1000000000)?></td>
		<td><?=round(($r->times["stop"] - $r->times["start"]) / 1000000000, 3)?> s</td>
		<td><?=$r->node?></td>
		<td><?=$r->attributes['description/user-name'];?></td>
	</tr>
<?php endforeach ?>
</tbody>
<tfoot><tr><?php include("table_nav.php"); ?></tr></tfoot>
</table>

<?php require_once("footer.php"); ?>
