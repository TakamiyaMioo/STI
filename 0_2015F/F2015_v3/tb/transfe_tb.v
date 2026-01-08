`timescale 1ns/1ns
module transfe_tb ;
reg clk_tb;
reg rst_n;
reg sent_en;

wire  TX_Pin_Out;


datasent datasent(
	.CLK_r(clk_tb),
	.Rst_n(rst_n),
	.EN(sent_en),
	
	.fxCnt(50000000),
	.fbaseCnt(4856912),
	.dutyCnt(156468),
	.upTime(415616),
	.TX_Pin_Out(TX_Pin_Out),
	.led()

);


initial begin 
		clk_tb=0;
		rst_n=0;
		sent_en=0;
		end

always 
	#10 clk_tb=~clk_tb;
initial 
	#27 rst_n=1;

initial begin 
		#135 sent_en=1;
		#179 sent_en=0;
		end
		
 
	 
			
endmodule 
