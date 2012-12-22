<?php

function ad2_parent_ads_page($node){
	
	
	$query = new EntityFieldQuery();
  $entities = $query->entityCondition('entity_type', 'ad2')
                    //->entityCondition('bundle', 'event')
                    ->propertyCondition('status', 1)
                    ->fieldCondition(AD2_SITE_FIELD, 'nid', $node->nid)
                    //->fieldOrderBy('field_date', 'value', 'ASC')
                    ->execute();


  $ads = isset($entities['ad2']) ? ad2_load_multiple(array_keys($entities['ad2'])) : array();
  
 // $build = array();
  // foreach ($ads as $ad ) {
  // //	tr( $ad );
// 	  
	  // tr( entity_view($ad, 'teaser') );
     // $build['ads' . $ad->aid] = entity_view($ad, 'teaser');
  // }
  
  //$views = ad2_view_multiple($ads, 'teaser');
  
  //$build['#nodes'] = $views['nodes'];
  //$build['#theme'] = 'whatever';

  //return $build;
	
	// $q = db_select('ad2' , 'a');
	// $q->condition('a.nid' , $node->nid );
	// $q->fields('a');
	// $results = $q->execute()->fetchAll();
	
	
	$rows = array();
	
	foreach ($ads as $ad ) {
		//$ad->id = (int) $ad->id;
		$row = array(
			$ad->aid,
			$ad->title,
			$ad->type,
			l(t('Edit') , 'ad2/' . $ad->aid .'/edit'),
			l(t('Delete') , 'ad2/' . $ad->aid .'/delete'),
			//$ad->status,
			//$ad->uid,
			//isset($node->field_ad_type['und']) ? $node->field_ad_type['und'][0]['value'] : '-',
		);
		
		$rows[] = $row;
	}
	
	$build = array();
	
	
	
	$build['table'] = array(
		'#theme' => 'table',
		'#rows' => $rows,
		'#header' => array('#' , t('Title') , t('Type') , t('Edit') , t('Delete') )
	);
	
	$types = ad2_types();
	$links = array();
	foreach ($types as $type => $info ) {
		$links[] = l( t('Add !label' , array('!label' => $type)) , 'ad2/add/' . $type , array('query' => array('site'=> $node->nid)));
	}
	//tr( $links );
	//tr( $types );
	$build['add'] = array(
		'#prefix' => '<ul><li>',
		'#markup' => implode('</li><li>' , $links ),
		'#suffix' => '</li></ul>',
	);
	
	
	return $build;
}
