<?php



function ad2_score_ad_scroes_page(){
	
	$header = array(
		'#',
		'כותרת מודעה',
		'סטטוס מודעה',
		'nid' => array('data' => 'צימר' , 'field' => 'n2.title' ) , 
		'tid' , 
		'bid' => array('data' => 'bid' , 'field' => 'ab.bid' ) , 
		'rank'=> array('data' => 'rank' , 'field' => 'ab.rank' , 'sort' => 'desc') , 
		'score' => array('data' => 'score' , 'field' => 'ab.score' ) , 
	);
	
	$modules = module_implements('had_score_component_calculate');
	$weights = variable_get('had_rank_weights' , array() );
	
	foreach ($modules as $m) {
		$header[] = array( 'data' => $m . "<br>" . $weights[$m] . "%" , 'field' => 'ab.comp_' . $m );
	}
	
	$query = db_select('ads' , 'a')->extend('PagerDefault')->extend('TableSort');
	$query->condition('a.adtype' , had_money_get_node_types() , 'IN');
	
	$query->join('ad_bids' , 'ab' , 'ab.aid=a.aid');
	
	$query->leftjoin('node' , 'n' , 'a.aid=n.nid');
	$query->leftjoin('node' , 'n2' , 'a.nid=n2.nid');
	$query->leftjoin('taxonomy_term_data' , 'ttd' , 'ab.tid=ttd.tid');
	$query->fields('n' , array('title'));
	$query->fields('n2' , array('title'));
	$query
		->fields( 'ab' )
		->fields( 'a' , array('nid' , 'adstatus'))
		->fields( 'ttd' , array('name'))
		->limit(50)
  		->orderByHeader($header)
		;
	  
	$ads = $query->execute()->fetchAll();
	//tr( $ads );
	  
	$sites = db_query("select nid,title from node where type = :type" , array(':type' => 'site'))->fetchAllKeyed();
	//tr( $sites );
	
	$rows = array();
	
	foreach ($ads as $ad) {
		
		//tr( $ad );
		$row = array(
			$ad->id,
			l($ad->title, 'node/' . $ad->aid , array('attributes' => array('target' => '_blank'))),
			$ad->adstatus,
			l($ad->n2_title, 'node/' . $ad->nid , array('attributes' => array('target' => '_blank'))),
			$ad->name,
			$ad->bid,
			$ad->rank,
			$ad->score,
		);
		
		foreach ($weights as $key => $w ) {
			$row[] = $ad->{'comp_' . $key};
		}
		
		$rows[] = $row;
	}
	
	$r = array(
		'title' => array(
			'#markup' => '<h3>ציוני מודעות</h3>',
			'#prefix' => '<div>',
			'#suffix' => '</div>',
		),
//		'filter' => array(
//			'#markup' => drupal_render(drupal_get_form('had_bids_filter_form')),
//			'#prefix' => '<div>',
//			'#suffix' => '</div>',
//		),
		'help2' => array(
			'#markup' => '<p>העמודות מצד שמאל מציגות את כל מרכיבי הציון לפני ההכפלה באחוזים.</p>' . '<p>טווח הציניונים נע בין 0.5 ל 1.5 כאשר הממוצע באתר הוא 1.</p>',
			'#prefix' => '<div>',
			'#suffix' => '</div>',
		),
		'help' => array(
			'#markup' => 'אחוז הקלקה ממוצע: ' . variable_get('had_avrage_click_thorugh' , '-' ) . "%",
			'#prefix' => '<div>',
			'#suffix' => '</div>',
		),
		'link' => array(
			'#markup' => l('חשב ציונים מחדש' , 'admin/config/had/rescores'),
			'#prefix' => '<div>',
			'#suffix' => '</div>',
		),
		'table' => array(
			'#theme' =>'table' , 
			'#rows' => $rows,
			'#header' => $header,
		),
		'pager' => array(
			'#theme' => 'pager',
		)
	);
	
	return $r;
}
// 
// function has_score_ad_scroes_page(){
// 	
	// // $node = node_load(24);
	// // tr( $node );
// // 	
	// // tr( $node->roof);
	// // tr( account_balance_spending_today( $node->nid ) );
	// // //_had_score_cron();
	// // die();
// 	
	// $header = array(
		// '#',
// 		
		// 'כותרת מודעה',
		// 'צימר',
		// 'tid' , 
		// 'bid' => array('data' => 'bid' , 'field' => 'ab.bid' ) , 
		// 'rank'=> array('data' => 'rank' , 'field' => 'ab.rank' , 'sort' => 'desc') , 
		// 'score' => array('data' => 'score' , 'field' => 'ab.score' ) , 
		// //'components' 
	// );
// 	
	// $modules = module_implements('had_score_component_calculate');
	// $weights = variable_get('had_rank_weights' , array() );
// 	
	// foreach ($modules as $m) {
		// $header[] = array( 'data' => $m . "<br>" . $weights[$m] . "%" , 'field' => 'ab.comp_' . $m );
	// }
	// //tr($modules );
	// //tr($weights );
// 	
	// $query = db_select('ad_bids' , 'ab')->extend('PagerDefault')->extend('TableSort');
	// $query->join('taxonomy_term_data' , 'ttd' , 'ab.tid=ttd.tid');
	// $query->join('node' , 'n' , 'ab.aid=n.nid');
	// //$query->join('node' , 'n2' , 'ab.nid=n2.nid');
	// $query->fields( 'ab' )
		// ->fields( 'ttd' , array('name'))
		// ->fields( 'n' , array('title'))
		// //->fields( 'n2' , array('site_title'))
		// ->limit(50)
  		// ->orderByHeader($header)
		// ;
// 	  
	// $ads = $query->execute()->fetchAll();
	// //tr( $ads );
// 	  
	// //$sites = db_query("select nid,title from node where type = :type" , array(':type' => 'site'))->fetchAllKeyed();
	// //tr( $sites );
// 	
	// $rows = array();
// 	
	// foreach ($ads as $ad) {
// 		
		// $row = array(
			// $ad->id,
// 			
			// l($ad->title, 'node/' . $ad->aid , array('attributes' => array('target' => '_blank'))),
			// '',
			// $ad->name,
			// $ad->bid,
			// $ad->rank,
			// $ad->score,
		// );
// 		
		// foreach ($weights as $key => $w ) {
			// $row[] = $ad->{'comp_' . $key};
		// }
// 		
		// $rows[] = $row;
	// }
// 	
	// $r = array(
		// 'title' => array(
			// '#markup' => '<h3>ציוני מודעות</h3>',
			// '#prefix' => '<div>',
			// '#suffix' => '</div>',
		// ),
// //		'filter' => array(
// //			'#markup' => drupal_render(drupal_get_form('had_bids_filter_form')),
// //			'#prefix' => '<div>',
// //			'#suffix' => '</div>',
// //		),
		// 'help2' => array(
			// '#markup' => '<p>העמודות מצד שמאל מציגות את כל מרכיבי הציון לפני ההכפלה באחוזים.</p>' . '<p>טווח הציניונים נע בין 0.5 ל 1.5 כאשר הממוצע באתר הוא 1.</p>',
			// '#prefix' => '<div>',
			// '#suffix' => '</div>',
		// ),
		// 'help' => array(
			// '#markup' => 'אחוז הקלקה ממוצע: ' . variable_get('had_avrage_click_thorugh' , '-' ) . "%",
			// '#prefix' => '<div>',
			// '#suffix' => '</div>',
		// ),
		// 'link' => array(
			// '#markup' => l('חשב ציונים מחדש' , 'admin/config/had/rescores'),
			// '#prefix' => '<div>',
			// '#suffix' => '</div>',
		// ),
		// 'table' => array(
			// '#theme' =>'table' , 
			// '#rows' => $rows,
			// '#header' => $header,
		// ),
		// 'pager' => array(
			// '#theme' => 'pager',
		// )
	// );
// 	
	// return $r;
// }

/**
 * Display a form for the ad module settings.
 */
function ad2_score_weights_settings($form_state) {
  
	
	$list = array();
	foreach (module_implements('ad2_score_component_info') as $module) {
		$list = $list + module_invoke($module , 'ad2_score_component_info' );
	}
	
	$weights = variable_get('ad2_score_weights' , array()  );
	
	//tr( $weights );
	
	$form['weights'] = array(
		'#type' => 'continer',
		'#tree' =>true,
	);
  
	foreach ($list as $key =>  $title ) {
		
		$form['weights'][$key] = array(
			'#type' => 'textfield',
			'#title' => $title,
			'#default_value' => isset( $weights[$key] ) ?  $weights[$key] : 0,
			'#size' => 5,
			//'#description' => t('If enabled, %tag will be added to advertisement links generated by this module.', array('%tag' => t('rel="nofollow"'))),
		);
	
	}


	$form['save'] = array(
		'#type' => 'submit',
		'#value' => t('Save'),
	);
	
	return $form;
}


function ad2_score_weights_settings_validate($form, &$form_state) {
  
	$weights = $form_state['values']['weights'];
	
	$sum = array_sum( $weights );
	
	if( $sum != 100 ){
		form_error($form['weights'] , 'sum should be 100');
	}
	
}



/**
 * Display a form for the rank weights settings.
 */
function ad2_score_weights_settings_submit($form, &$form_state) {
  
	$weights = $form_state['values']['weights'];
	
	variable_set('ad2_score_weights' , $weights );
	
	drupal_set_message('Data Saved.');
}

