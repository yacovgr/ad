<?

function had_admin_batch_form(){

  $form['batch'] = array(
    '#type' => 'container',
    '#tree' => true,
  );


  $form['batch']['delete'] = array(
    '#type' => 'checkboxes',
    '#title' => 'Delete Ads',
    '#options' => array(
    	'all' 		=> 'delete all ads.' ,
  	),
  );

  $form['batch']['site_ad'] = array(
    '#type' => 'checkboxes',
    '#title' => 'site_ad Ads',
    '#options' => array(
  		'delete' 	=> 'delete all site_ad ads',
    	'create' 	=> 'create site_ad nodes for all sites' ,
    ),
  );

  $form['batch']['site_click_ad'] = array(
    '#type' => 'checkboxes',
    '#title' => 'site_click_ad',
    '#options' => array(
  		'delete' 	=> 'delete all site_click_ad ads',
    	'create' 	=> 'one site_click_ad nodes for all sites' ,
    	'clicks'	=> 'add clicks from old site',
    ),
  );

  $form['batch']['site_ad_post'] = array(
    '#type' => 'checkboxes',
    '#title' => 'site_ad_post Ads',
    '#options' => array(
    	'delete' 	=> 'delete all site_ad_post ads',
    	'create' 	=> 'create site_ad_post ads for all sites.',
    ),
  );

  $form['batch']['fix'] = array(
    '#type' => 'checkboxes',
    '#title' => 'Fix Ads',
    '#options' => array(
    	'body' 	=> 'fix all ads body',
    	'filtered_html' => 'change all body fields filter to filtered_html',
    	'strip_html' 	=> 'strip html from ads text',
    	'images' 		=> 'add missing images',
    	//'comps' 		=> 'fix components',
    	'reset'			=> 'reset ads price and auto and bids bid and auto',
    	'reset_views'   => 'reset all adsstats and clicks',			
    ),
  );


  $form['batch']['site_ad_front'] = array(
    '#type' => 'checkboxes',
    '#title' => 'site_ad_front Ads',
    '#options' => array(
    	'delete' 	=> 'delete all site_ad_front ads',
    	'create' 	=> 'create site_ad_front ads for all sites.',
    ),
  );


  $form['batch']['bids'] = array(
    '#type' => 'checkboxes',
    '#title' => 'Ads Bids',
    '#options' => array(
    	'remove_duplicate' 	=> 'remove duplicate rows from table ad_bids',
    	'reset_status' 	=> 'reset status to 1 in all rows in table ad_bids',
    ),
  );

  $form['batch']['site_ad_price'] = array(
    '#type' => 'checkboxes',
    '#title' => 'Site Ads',
    '#options' => array(
    	'fix_price_field' 	=> 'pupolate price field',
    	//'reset_status' 	=> 'reset status to 1 in all rows in table ad_bids',
    ),
  );



  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Go',
  );

  return $form;
}



function had_admin_batch_form_submit($form, &$form_state) {

	$items = array();

	foreach ($form_state['values']['batch'] as $k => $v  ) {
		foreach ($v as $key => $op ) {
			if( $op )
				$items[] = "had_batch_" . $k . '_' . $key;
		}

	}
//tr($items);die();
	$operations = array();
	foreach ($items as $function ) {
		 $function( $operations );
	}

	if( $items ){
		$batch = array(
			'operations' => $operations,
			'finished' => 'hapisga_had_batch_finished',
			'file' => drupal_get_path('module', 'had') . '/include/had.tasks.inc',
		);


		batch_set($batch);
	} else {
		drupal_set_message('nothing to do');
	}

}


function hapisga_had_batch_finished( $success, $results, $operations){

	if ($success) {
		// Here we could do something meaningful with the results.
		// We just display the number of nodes we processed...

		foreach ($results as $result) {
//			if( is_array($result) ){
//				$text = implode("," , $result );
//				drupal_set_message(t('result @section "%final"', array('%final' => $text )));
//			} else {
				drupal_set_message(t('result "%final"', array('%final' => $result )));
			//}
		}
		drupal_set_message(t('@count results processed .', array('@count' => count($results))));
	}
	else {
		// An error occurred.
		// $operations contains the operations that remained unprocessed.
		$error_operation = reset($operations);
		drupal_set_message(t('An error occurred while processing @operation with arguments : @args', array('@operation' => $error_operation[0], '@args' => print_r($error_operation[0], TRUE))));
	}

	drupal_set_message( l( 'דו"ח שגיאות' , 'admin/reports/dblog' , array('attributes' => array('target' => '_blank')) ));
	//cache_clear_all();
}

function had_batch_site_click_ad_delete(&$operations){
	$rows = db_query("select nid from node where type='site_click_ad' ");
	foreach ($rows  as $i => $row) {
  		$operations[] = array('_had_delete_had', array($row->nid));
	}
}

function had_batch_site_click_ad_create( &$operations ){
	$rows = db_query("select nid from node where type='site' ")->fetchAll();
  	foreach ($rows  as $i => $row) {
  		$operations[] = array('_had_create_ad', array($row->nid , 'site_click_ad'));
	}
}


function had_batch_site_click_ad_clicks( &$operations ){
	
	$db = _hapisga_import_getdb();
	
	$select = $db-> select()
			-> from(array('sa' =>'click_site_account' ) , array('nid' =>'sa.siteID' , "total" => "sum(clicks)") )
			-> group("sa.siteID")
			//-> where("sa.siteID = ? " ,  $site->id )
			;
    	
    $map = $db->fetchPairs($select);
	//tr( $map );
	
	$select2 = $db-> select()
			-> from(array('sa' =>'click_site_targets' ) , array('nid' =>'sa.siteID' , "override" => "sum(override)") )
			-> group("sa.siteID")
			-> where("sa.date >= ? " ,  '2012-01-05' )
			;
    	
    $map2 = $db->fetchPairs($select2);
	//tr( $map2 );
	//die();
	
	foreach ($map2 as $nid => $override) {
		if( isset($map[$nid])){
			$map[$nid] += $override;
		} else {
			$map[$nid] = $override;
			//tr(' not set ');
		}
	}
	
	//tr( $map );
	//die();
	
	
	//$rows = db_query("select nid from node where type='site' ")->fetchAll();
	
	$rows = db_query("select nid from node where type='site_click_ad' ")->fetchAll();
	
	
  	//die();
  	foreach ($rows  as $i => $row) {
  		
		$operations[] = array('_had_update_site_clicks_ad', array($row->nid ,  $map ));
	}
	
	//die();
}


function _had_update_site_clicks_ad($nid , $map ,   &$context){
	// here i need to find out 
	
	$ad = node_load($nid);
	
	if( $ad ){
		$site_nid = $ad->ad->nid;
		if( isset($map[$site_nid]) ){
			$clicks = $map[$site_nid];
			if( $clicks < 0 )
				$clicks = 0;
			$ad->field_clicks_left['und'][0]['value'] = $clicks;
		} else {
			$clicks = 0;
			$ad->field_clicks_left['und'][0]['value'] = $clicks;
		}
		
		$ad->status = 1;
		if( $clicks == 0 ){
			$ad->status = 0;
		}
		
		//$ad->redirect_url = '/node/' . $ad->ad->nid;
		try{
			node_save( $ad );
		} catch(exception $e ){
			$context['results'][] = 'ad ' . $ad->nid . 'site ' . $site_nid . ' error: ' . $e->getMessage();
		}
	}
}


function had_batch_delete_all( &$operations ){

	$q = db_select('node' , 'n');
	$q->fields('n' , array('nid'));
	$q->condition('type' , had_get_node_types() , 'IN' );

	$nids = $q->execute()->fetchCol();
	foreach ($nids  as $nid) {
  		$operations[] = array('_had_delete_had', array($nid));
	}
}

function _had_delete_had($nid , &$context ){
	node_delete($nid);
	$context['message'] = t('Deleting node "@title"', array('@title' => $nid));
}

function had_batch_site_ad_create( &$operations ){
	$rows = db_query("select nid from node where type='site' ");
	foreach ($rows  as $i => $row) {
  		$operations[] = array('_had_create_ad', array($row->nid , 'site_ad'));
	}
}


function had_batch_site_ad_delete(&$operations){
	$rows = db_query("select nid from node where type='site_ad' ");
	foreach ($rows  as $i => $row) {
  		$operations[] = array('_had_delete_had', array($row->nid));
	}
}

function _had_create_ad($nid , $type ,  &$context ){

	$node = node_load($nid);

	$ad = new stdClass();
	$ad->type 		= $type;
	$ad->uid 		= $node->uid;
	$ad->status 	= 1;//, $node->status;
	$ad->language 	= 'he';
	//$ad->redirect_url = "/node/" . $nid;
	$ad->master 	= 1;

	$ad->field_site['und'][0]['nid'] = $node->nid;
	


	switch ($type) {
		case 'site_ad_post':

			$comment_cid = _had_get_comment($node->nid);

			/**
			 * create only for sites that have comments - if dont have comment - dont create
			 */
			if( ! $comment_cid ){
				continue;
			}


			
			$ad->title = 'מודעת מערכת'; 
			$ad->title_field['he'][0]['value'] = 'מודעת מערכת'; 
			$ad->title_field['en'][0]['value'] = 'system ad';

			$ad->field_ad_body = $node->field_teaser;


			//$ad->field_click_price['und'][0]['value'] = had_get_default_click_price();

			if( isset( $node->field_image['und'][0]['fid']) &&   $node->field_image['und'][0]['fid']){
				$ad->field_ad_image['und'][0]['fid'] = $node->field_image['und'][0]['fid'];
			}

			$ad->field_comment_cid['und'][0]['value'] = $comment_cid;

			node_save($ad);


			$time = time();
	 		$t = array(
	 			'entity_type' 	=> 'node',
	 			'entity_id'		=> $ad->nid,
	 			'language'		=> 'en',
	 			'source'		=> 'he',
	 			'uid'			=> $ad->uid,
		 		'status'		=> $ad->status,
		 		'translate'		=> 0,
		 		'created'		=> $time,
		 		'changed'		=> $time,
	 		);

	 		$return_value = db_insert('entity_translation')
	                    ->fields($t)
	                    ->execute();
						

			break;

		case 'site_ad_front':

			
			$ad->title = 'מודעת מערכת'; 
			$ad->title_field['he'][0]['value'] = 'מודעת מערכת'; 
			$ad->title_field['en'][0]['value'] = 'system ad';
			
			
			$ad->field_ad_body = $node->field_teaser;

			//$ad->ad->price = had_get_default_click_price();
			//$ad->field_click_price['und'][0]['value'] = had_get_default_click_price();

			if( isset( $node->field_image['und'][0]['fid']) &&   $node->field_image['und'][0]['fid']){
				$ad->field_ad_image['und'][0]['fid'] = $node->field_image['und'][0]['fid'];
			}

			node_save($ad);
		
			
			$time = time();
	 		$t = array(
	 			'entity_type' 	=> 'node',
	 			'entity_id'		=> $ad->nid,
	 			'language'		=> 'en',
	 			'source'		=> 'he',
	 			'uid'			=> $ad->uid,
		 		'status'		=> $ad->status,
		 		'translate'		=> 0,
		 		'created'		=> $time,
		 		'changed'		=> $time,
	 		);

	 		$return_value = db_insert('entity_translation')
	                    ->fields($t)
	                    ->execute();
						

			break;

		case 'site_ad':

			
			$ad->title = 'מודעת מערכת'; 
			$ad->title_field['he'][0]['value'] = 'מודעת מערכת'; 
			$ad->title_field['en'][0]['value'] = 'system ad';
			
			
			$ad->field_ad_body = $node->field_teaser;


			//$ad->field_click_price['und'][0]['value'] = had_get_default_click_price();

			if( isset( $node->field_image['und'][0]['fid']) &&   $node->field_image['und'][0]['fid']){
				$ad->field_ad_image['und'][0]['fid'] = $node->field_image['und'][0]['fid'];
			}

			node_save($ad);

			
			$time = time();
	 		$t = array(
	 			'entity_type' 	=> 'node',
	 			'entity_id'		=> $ad->nid,
	 			'language'		=> 'en',
	 			'source'		=> 'he',
	 			'uid'			=> $ad->uid,
		 		'status'		=> $ad->status,
		 		'translate'		=> 0,
		 		'created'		=> $time,
		 		'changed'		=> $time,
	 		);

	 		$return_value = db_insert('entity_translation')
	                    ->fields($t)
	                    ->execute();
						
			
			
			break;


		case 'site_click_ad':

			$ad->title = 'מודעת מערכת'; 
			$ad->title_field['he'][0]['value'] = 'מודעת מערכת'; 
			$ad->title_field['en'][0]['value'] = 'system ad'; // 'פרסומת לצימר ' . isset($node->title_field['en'][0]['value'] ) ? $node->title_field['en'][0]['value'] : $nid;



			$ad->field_ad_body = $node->field_teaser;

			$ad->field_clicks_left['und'][0]['value'] = 10000;

			if( isset( $node->field_image['und'][0]['fid']) &&   $node->field_image['und'][0]['fid']){
				$ad->field_ad_image['und'][0]['fid'] = $node->field_image['und'][0]['fid'];
			}

			node_save($ad);

			$time = time();
	 		$t = array(
	 			'entity_type' 	=> 'node',
	 			'entity_id'		=> $ad->nid,
	 			'language'		=> 'en',
	 			'source'		=> 'he',
	 			'uid'			=> $ad->uid,
		 		'status'		=> $ad->status,
		 		'translate'		=> 0,
		 		'created'		=> $time,
		 		'changed'		=> $time,
	 		);

	 		$return_value = db_insert('entity_translation')
	                    ->fields($t)
	                    ->execute();
						
							
			break;


		default:

			break;
	}


	$context['results'][] = "creating ad for node " . $node->title;
	$context['message'] = t('Loading node "@title"', array('@title' => $nid));
}

function had_batch_site_ad_front_create(&$operations){
	$rows = db_query("select nid from node where type='site' ");
	foreach ($rows  as $i => $row) {
  		$operations[] = array('_had_create_ad', array($row->nid , 'site_ad_front'));
	}
}

function had_batch_site_ad_post_delete(&$operations){
	$rows = db_query("select nid from node where type='site_ad_post' ");
	foreach ($rows  as $i => $row) {
  		$operations[] = array('_had_delete_had', array($row->nid));
	}
}


function had_batch_site_ad_post_create(&$operations){
	$rows = db_query("select nid from node where type='site' ");
	foreach ($rows  as $i => $row) {
  		$operations[] = array('_had_create_ad', array($row->nid , 'site_ad_post'));
	}
}

function had_batch_site_ad_front_delete(&$operations){
	$rows = db_query("select nid from node where type='site_ad_front' ");
	foreach ($rows  as $i => $row) {
  		$operations[] = array('_had_delete_had', array($row->nid));
	}
}



function _had_get_comment($nid , $score = null ){

	$query = db_select('comment', 'c');

	$query
	  ->condition('c.status', 1, '=')
	  ->fields('c', array('cid'))
	  ->join('node' , 'n' , 'c.nid = n.nid')
	  ;

	if( $score ){
		$query->join('field_data_field_scores' , 'fs' , 'fs.entity_id = c.cid');
		$query->condition('fs.entity_type', 'comment');
		$query->condition('fs.bundle', 'comment_node_site');
		$query->condition('fs.deleted', 0 );
		$query->condition('fs.delta', 0 );
		$query->condition('fs.language', 'und' );
		$query->condition('fs.field_scores_score', $score , '>' );
	}

	$query->orderBy('c.created', 'DESC');
	$query->range(0, 1);

	$query->condition('c.nid' , $nid );
	$result = $query->execute()->fetchObject();

	if( $result ){
		return $result->cid;
	} else {
		return 0;
	}

}


// this maybe should be in bids module
function had_batch_bids_remove_duplicate(&$operations){

	$rows = db_query("select id,aid,tid from ad_bids ")->fetchAll();

	$ads = array();
	foreach ($rows as $row) {
		$ads[$row->aid][$row->tid][] = $row->id		;
	}

	foreach ($ads as $aid => $tids ) {
		foreach ($tids as $tid => $ids ) {
			if( count($ids) == 1  ){
				unset($ads[$aid][$tid]);
			}
		}
	}

	foreach ($ads as $aid => $tids ) {
		if( ! count($tids) ){
			unset($ads[$aid]);
		}
	}
	//tr( $ads );
	$count = 0;
	foreach ($ads as $aid => $tids ) {
		foreach ($tids as $tid => $ids ) {
			array_shift($ids);
			$res = db_delete('ad_bids')
				->condition('id' , $ids , 'IN')
				->execute()
				;
			$count += 	$res;

		}
	}

	drupal_set_message($count . ' rows deleted from ad_bids');

}

// this maybe should be in bids module
function had_batch_bids_reset_status(&$operations){

	db_update('ad_bids')
		//->condition('id' , $row->id )
		->fields(array(
			'status' => 1,
		))
		->execute()
		;

	drupal_set_message(/*count($rows) .*/ 'all rows where updated from ad_bids');

}


function had_batch_fix_images(&$operations){
			
	
	$q = db_select('node' , 'n');
	$q->fields('n' , array('nid') );
	$q->condition('type' , had_get_node_types() , 'IN' );

	$nids = $q->execute()->fetchCol();

	foreach ($nids as $nid ) {
		$operations[] = array('_had_batch_fix_the_images', array($nid ));
	}
}

function had_batch_fix_reset(&$operations){

	$q = db_select('ads' , 'a');
	$q->fields('a'  );
	$rows = $q->execute()->fetchAll();

	$chunks = array_chunk($rows, 75 );
	foreach ($chunks as $chunk ) {
		$operations[] = array('_had_batch_fix_the_ads', array($chunk ));
	}


	$q = db_select('ad_bids' , 'ab');
	$q->fields('ab'  );

	$rows = $q->execute()->fetchAll();

	$chunks = array_chunk($rows, 75 );
	foreach ($chunks as $chunk ) {
		$operations[] = array('_had_batch_fix_the_ads2', array($chunk ));
	}



	$q = db_select('node' , 'n');
	$q->fields('n' , array('nid') );
	$q->condition('type' , 'site' );

	$rows = $q->execute()->fetchCol();

	foreach ($rows as $nid ) {
		//$operations[] = array('_had_batch_fix_the_ads3', array($nid ));
	}



	$q = db_select('node' , 'n');
	$q->fields('n' , array('nid') );
	$q->condition('type' , array('site_ad' , 'site_ad_front' , 'site_ad_post') , 'IN' );

	$rows = $q->execute()->fetchCol();

	foreach ($rows as $nid ) {
		//$operations[] = array('_had_batch_fix_the_ads4', array($nid ));
	}




}

function had_batch_fix_comps(&$operations){

	$q = db_select('ad_bids' , 'ab');
	$q->fields('ab'  );
	//$q->range(12000,200);

	$rows = $q->execute()->fetchAll();

	$chunks = array_chunk($rows, 75 );
	//$weights = variable_get('had_rank_weights' , array() );
	//tr( $weights );
	
	foreach ($chunks as $chunk ) {
		
		//tr( $row );
		// $comps = unserialize($row->components);
		// tr( $comps );
		// $fields = array();
		// foreach ($weights as $key => $value) {
			// $fields['comp_' . $key] = $comps[$key] / ( $value / 100 );
		// }
		// tr( $fields );
// 		
		// db_update('ad_bids' , 'ab')
		// ->fields($fields)
		// ->condition('ab.id' , $row->id )
		// ->execute()
		// ;
		
		//$operations[] = array('_had_batch_fix_the_comps', array($chunk ));
	}
	
	//die();
}

function had_batch_fix_reset_views(&$operations){

	$q = db_delete('ad_statistics')
		->condition('action' , 'create' , '<>')
		->execute()
		;
		
	

	$q = db_delete('ad_clicks')
		//->condition('action' , 'create' , '<>')
		->execute()
		;
		
	
		
}

function had_batch_fix_body(&$operations){

	$q = db_select('node' , 'n');
	$q->fields('n' , array('nid') );
	$q->condition('type' , had_get_node_types() , 'IN' );

	$nids = $q->execute()->fetchCol();

	foreach ($nids as $nid ) {
		$operations[] = array('_had_batch_fix_the_body', array($nid ));
	}
}

function had_batch_site_ad_price_fix_price_field(&$operations){

	$q = db_select('node' , 'n');
	$q->fields('n' , array('nid') );
	$q->condition('type' ,'site_ad' );

	$nids = $q->execute()->fetchCol();

	foreach ($nids as $nid ) {
		$operations[] = array('_had_batch_site_ad_price_fix_price_field', array($nid ));
	}
}

function had_batch_fix_filtered_html(&$operations){

	//tr( node_load(3790));
	//die();

	$q = db_select('node' , 'n');
	$q->fields('n' , array('nid') );
	$q->condition('type' , had_get_node_types() , 'IN' );

	$nids = $q->execute()->fetchCol();

	foreach ($nids as $nid ) {
		$operations[] = array('_had_batch_fix_filtered_html', array($nid ));
	}
}


function had_batch_fix_strip_html(&$operations){

	
	$q = db_select('node' , 'n');
	$q->fields('n' , array('nid') );
	$q->condition('type' , had_get_node_types() , 'IN' );

	$nids = $q->execute()->fetchCol();

	foreach ($nids as $nid ) {
		$operations[] = array('_had_batch_fix_strip_html', array($nid ));
	}
}



function _had_batch_fix_the_body($nid ,  &$context){

	$ad = node_load($nid);
	$site_nid = $ad->field_site['und'][0]['nid'];

	if( $site_nid ){
		$site = node_load($site_nid);

		$ad->field_ad_body = $site->field_teaser;

		try{
			node_save($ad);
			drupal_set_message("ad $ad->nid good!");
		} catch ( Exception $e  ) {
			//echo  $e->getMessage() . "<br>";
			drupal_set_message($e->getMessage() , 'error');
		}
	}
}


function _had_batch_fix_the_images($nid ,  &$context){

	$ad = node_load($nid);
	$site_nid = $ad->field_site['und'][0]['nid'];

	
	//if( count( $ad->field_ad_image ) == 0  ){
	$node = node_load( $site_nid );
	if( ! $node || $node->status == 0 ){
		//$context['results'][] = 'site not found or disabled ' . $site_nid;
		return;
	}
			
	if( isset( $node->field_galleries['und'][0]['nid'] ) ){
		$gid = $node->field_galleries['und'][0]['nid'];
		if( $gid ){
			$g = node_load( $gid );
			
			if( $g ){
				
				$file = file_load( $g->field_image['und'][0]['fid'] );
				if( $file ){
					if( $ad->field_ad_image['und'][0]['fid'] != $g->field_image['und'][0]['fid'] ){
						$ad->field_ad_image['und'][0]['fid'] = $g->field_image['und'][0]['fid'];
						node_save($ad);
				
						$context['results'][] = $ad->nid . ' missing image added: ' . $g->field_image['und'][0]['fid'];
					} else {
						$context['results'][] = 'not updated ' . $ad->nid;
					}
				} else {
					$context['results'][] = 'file not good ';
				}
			} else {
				$context['results'][] = 'gallery not found ' . $gid;
			}
		} else {
			$context['results'][] = 'no gid ' . $gid;
		}
	} else {
		$context['results'][] = 'site has no galleries ' . $site_nid;
	}
		

	//}
	
}



function _had_batch_site_ad_price_fix_price_field($nid ,  &$context){
	$ad = node_load($nid);
	$ad ->field_click_price['und'][0]['value']=had_get_default_click_price();

	node_save($ad);
}




function _had_batch_fix_the_ads( $rows ,  &$context){

	$count = 0;
	foreach ($rows as $row ) {
		
		//if( $row->auto != 0 || $row->price != 1.2 ){
			
			db_update('ads')
				->condition('aid' , $row->aid )
				->fields(array(
					'auto' => 0,
					'price' => 1.2,
					'auto_manual' => -1,
					'price_manual' => -1,
				))
				->execute()
				;
				
			$count++;
				
		//}
		
		
	}
	
	$context['results'][] = ' count = ' . $count;

}


function _had_batch_fix_the_ads2( $rows ,  &$context){

	$count = 0;
	foreach ($rows as $row ) {
		
		//if( $row->auto != 0 || $row->bid_manual != 0 || $row->auto_manual !=-1){
			
			db_update('ad_bids')
				->condition('id' , $row->id )
				->fields(array(
					'bid' => 1.2,
					'auto' => 0,
					'bid_manual' => 0,
					'auto_manual' => -1,
					'status' => 1,
				))
				->execute()
				;
				
			$count++;
				
		//}
		
		
	}
	
	$context['results'][] = ' count = ' . $count;

}


function _had_batch_fix_the_ads3( $nid ,  &$context){

	$node = node_load( $nid );
	
	$node->field_auto_campaign['und'][0]['value'] = 0;
	$node->field_click_price['und'][0]['value'] = 1.2;
	
	
	$context['results'][] = ' node  ' . $node->nid . ' saved';

	node_save( $node );
}


function _had_batch_fix_the_ads4( $nid ,  &$context){

	$node = node_load( $nid );
	
	$node->field_click_price['und'][0]['value'] = 1.2;
	$context['results'][] = ' node  ' . $node->nid . ' saved';

	node_save( $node );
}



// 
// function _had_batch_fix_the_comps( $rows ,  &$context){
// 
// 	
	// $weights = variable_get('had_rank_weights' , array() );
	// //tr( $weights );
// 	
	// foreach ($rows as $row ) {
// 		
		// $comps = unserialize($row->components);
		// $fields = array();
		// foreach ($weights as $key => $value) {
			// $fields['comp_' . $key] = $comps[$key] / ( $value / 100 );
		// }
		// //tr( $fields );
// 		
		// db_update('ad_bids' )
		// ->fields($fields)
		// ->condition('id' , $row->id )
		// ->execute()
		// ;
// 		
	// }
// 	
// 
// }

function _had_batch_fix_filtered_html($nid ,  &$context){

	$ad = node_load($nid);

	$langs = array('he' , 'en');

	$do = false;
	foreach ($langs as $langcode){
		if( isset( $ad->field_ad_body[$langcode][0]['value'] )){
			if( $ad->field_ad_body[$langcode][0]['format'] != 'filtered_html' ){
				$ad->field_ad_body[$langcode][0]['format'] = 'filtered_html';
				$do = true;
			}
		}
	}

	if( $do ){
		try{
			node_save($ad);
			watchdog('had', 'task fix_filtered_html @nid good' , array('@nid' => $nid) , WATCHDOG_DEBUG );
			//drupal_set_message("ad $ad->nid good!");
		} catch ( Exception $e  ) {
			//echo  $e->getMessage() . "<br>";
			watchdog('had', 'task fix_filtered_html @nid error: @msg' , array('@nid' => $nid , '@msg' => $e->getMessage()) , WATCHDOG_DEBUG );
			//drupal_set_message($e->getMessage() , 'error');
		}
	}

}

function _had_batch_fix_strip_html($nid ,  &$context){

	$ad = node_load($nid);

	$langs = array('he' , 'en');

	$do = false;
	foreach ($langs as $langcode){
		if( isset( $ad->field_ad_body[$langcode][0]['value'] )){

			$striped = strip_tags ( $ad->field_ad_body[$langcode][0]['value'] , '<a>');

			if( $ad->field_ad_body[$langcode][0]['value'] != $striped ){
				$ad->field_ad_body[$langcode][0]['value'] = $striped;
				$do = true;
			}
		}
	}

	if( $do ){
		try{
			node_save($ad);
			watchdog('had', 'task fix_strip_html @nid good' , array('@nid' => $nid) , WATCHDOG_DEBUG );
		} catch ( Exception $e  ) {
			watchdog('had', 'task fix_strip_html @nid error: @msg' , array('@nid' => $nid , '@msg' => $e->getMessage()) , WATCHDOG_DEBUG );
		}
	}

}