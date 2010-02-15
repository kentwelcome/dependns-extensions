function Match( AnswerList , HistoryList , oneTimeCount ) {
	private:
		var AnswerList=new ArrayList();
		var HistoryList=new ArratList();
		var IpListAll=new ArratList();
		var IpHistoryList=new ArratList();
		var region;
		var onTimeCount;
	public: 
		this.AnswerList = AnswerList;
		this.HistoryList = HistoryList;
		this.oneTimeCount = oneTimeCount

	this.runMatchAlgorithm = runMatchAlgorithm;
	function runMatchAlgorithm( resolverCount ){
		arrangeAnswer( resolverCount );
		printAnswer();
		
		ipDifference();	//alpha
		historyDifference(); //beta
		bClassDifference(); //gamma
		countRegion();
	}	
	function countRegion()
	{
		region = ipListAll.size() / ineTimeCount;
		alert("Region:"+region);
	};
};
