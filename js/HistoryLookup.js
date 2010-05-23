function HistoryLookup( domainName ){
	private:
		var historyList = new ArrayList();
		var DB;
		var DB2;
	public:
		var mutex;
		var DomainName;
		var resolveAnswer;
		var dolookup;
		var oneTimeCount;
		//this.domainName = domainName;
		try{
			DB = window.openDatabase( "dependns", "1.1", "DepenDNS History Database",  1024*1024 );
			//DB = window.openDatabaseSync( "dependns", "1.1", "DepenDNS History Database",  1024*1024 );
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
							getAnswerFromDB(result);
						} else {
							display("insert:"+DomainName);
							var insert_sql = "INSERT INTO domain_id (id,domain_name)VALUES(NULL,'"+DomainName+"');";
							DB.transaction( function(TX){
								TX.executeSql(insert_sql,[],
									function(TX,rs1){
										var new_id = rs1.insertId;
										create_table_callback( new_id );
									},
									function(TX,err1){
										display("no domain_id table");
									}
								);}
							);
							
						}
					},
					function(tx,err){
						display("Err in getDomainID:"+err);
						var create_sql = "CREATE TABLE domain_id (`id` INTEGER PRIMARY KEY , `domain_name` TEXT );"
						DB.transaction( function(tx){
								tx.executeSql(create_sql,
									[],
									function(tx,rs){
										display("create table domain_id");
									},
									function(tx,err){}
									);}
							      );
					}	
				);		
			});
			return result;
		}


		function create_table_callback( id ){
			var create_sql = "CREATE TABLE domain_"+id+" (`id` INTEGER PRIMARY KEY , `ip` TEXT );"
			DB.transaction( function(tx){
				tx.executeSql(create_sql,
				     [],
				     function(tx,rs){
				     	display("create table domain_"+id);
					insertResolver_callback(id);
				     },
				     function(tx,err){}
				);}
			);
		}

		function insertResolver_callback( id ){
			var sql;

			for ( var i = 0 ; i < resolveAnswer.length ; i++ ){
				var ResTmp = resolveAnswer[i];
				for ( var j = 0 ; j < ResTmp.length ; j++ ){
					sql = "INSERT INTO domain_"+id+" (id,ip)VALUES(NULL,'"+ResTmp[j]+"');";
					DB.transaction( function(tx){
							tx.executeSql(sql,
							[],
							function(tx,rs){
								//display("insert success!");
							},
							function(tx,err){
								//display("insert error!");
							}
							);}
					);
				}
			}
		
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
						insertResolver_callback(id);
						//var grade = dolookup(resolveAnswer,oneTimeCount,historyList),
						mutex = false;
					},
					function(tx,err){
						display("Err in getAnswerFromDB:"+err);
						create_table_callback(id);
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
		
		this.setResolveAnswer = setResolveAnswer;
		function setResolveAnswer( resArray ){
			resolveAnswer = resArray;
			//display("res:"+resolveAnswer.length);
		}

		this.setDnslookup = setDnslookup;
		function setDnslookup( lookup_function , onetimecount ){
			dolookup = lookup_function;
			oneTimeCount = onetimecount;
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
