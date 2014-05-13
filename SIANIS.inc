<?php

// Sidora Analysis API for Islandora
// file: SIANIS.inc
// author: Gert Schmeltz Pedersen gertsp45@gmail.com

class SIANIS {

  function SIANIS() {
  }

  function sianisGetProjectStructure($xslt) {
		global $user;
	 	$datestamp = date('Y-m-d H:i:s');
 		$logdatestamp = date('Y-m-d');
		$logfp = fopen('/tmp/sianis-' . $logdatestamp . '.log', 'a');
		fwrite($logfp, "\n\n[$datestamp] $user->name sianisGetProjectStructure: xslt=$xslt ");
    $solrUrl = variable_get('sianis_block_solr','noSolrUrl');
    $solrMax = variable_get('sianis_block_solr_max','1000');
    $solrXslt = $xslt;
    if ($xslt == '' || $xslt == 'default') {
    	$solrXslt = variable_get('sianis_block_solr_xslt_tree','none');
    }
    $xsltParams =  '';
    if ($solrXslt != 'none') {
    	$xsltParams = '&wt=xslt&tr=' . $solrXslt;
    }
    $q = 'cameraLongitude%3A%5B-180+TO+180%5D+AND+cameraLatitude%3A%5B-90+TO+90%5D';
	$command = 'curl --silent "' . $solrUrl . '?q=' . $q . '&sort=PID+asc&version=2.2&start=0&rows=' . $solrMax . '&indent=on' . $xsltParams . '" 2>&1';
		fwrite($logfp, "\n[$datestamp] command: $command");
	$curlResult = shell_exec($command);
	 	$datestamp = date('Y-m-d H:i:s');
	 	$logString = substr($curlResult,0,600);
		fwrite($logfp, "\n[$datestamp] $user->name curlResult: $logString");
		fclose($logfp);
	print $curlResult;
	module_invoke_all('exit');
	exit();
  }

  function sianisGetSpecies($obstablePids) {
		global $user;
	 	$datestamp = date('Y-m-d H:i:s');
 		$logdatestamp = date('Y-m-d');
		$logfp = fopen('/tmp/sianis-' . $logdatestamp . '.log', 'a');
		fwrite($logfp, "\n\n[$datestamp] $user->name sianisGetSpecies: obstablePids= $obstablePids");
    $fedoraUrl = variable_get('sianis_block_fedora', 'nofedoraurl');
    $fedoraUserPass = variable_get('sianis_block_fedora_userpass', 'nofedorauser:password');
    $params = $max . '/sianisProjectStructureToHtml.xslt';
	$speciesnames = array();
	$obstablePidArray = explode(",", $obstablePids);
	$countPids=count($obstablePidArray);
	$countObsLines = 0;
	for($i=0;$i<$countPids;$i++) {
		$obstablePid = trim($obstablePidArray[$i]);
		$command = 'curl --silent -u ' . $fedoraUserPass . ' "' . $fedoraUrl . '/objects/' . $obstablePid . '/datastreams/CSV/content" 2>&1';
			fwrite($logfp, "\n[$datestamp] $user->name command: $command");
		$curlResult = shell_exec($command);
//			fwrite($logfp, "\n[$datestamp] $user->name curlResult: $curlResult");
		$lines = explode("\r", $curlResult);
		$countLines=count($lines);
		for($j=1;$j<$countLines;$j++) {
			$line = $lines[$j];
			$columns = explode(",", $line);
			$begintime = trim($columns[3]);
			$speciesname = trim($columns[5]);
			if ($speciesname and $begintime) {
				$commonname = trim($columns[6]);
				if ( array_key_exists($speciesname, $speciesnames) ) {
					$countObs = $speciesnames[$speciesname][1];
					$speciesnames[$speciesname] = array($commonname, $countObs + 1);
				} else {
					$speciesnames[$speciesname] = array($commonname, 1);
				}
				$countObsLines++;
				$count = $speciesnames[$speciesname][1];
//					fwrite($logfp, "\n[$datestamp] $user->name $speciesname: $commonname ( $count )");
			}
		}
	}
	$countSpeciesNames = count($speciesnames);
	 	$datestamp = date('Y-m-d H:i:s');
		fwrite($logfp, "\n[$datestamp] $user->name countSpeciesNames: $countSpeciesNames species names");
	ksort($speciesnames);
	$result = '
		<option value=" ">' . $countSpeciesNames . ' names from ' . $countObsLines . ' observations</option>';
	foreach ($speciesnames as $key => $value) {
		$result = $result . '
		<option value="' . $key . '">' . $key . ' (' . $value[0] . ') (' . $value[1] . ')</option>';
	}
		fwrite($logfp, "\n[$datestamp] $user->name result: $result");
		fclose($logfp);
	print '
	<div id="sianisGetSpeciesResult">';
	print $result;
	print '
	</div>
	
	';
	module_invoke_all('exit');
	exit();
  }

  function sianisSelectObstables($south='-90', $west='-180', $north='90', $east='180', $beginDateFrom='20000101', $beginDateTo='20991231', $endDateFrom='20000101', $endDateTo='20991231', $xslt) {
		global $user;
	 	$datestamp = date('Y-m-d H:i:s');
 		$logdatestamp = date('Y-m-d');
		$logfp = fopen('/tmp/sianis-' . $logdatestamp . '.log', 'a');
		fwrite($logfp, "\n\n[$datestamp] $user->name sianisSelectObstables: ( $south , $west ),( $north , $east ) [Ê$beginDateFrom - $beginDateTo ],[ $endDateFrom - $endDateTo ]");
    $solrUrl = variable_get('sianis_block_solr','noSolrUrl');
    $solrMax = variable_get('sianis_block_solr_max','1000');
    $solrXslt = $xslt;
    if ($xslt == '' || $xslt == 'default') {
    	$solrXslt = variable_get('sianis_block_solr_xslt_filtered','none');
    }
    $xsltParams =  '';
    if ($solrXslt != 'none') {
    	$xsltParams = '&wt=xslt&tr=' . $solrXslt;
    }
    $q1 = 'cameraLongitude%3A%5B' . $west . '+TO+' . $east . '%5D+AND+cameraLatitude%3A%5B' . $south . '+TO+' . $north . '%5D';
    $q2 = '+AND+cameraDeploymentBeginDate%3A%5B' . $beginDateFrom . '+TO+' . $beginDateTo . '%5D+AND+cameraDeploymentEndDate%3A%5B' . $endDateFrom . '+TO+' . $endDateTo . '%5D';
    $command = 'curl --silent "' . $solrUrl . '?q=' . $q1 . $q2 . '&sort=PID+asc&version=2.2&start=0&rows=' . $solrMax . '&indent=on' . $xsltParams . '" 2>&1';
		fwrite($logfp, "\n[$datestamp] command: $command");
	$curlResult = shell_exec($command);
	 	$datestamp = date('Y-m-d H:i:s');
	 	$logString = substr($curlResult,0,1000);
		fwrite($logfp, "\n[$datestamp] $user->name curlResult: $logString");
		fclose($logfp);
	print $curlResult;
	module_invoke_all('exit');
	exit();  }
}

?>
