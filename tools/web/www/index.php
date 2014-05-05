<?php 
require_once("include/DB.php"); 
require_once("include/Activity.php"); 
require_once("include/Program.php"); 
require_once("include/SIOX.php"); 
require_once("include/Stats.php"); 
require_once("include/Util.php"); 
$site_title = "Program Execution Overview";
$site_description = "Overview of the stored program runs";
require_once("header.php"); 
?>

<h1>Program Execution List</h1>

<?php 
$cmd_sel  = isset($_POST["cmd"])  ? $_POST["cmd"]  : "all_cmds"; 
$node_sel = isset($_POST["node"]) ? $_POST["node"] : "all_nodes"; 
$user_sel = isset($_POST["user"]) ? $_POST["user"] : "all_users"; 

$page_size = 200; 
$total_runs = Program::get_count($cmd_sel == "all_cmds" ? NULL : $cmd_sel, NULL, $user_sel == "all_users" ? NULL : $user_sel); 
$total_pages = max(ceil($total_runs / $page_size), 1); 
$current_page = isset($_POST['page']) && $_POST['page'] <= $total_pages ? $_POST['page'] : $total_pages; 
$runs = Program::get_list($current_page, $page_size, $cmd_sel == "all_cmds" ? NULL : $cmd_sel, NULL, $user_sel == "all_users" ? NULL : $user_sel); 
$stats_avail = Stats::get_list(); 
$user_list = SIOX::get_user_list(); 
$cmd_list  = SIOX::get_cmd_list(); 
$node_list = SIOX::get_node_list(); 
?>


<form name="prog_list_frm" id="prog_list_frm" method="post" action="index.php">

	<input type="submit" value="Purge database"     onclick="return confirm('You are about to delete all data. Are you sure?')" />
	<input type="submit" value="Topology overview"  onclick="return action='topology.php'" />
	<input type="submit" value="General statistics" onclick="return action='statistics.php'" />
	<input type="submit" value="Compare selected"   onclick="return action='comparison.php'" />
	<input type="hidden" name="page" id="page" value="<?=$current_page?>" />

	<select name="stats[]" multiple="multiple" size="4">
<?php foreach ($stats_avail as $s):?>
		<option name="<?=$s->childobjectid?>"><?=$s->childname?></option>
<?php endforeach ?>
	</select>

<br /><br />

<?php
$link_first = "<a href=\"#\" onclick=\"page.value=1;document.getElementById('prog_list_frm').submit();return false;\">↶ first</a>";
$link_prev  = "<a href=\"#\" onclick=\"page.value=";
$link_prev .= $current_page == 1 ? 1 : $current_page-1;
$link_prev .= ";document.getElementById('prog_list_frm').submit();return false;\">← previous</a>";
$link_next  = "<a href=\"#\" onclick=\"page.value=";
$link_next .= $current_page == $total_pages ? $current_page : $current_page+1;
$link_next .= ";document.getElementById('prog_list_frm').submit();return false;\">next →</a>";
$link_last  = "<a href=\"#\" onclick=\"page.value=$total_pages;document.getElementById('prog_list_frm').submit();return false;\">last ↷</a>";
?>

<table id="big_list" cellspacing="0" cellpadding="0">
<thead>
	<tr><?php include("table_nav.php"); ?></tr>
	<tr>
		<th>
			<select name="cmd" onchange="action='index.php';submit()">
				<option value="all_cmds" <?= $cmd_sel == "all_cmds" ? "selected=\"selected\"" : ''?>>All commands</option>
<?php foreach ($cmd_list as $cmd):?>
				<option <?= $cmd == $cmd_sel ? "selected=\"selected\"" : ''?>><?=$cmd?></option>
<?php endforeach?>
			</select>
		</th>
		<th>Start</th>
		<th>Duration</th>
		<th>
			<select name="node" onchange="action='index.php';submit()" disabled>
				<option value="all_nodes" <?= $node_sel == "all_nodes" ? "selected=\"selected\"" : ''?>>All nodes</option>
<?php foreach ($node_list as $node):?>
				<option <?= $node == $node_sel ? "selected=\"selected\"" : ''?>><?=$node?></option>
<?php endforeach?>
			</select>
		</th>
		<th>
			<select name="user" onchange="action='index.php';submit()">
				<option value="all_users" <?= $user_sel == "all_users" ? "selected=\"selected\"" : ''?>>All users</option>
<?php foreach ($user_list as $user):?>
				<option <?= $user == $user_sel ? "selected=\"selected\"" : ''?>><?=$user?></option>
<?php endforeach?>
			</select>
		</th>
		<th></th>
	</tr>
</thead>
<tbody>
<?php if (count($runs) == 0): ?>
	<tr class="even">
		<td colspan="6" style="text-align: center;">No matching data in the database.</td>
	</tr>
<?php endif ?>
<?php $i = 0; ?>
<?php foreach ($runs as $r): ?>
	<tr class="<?=$i++ % 2 == 0 ? "even" : "odd";?>">
		<td class="prog" onclick="window.location='activities.php?nid=<?=$r->nid?>&amp;pid=<?=$r->pid?>&amp;time=<?=$r->time?>&amp;pnum=<?=$r->childobjectid?>'"><?=$r->attributes['description/commandLine'];?></td>
		<td><?=date("d.m.Y H:i:s", floor($r->times["start"] / 1000000000)).".".($r->times["start"] % 1000000000)?></td>
		<td><?=format_seconds(round(($r->times["stop"] - $r->times["start"]) / 1000000000))?></td>
		<td><?=$r->node?></td>
		<td><?=$r->attributes['description/user-name'];?></td>
		<td><input type="checkbox" name="progs[]" value="<?=$r->childobjectid?>" /></td>
	</tr>
<?php endforeach ?>
</tbody>
<tfoot><tr><?php include("table_nav.php"); ?></tr></tfoot>
</table>

</form>

<?php require_once("footer.php"); ?>
