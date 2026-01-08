module  top(
			clk,     //50M
			rst_n,
			sig_A_in,//输入A路信号，正弦波/方波 进行测频和占空比
			sig_B_in,//输入B信号，与A同频率的信号，用来测时间间隔
			TX_Pin_Out,
			en,   //单片机下降沿触发的使能信号
			led			
			
);
  
input   clk;
input   rst_n;
//调理信号输入
input 	sig_A_in;  
input		sig_B_in;  


input      	en;
output wire TX_Pin_Out;
output wire [1:0]  led;



wire start  ; 
  
//**************************************
//例化时钟，2个200M时钟信号，clk_b相移90度
wire clk_a,clk_b;

pll_200	pll_200_inst (
	.inclk0 ( clk ),
	.c0 ( clk_a ),
	.c1 ( clk_b )
	);

//**************************************
//例化测频模块
  wire [31:0]  sig_A_Cnt;
  wire [31:0]  fbase_Cnt;
  wire [31:0]  duty_Cnt;
  wire [31:0]  delay_cnt;
  
  wire [7:0]   stage ;
  wire 		   sent_en;
  
fre_cnt fre_cnt (
		.sig_A_in(sig_A_in),
		.fbase(clk_a),// 基准信号 200M
		.rst_n(rst_n),
		.start(start),
		.done(sent_en),
		.busy(),
		.sig_A_Cnt(sig_A_Cnt),
		.fbase_Cnt(fbase_Cnt),
		.duty_Cnt(duty_Cnt) //占空比计数

   );     
	
	
 time_measure time_measure(
						.clk(clk_a),		//时钟计数信号   200m
						.clk_dly90(clk_b),  // 相位滞后90度信号，
						.sig_a(sig_A_in),   
						.sig_b(sig_B_in),
						.rst_n(rst_n),
						.start(start),   //开始信号，保持一个时钟的高电平信号
						.delay_cnt(delay_cnt),//输出的粗测时间
						.stage(stage),   //输出两个状态
						.done(),    // 完成信号
						.busy()    //忙信号
		
						);
	
//***************************************
//例化数据发送模块
datasent datasent(
			.CLK_r(clk),
			.Rst_n(rst_n),
			.EN(sent_en),
			.done(),
			.fxCnt(sig_A_Cnt),
			.fbaseCnt(fbase_Cnt),
			.dutyCnt(duty_Cnt),
			.delay_cnt(delay_cnt),
			.stage(stage),
			.TX_Pin_Out(TX_Pin_Out)
			
);

//*************************************
		//检测开始信号下降沿
	wire en;	
	reg H2L_F3 ;
	reg H2L_F4 ;

	
	 always @ ( posedge clk or negedge rst_n )
	 if(!rst_n)begin 
					H2L_F3 <=0;
					H2L_F4 <=0;
					end
	  else  begin
			H2L_F3 <= en;
			H2L_F4 <= H2L_F3;
		end
	
	assign start = H2L_F4 & !H2L_F3;
	


			
assign  led[1]=~start;
assign  led[0]=~sent_en;

	endmodule
	