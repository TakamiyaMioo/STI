/*
Description:
1.时间间隔测量模块
2.方法：状态法测短时间，有粗测时间和精细时间
		clk与clk_dly90配合记录状态，能将clk计数时钟细分4份，等效于主频提升4倍
       clk--A
		 clk_dly90--B
3.计算方法：
		start_stage              
		AB | 
		00 | +0           
		01 | +0.25T            
		11 | +0.5T       
		10 | +0.75T  
		
		end_stage
		AB | 
		10 | -0
		11 | -0.25T
		01 | -0.5T
		00 | -0.75T		
   粗测时间=delay_cnt*T
	细测时间= start_stage+end_stage
	总的时间间隔=粗测时间+细测时间；
4.计算举例
  例子在time_mas_tb中，仿真结果仿真图
  sig_a与sig_b 时间间隔为27.321ns （modelsim中用卡尺测量）
  粗测时间=3*10ns=30ns
  细测时间=start_stage（01）+end_stage（01）
         =2.5ns-5ns=-2.5ns
  总时间=30-2.5=27.5ns
  误差为27.5-27.321=0.2ns



*/


module time_measure(
		clk,		//时钟计数信号   200m
		clk_dly90,  // 相位滞后90度信号，
		sig_a,   
		sig_b,
		rst_n,
		start,   //开始信号，保持一个时钟的高电平信号
		delay_cnt,//输出的粗测时间
		stage,   //输出两个状态
		done,    // 完成信号
		busy    //忙信号
		
		);
	input clk;
	input clk_dly90;
	input sig_a;
	input sig_b;
	input rst_n;
	input start;
	output reg [7:0] stage ;
	output reg [31:0]delay_cnt;
	output wire done;
	output wire busy;
//*******************************************
//   转换使能信号
reg pre_gate ;
always @ (posedge clk or negedge rst_n)  begin
  if (!rst_n)
		pre_gate<=0;
  else if (start)
		pre_gate <= 1;
  else if (done )
		pre_gate<=0;
  else 
		pre_gate <=pre_gate;
  end
//*********************************************
//同步闸门门信号
reg  gate;
always @ ( posedge sig_a or negedge rst_n ) begin
  if (!rst_n)
	 gate <= 0;
  else if(pre_gate) 
    gate <= 1;
  else
    gate <= 0;
end
/*
//***************时间间隔测量**********************
//用边沿检测的方式，用状态法测量短时间***************
//**********************************************


	//sig_A_in_up  A信号上升沿脉冲信号
	wire sig_A_in_up; 
	reg L2H_F1 = 1'b0;
	reg L2H_F2 = 1'b0;
	 
	 always @ ( posedge clk)
	    begin
			L2H_F1 <= sig_a;
			L2H_F2 <= L2H_F1;
		end
	assign sig_a_up = L2H_F1  & ! L2H_F2;		
	
	//sig_B_in_up  B信号上升沿脉冲
	wire sig_B_in_up; 
	reg L2H_F3 = 1'b0;
	reg L2H_F4=  1'b0;
	 
	 always @ ( posedge clk)
	    begin
			L2H_F3 <= sig_b;
			L2H_F4 <= L2H_F3;
		end
	
	assign sig_b_up = L2H_F3 & !L2H_F4;
*/
//*****************************************
//200ms计数器，即每200MS的时间内测量一次时间间隔
parameter time_200ms=(20000000-1);
reg [31:0] cnt_200ms;
always @ (posedge clk or negedge rst_n)  begin
  if (!rst_n)
		cnt_200ms<=0;
  else if(gate)begin 
			if(cnt_200ms==time_200ms)
					cnt_200ms<=0;
			else 
				cnt_200ms<=cnt_200ms+1;
			end
	else 
		cnt_200ms<=0;
	end
//************************************************
//记录开始时刻状态与计数值
reg [1:0] stage_start;
reg [31:0] cnt_start;
reg 	   next_s;
always @ (posedge sig_b or negedge rst_n)  begin
  if (!rst_n)begin 
		stage_start<=0;
		cnt_start<=0;
		next_s<=1;
		end
  else if(gate && next_s && (cnt_200ms !=time_200ms ))begin 
			next_s<=0;
			cnt_start<=cnt_200ms;//记录开始时刻的计数值
			stage_start<={clk,clk_dly90};//记录开始时刻的状态
			end
	    else begin 
			stage_start<=0;
			cnt_start<=0;
			if(!gate)
				next_s<=1;
			end
	end 
	
//*****************************************************
//记录结束时刻状态与计数值
reg [1:0] stage_end;
reg [31:0] cnt_end;
reg 	   next_e;
always @ (posedge sig_a or negedge rst_n)  begin
  if (!rst_n)begin 
		stage_end<=0;
		cnt_end<=0;
		next_e<=1;
		end
   else if(gate && next_e &&  (cnt_200ms !=time_200ms ) ) begin 
			next_e<=0;
			cnt_end<=cnt_200ms;//记录结束时刻的计数值
			stage_end<={clk,clk_dly90};//记录结束时刻的状态
			end
	else begin 
			stage_end<=0;
			cnt_end<=0;
			if(!gate)
				next_e<=1;
			end
	end
assign done= ~next_e;   //完成信号
assign busy= gate;  //忙信号		


always @ (posedge done or negedge rst_n)  begin//完成时进行数据采集输出
  if (!rst_n)begin 
		stage<=0;
		delay_cnt<=0;
		end
	else begin 
		delay_cnt <= cnt_end - cnt_start;
		stage <={stage_start,stage_end};
		end
		
	end
	
	
	
	
endmodule 

	