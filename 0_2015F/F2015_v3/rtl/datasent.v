
module datasent(
			CLK_r,
			Rst_n,
			EN,
			done,
			fxCnt,
			fbaseCnt,
			dutyCnt,
			delay_cnt,
			stage,
			TX_Pin_Out
			
);
	input CLK_r;
	input Rst_n;
	input EN;
	input[31:0]  fxCnt;
	input[31:0]  fbaseCnt;
	input[31:0]  dutyCnt;
	input[31:0]  delay_cnt;
	input[7:0]   stage;
	
	output wire  TX_Pin_Out;
	output wire  done;

	wire isDone;  //tx发送完成  一次

	reg rEN=1'b0;
	reg [7:0]data;

	uart_byte_tx uart_byte_tx(
		.Clk(CLK_r),
		.Rst_n(Rst_n),
		.data_byte(data),
		.send_en(rEN),
		.baud_set(3'd0),
		.Rs232_Tx(TX_Pin_Out),
		.Tx_Done(isDone),
		.uart_state()
	);
	
	reg [7:0] Data[16:0]; //数据缓存 8位*17个
	reg [5:0] i = 6'd0;  //计数发送
	
	//****************************************
wire  isDone_up;
reg L2H_F1 = 1'b0;
reg L2H_F2 = 1'b0;
	 
	 always @ ( posedge CLK_r or negedge Rst_n )begin 
	 if(!Rst_n)begin 
			L2H_F1 <= 0;
			L2H_F2 <= 0;
			end
		else 
	     begin
			L2H_F1 <= isDone;
			L2H_F2 <= L2H_F1;
		  end
	  end
				
	/***************************************/
	
	assign isDone_up = L2H_F1 & !L2H_F2;
	
	/***************************************/

	always@(posedge CLK_r or posedge EN)		
		if(EN)
			begin
				
				Data[16] <=stage;
				Data[15] <=dutyCnt[31:24];
				Data[14] <=dutyCnt[23:16];
				Data[13] <=dutyCnt[15:8]; 
				Data[12] <=dutyCnt[7:0]; 
				
				Data[11] <=delay_cnt[31:24];   
				Data[10] <=delay_cnt[23:16];   
				Data[9]  <=delay_cnt[15:8];    
				Data[8]  <=delay_cnt[7:0]; 
				
				Data[7] <=fxCnt[31:24];
				Data[6] <=fxCnt[23:16];
				Data[5] <=fxCnt[15:8]; 
				Data[4] <=fxCnt[7:0]; 
				
				Data[3] <=fbaseCnt[31:24];   
				Data[2] <=fbaseCnt[23:16];   
				Data[1] <=fbaseCnt[15:8];    
				Data[0] <=fbaseCnt[7:0]; 
				i <= 1'b1;				
			end
		else 
			begin
				if(rEN == 1'b0 && i>= 1'b1)
					begin
						case(i)
						6'd1  : begin data <=  8'hf0 ;  rEN <= 1'b1;  i <= i +1'b1; end
						6'd2  : begin data <= Data[16]; rEN <= 1'b1;  i <= i +1'b1; end	//stage
						
						6'd3  : begin data <= Data[15]; rEN <= 1'b1;  i <= i +1'b1; end   //dutyCnt
						6'd4  : begin data <= Data[14]; rEN <= 1'b1;  i <= i +1'b1; end
						6'd5  : begin data <= Data[13]; rEN <= 1'b1;  i <= i +1'b1; end	        
						6'd6  : begin data <= Data[12]; rEN <= 1'b1;  i <= i +1'b1; end
						
						6'd7  : begin data <= Data[11]; rEN <= 1'b1;  i <= i +1'b1; end  //delay_cnt
						6'd8  : begin data <= Data[10]; rEN <= 1'b1;  i <= i +1'b1; end
						6'd9  : begin data <= Data[9]; rEN <= 1'b1;   i <= i +1'b1; end        
						6'd10 : begin data <= Data[8]; rEN <= 1'b1;  i <= i +1'b1; end
						
						6'd11 : begin data <= Data[7]; rEN <= 1'b1;  i <= i +1'b1; end  //fxCnt
						6'd12 : begin data <= Data[6]; rEN <= 1'b1;  i <= i +1'b1; end
						6'd13 : begin data <= Data[5]; rEN <= 1'b1;  i <= i +1'b1; end      
						6'd14 : begin data <= Data[4]; rEN <= 1'b1;  i <= i +1'b1; end
						
						6'd15 : begin data <= Data[3]; rEN <= 1'b1;  i <= i +1'b1; end  //fbaseCnt
						6'd16 : begin data <= Data[2]; rEN <= 1'b1;  i <= i +1'b1; end
						6'd17 : begin data <= Data[1]; rEN <= 1'b1;  i <= i +1'b1; end
						6'd18 : begin data <= Data[0]; rEN <= 1'b1;  i <= i +1'b1; end
						
                  //sent mode code,equal test freq
						6'd19: begin data <= 8'h0d  ; rEN <= 1'b1;  i <= i +1'b1; end
						6'd20: begin data <= 8'h0f  ; rEN <= 1'b1;  i <= i +1'b1; end
						6'd21: i <= 1'b0;
						endcase
					end
					
				if(isDone_up == 1'b1)		//isDone有效  开始计数发送 
					begin
						rEN <=1'b0;
		       	end	

			end
						
					
					
						
endmodule
	