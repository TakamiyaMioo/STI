
module fre_cnt (
    sig_A_in,

    fbase,// 基准信号 200M
    rst_n,
	 start,
	 done,
	 busy,

    sig_A_Cnt,
    fbase_Cnt,
	 duty_Cnt,  //占空比计数

   );     
  
  input  wire      sig_A_in;     //待测信号A
  input  wire      fbase; 	  // 基准信号 200M
  
  input  wire		start; // 开始信号 
  input	wire 	 	rst_n;
  output wire     done ;// 一个脉冲的高电平
  output wire		busy;
  
  output reg [31:0]  sig_A_Cnt;
  output reg [31:0]  fbase_Cnt;
  output reg [31:0]  duty_Cnt;



//*****************************************************************************		
///***************1s闸门产生*******
parameter time_1s = (100_000_000*2-1);//50_000;  // fbase 100M时//(100_000_000-1)
reg   [31:0]  cnt_1s;
reg 	pre_gate;  //1s闸门信号		


always @ ( posedge fbase  or negedge rst_n )begin
			if(!rst_n)begin 
							pre_gate<=0;
							cnt_1s<=0;
							end
			else if(cnt_1s == (time_1s))begin 
							pre_gate<=0;
							cnt_1s<=0;
							end
			else if(pre_gate ==1)
			        cnt_1s<=cnt_1s+1;
			else if(start)
			        pre_gate<=1;
			end
							
					
			
			
	

//**************************************************	
//同步闸门门信号
reg   gate; 
always @ ( posedge sig_A_in or negedge rst_n ) begin
  if (!rst_n)
	 gate <= 0;
  else if(pre_gate) 
    gate <= 1;
  else
    gate <= 0;
end


//*************************************************
//计数模块
	reg [31:0]	sig_A_Cnt_Temp;
	reg [31:0] 	fbase_Cnt_Temp;
	reg [31:0] 	duty_Cnt_Temp;

//标准时钟计数
always @ (posedge fbase or negedge rst_n)  begin
  if (!rst_n)
		fbase_Cnt_Temp <= 31'h00000000;
  else if(gate)
		fbase_Cnt_Temp <= fbase_Cnt_Temp+1;
  else
		fbase_Cnt_Temp <= 31'h00000000;
end
// 待测信号计数 
always @ (posedge sig_A_in or negedge rst_n)   begin
	  if (!rst_n)
		 sig_A_Cnt_Temp <= 31'h00000000;
	  else if(gate)
			sig_A_Cnt_Temp <= sig_A_Cnt_Temp + 1;
		else
			sig_A_Cnt_Temp <= 31'h00000000;
	end	

//************************************************
//占空比 是测量sig_A_in 在高电平期间  fbase时钟的计数
	
	always @ (posedge fbase or negedge rst_n)  begin
		if(!rst_n)
				duty_Cnt_Temp <= 0;
		 else if(pre_gate ==1 && gate == 0)
					duty_Cnt_Temp <= 0;
	    else if(gate && sig_A_in)
				duty_Cnt_Temp <= duty_Cnt_Temp + 1;
	    else 
				duty_Cnt_Temp <= duty_Cnt_Temp;

	end	

//	always @ (posedge fbase or negedge rst_n)  begin
//		if(!rst_n)
//				duty_Cnt_Temp <= 0;
//		 else if(pre_gate ==1 && gate == 0)
//					duty_Cnt_Temp <= 0;
//	    else if(gate && sig_A_in)
//				duty_Cnt_Temp <= duty_Cnt_Temp + 1;
//	    else if(gate && !sig_A_in)
//				duty_Cnt_Temp <= duty_Cnt_Temp;
//		else 
//				duty_Cnt_Temp <= 0;
//	end	
//注释掉的是错误代码！！！！


//*************输出******************
//
always @ (negedge gate or negedge rst_n) begin
  if (!rst_n) begin 
		sig_A_Cnt   <=0;
		fbase_Cnt   <=0;
		duty_Cnt    <=0;
		end
	else begin 
		sig_A_Cnt <= sig_A_Cnt_Temp;
		fbase_Cnt <= fbase_Cnt_Temp;
		duty_Cnt  <=duty_Cnt_Temp;
         end
  
end


//****************************************
reg H2L_F1 = 1'b1;
reg H2L_F2 = 1'b1;
	 
	 always @ ( posedge fbase)
	    begin
			H2L_F1 <= gate;
			H2L_F2 <= H2L_F1;
		end
				
	/***************************************/
	
	assign done = H2L_F2 & !H2L_F1;  //下降沿
	assign busy =gate;
	/***************************************/
 

endmodule
