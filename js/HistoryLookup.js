function HistoryLookup( domainName ){
	private:
		var historyList = new ArrayList();
		var DB;
		var DB2;
	public:
		var mutex;
		var DomainName;
		//this.domainName = domainName;
		try{
			DB = window.openDatabase( "dependns", "1.1", "DepenDNS History Database",  1024*1024 );
			if ( DB == null )
				display("error!");

		}catch(ex){
			display('Could not create database: ' + ex.message);
		}
		
		//var temp_sql = //DB.SQLTransactionSync() ;	
		//display(temp_sql);
		//temp_sql.executeSql("SELECT id FROM domain_id WHERE domain_name='www.google.com';");

		this.getDomainID = getDomainID;
		function getDomainID(){
			//mutex = true;
			var sql = "SELECT id FROM domain_id WHERE domain_name='"+DomainName+"';";
			var result;
			if ( DB != null ){
			DB.readTransaction(function(tx){
				tx.executeSql(
					sql,
					[],
					function(tx,rs){
						if (rs.rows.length != 0){
							var Row = rs.rows.item(0);
							result = Row["id"];	
							//display("domain:"+DomainName+" id:"+result);
							getAnswerFromDB(result);
						} else {
							var insert_sql = "INSERT INTO domain_id (id,domain_name)VALUES(NULL,'"+DomainName+"');";
							//display("add name:"+DomainName);
							DB.transaction( function(TX){
								TX.executeSql(insert_sql,
									[],
									function(TX,rs1){
									},
									function(TX,err1){
									}
								);}
							);
						}
					},
					function(tx,err){
						display("Err in getDomainID:"+err);
						
						/*DB.readTransaction(function(TX){
								TX.executeSql(
										insert_sql,
										[],
										function(TX,RS){	// result
										},
										function(TX,RS){	// error
										}
									);
							});*/
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
						display("Err in getAnswerFromDB:"+err);
						var create_sql = "CREATE TABLE domain_"+id+" (`id` INTEGER PRIMARY KEY , `ip` TEXT );";
						DB.transaction( function(TX){
							TX.executeSql(create_sql,
								[],
								function(TX,rs1){
								},
								function(TX,err1){
								}
							);}
						);
						mutex = false;
					});

			});
			}
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

		//function CreateTable(){
		
		//}

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

		this.setDomainName = setDomainName;
		function setDomainName( domain ){
			DomainName = domain;
			//display(DomainName);
		}
		
		this.run = run;
		function run( input ){
			DomainName = input;
			//display("run DB~"+DomainName);
			mutex = false;
			getDomainID();
			//while ( mutex == true);
		}
}
