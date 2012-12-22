<?php

function had_bids_bids_form( $form , &$form_state , $node ){

	drupal_add_css(drupal_get_path('module',  'had_bids') . "/had_bids.css" );
	
	$site_nid 	= $node->field_site['und'][0]['nid'];
	$site 		= node_load( $site_nid );
		
	$form['#node'] = $node;
	
	$form['master-ad-bid'] = array(
		'#type' => 'markup',
		'#prefix' => '<div>',
		'#suffix' => '</div>',
		'#markup' => "<b>" . t('Ad Master Bid') . ": </b>" .  $node->ad->price,
	);
	
	$form['master-ad-auto'] = array(
		'#type' => 'markup',
		'#prefix' => '<div>',
		'#suffix' => '</div>',
		'#markup' => "<b>" . t('Ad Master Auto') . ": </b>" .  $node->ad->auto,
	);
	
	$form['bids'] = array(
		'#type' => 'container',
		'#tree' => true,
		'#weight' => 10,
	);
  
	$master = $node->ad->price; // $node->field_click_price['und'][0]['value'];
	
	foreach ($node->bids as $row  ) {

		$tid = $row['tid'];
		$term = taxonomy_term_load( $tid );
		
		$form['bids'][$tid] = array(
			'#type' => 'container',
			'#tree' => true,
		);
		$form['bids'][$tid]['id'] = array(
			'#type' => 'hidden',
			'#value' => $row['id'],
		);

		$form['bids'][$tid]['term'] = array(
			'#type' => 'markup',
			'#markup' => $term->name,
		);


		if( $node->ad->master ){
			$form['bids'][$tid]['status'] = array(
				'#type' => 'markup',
				'#markup' => 'פעיל',
			);
			
		} else {
			$form['bids'][$tid]['status'] = array(
				'#type' => 'checkbox',
				'#default_value' => $row['status'],
			);
		}
		
		$value = $row['bid_manual'] != 0 ? $row['bid_manual'] : $row['bid'];
		if( $row['auto'] ){
			$value = $row['bid_manual'] != 0 ? $row['bid_manual'] : $master;
			$value = "<div class='crossed'>$value</div>";
		} else {
			
		}
		
		$form['bids'][$tid]['bid'] = array(
			'#type' => 'markup',
			'#markup' => $value,
			'#size' => 4,
		);
		
		
		if( $row['auto'] ){
			$form['bids'][$tid]['bid_manual'] = array(
				'#type' => 'markup',
				'#markup' => '',
			);
			
		} else {
			$form['bids'][$tid]['bid_manual'] = array(
				'#type' => 'textfield',
				'#default_value' => $row['bid_manual'] != 0 ? $row['bid_manual'] : '',
				'#size' => 4,
			);
		}

		
		$attributes = array();
		if( $row['auto_manual'] != -1 ){
			$attributes = array('class'=> array('auto-manual'));
		}
		$form['bids'][$tid]['auto'] = array(
			'#type' 			=> 'checkbox',
			'#default_value' 	=> $row['auto'],
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

function had_bids_bids_form_validate( $form , &$form_state ){
	foreach ($form_state['values']['bids'] as $tid => $data ) {
		if( isset($data['bid_manual']) ){
			if( $data['bid_manual'] && $data['bid_manual'] < had_get_default_click_price() ){
				form_error($form['bids'][$tid]['bid_manual'] , t('לא ניתן להזין ערך נמוך מ @min.' , array('@min' => had_get_default_click_price() )));
			}
		}
	}
}

function had_bids_bids_form_submit( $form , &$form_state ){

	$node = $form['#node'];
		
	foreach ($form_state['values']['bids'] as $tid => $data ) {
			
		if( isset($data['status']) ){
			$node->bids[$tid]['status'] = $data['status'];
		}
		
		$node->bids[$tid]['new_auto'] 	= $data['auto'];
		
		if( isset($data['bid_manual'] )){
			$node->bids[$tid]['new_bid_manual'] = $data['bid_manual'];
		}
	}
	
	node_save($node );
	
	had_score_rerank();
	
	drupal_set_message('הנתונים נשמרו.');
}

function theme_had_bids_bids_form( &$vars ){
	
	$form = $vars['form'];
	$node = $form['#node'];
	
	
	$html = "";
	$html .= drupal_render($form['master-ad-bid']);
	$html .= drupal_render($form['master-ad-auto']);
	$html .= '<br>';
	
	$rows = array();
	foreach ($node->bids as $tid => $dummy  ) {
		$row = array(
			drupal_render($form['bids'][$tid]['term']) . drupal_render($form['bids'][$tid]['id']),
			drupal_render($form['bids'][$tid]['status']),
			drupal_render($form['bids'][$tid]['auto']) , 
			drupal_render($form['bids'][$tid]['bid']) ,
			drupal_render($form['bids'][$tid]['bid_manual']) ,
		);
		
		if( user_access('edit any advertisement') ){
			$row[] = $node->bids[$tid]['rank'];
			$row[] = $node->bids[$tid]['score'];
			$row[] = '&nbsp;&nbsp;&nbsp;' . $node->bids[$tid]['bid'];
			$row[] = $node->bids[$tid]['bid_manual'] ? $node->bids[$tid]['bid_manual'] : '';
			
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

