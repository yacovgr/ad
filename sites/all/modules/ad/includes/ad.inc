<?php

abstract class AdBlock{
	
	public $display = 'raw'; // can be jquery, iframe
	public $quantity = 2; 
	public $view_mode = 'teaser'; 
	public $ad_type = 'default'; 
	public $type ; // this is to do with the field shown in the block and the selection method
	public $ad_size = '468 x 60'; // size of ads - the block can be of fixed width but long or fixed hight but wide
	public $width;
	public $height;
}

class AdBlockDefault extends AdBlock{
			
} 

class AdRequest{
	public $quantity = 1;
	public $types = array();
	public $page = 0;
	public $status;
	public $source = 'block'; // could be also page
}

class AdResponse{
	protected $ads = array();
	
	function getCount(){
		
	}
	
	
	
	function getAds(){
		
	}
	
	
	
	function reset(){
		
	}
	
	
	function setAds($ads){
		$this->ads = $ads;
		
		
	}
	
	
	protected $ads = array();
	public $types = array();
	public $page = 0;
	public $status;
	public $source = 'block';
}


class AdService{
	
	public function fetch(AdRequest $request){
		$responde = new AdResponse();
		
		return $responde;
	}
	
	
}

//require 'ad.controller.inc';

// pull ads from service 
// type(s), count, .....  ?


// $ads = new AdService();  -with a certian service 
// $ads->countViews($ad);
// $ads->countClicks($ad);

// $request - can be from block, page, web service
// $request->types quantity , page etc.

// $response->getAds();
// $response->setAds($ads );
// $response->count();
// $response->reset();

// $response = $ads->fetch( $request );
// $ads->getTypes();
// $ads->getSizes();
// $ads->getViewModes();


// $ads->click($ad);
// $ads->view($ads);