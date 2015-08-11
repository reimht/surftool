<?php
/*
	surftool3.inc
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


	class surftoolSquidGuardConf{

		var $debug=8;
		var $squidGuardConfPath="/usr/pbi/squidguard-amd64/etc/squidGuard/squidGuard.conf";
		var $squidGuardOnlyName="surftool_only";
		var $squidGuardOnlyPath="/var/db/squidGuard/surftool_only/domains";
		var $squidGuardOnplusName="surftool_onplus";
		var $squidGuardOnplusPath="/var/db/squidGuard/surftool_onplus/domains";
		var $squidBinary = "/usr/local/bin/squid";
		var $squidGuardBinary = "/usr/local/bin/squidGuard";

		var $squidGuardConfText = ""; //Array: The hole Text of the config file - one line <=> one field
		var $squidGuardConf = array(); //

		//Onplus Domains
		var $OnplusChangesActiv=false;
		var $squidGuardOnplusDomains=""; //String, seperator: " "
		
		//Only Domains
		var $OnlyChangesActiv=false;
		var $squidGuardOnlyDomains=""; //String, seperator: " "

		//ACL-Modes changes
		var $aclChangeModeActiv=false;
		
		//Reload squid config 
		var $squidReloadActiv=false;


		function __construct($config){
		
		//$squidGuardConfPathNew, $squidGuardOnplusPathNew, $squidGuardOnlyPathNew) {

			$this->aclChangeModeActiv=false; //No changes at beginning
			$this->squidReloadActiv=false; //No changes at beginning
			
			//Get Systemconfiguration
			$this->squidGuardConfPath = $config["squidGuard_configfile"];
			$this->squidGuardOnplusName = $config["group_onplus"];
			$this->squidGuardOnplusPath = $config["squidGuard_database"].$config["group_onplus"]."/domains";
			$this->squidGuardOnlyName = $config["group_only"];
			$this->squidGuardOnlyPath = $config["squidGuard_database"].$config["group_only"]."/domains";
			$this->squidBinary = $config["squid_binary_path"];
			$this->squidGuardBinary = $config["squidGuard_binary_path"];
			
			//Check data
			$this->check();
			
			//Get Data from squidGuard.conf
			$this->parse_squidGuard();
			//Get Group Mode (on,off,only,onplus,adminfree)
			$this->getAclMode();

			//Get Array with domains for onplus mode
			$this->squidGuardOnplusDomains=$this->parse_squidGuardDomains($this->squidGuardOnplusPath);
			
			//Get Array with domains for only mode
			$this->squidGuardOnlyDomains=$this->parse_squidGuardDomains($this->squidGuardOnlyPath);
			
			
			
			//print_r($this->squidGuardOnplusDomains );

		}
		
		function check(){
			$error_msg=array();			
			if( !file_exists($this->squidBinary)){
				$error_msg[]="Warning: squid binary '$this->squidBinary' do not exists\n";
			}
			if( !file_exists($this->squidGuardBinary)){
				$error_msg[]="Warning: squidGuard binary '$this->squidGuardBinary' do not exists\n";
			}
			if( !file_exists($this->squidGuardConfPath)){
				$error_msg[]="Warning: squidGuard configuration file '$this->squidGuardConfPath' do not exists\n";
			}
			if( !file_exists($this->squidGuardOnplusPath)){
				$error_msg[]="Warning: onplus group - file '$this->squidGuardOnplusPath' do not exists\n";
			}
			if( !file_exists($this->squidGuardOnlyPath)){
				$error_msg[]="Warning: only group - file '$this->squidGuardOnlyPath' do not exists\n";
			}			
			if(sizeof($error_msg)>0) print_r($error_msg);
		}
		
		function squidReload(){
			if($this->squidReloadActiv){
				$command=$this->squidBinary." -k reconfigure"; //#Squid/SquidGuard Config reload 
				$output=array();
				$return_var="";
				exec (  $command, $output , $return_var );
				if($return_var!=0 AND $this->debug>=2) echo "Error: squidReload: Can't exec '$command'!\n";
				else if($this->debug>=3) echo "Info - squidReload: exec '$command' returns: '$return_var'!\n";
				else if($this->debug>=6) print_r($output);
			}
			else{
				if($this->debug>=3) echo "Info - squidReload: no reload\n";
			}
		}
		
		function write_squidGuardConf(){
			if($this->aclChangeModeActiv==true){
				// Array-->String (with End Of Line)
				$str = implode(PHP_EOL, $this->squidGuardConfText );
				file_put_contents("$this->squidGuardConfPath", $str);
				$this->squidReloadActiv=true;
			}
			else{
				if($this->debug>=2) echo "Info - write_squidGuardConf: no changes\n"; 
			}
			
		}

		function parse_squidGuardDomains($file){
			if(!file_exists($file)){
				echo "parse_squidGuardDomains - Error: Can't access '$file'<br>\n";
			}
			else{
				$t = file_get_contents($file);
				$t = str_replace("\r\n", "\n", $t);
				$t = str_replace("\n", " ", $t);
				$domains=explode(" ",$t);
				$t="";
				//Search wrong entries
				foreach($domains AS $domain){
					$t.=" ".$this->clean_domain($domain);
				}
				return $t;
			}
		}

		function endsWith( $haystack, $needle,$debug=0){
			$length = strlen($needle);
			if ($length == 0) {
				return true;
			}
			if($debug>=3) echo "endsWith".$length.substr($haystack, -$length)."\n";
			return (substr($haystack, -$length) === $needle);
		}

		function setDomains($group,$add,$remove){
			
			if( sizeof($remove)>0 OR sizeof($add)>0 ){
				if($group=="onplus" ){
					$this->OnplusChangesActiv=true; 
					$target=&$this->squidGuardOnplusDomains;
				}
				else if($group=="only" ){
					$this->OnlyChangesActiv=true; 
					$target=&$this->squidGuardOnlyDomains;
				}
				else {
					echo "Error setDomains: unknown group: '$group'\n";
				}			
			}
			
			//Search wrong entries
			$domains=explode(" ",$target);
			$target="";
			foreach($domains AS $domain){
				$target.=" ".$this->clean_domain($domain);
			}

			
			if(sizeof($remove)>0){
				foreach($remove AS $domain){
					if($this->debug>=2) echo "remove $domain from $group<br>\n";
					$target=str_replace("$domain","",$target);
				}
			}
			if(sizeof($add)>0){
				foreach($add AS $domain){
					if($this->debug>=2) echo "add $domain to $group<br>\n";
					//First replace -> no double entries
					$target=str_replace("$domain","",$target);
					$target.=" ".$domain;
				}
			}
			//Remove double blanks
			$target = preg_replace ('#\s+#' , ' ' , $target );
			$target = trim($target );
		}
		
		function write_squidGuardDomains(){
	
			if($this->OnplusChangesActiv){
				//echo $this->squidGuardOnplusDomains; //String, seperator: " "
				$str=trim($str);
				$str =str_replace(" ","\n",$this->squidGuardOnplusDomains);

				//file_put_contents("$this->squidGuardOnplusPath", $str);
				$fp = fopen("$this->squidGuardOnplusPath","w");
				fwrite($fp, $str);
				fclose($fp);
			
				//Activate new domainlist
				$command=$this->squidGuardBinary." -C ".$this->squidGuardOnplusName."/domains -c ".$this->squidGuardConfPath; 
				$return_var="";
				system($command, $return_var );

				//Activation ok?
				if($return_var!=0 AND $this->debug>=1) echo "error: write_squidGuardDomains: Can't exec '$command' returns retval:'$return_var'!\n";
				if($this->debug>=3) echo "Info - write_squidGuardDomains: Exec '$command' with retval: $return_var !\n";
				if($this->debug>=6) print_r($output);
				//Set flag to reload squid configuration
				$this->squidReloadActiv=true;
			}
			else{
				if($this->debug>=6) echo "Info - write_squidGuardDomains: OnplusChangesActiv not activ !\n";
			}

			if($this->OnlyChangesActiv){
				//echo $this->squidGuardOnlyDomains; //String, seperator: " "
				$str=trim($str);
				$str =str_replace(" ","\n",$this->squidGuardOnlyDomains);
				//file_put_contents("$this->squidGuardOnlyPath", $str);
				$fp = fopen("$this->squidGuardOnlyPath","w");
				fwrite($fp, $str);
				fclose($fp);
				
				//Activate new domainlist
				$command=$this->squidGuardBinary." -C ".$this->squidGuardOnlyName."/domains -c ".$this->squidGuardConfPath." > /dev/null 2>&1"; 
				$return_var="";
				system($command, $return_var );

				//Activation ok?
				if($return_var!=0 AND $this->debug>=1) echo "Error: write_squidGuardDomains: Can't exec '$command'!\n";
				if($this->debug>=3) echo "Info - write_squidGuardDomains: Exec '$command' with retval: $return_var !\n";
				if($this->debug>=6) print_r($output);
				//Set flag to reload squid configuration
				$this->squidReloadActiv=true;
			}
			else{
				if($this->debug>=6) echo "Info - write_squidGuardDomains: OnlyChangesActiv not activ !\n";
			}

		}


		function setAclModeText($oldpasstext,$newmode){
			//echo "<br>$oldpasstext->$newmode<br>";
			//Make a clear start point
			$oldpasstext=str_replace("pass ","",$oldpasstext);
			$oldpasstext=str_replace("adminfree ","",$oldpasstext);
			$oldpasstext=str_replace($this->squidGuardOnlyName." ","",$oldpasstext);
			$oldpasstext=str_replace($this->squidGuardOnplusName." ","",$oldpasstext);

			 if($this->endsWith( $oldpasstext, "none" )){
				$oldpasstext=str_replace(" none","",$oldpasstext);
			 }
			 if($this->endsWith( $oldpasstext, "all" )){
				$oldpasstext=str_replace(" all","",$oldpasstext);
			 }
			 
			 
			if($newmode=="on"){ //Normale freigabe
				$oldpasstext="pass ".$oldpasstext." all";
			}
			else if($newmode=="onplus"){ //Erweiterte Freigabe (Normal+WhiteList)
				$oldpasstext="pass ".$this->squidGuardOnplusName." ".$oldpasstext." all";
			}
			else if($newmode=="only"){ //Erweiterte Freigabe (Normal+WhiteList)
				$oldpasstext="pass ".$this->squidGuardOnlyName." ".$oldpasstext." none";
			}
			else if($newmode=="adminfree"){ //Alles mittels der Dest adminfree freigeben;
				$oldpasstext="pass adminfree ".$oldpasstext." all";
			}
			else{
				//Default:OFF
				$oldpasstext="pass ".$oldpasstext." none";
			}
			//echo "<br>a:$oldpasstext<br>";
			$oldpasstext = preg_replace ('#\s+#' , ' ' , $oldpasstext ); 
			return $oldpasstext;
		}

		function setAclMode(&$aclChangeMode){
			if(sizeof($this->squidGuardConfText)>2){
				if(is_array($aclChangeMode)){
					foreach($aclChangeMode AS $aclname => $mode){
						//Is there a acl with this name?
						if(isset($this->squidGuardConf["acl"]["$aclname"])){
							if($this->debug>=2) echo "$aclname from '".$this->squidGuardConf["acl"]["$aclname"]["mode"]." to '$mode'\n";
							//Are there any changes?
							if($this->squidGuardConf["acl"]["$aclname"]["mode"] != $mode){
								$pass_linenr = $this->squidGuardConf["acl"]["$aclname"]["pass-linenr"];
								//Is there line with this number?
								if(isset($this->squidGuardConfText["$pass_linenr"])){
									//There are changes to write
									$this->aclChangeModeActiv=true;
									//Change the text of the acl pass line
									$this->squidGuardConfText["$pass_linenr"]=$this->setAclModeText($this->squidGuardConfText["$pass_linenr"],$mode);
									//echo "<br>pass_linenr:".$pass_linenr."<br>pass_text1:".$pass_text1."<br>pass_text2:".$pass_text2."<br>";
								}
								else{
									echo "error in surftoolSquidGuardConf->setAclMode: can't find line nr '$pass_linenr' in  squidGuardConfText! <br>\n";
								}
 								
							}
						}
						else{
							echo "error in surftoolSquidGuardConf->setAclMode: can't find acl '$acl'! ";
						}
					}
				}
				else{
					echo "error in surftoolSquidGuardConf->setAclMode: aclChangeMode no array";
				}
			}
			else{
				echo "error in surftoolSquidGuardConf->setAclMode: squidGuardConf empty or to short";
			}
		}

		function getAclMode(){
			
			foreach($this->squidGuardConf["acl"] AS $aclname => $acl){
				$pass=trim($acl["pass"]);
				
				if(strstr($pass,"adminfree ") ){
					$mode="adminfree";
				}		
				else if($this->endsWith( $pass, "none" )){
					if(strstr($pass,$this->squidGuardOnlyName." ") ){
						$mode="only";
					}
					else{
						$mode="off";
					}
				}
				else if($this->endsWith( $pass, "all" )){
					if(strstr($pass,$this->squidGuardOnplusName." ") ){
						$mode="onplus";
					}
					else{
						$mode="on";
					}
				}
				else{
					$mode="unknown";
				}	
				
				$this->squidGuardConf["acl"][$aclname]["mode"]=$mode;
				if($debug>=3) echo "getAclMode: $aclname - mode:$mode - $pass<br>\n";
			}
			

		}

		function compare_domains_onplus($destination){
			return  $this->compare_domains($this->squidGuardOnplusDomains, $destination);
		}

		function compare_domains_only($destination){
			return  $this->compare_domains($this->squidGuardOnlyDomains, $destination);
		}


		function clean_domain($domain){

			//no http://
			$part = str_replace(array("http://", "http:/", "http:",   "http//", "http/"), ' ', $part);
			//Clean domain
			$domain = str_replace(array(",", ";","'",":"," ","\r\n", "\r", "\n"), '', $domain); 
			//Remove double blanks
			$domain = preg_replace ('#\s+#' , ' ' , $domain ); 
			//only domains makes www.bbs2leer/school -> www.bbs2leer.de

			if(strpos($domain, '/')>0){
				$domain = substr($domain, 0, strpos($domain, '/'));
			}

			//Clean domain
			$domain=trim($domain);

			//test top level domain
			$domainparts = explode(".", $domain);

			//min 2 parts like "aaa.de" 
			if(sizeof($domainparts)<2){
				return false;
			}
			
			return $domain;
			
		}

		function compare_domains(&$destination_on_server, &$destination){

			//Output for user
			$msg="";
			//Clean URLs
			$destination = str_replace(array(",", ";","'"), ' ', $destination); 
			//Remove double blanks
			$destination = preg_replace ('#\s+#' , ' ' , $destination ); 

			$destination_parts = explode(" ", $destination);
			$destination="";


			foreach($destination_parts AS $part){
				//no http://
				$part = str_replace(array("http://", "http:/", "http:",   "http//", "http/"), ' ', $part);
				//no Enter and NewLine
				$part = str_replace(array("\r\n", "\r", "\n"), ' ', $part); 
				
				//only domains makes www.bbs2leer/school -> www.bbs2leer.de
				if(strpos($part, '/')>0){
					$part = substr($part, 0, strpos($part, '/'));
				}

				$part = trim($part);
				if(strlen($part)>2){
					$part_clean=preg_replace('/[^0-9a-zA-Z. -]/', '', $part);

					//test top level domain
					$domainparts = explode(".", $part);
					
					
					if(sizeof($domainparts)<2){
						$msg .= "<br>Fehler: Der Eintrag '<b>$part</b>' ist keine Domain.<br>";
					}
					else if($part_clean!=$part){
						$msg .= "<br>Fehler: Der Eintrag <b>$part</b> enthält ungültige Zeichen! Es sind nur die Zeichen a-z,A-Z,0-9,- und . erlaubt <br>";
					}
					else if(strlen($part)>0 AND strlen($part)<4){
						$msg .= "<br>Fehler: Eintrag <b>$part_clean</b> ist zu kurz (Nur ".strlen($part)." Zeichen)!<br>";
					}
					else{
						$destination.=$part_clean." ";
					}
				}
			}



			//Find Changes
			$old = explode(" ", $destination_on_server);
			$new = explode(" ", trim($destination));

			$add_domain = array_diff($new, $old);
			foreach($add_domain AS $entry){
				$add_domain_text.=$entry." ";
			}
			
			$remove_domain = array_diff($old, $new);
			foreach($remove_domain AS $entry){
				$remove_domain_text.=$entry." ";
			}
			
			$msg .= "ADD: $add_domain_text<br>REMOVE: $remove_domain_text<br>";

			$retval=array();
			$retval["msg"]=$msg;
			$retval["add"]=$add_domain_text;
			$retval["remove"]=$remove_domain_text;
			return $retval;

		}



		/**
		* $t = Text from file squidGuard.conf.
		*/
		function parse_squidGuard(){
		
			//Get Data from squidGuard.conf
			//$squidGuardConfText
			$t=file_get_contents($this->squidGuardConfPath);
			
			/* replace line breaks */
			$t = str_replace("\r\n", "\n", $t);
			/* replace tabs */
			$t = str_replace("\t", " ", $t);	
			/* replace multiple spaces */
			//$t=preg_replace('#[\s]{2,}#s', ' ', $t);

			/** Read squidGuard.conf
			**   { - open a subgroup
			**   } - close a subgroup
			**
			*/
			$data=array();
			$data["src"]=array();
			$data["acl"]=array();
			$this->squidGuardConfText=explode("\n",$t);
			$s=$this->squidGuardConfText;
			for($i=0;$i<sizeof($s);$i++){
				$s[$i]=trim($s[$i]);
				$s[$i]=explode("{",$s[$i]);
				//Found a "{"
				for($j=0;$j<sizeof($s[$i]);$j++){
					$s[$i][$j]=trim($s[$i][$j]);
					$s[$i][$j]=explode("}",$s[$i][$j]);
					//Found a "}"
					for($k=0;$k<sizeof($s[$i][$j]);$k++){
						$s[$i][$j][$k]=trim($s[$i][$j][$k]);
					}
				}
			}


			/** parser squidGuard.conf
			**   acl 
			**
			*/
			$aclname=""; 	//Declare Variable
			$linenr=0;	//Declare Variable
			$depth=0;
			$aclarea=false;
			$comment_last="";
			for($i=0;$i<sizeof($s);$i++){
				for($j=0;$j<sizeof($s[$i]);$j++){
					for($k=0;$k<sizeof($s[$i][$j]);$k++){
						
						//Split configuration an comment part from a line
						$poshashkey=strpos($s[$i][$j][$k],"#");
						if($poshashkey!==false){
							$line=substr($s[$i][$j][$k],0,$poshashkey-1);
							$comment=substr($s[$i][$j][$k],$poshashkey+1);
							$comment_last=$comment;
						}
						else{
							$line=$s[$i][$j][$k];
							$comment="";
						}
						
						$line=preg_replace('#[\s]{2,}#s', ' ', $line);
						$linee=explode(" ",$line);
						//print_r($linee);
						
						//Search a acl name
						if($depth==1 AND sizeof($linee) >= 1 AND $aclarea AND strlen($linee[0])>1){
							$aclname=$linee[0];
							if($debug>2) echo "Found acl: '$aclname'";
							$acl=array();
							$acl["acl-name"]=$aclname;
							$acl["acl-linenr"]=$i;
							$data["acl"]["$aclname"]=$acl;
						}
						
						//Search a "pass" part from a acl
						if($depth==2 AND $aclarea==true){
							if(substr($line,0,4)=="pass"){
								$pass=$line;
								if($debug>4) echo "found passpart form acl $aclname: $pass\n";
								if(isset($data["acl"]["$aclname"])){
									$data["acl"]["$aclname"]["pass"]=$pass;
									$data["acl"]["$aclname"]["pass-linenr"]=$i;
									$data["acl"]["$aclname"]["comment"]=$comment_last;
								}
								else{
									echo "error no acl $aclname\n";
								}
							}
						}

						//Search a "ip" part from a src
						if($depth==1 AND $srcarea==true){
							if($linee[0]=="ip"){
								//echo "$srcname--".$linee[1]."-$line<br>\n";
								$data["src"]["$srcname"]["ip"][]=$linee[1];
							}
						}
						
						if($depth==0 AND sizeof($linee) >= 1){
							if($linee[0]=="src"){
								$srcname=$linee[1];
								$data["src"]["$srcname"]=array();
								$data["src"]["$srcname"]["ip"]=array();
								$srcarea=true;
								
							}
							else if($linee[0]=="acl"){
								$aclarea=true;
							}
						}
						if($debug>2) echo $s[$i][$j][$k]."$depth:$poshashkey-$line-$comment-a:$aclarea\n";
						if(isset($s[$i][$j][$k+1])) {
							//here was a "}";
							$depth--;
							//Leave acl part from configfile
							if($aclarea AND $depth==0){
								$aclarea=false;
								$aclname="";
							}
							//Leave src part from configfile
  							if($srcarea AND $depth==0){
								$srcarea=false;
								$srcname="";
							}
						}
						if($debug>4) echo "$aclname-$line\n";
					}
					if(isset($s[$i][$j+1])){
						//here was a "{"
						$depth++;
					}
				}

			}
			
			$this->squidGuardConf=$data;
			
		}		

	}
	
	class commandReader{

		var $debug=1;
		var $commandpath="/tmp/surftool";
		var $changes=false;
		var $commandfiles=array();
		//ACL-Modes changes
		var $aclChangeMode=array();
		//Domain changes
		var $domainsOnplusAdd=array();
		var $domainsOnplusRemove=array();
		var $domainsOnlyAdd=array();
		var $domainsOnlyRemove=array();
 
		function __construct($commandpathNew){
			$this->changes=false; //At start no changes
			$this->commandpath = $commandpathNew;
			$this->readCommandFiles();
			//$this->debuginfo();
		}
  
		function debuginfo(){
			echo "<PRE>";
			echo "aclChangeMode\n";
			print_r($this->aclChangeMode);
			echo "domainsOnplusAdd\n";
			print_r($this->domainsOnplusAdd);
			echo "domainsOnplusRemove\n";
			print_r($this->domainsOnplusRemove);
			echo "domainsOnlyAdd\n";
			print_r($this->domainsOnlyAdd);
			echo "domainsOnlyRemove\n";
			print_r($this->domainsOnlyRemove);
			echo "</PRE>";
		}
  
		function addDomainsToArray(&$domainArray,$domainString){
			$domains=explode(" ",$domainString);
			foreach($domains AS $domain){
				$domainArray[]=$domain;
			}
		}
		
		function deleteCommandFiles(){
			foreach($this->commandfiles AS $file){
				//echo "$this->commandpath/$file";
				$filename=$this->commandpath."/".$file;
				if(!unlink($filename)) echo "Error - deleteCommandFiles: Can't delete commandfile:'".$filename."'!\n";
				
			}

		}
  
		function readCommandFiles(){
			if(!file_exists("$this->commandpath")) {
				mkdir("$this->commandpath");
			}
			@$handle=opendir ("$this->commandpath");
			if($handle === false){
				echo "Error can't open '"."$this->commandpath"."'!\n";
			}
	  
			//Read commandfiles
				
			while ($filename = readdir ($handle)) {
				//use only filenames like surftool*.txt
				if( substr($filename,0,8)=="surftool" AND $this->endsWith( $filename, ".txt") ){
					$this->commandfiles[]=$filename;
				}
			}
			closedir($handle);
			if(sizeof($this->commandfiles)>0) $this->changes=true; //Maybe there are changes
			sort($this->commandfiles); //Last modification wins
			foreach($this->commandfiles AS $file){
				//echo "$this->commandpath/$file";
				$handle = fopen("$this->commandpath/$file", "r");
				$line = fgetcsv($handle, 1024,",","'");
				if(sizeof($line)>=3){
					$cmd=trim($line[0]);
					//print_r($line);
					if($cmd=="set_acl_mode"){
						$aclname = trim($line[1]);
						$newmode = trim($line[2]);
						$this->aclChangeMode["$aclname"]=$newmode;
						if($this->debug>=3)echo "switch $aclname to $newmode<br>\n";
					}
					else if($cmd=="set_domains"){
						$domaingroup=trim($line[1]);
						$domainsadd=trim($line[2]);
						$domainsremove=trim($line[3]);
						if($this->debug>=3)echo "$domaingroup add:'$domainsadd' remove:'$domainsremove'<br>\n";
						if($domaingroup=="only"){
							$this->addDomainsToArray($this->domainsOnlyAdd, $domainsadd);
							$this->addDomainsToArray($this->domainsOnlyRemove, $domainsremove);
						}
						if($domaingroup=="onplus"){
							$this->addDomainsToArray($this->domainsOnplusAdd, $domainsadd);
							$this->addDomainsToArray($this->domainsOnplusRemove, $domainsremove);
						}          
					}
					else{
						if($this->debug>=1) echo "readCommandFiles error: unknown command '".$line[0]."'!<br>\n";
					}
		  
					}
					fclose($handle);
				}
				//Remove double entries
				$this->domainsOnlyAdd=array_unique($this->domainsOnlyAdd);
				$this->domainsOnlyRemove=array_unique($this->domainsOnlyRemove);
				$this->domainsOnplusAdd=array_unique($this->domainsOnplusAdd);
				$this->domainsOnplusRemove=array_unique($this->domainsOnplusRemove);          
	      
				if($this->debug>=4){
					echo "aclChangeMode:<br>\n";
					print_r($this->aclChangeMode);
					print_r($this->domainsOnlyAdd);
					print_r($this->domainsOnlyRemove);
					print_r($this->domainsOnplusAdd);
					print_r($this->domainsOnplusRemove);
				}
		}
    
		function endsWith( $haystack, $needle,$debug=0){
			$length = strlen($needle);
			if ($length == 0) {
				return true;
			}
			if($debug>=3) echo "endsWith".$length.substr($haystack, -$length)."\n";
			return (substr($haystack, -$length) === $needle);
		}  
    
	}

?>
