<?php

function ad2_bids_bids_form( $form , &$form_state , $ad ){

	$q = db_select('ad2_rank' , 'a');
	$q->fields('a');
	$q->condition('aid' , $ad->aid );
	$rows = $q->execute()->fetchAll();
	
	//tr( $rows );
	
	drupal_add_css(drupal_get_path('module',  'ad2_bids') . "/ad2_bids.css" );
	
	$site_nid 	= $ad->{AD2_SITE_FIELD}['und'][0]['nid'];
	$site 		= node_load( $site_nid );
	
	$ad->bids = $rows;
	$form['#ad2'] = $ad;
	
	
	//tr( $ad );
	
	$price = isset( $ad->{AD2_CLICKPRICE_FIELD}['und'][0]['value'] ) ? $ad->{AD2_CLICKPRICE_FIELD}['und'][0]['value'] : 1.57;
	$form['master-ad-bid'] = array(
		'#type' => 'markup',
		'#prefix' => '<div>',
		'#suffix' => '</div>',
		'#markup' => "<b>" . t('Ad Master Bid') . ": </b>" . $price,
	);
	
	$form['master-ad-auto'] = array(
		'#type' => 'markup',
		'#prefix' => '<div>',
		'#suffix' => '</div>',
		'#markup' => "<b>" . t('Ad Master Auto') . ": </b>" .  $ad->auto,
	);
	
	$form['bids'] = array(
		'#type' => 'container',
		'#tree' => true,
		'#weight' => 10,
	);
  
  
	$master = 1.33; // $node->ad->price; // $node->field_click_price['und'][0]['value'];
	
	foreach ( /*$node->bids */ $rows as $row  ) {

		$tid = $row->tid;
		$term = taxonomy_term_load( $tid );
		
		$form['bids'][$tid] = array(
			'#type' => 'container',
			'#tree' => true,
		);
		$form['bids'][$tid]['id'] = array(
			'#type' => 'hidden',
			'#value' => $row->id,
		);

		$form['bids'][$tid]['term'] = array(
			'#type' => 'markup',
			'#markup' => $term->name,
		);


		if( $ad->master ){
			$form['bids'][$tid]['status'] = array(
				'#type' => 'markup',
				'#markup' => 'פעיל',
			);
			
		} else {
			$form['bids'][$tid]['status'] = array(
				'#type' => 'checkbox',
				'#default_value' => $row->status,
			);
		}
		
		$value = $row->bid;
	 	$value = $row->bid_manual != 0 ? $row->bid_manual : $row->bid;
		if( $row->auto ){
			$value = $row->bid_manual != 0 ? $row->bid_manual : $master;
			$value = "<div class='crossed'>$value</div>";
		} else {
			
		}
		
		$form['bids'][$tid]['bid'] = array(
			'#type' => 'markup',
			'#markup' => $value,
			'#size' => 4,
		);
		
		
		if( $row->auto ){
			$form['bids'][$tid]['bid_manual'] = array(
				'#type' => 'markup',
				'#markup' => '',
			);
			
		} else {
			$form['bids'][$tid]['bid_manual'] = array(
				'#type' => 'textfield',
				'#default_value' => $row->bid_manual != 0 ? $row->bid_manual : '',
				'#size' => 4,
			);
		}

		
		$attributes = array();
		if( $row->auto_manual != -1 ){
			$attributes = array('class'=> array('auto-manual'));
		}
		$form['bids'][$tid]['auto'] = array(
			'#type' 			=> 'checkbox',
			'#default_value' 	=> $row->auto,
			'#attributes' 		=> $attributes,
		);
		
	}
	
	$form['submit'] = array(
		'#type' => 'submit', 
		'#title' => t('Save'), 
		'#value' => t('Save'), 
	);
	
	return $form;
}

function ad2_bids_bids_form_validate( $form , &$form_state ){
	return;
	
	foreach ($form_state['values']['bids'] as $tid => $data ) {
		if( isset($data['bid_manual']) ){
			if( $data['bid_manual'] && $data['bid_manual'] < had_get_default_click_price() ){
				form_error($form['bids'][$tid]['bid_manual'] , t('לא ניתן להזין ערך נמוך מ @min.' , array('@min' => had_get_default_click_price() )));
			}
		}
	}
}

function ad2_bids_bids_form_submit( $form , &$form_state ){

	$ad = $form['#ad2'];
		
		//tr( $form_state['values']['bids'] );
		
		//die();
		
	foreach ($form_state['values']['bids'] as $tid => $data ) {
			
		$status = 1;
		if( isset($data['status']) ){
			$ad->bids[$tid]->status = $data['status'];
			$status = $data['status'];
		}
		
		//$node->bids[$tid]['new_auto'] 	= $data['auto'];
		
		// if( isset($data['bid_manual'] )){
			// $node->bids[$tid]['new_bid_manual'] = $data['bid_manual'];
		// }
		
		
		db_update('ad2_rank')
			->fields(array(
				'status' 	=>  $status,
				'auto' 		=>  $data['auto'],
				'rerank'	=>  1,
			))
			->condition('id' , $data['id'])
			->execute()
			;
	}
	
	//node_save($node );
	
	ad2_score_rerank();
	
	drupal_set_message('הנתונים נשמרו.');
}

function theme_ad2_bids_bids_form( &$vars ){
	
	$form = $vars['form'];
	$ad = $form['#ad2'];
	
	//tr( $ad->bids);
	
	$html = "";
	$html .= drupal_render($form['master-ad-bid']);
	$html .= drupal_render($form['master-ad-auto']);
	$html .= '<br>';
	
	$rows = array();
	foreach ($ad->bids as $r  ) {
		$tid = $r->tid;
		$row = array(
			drupal_render($form['bids'][$tid]['term']) . drupal_render($form['bids'][$tid]['id']) . $tid,
			drupal_render($form['bids'][$tid]['status']),
			drupal_render($form['bids'][$tid]['auto']) , 
			drupal_render($form['bids'][$tid]['bid']) ,
			drupal_render($form['bids'][$tid]['bid_manual']) ,
		);
		
		if( user_access('edit any advertisement') ){
			$row[] = $r->rank;
			$row[] = $r->score;
			$row[] = '&nbsp;&nbsp;&nbsp;' . $r->bid;
			//$row[] = $tid;
			$row[] = $r->bid_manual ? $r->bid_manual : '';
			
		}
		
		$rows[] = $row;
	}
	
	$headers =  array( 
		t('Category - Term') , 	t('Show Ad') , 	t('Auto Bid') , 
		t('Bid') , 	t('Change Bid'),
	);
	
	if( user_access('edit any advertisement') ){
		$headers[] = t('Rank');
		$headers[] = t('Score');
		$headers[] = t('Actual Bid');
		$headers[] = t('Bid Manual');
	}
	$html .= theme('table' , array('rows' => $rows , 'header' => $headers) );
	$html .= drupal_render($form['submit']);
	$html .= drupal_render_children( $form );
	
	return $html;
	
}

