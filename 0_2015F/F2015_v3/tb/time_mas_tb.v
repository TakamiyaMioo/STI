`timescale 1ns/1ns
module time_mas_tb();

reg clk_a,clk_b,sig_A_in,sig_B_in,rst_n,start;
wire [31:0] delay_cnt;
wire [7:0] stage ;
wire done ,busy;
 

 time_measure time_measure(
						.clk(clk_a),		//时钟计数信号   100m
						.clk_dly90(clk_b),  // 相位滞后90度信号，
						.sig_a(sig_A_in),   
						.sig_b(sig_B_in),
						.rst_n(rst_n),
						.start(start),   //开始信号，保持一个时钟的高电平信号
						.delay_cnt(delay_cnt),//输出的粗测时间
						.stage(stage),   //输出两个状态
						.done(done),    // 完成信号
						.busy(busy)    //忙信号
		
						);
						
						
initial begin 
				clk_a=0;
				clk_b=0;
				sig_A_in=0;
				sig_B_in= 0;
				rst_n=0;
				start=0;
			end
initial
		forever #5 clk_a=~clk_a;

		
initial begin 
		#4 ;
		forever #5 clk_b = ~clk_b;
		end

initial
		forever #20 sig_A_in=~sig_A_in;
		
initial begin 
		#93 ;
		forever #20 sig_B_in = ~sig_B_in;
		end
		
initial begin 
		#33;
		rst_n=1;
		#73;
		start=1;
		#13;
		start=0;
		end
		
endmodule
