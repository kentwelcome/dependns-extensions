function IPChoice( ipListAll , region ) {
	private:
		var ipListCanRandom;
	public:
		this.ipListAll = ipListAll;
		this.region = region;
		ipListCanRandom = new ArrayList();

		this.countGrade = countGrade;
		function countGrade(){
			var G;
			// 計算每個IP的分數
			for( var i = 0 ; i < ipListAll.size() ; i++ )
			{
				var iplist = ipListAll.get(i);
				display( iplist.getIP() );
				var A = iplist.getA();
				var B = iplist.getB();
				var C = iplist.getC();
				display(A+"&nbsp;"+B+"&nbsp;"+C);
				if ( region < 7 )
				{
					G = A*(60-(region-1)*10) + 0.5*(B+C)*(40+(region-1)*10);
				}else{
					G = B*60.0 + C*40.0;
				}
				display("Grade:&nbsp;" + G );
				if(G >= 60.0){
					display("Can:&nbsp;"+iplist.getIP());
					ipListCanRandom.add(iplist);
				}
			}
		}
		
		this.getIPCanRandomList = getIPCanRandomList;
		function getIPCanRandomList(){
			var temp = "";
			for( var i = 0 ; i < ipListCanRandom.size() ; i++ ){
				var ipcanrand = ipListCanRandom.get(i);
				temp = temp + "IP " + (i+1) + ": " + ipcanrand.getIP();
			}
			return temp;
		}
}
