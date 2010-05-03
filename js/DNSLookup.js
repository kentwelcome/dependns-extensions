function DNSLookup(){
	private:
		var domainName;
		var resultIngo;
		var type;
		var resolverCount;
		//var sr;
		var resolverList = new ArrayList();
		var lookupThread;
		var historyThread;
		var answerList = new ArrayList();
		var response = new Response();
		//var OneTimeCount = new Array();
	public:
		domainName = "www.google.com.tw";
		//type = Type.A;
		resloverCount = 0;

		this.setQuery = setQuery;
		function setQuery( domainName ){
			this.domainName = domainName;
			//this.type = type;
			resultInfo = "";
			setResultInfo("Lookup for "+domainName);
		}

		this.setResolver = setResolver;
		function setResolver( resolverList ){
			//this.sr = sr;
			//this.resolverList = resolverList;
			resolverCount = resolverList.size();
			setResultInfo("The Number of Resolvers:"+resolverCount);
		}

		this.doLookup = doLookup;
		function doLookup(ResolveAns,oneTimeCount){

			// Lookup by resolver
			/*for( var i = 0 ; i < resolverCount ; i++ )
			{
				var Res = resolverList.get(i);
				//if( Res.getAvailable() ){
					// start Lookup URL
				//}
				//LoockUp.SendQuery(this.domain,Res);
				
			}*/
			//display(ResolveAns);
			//display(ResolveAns.length);
			OneTimeCount = oneTimeCount;
			for ( var i = 0 ; i < ResolveAns.length ; i++ ){
				var ResTmp = ResolveAns[i];
				for ( var j = 0 ; j < ResTmp.length ; j++ ){
					response.addToAnswerList( ResTmp[j] , 32 );
					display("DNS" + i +":"+ ResTmp[j]);
				}
			}


			// run History thread
			var historyList = new ArrayList();
			//¾ã²zResponse
			orderResponse();

			var mode_OneTimeCount = checkOneTimeCount();
			//display(mode_OneTimeCount);
			var match = new Match( answerList , historyList , mode_OneTimeCount );
			match.runMatchAlgorithm(resolverCount);
			var ipChoice = new IPChoice( match.getIPListAll() , match.getRegion() );
			ipChoice.countGrade();
			display("Can Use IP List:"+ipChoice.getIPCanRandomList());
			var grade = ipChoice.Grade;
			delete historyList;
			delete ipChoice;
			delete match;
			return Math.round(grade);
		}

		this.checkOneTimeCount = checkOneTimeCount;
		function checkOneTimeCount(){
			var countList = new Array(10);
			for ( var i = 0 ; i < 10 ; i++ ){
				ConTmp = new Array(2);
				ConTmp[0] = 0;
				ConTmp[1] = 0;
				countList[i] = ConTmp;
			}
			var mode = 0;
			var modeIndex = 0;
			var arraySize = 0;
			
			for ( var i = 0 ; i < resolverCount ; i++ ){
				var exist = false;
				for ( var j = 0 ; j < arraySize ; j++ ){
					var ConTmp = countList[j];
					if (ConTmp[0] == OneTimeCount[i]){
						ConTmp[1]++;
						exist = true;
					}
				}
				if (!exist){
					var ConTmp = countList[arraySize];
					ConTmp[0] = OneTimeCount[i];
					ConTmp[1] = 1;
					//display(countList[arraySize]);
					arraySize++;
				}
			}
			var ConTmp = countList[modeIndex];
			return ConTmp[0];
		}

		this.orderResponse = orderResponse;
		function orderResponse(){
			var answerIPList = response.getAnswerIPList();
			for( var i = 0 ; i < response.getCount() ; i++ ){
				var bClass = getBClass(answerIPList[i]);
				var ok = false;
				for( var j = 0 ; j < answerList.size() ; j++ ){
					var Ans = answerList.get(j);
					if(bClass == Ans.getBClass() )
					{
						Ans.addIP(answerIPList[i]);
						ok = true;
						break;
					}
				}
				if( !ok ){
				answerList.add(new Answer(bClass , answerIPList[i] ));
				}
			}
			
		}

		this.getBClass = getBClass;
		function getBClass( ip ){
			var dotCount = 0;
			for ( var i = 0 ; i < ip.length ; i++ ){
				if( ip.substring(i , i+1) == "." )
					dotCount++;
				if( dotCount == 2 )
					return ip.substring(0,i);
			}
			return "";
		}

		this.setResultInfo = setResultInfo;
		function setResultInfo( temp ){
			resultInfo = resultInfo+temp;
		}

		this.getResultInfo = getResultInfo;
		function getResultInfo(){
			return resultInfo;
		}
		
		this.dodebug = function(){
			domainName = "www.nthu.edu.tw";
			var Ans = new ArrayList();
			var His = new ArrayList();
			for( var i = 0 ; i < 17 ; i++ ){
				//if(i%2 == 0)
					response.addToAnswerList("140.114.63.10" , 32 );
				//else
				//	response.addToAnswerList("140.112.64.1" , 32 );
			}
			
			var HisDB = new HistoryLookup("www.nthu.edu.tw");
			HisDB.run();

			var HisList = new Answer( "140.114" , "140.114.63.1");
			for( var i = 0 ; i < 5667 ; i++ )
				HisList.addIP("140.114.63.1");

			His.add(HisList);
			orderResponse();
			var match = new Match( answerList , His , 1 );
			match.runMatchAlgorithm(17);
			var iplist = match.getIPListAll();
			var region = match.getRegion();
			var ipChoice = new IPChoice( iplist , region );
			ipChoice.countGrade();
			return ipChoice.Grade;

		}
}
