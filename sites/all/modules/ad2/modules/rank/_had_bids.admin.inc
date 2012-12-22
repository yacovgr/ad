<?php
// 
// function had_bids_report_auto(){
// 	
	// return 'דף זה יוסר בקרוב';
	// $build = array();
// 	
// 	
	// $hours = _had_auto_clicks_by_hour_info();
	// $h = date('G');
// 	
	// $build['prec'] = array(
		// '#markup' => '<div>' . 'אחוז מהיום כרגע: ' . $hours[$h] . '</div>',
	// );
// 		
	// $roofs = db_query('select nid, roof from ad_roof')->fetchAllKeyed();
// 	
// 	
	// $timestamp = mktime(0,0,0,date('m') , date('d') , date('Y'));
	// $q = db_select('account' , 'a');
	// $q->addExpression('sum(ammount)', 'sum');
	// $q->fields('a' , array('nid'));
	// //$q->condition('a.nid' , $account_id );
	// $q->condition('a.ammount' , 0 , '<' );
	// $q->condition('time' , $timestamp , '>');
	// $q->groupBy('a.nid');
	// $spending = $q->execute()->fetchAllKeyed();
// 		
	// //tr( $spending );
	// //return $spending;
// 	
// 	
	// //tr( $roofs );
// 	
	// $nid = 6;
// 	
	// $q = db_select('ad_bids' , 'ab')->extend('PagerDefault');
	// $q->fields('ab' , array('aid' , 'bid' , 'tid' ));
	// $q->condition('ab.auto' , 1);
	// $q->condition('ab.status' , 1);
	// $q->limit(100);
// 	
	// $q->join('ads' , 'a' , 'ab.aid = a.aid');
	// $q->fields('a' , array('adtype' , 'adstatus' , 'over'));
// 	
	// $q->join('node' , 'n' , 'n.nid = a.nid');
	// $q->fields('n' , array('title' , 'nid'));
// 	
	// //$q->condition('n.nid' , $nid );
// 	
	// //$q->join('node' , 'n' , 'n.nid = a.nid');
	// //$q->fields('n' , array('title'));
// 	
	// $results = $q->execute()->fetchAll();
// 	
	// $rows = array();
	// foreach ($results as $result ) {
// 		
		// $base = 1.2;
		// $max = 10;
// 		
		// $roof = $roofs[$result->nid];
// 		
		// foreach (_had_auto_default_values_info() as $row ) {
			// if( $roof >= $row['min'] && $roof < $row['max'] ){
				// $base = $row['bid'];
				// $max = $row['bid_max'];
			// }
		// }
// 		
		// $hours = _had_auto_clicks_by_hour_info();
		// $h = date('G');
		// $a = $hours[$h] /100;
// 		
		// $done = -1 *  $spending[$result->nid];
		// if( $done == 0 ) $done = 0.01;
		// //tr( "done = " . $done );
// 		
		// $p =  $done / $roof;
// 		
		// $q = $a / $p;
// 		
		// $bid2 = $q * $base;
// 		
		// if( $bid2 > $max){
			// $bid2 = $max;
		// }
// 		
		// if( $bid2 < 0.9 ){
			// $bid2 = 0.9;
		// }
// 		
// 		
		// //tr( $base );
		// //tr( $max );
// 		
		// $row = array(
			// $result->title,
			// isset( $roofs[$result->nid]) ? $roofs[$result->nid] : '-',
			// isset( $spending[$result->nid]) ? -1 * $spending[$result->nid] : '-',
			// round(100*$p,2),
			// $base,
			// $max,
// 			
			// round($q,2),
			// round($bid2,2),
			// //$result->status,
			// round($result->bid,2),
			// $result->aid,
// 			
			// $result->adtype,
			// $result->adstatus,
			// $result->over,
			// $result->tid,
		// );
		// $rows[] = $row;
// 		
	// }
// 	
	// $header = array(
		// 'צימר',
		// 'תקרה יומית',
		// 'מימוש היום',
		// '% מימוש',
		// 'מחיר בסיס',
		// 'מחיר מקסימום',
		// 'מכפיל',
		// 'ביד מחושב כרגע',
		// 'ביד שמור בטבלה',
		// 'ad id',
// 		
		// 'ad type',
		// 'ad status',
		// 'over ?',
		// 'tid',
	// );
	// $build['table'] = array(
		// '#theme' =>"table",
		// '#rows' => $rows,
		// '#header' => $header,
	// );
// 
// 	
	// $build['pager'] = array(
		// '#theme' =>'pager',
	// );
// 
// 
	// return $build;
// 	
// }
