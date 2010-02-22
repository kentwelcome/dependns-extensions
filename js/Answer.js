function Answer( bClass , ip ) {
	private:
		//var bClass;
		var classCount = 0;
		var classCountPercent = 0;
		var ipList = new ArrayList();

	public:
		if( !bClass && !ip ){
			this.bClass = "";
			this.classCount = 0;
			this.classCountPercent = 0.0;
			this.ipList = ipList;
		} else {
			this.bClass = bClass;
			this.classCount = 0;
			this.classCountPercent = 0;
			this.ipList = ipList;
			addIP(ip);
		}
	

		this.addIP = addIP;
		function addIP( ip ){
			for ( var i = 0 ; i < ipList.size() ; i++ )
			{
				var AnsIP = ipList.get(i);
				if( ip == AnsIP.getIP() )
				{
					AnsIP.addCount();
					classCount++;
					return 0;
				}
			}
			ipList.add( new AnswerIP(ip) );
			classCount++;
			return -1;
		}

		this.setClassCountPercent = setClassCountPercent;
		function setClassCountPercent( resolverCount ){
			classCountPercent = classCount / resolverCount;
			// what's this mean? 
			classCountPercent = ( Math.floor(classCountPercent*1000) )/1000.0;  
			for( var i = 0 ; i < ipList.size() ; i++ )
			{
				var AnsIP = ipList.get(i);
				AnsIP.setCountPercent(resolverCount);
			}
		      	
		}
		
		this.getBClass = getBClass;
		function getBClass(){
			return bClass;
		}

		this.getClassCount = getClassCount;
		function getClassCount(){
			return classCount;
		}

		this.getClassCountPercent = function(){
			return classCountPercent;
		}
		
		this.getIPList = getIPList;
		function getIPList(){
			return ipList;
		}
		
		this.printAnswer = printAnswer;
		function printAnswer(){
			for( var i = 0 ; i < ipList.size() ; i++ ){
				var IP = ipList.get(i);
				display("IP " + (i+1) + ": " + IP.getIP() + "&nbsp;" + IP.getCount() + "&nbsp;" + IP.getCountPercent());
			}
			// todo fix with display in HTML 
		}
}
