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

		this.getDomainID = function (){
			var sql = "SELECT id FROM domain_id WHERE domain_name=?;";
			var result;
			DB.transaction(function(tx){
				tx.executeSql(
					sql,
					[domain_name],
					function(tx,rs){
						var Row = rs.rows.item(0);
						result = Row["id"];	
					},
					function(tx,err){
						display("Err:"+err);
					}	
				);		
			});

			return result;
		}

		this.getAnswerFromDB = function(){
			var sql = "SELECT * FROM domain_?;";
			var id = getDomainID();
			DB.transaction(function(tx){
				tx.executeSql(
					sql,
					[id],
					doQuery(tx,rs),
					function(tx,rs){
						display("Err:"+err);
					});

			});
		}

		this.doQuery = function( tx , rs ){
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

		this.getBClass = function( ip ){
			var dotCount = 0;
			for ( var i = 0 ; i < ip.length ; i++ ){
				if( ip.substring(i,i+1) == "." )
					dotCount++;
				if( dotCount == 2 )
					return ip.substring(0,i);
			}	
			return "";
		}

		this.getHistoryList = function(){
			return historyList;
		}
		
		this.run = function(){
			getAnswerFromDB();
		}
}
