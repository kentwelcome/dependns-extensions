function Answer( bClass , ip ) {
	private:
		var bClass;
		var classCount;
		var classCountPercent;
		var ipList;

	public:
		this = function (){
			this.bClass = "";
			this.classCount = 0;
			this.classCountPercent = 0.0;
			this.ipList = new ArrayList();
		}
		
		this = function( bClass , ip ){
			this.bClass = bClass;
			this.classCount = 0;
			this.classCountPercent = 0;
			this.ipList = new ArrayList();
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
			classCountPercent = (classCountPercent*1000)/1000.0;  
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

		this.getClassCountPercent(){
			return classCountPercent;
		}
		
		this.getIPList = getIPList;
		function getIPList(){
			return ipList;
		}
		
		this.printAnswer = printAnswer;
		function printAnswer(){
			alert("print Answer");
			// todo fix with display in HTML 
		}
}
