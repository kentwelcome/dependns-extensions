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
	
	this.countRegion = countRegion;
	function countRegion()
	{
		region = ipListAll.size() / ineTimeCount;
		alert("Region:"+region);
	}

	this.ipDifference = ipDifference;
	function ipDifference(){
		//將answer list全整理到單一array list中
		IpListAll = new ArrayList();
		for ( var i = 0 ; i < AnswerList.size() ; i++ )
		{
			var ipList = new ArrayList();
			var Ans = AnswerList.get(i)
			ipList = Ans.getIPList();
		}

		//找出n_max
		var n_max = 0;
		for ( var i = 0 ; i < IPListAll.size() ; i++ ){
			var Ans = IpListAll.get(i);
			if (Ans.getCount() > n_max){
				n_max = Ans.getCount();
			}
		}
		//容忍值
		confidence = 0.80;

		//Count alapha
		for( var i = 0 ; i < IpListAll.size() ; i++ ){
			var Ans = IpListAll.get(i);
			if ( Ans.getCount() >= N_max*confidence ){
				Ans.setA(1);
			}
		
		}
	}

	this.historyDifference = historyDifference;
	function historyDifference() {
	//將history list全整理到單一array list中
		IpHistoryList = new ArratList();
		for ( var i = 0 ; i < HistoryList.size() ; i++ )
		{
			var ipList = new ArrayList();
			var Ans = HistoryList.get(i);
			ipList = Ans.getIPList();
			IpHistoryList.addAll(ipList);
		}
		//比對歷史資料
		for ( var i = 0 ; i < IpListAll.size() ; i++ )
		{
			var AnsIP1 = IpListAll.get(i);
			var ip1 = AnsIP.getIP();
			for ( var j = 0 ; j < IpHistoryList().size() ; j++ )
			{
				var AnsIP2 = IpHistoryList().get(i);
				var ip2 = AnsIP.getIP();
				if( ip1.equals(ip2) )
				{
					AnsIP1.setB(1);
					break;
				}
			}
		}

		this.bClassDifference = bClassDifference;
		function bClassDifference(){
			//如果只有一個class B, 則僅僅比對該class中的IP
			//否則比對class B百分比是否跟歷史資料相近
			if ( AnswerList.size() == 1)
			{
				var Ans = AnswerList.get(0);
				var bClass = Ans.getBClass();
				var tempIP = new ArrayList();

				for ( var i = 0 ; i < HistoryList.size() ; i++ )
				{
					var His = HistoryList.get(i);
					if( bClass.equals(His.getBClass()) )
					{
						tempIP = His.getIPlist();
						break;
					}
				}

				for( var i = 0 ; i < IpListALL.size() ; i++ )
				{
					var AnsIP = IpListAll.get(i)
					var ip = AnsIP.getIP();
					for( var j = 0 ; j < tempIP.size() ; j++ )
					{
						
					}	
				}
			}
		}
	}
};
