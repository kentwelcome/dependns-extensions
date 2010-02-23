function DNSLookup(){
	private:
		var domainName;
		var resultIngo;
		var type;
		var resolverCount;
		var sr;
		var resolverList = new ArrayList();
		var lookupThread;
		var historyThread;
		var answerList = new ArrayList();
		var response = new Response();
	public:
		domainName = "www.google.com.tw";
		//type = Type.A;
		resloverCount = 0;

		this.setQuery = setQuery;
		function setQuery(domainName , type ){
			this.domainName = domainName;
			this.type = type;
			resultInfo = "";
			setResultInfo("Lookup for "+domainName);
		}

		this.setResolver = setResolver;
		function setResolver( sr , resolverList ){
			this.sr = sr;
			this.resolverList = resolverList;
			resolverCount = resolverList.size();
			setResultUIfo("The Number of Resolvers:"+resolverCount);
		}

		this.doLookup = doLookup;
		function doLookup(){

			// Lookup by resolver
			for( var i = 0 ; i < resolverCount ; i++ )
			{
				var Res = resolverList.get(i);
				if( Res.getAvailable() ){
					// start Lookup URL
				}
			}

			// run History thread
			
			//¾ã²zResponse
			orderResponse();

			var mode_OneTimeCount = checkOneTimeCount();
			var match = new Match( answerList , historyList , mode_OneTimeCount );
			match.renMatchAlgorithm(resolverCount);
			var ipChoice = new IPChoice( match.getIPListAll() , match.getRegion() );
			ipChoice.countGrade();
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
				if(i%2 == 0)
					response.addToAnswerList("140.114.63.10" , 32 );
				else
					response.addToAnswerList("140.112.64.1" , 32 );
			}
			
			var HisDB = new HistoryLookup();

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

		}
}
