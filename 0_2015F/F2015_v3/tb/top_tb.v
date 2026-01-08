`timescale 1ns/1ns

module top_tb();

reg clk,rst_n;
reg sig_A_in,sig_B_in;
reg en;
wire TX_Pin_Out;


 top top(
			clk,     //50M
			rst_n,
			sig_A_in,//输入A路信号，正弦波/方波 进行测频和占空比
			sig_B_in,//输入B信号，与A同频率的信号，用来测时间间隔
			TX_Pin_Out,
			RX_Pin_In,
			en,
			led			
			
);
initial begin 
				clk=0;
				sig_A_in=0;
				sig_B_in= 0;
				rst_n=0;
				en=1;
			end
initial
		forever #10 clk=~clk;
		
		
initial 
		begin 
		#25;
		forever #20 sig_A_in=~sig_A_in;
		end
		
initial begin 
		#58 ;
		forever #20 sig_B_in = ~sig_B_in;
		end
		
initial begin 
		#13;
		rst_n=1;
		en=1;
		#173;
		en=0;
		#803;
		en=1;
		end

		
endmodule 
