<?

function hook_ad2_cron_info(){
	
	return array(
		'functionname1' =>'label1',
		'functionname2' =>'label2',
	);
}


 
 
function hook_ad2_cron_run($delta){
	
	
	// do something here
	
	//watchdog('budget cron' , '@n rows updated' , array('@n' => $count ) );

}
 
 
 