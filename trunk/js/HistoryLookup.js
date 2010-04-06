function HistoryLookup( domainName ){
	private:
		var historyList = new ArrayList();
		var DB;
		var DB2;
	public:
		var mutex;
		this.domainName = domainName;
		try{
			DB = window.openDatabase( "dependns", "1.1", "DepenDNS History Database",  1024*1024 );

		}catch(ex){
			display('Could not create database: ' + ex.message);
		}
		
		//var temp_sql = //DB.SQLTransactionSync() ;	
		//display(temp_sql);
		//temp_sql.executeSql("SELECT id FROM domain_id WHERE domain_name='www.google.com';");

		this.getDomainID = getDomainID;
		function getDomainID(){
			//mutex = true;
			var sql = "SELECT id FROM domain_id WHERE domain_name='"+domainName+"';";
			var result;
			DB.readTransaction(function(tx){
				tx.executeSql(
					sql,
					[],
					function(tx,rs){
						var Row = rs.rows.item(0);
						result = Row["id"];	
						//display(result);
						getAnswerFromDB(result);
					},
					function(tx,err){
						display("Err:"+err);
					}	
				);		
			});
			return result;
		}

		this.getAnswerFromDB = getAnswerFromDB;
		function getAnswerFromDB(id){
			//var id = getDomainID();
			//id = 3;
			var sql = "SELECT * FROM domain_"+id+";";
			DB.readTransaction(function(tx){
				tx.executeSql(
					sql,
					[],
					function(tx,rs){
						doQuery(tx,rs);
						//var tmp = rs.length ;
						//display( id + ":" + rs.rows.length );
						mutex = false;
					},
					function(tx,err){
						display("Err:"+err);
						mutex = false;
					});

			});
		}

		this.doQuery = doQuery;
		function doQuery( tx , rs ){
			for( var i = 0 ; i < rs.rows.length ; i++ ){
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
			mutex = false;
			getDomainID();
			//while ( mutex == true);
		}
}
