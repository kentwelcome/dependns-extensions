function Match( AnswerList , HistoryList , oneTimeCount ) {
	private:
		//var AnswerList= new ArrayList();
		//var HistoryList= new ArrayList();
		var IpListAll= new ArrayList();
		var IpHistoryList= new ArrayList();
		var region;
		//var onTimeCount;
	public: 
		this.AnswerList = AnswerList;
		this.HistoryList = HistoryList;
		this.oneTimeCount = oneTimeCount;
		
		//display("His size "+this.HistoryList.size());

		this.runMatchAlgorithm = runMatchAlgorithm;
		function runMatchAlgorithm( resolverCount ){
			arrangeAnswer( resolverCount );
			//printAnswer();

			ipDifference();	//alpha
			historyDifference(); //beta
			bClassDifference(); //gamma
			countRegion();
		}	

		this.countRegion = countRegion;
		function countRegion()
		{
			region = IpListAll.size() / oneTimeCount;
			//display("IPListAll.size=" + IpListAll.size() + "oneTimecount:" + oneTimeCount );
			//display("Region:"+region);
		}

		this.ipDifference = ipDifference;
		function ipDifference(){
			//將answer list全整理到單一array list中
			//IpListAll = new ArrayList();
			for ( var i = 0 ; i < AnswerList.size() ; i++ )
			{
				var Ans = AnswerList.get(i)
				var ipList = Ans.getIPList();
				IpListAll.addAll(ipList);
			}

			//找出n_max
			var n_max = 0;
			for ( var i = 0 ; i < IpListAll.size() ; i++ ){
				var AnsIP = IpListAll.get(i);
				if (AnsIP.getCount() > n_max){
					n_max = AnsIP.getCount();
				}
			}
			//容忍值
			confidence = 0.80;

			//Count alapha
			for( var i = 0 ; i < IpListAll.size() ; i++ ){
				var Ans = IpListAll.get(i);
				if ( Ans.getCount() >= n_max*confidence ){
					Ans.setA(1);
				}

			}
		}

		this.historyDifference = historyDifference;
		function historyDifference() {
			//將history list全整理到單一array list中
			//IpHistoryList = new ArrayList();
			for ( var i = 0 ; i < HistoryList.size() ; i++ )
			{
				var His = HistoryList.get(i);
				var ipList = His.getIPList();
				IpHistoryList.addAll(ipList);
			}
			//比對歷史資料
			for ( var i = 0 ; i < IpListAll.size() ; i++ )
			{
				var iplist = IpListAll.get(i);
				var ip1 = iplist.getIP();
				for ( var j = 0 ; j < IpHistoryList.size() ; j++ )
				{
					var HisIP = IpHistoryList.get(j);
					var ip2 = HisIP.getIP();
					if( ip1 == ip2 )
					{
						iplist.setB(1);
						break;
					}
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
					if( bClass == His.getBClass() )
					{
						tempIP = His.getIPList();
						break;
					}
				}

				for( var i = 0 ; i < IpListAll.size() ; i++ )
				{
					var AnsIP = IpListAll.get(i)
					var ip = AnsIP.getIP();
					for( var j = 0 ; j < tempIP.size() ; j++ )
					{
						var temp = tempIP.get(j);
						if( ip == temp.getIP() )
						{
							difference = AnsIP.getCountPercent() - temp.getCountPercent();
							if ( Math.abs(difference) < 0.10 )
							{
								AnsIP.setC(1);
							}
						}	
					}	
				}
			} else 	{ 

				for(var i=0 ; i<AnswerList.size() ; i++)
				{
					var Ans = AnswerList.get(i);
					var bClass = Ans.getBClass();
					for(var j=0 ; j<HistoryList.size() ; j++)
					{
						var His = HistoryList.get(j);
						if( bClass == His.getBClass() )
						{
							difference = Ans.getClassCountPercent() - His.getClassCountPercent();
							//display("DIFF"+difference);
							if(Math.abs(difference)<0.10)
							{
								var temp = new ArrayList();
								var temp = Ans.getIPList();
								for ( var x=0 ; x < temp.size() ; x++ )
								{
									var TempIP = temp.get(x);
									var tempIP = TempIP.getIP();
									for( var y=0 ; y < IpListAll.size() ; y++ )
									{
										var AnsIP = IpListAll.get(y);
										if( TempIP == AnsIP.getIP() )
										{
											AnsIP.setC(1);
										}
									}
								}
							}
						}
					}
				}

			}
		}

		this.arrangeAnswer = arrangeAnswer;
		function arrangeAnswer( resolverCount ){
			for( var i = 0 ; i < AnswerList.size() ; i++ )
			{
				var Ans = AnswerList.get(i);
				//display(Ans);
				Ans.setClassCountPercent(resolverCount);
			}
			for( var i = 0 ; i < HistoryList.size() ; i++ )
			{
				var His = HistoryList.get(i);
				His.setClassCountPercent(resolverCount*336); // 336 ?
			}

		}

		this.printAnswer = printAnswer;
		function printAnswer(){
			//display("歷史資料");
			for( var i = 0 ; i < HistoryList.size() ; i++ ){
				//var Ans = AnswerList.get(i);
				var His = HistoryList.get(i);
				var temp = His.getClassCount()/48.0;
				temp = Math.floor(temp*1000) / 1000.0;
				display("子網路:&nbsp;" + His.getBClass() + "&nbsp;" + temp + "&nbsp;" + His.getClassCountPercent() );
				His.printAnswer();
			}
			display("這次資料");
			for( var i = 0 ; i < AnswerList.size() ; i++ ){
				var Ans = AnswerList.get(i);
				display("子網路:&nbsp;" + Ans.getBClass()+"&nbsp;"+ Ans.getClassCount() + "&nbsp;" + Ans.getClassCountPercent() );
			}
		}

		this.getIPListAll = getIPListAll;
		function getIPListAll(){
			return IpListAll;
		}

		this.getRegion = getRegion;
		function getRegion(){
			return region;
		}
};
