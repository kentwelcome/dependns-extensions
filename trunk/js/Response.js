function Response(){
	private:
		var count = 0;
		var answerIPList = new Array();
		var answerTTLList = new Array();  
	public:
		this.addToAnswerList = function( ip , ttl ){
			answerIPList[count] = ip;
			answerTTLList[count] = ttl;
			count++;
		}

		this.getAnswerIPList = function(){
			return answerIPList;
		}

		this.getAnswerTTLList = function(){
			return answerTTLList;
		}

		this.getCount = function(){
			return count;
		}

		this.clear = function(){
			answerIPList = null;
			answerTTLList = null;
			count = 0;
		}

}
