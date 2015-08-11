<?php
/*
	set.php
	Copyright (C) 2015 H-T Reimers <reimers@mail.de>
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice,
	   this list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above copyright
	   notice, this list of conditions and the following disclaimer in the
	   documentation and/or other materials provided with the distribution.

	THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
	AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
	AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
	OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.
*/
	session_start();
	$encoding="ISO-8859-1";
	mb_internal_encoding("$encoding");

	//Read configuration
	$config=parse_ini_file ("surftool.cfg");

	function print_header($meta=""){
		echo "
		<!DOCTYPE html PUBLIC '-//W3C//DTD HTML 4.01 Transitional//EN'
		'http://www.w3.org/TR/html4/loose.dtd'>
			<html>
			<head>
			$meta
			<title>Internet freischalten </title>
				<meta http-equiv='cache-control' content='max-age=0'>
				<meta http-equiv='cache-control' content='no-cache'>
				<meta http-equiv='expires' content='-1'>
				<meta http-equiv='Pragma' CONTENT='no-cache'>

				<style type='text/css'>
					.Color_on { 
						background-color:darkgreen;
						color:black;
					}
					.Color_onplus { 
						background-color:springgreen;
						color:black;			
					}
					.Color_only { 
						background-color:gold;
						color:black;
					}
					.Color_off { 
						background-color:red;
						color:black;
					}
					.Color_adminfree { 
						background-color:black;
						color:white;
					}
					.showMode{
						width:100px;
						valign:middle;
						align:center; 
						padding:0.3em;
						font-weight: bolder;
						font-size: 12pt;
					}
					.Button{ 
						width:200px;
						height:60px;
						valign:middle;
						align:center; 
						padding:1.5em;
						font-weight: bolder;
						font-size: 15pt;
					}
				</style>


				<script language='JavaScript'>
				<!--

				function switch_2_expert(){
					document.getElementById('ModusSimpleHeader').style.display = 'none'; //Link - ModusSimple verbergen
					document.getElementById('ModusSimpleLink').style.display = 'none'; //Link - ModusSimple verbergen
					document.getElementById('ModusExpertLink').style.display = 'block'; //Link - ModusExpert anzeigen
					document.getElementById('ModusExpertHeader').style.display = 'block'; //Link - ModusExpert anzeigen
					document.getElementById('ModusExpertSettings').style.display = 'block'; //ModusExpert anzeigen
					document.getElementById('ModusNormalRadio').checked = 'true';		

				}

				function switch_2_simple(){
					document.getElementById('ModusSimpleHeader').style.display = 'block'; //ModusSimple anzeigen
					document.getElementById('ModusSimpleLink').style.display = 'block'; //ModusSimple anzeigen
					document.getElementById('ModusExpertHeader').style.display = 'none'; //ModusExpert verbergen 
					document.getElementById('ModusExpertLink').style.display = 'none'; //ModusExpert verbergen 
					document.getElementById('ModusExpertSettings').style.display = 'none'; //ModusExpert verbergen
				}


				function switch_mode(newmode){

					if(newmode=='on'){
						document.getElementById('showmodus').firstChild.nodeValue = 'gewählter Modus: Normale Freigabe';
						document.getElementById('whitelist').style.display = 'none'; //Whitelist verbergen
						document.getElementById('onlylist').style.display = 'none'; //Onlylist verbergen
					}
					else if(newmode=='off'){
						document.getElementById('showmodus').firstChild.nodeValue = 'gewählter Modus: Internetzugang ausschalten';
						document.getElementById('whitelist').style.display = 'none'; //Whitelist verbergen
						document.getElementById('onlylist').style.display = 'none'; //Onlylist verbergen
					}
					else if(newmode=='onplus'){
						document.getElementById('showmodus').firstChild.nodeValue = 'gewählter Modus: Erweiterte Freigabe mit zusätzlichen Seiten';
						document.getElementById('whitelist').style.display = 'block'; //Whitelist anzeigen
						document.getElementById('onlylist').style.display = 'none'; //Onlylist verbergen
					}
					else if(newmode=='only'){
						document.getElementById('showmodus').firstChild.nodeValue = 'gewählter Modus: Nur bestimmte Seiten freigeben';
						document.getElementById('whitelist').style.display = 'none'; //Whitelist verbergen
						document.getElementById('onlylist').style.display = 'block'; //Onlylist anzeigen
					}
					else if(newmode=='adminfree'){
						document.getElementById('showmodus').firstChild.nodeValue = 'gewählter Modus: Administrative Freigabe';
						document.getElementById('whitelist').style.display = 'none'; //Whitelist verbergen
						document.getElementById('onlylist').style.display = 'none'; //Onlylist verbergen
					}

					
				}
				//-->
				</script>
				
			    <meta http-equiv='Content-Type' content='text/html; charset=<?php echo $encoding; ?>'>
			    <link rel='stylesheet' title='Schulkonsole' href='surftoolstyle.css' type='text/css'>
			   
			    
			    <meta name='copyright' content='Copyright 2012 H. Reimers, mail@hreimers.com'>
			    <meta name='description' content='myBBS Linux 1.0 Startseite'>
			<script language='JavaScript'>
			<!--

			window.onload = function() {
				//alert('I am an alert box!');
				//location.replace('index.php');
			}
			//-->
			</script>
			</head>
			<body bgcolor= #fefae8>";
	}



	$debug=1;
	if(isset($config["debuglevel"])) $debug=$config["debuglevel"];
	
	$switchtime=2;
	if(isset($config["refresh_time"])) $switchtime=$config["refresh_time"]+1;
	
	include "surftool3.inc";
 
 	if(!isset($_SESSION["login"])){
		$meta="<meta http-equiv='refresh' content='0; URL=index.php'>";
		print_header($meta);
		echo "login first <a href='index.php'> weiter </a>";
		exit(0);
	}
	else if(isset($_POST["mode"])){
		$meta="<meta http-equiv='refresh' content='".$switchtime."' />";
		print_header($meta);
	}
	else{
		$meta="";
		print_header($meta);
	}
	


	//Print logout button
	echo "<form action='index.php' method='post'>
		<input type='submit' name='logout' value='logout'>
	</form> ";	


	function write_domains_command($mode,$add,$remove){
		write_command("'set_domains','$mode','$add','$remove'");
	}

	function write_acl_command($aclname, $newmode){
		write_command("'set_acl_mode','$aclname','$newmode'");
	}

	function write_command($command){
		$i=0;
		$path="/tmp/surftool/";
		$filename="/tmp/surftool/surftool-".date("Y-m-d-H-i")."-";
		
		if(!file_exists($path)){
			if (!mkdir($path, 0700, true)) {
				die('Failed to create folders...');
			}
		}
		
		//don't change existing files
		while(  file_exists($filename.$i.".txt")  ){
			$i++;
		}

		$fp = fopen($filename.$i.".txt", "a");
		fwrite($fp, "$command");
		fclose($fp);
	}

	function print_switch_mode($squidGuardOnlyDomains, $squidGuardWhiteDomains,$admin=true){
		$html = "<h2 id='ModusSimpleHeader'>Modus: normal</h2>";
		$html.= "<h2 id='ModusExpertHeader' style='display:none;'>Modus: erweitert</h2>";
		$html.= "<a href='#' id='ModusSimpleLink' onclick='switch_2_expert();return false'>(Hier klicken für den erweiterten Modus)</a>";
		$html.= "<a href='#' id='ModusExpertLink' onclick='switch_2_simple();return false' style='display:none;'>(Hier klicken für den normalen Modus)</a>";

		//=== Print Expert Modus	
		$html.= "<table id='ModusExpertSettings' style='display:none;'>";
		$html.= " <tr><td colspan='2'><h2>Erweiterter Modus</h2></td></tr>\n";
		$html.= "	<tr><td colspan='2' id='showmodus'>gewählter Modus: on</td></tr>\n
			<tr><td><input type='radio' name='mode' value='on' checked id='ModusNormalRadio' onclick='switch_mode(\"on\");' >Normale Freigabe</td>     <td class='Color_on showMode'> Raum </td></tr>\n
			<tr><td><input type='radio' name='mode' value='onplus' onclick='switch_mode(\"onplus\");' >Erweiterte Freigabe mit zusätzlichen Seiten</td><td class='Color_onplus showMode'> Raum </td></tr>\n
			<tr><td><input type='radio' name='mode' value='only' onclick='switch_mode(\"only\");' >Nur bestimmte Seiten freigeben</td>                 <td class='Color_only showMode'> Raum </td></tr>\n
			<tr><td><input type='radio' name='mode' value='off' onclick='switch_mode(\"off\");' >Internetzugang ausschalten</td>                       <td class='Color_off showMode'> Raum </td></tr>\n";


		if($admin){
			$html.= "<tr><td><input type='radio' name='mode' value='adminfree' onclick='switch_mode(\"adminfree\");' >Administrative Freigabe<br></td>    <td class='Color_adminfree showMode'> Raum </td></tr>\n";
		}

	//      
		$html.= "</table>";
		
		$html.= "<table border='0' id='whitelist' style='display:none;'>
			<tr>
				<td><h2>Liste der <u>zusätzlich</u> freigegebenen Seiten</h2></td>
			</tr>
			<tr>
				<td><textarea name='domains_onplus' cols='80' rows='15' >$squidGuardWhiteDomains</textarea></td>\n
			</tr>
			<tr>
				<td>Hinweis: Diese Seiten/Domain erweitert die normale Freigabe.  Änderungen dieser Liste werden durch einen Klick auf den gewünschten Raum übernommen.
				    Diese Liste gilt für alle Räume in diesem Modus. Aus diesem Grund sollten Änderungen nicht leichtfertig erfolgen.
				 Änderungen werden protokoliert.
				    Erweiterungen der Freigaben sind nur für pädagogisch unterrichtsrelevante Seiten zulässig. </td>\n
			</tr>
		      </table>";

		$html.= "<table border='0' id='onlylist' style='display:none;'>
			<tr>
				<td><h2>Liste der <u>einzigen</u> freigegebenen Seiten</h2></td>
			</tr>
			<tr>
				<td><textarea name='domains_only' cols='80' rows='15'>$squidGuardOnlyDomains</textarea></td>\n
			</tr>
			<tr>
				<td>Hinweis: Nur diese Seiten/Domain werden freigegeben. Änderungen dieser Liste werden durch einen Klick auf den gewünschten Raum übernommen.
				    Diese Liste gilt für alle Räume in diesem Modus. Aus diesem Grund sollten Änderungen nicht leichtfertig erfolgen.
				 Änderungen werden protokoliert.
				    Freigaben sind nur für pädagogisch unterrichtsrelevante Seiten zulässig. </td>\n
			</tr>
		      </table>";

		$html.= "</td></tr></table>";
		return $html;
		
	}


	function preecho($data){
		echo "<PRE>";
		print_r($data);
		echo "</PRE>";
	}


	function create_room_button($aclname,$mode){
		echo "	<td><input type='submit' name='group' value='$aclname' class='Color_$mode Button'></td>\n";
	}

	function print_table($acls){
	
		if(sizeof($acls)>0){
			$spalten=4;
			$i=0;
			echo "<table border='0'>\n<tr>\n";
			foreach($acls["acl"] AS $acl){
				$i++;
				echo create_room_button($acl["acl-name"],$acl[mode]);

				if($i%$spalten==0){
					echo "\n    </tr>\n    <tr>\n";
				}
			}
			$rest=$spalten-$i%$spalten;
			if($rest>0){
				echo "  <td colspan=$rest>&nbsp;<td>\n</tr>\n";
			}
			echo "</table>\n";
		}
		else{
			echo "keine<br>";
		}
	}

	
	
	$sgconf = new surftoolSquidGuardConf($config);
	

	$changes=false;
	//preecho($_POST);
	//Check Post-Data
	if(isset($_POST["group"]) AND isset($_POST["mode"]) ){
		$group=$_POST["group"];
		$mode=$_POST["mode"];
		if(isset($sgconf->squidGuardConf["acl"]["$group"])){
			if($mode=="on" AND $sgconf->squidGuardConf["acl"]["$group"]["mode"]=="on") $mode="off";
			write_acl_command($group, $mode);
			echo "Switch $group to $mode<br>\n";
			$changes=true;
		}
		else{
			echo "Error unknown group:'$group'<br>\n";
		}
		
	}

	if(isset($_POST["mode"]) AND isset($_POST["domains_onplus"]) ){
		if($_POST["mode"]=="onplus"){
			//Any Changes?
			if($_POST["domains_onplus"] != $sgconf->squidGuardOnplusDomains){
				//What Changes?
				$changes=$sgconf->compare_domains_onplus( $_POST["domains_onplus"] );

				write_domains_command("onplus", $changes["add"],$changes["remove"] );
				echo "<br>".$changes["msg"];
				$changes=true;
			}
			else{
				if($debug>2) echo "OnplusDomains: no changees<br>\n";
			}
		}
	}
	
	if(isset($_POST["mode"]) AND isset($_POST["domains_only"]) ){
		if($_POST["mode"]=="only"){
			//Any Changes?
			if($_POST["domains_only"] != $sgconf->squidGuardOnlyDomains){
				//What Changes?
				$changes=$sgconf->compare_domains_only( $_POST["domains_only"] );
				
				write_domains_command("only", $changes["add"],$changes["remove"] );
				echo "<br>".$changes["msg"];
				$changes=true;
			}
			else{
				if($debug>2) echo "OnlyDomains: no changees<br>\n";
			}
		}
	}	

	if($changes){
		echo "<br>Please wait ".$switchtime."s<br>";
		echo "<br><a href=''>Switch an other group</a><br>"; 
		exit(0);
	}
		
	//===
	echo "\n<form action='' method='post'>\n";
	echo "<form accept-charset='$encoding'> ";
	echo print_switch_mode($sgconf->squidGuardOnlyDomains, $sgconf->squidGuardOnplusDomains);
	
	//Get groups/acls
	$groupsAll=$sgconf->squidGuardConf;

	//=== Find Groups with similar ip.
	// ip adresses ar in $groupsAll["src"]["NAME"] and acls are in => $groupsAll["acl"]["NAME"]
	$ip = $_SERVER["REMOTE_ADDR"];
	$lastpoint=strripos($ip,'.');
	$firstpart=substr($ip,0,$lastpoint);
	$groupsNear=array();
	foreach($groupsAll["src"] AS $srcname => $src){
		if(isset($src["ip"])){
			foreach($src["ip"] AS $ip){
				$pos = strpos($ip, $firstpart);
				if( $pos !== false){
					if(isset($groupsAll["acl"]["$srcname"])){
						$groupsNear["acl"]["$srcname"]=$groupsAll["acl"]["$srcname"];
					}
					else{
						echo "Warning: no acl:'$srcname'<br>\n ";
					}
				}
			}
		}
		else echo "no src<br>\n";
	}


	echo "<pre>";
	//print_r($sgconf);
	echo "</pre>";
	
	echo "<h2>Vorschläge</h2>";
	print_table($groupsNear);	
	echo "<h2>Alle Räume</h2>";
	print_table($groupsAll);


	echo "\n</form>\n";
	


	//if($debug>1) preecho($data);

	echo"
	<form action='index.php' method='post'>
		<input type='submit' name='logout' value='logout'>
	</form> ";	



?>
