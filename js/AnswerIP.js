function AnswerIP( ip ) {
	private:
		//var ip;
		var count;
		var countPercent;
		var a,b,c;
		var canRandom;

		if(!ip){
			ip = "";
			count = 0;
			canRandom = false;
			a = 0;
			b = 0;
			c = 0;
		}else{
			this.ip = ip;
			count = 1;
			canRandom = false;
			a = 0;
			b = 0;
			c = 0;
		}

		this.addCount = addCount;
		function addCount(){
			count++;
			if (count ==4 )
			{
				setCanRandom(true);
			}
		}

		this.setCountPercent = setCountPercent;
		function setCountPercent( bClassTotalCount ){
			countPercent = count / bClassTotalCount;
			countPercent = ( Math.floor(countPercent*1000) )/1000.0;
		}
		
		this.setCanRandom = setCanRandom;
		function setCanRandom( canRandom ){
			this.canRandom = canRandom;
		}

		this.setA = setA;
		function setA(A){
			a = A;
		}

		this.setB = setB;
		function setB(B){
			b = B;
		}

		this.setC = setC;
		function setC(C){
			c = C;
		}
		
		this.getIP = getIP;
		function getIP(){
			return ip;
		}

		this.getCount = function(){
			return count;
		}

		this.getCountPercent = function(){
			return countPercent;
		}

		this.getA = function (){
			return a;
		}

		this.getB = function(){
			return b;
		}

		this.getC = function()
		{
			return c;
		}

		this.getCanRandom = function(){
			return canRandom;
		}
}
