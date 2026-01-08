
`timescale 1ns/1ns
module fre_cnt_tb();


reg sig_A_in;
reg clk_a;
reg en;
reg rst_n;

 wire 		  done;
 wire [31:0]  sig_A_Cnt;
 wire [31:0]  fbase_Cnt;
 
 wire [31:0]  duty_Cnt;



fre_cnt fre_cnt (
		.sig_A_in(sig_A_in),
		.fbase(clk_a),// 基准信号 100M
		.rst_n(rst_n),
		.start(en),
		.done(done),
		.busy(),
		.sig_A_Cnt(sig_A_Cnt),
		.fbase_Cnt(fbase_Cnt),
		.duty_Cnt(duty_Cnt) //占空比计数

   );     
	 
initial 
	begin 
	

	 clk_a=0;
	 en=0;
	 rst_n=0;
	 #37;
	end
		
		
always 
	   #5 clk_a=~clk_a;
	
initial 
		begin 
		sig_A_in=0;
		#70;
		forever #50 sig_A_in=~sig_A_in;
		end
		




initial
		begin 
		#47 rst_n=1;
		 en=0;
	#33 en=1;
	#33 en=0;
		end
			
	
endmodule
