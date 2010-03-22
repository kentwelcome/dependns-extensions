function HistoryLookup( domainName ){
	private:
		var historyList = new ArrayList();
		var DB;
	public:
		this.domainName = domainName;
		try{
			DB = window.openDatabase("dependns", "1.1", "DepenDNS History Database",  1024 * 1024);

		}catch(ex){
			display('Could not create database: ' + ex.message);
		}

		this.getDomainID = getDomainID;
		function getDomainID(){
			var sql = "SELECT id FROM domain_id WHERE domain_name='"+domainName+"';";
			var result;
			DB.transaction(function(tx){
				tx.executeSql(
					sql,
					[],
					function(tx,rs){
						//var i = rs.length;
						//display(i);
						var Row = rs.rows.item(0);
						result = Row["id"];	
						//display(result);
					},
					function(tx,err){
						display("Err:"+err);
					}	
				);		
			});

			return result;
		}

		this.getAnswerFromDB = getAnswerFromDB;
		function getAnswerFromDB(){
			var id = getDomainID();
			//display(id);
			var sql = "SELECT * FROM domain_'"+id+"';";
			DB.transaction(function(tx){
				tx.executeSql(
					sql,
					[],
					function(tx,rs){
						doQuery(tx,rs);
					},
					function(tx,err){
						display("Err:"+err);
					});

			});
		}

		this.doQuery = doQuery;
		function doQuery( tx , rs ){
			for( var i = 0 ; i < rs.length ; i++ ){
				var Row = rs.rows.item(i);
				var ip = Row["ip"];
				var bClass = getBClass(ip);
				var ok = false;
				for(var j = 0 ; j < historyList.size() ; j++){
					var His = historyList.get(j);
					His.addIP(ip);
					ok = true;
					break;
				}
				if(!ok){
					historyList.add(new Answer(bClass,ip) );
				}
			}
		}

		this.getBClass = getBClass;
		function getBClass( ip ){
			var dotCount = 0;
			for ( var i = 0 ; i < ip.length ; i++ ){
				if( ip.substring(i,i+1) == "." )
					dotCount++;
				if( dotCount == 2 )
					return ip.substring(0,i);
			}	
			return "";
		}

		this.getHistoryList = getHistoryList;
		function getHistoryList(){
			return historyList;
		}
		
		this.run = run;
		function run(){
			getAnswerFromDB();
		}
}
