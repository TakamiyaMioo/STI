module LTC2220 (
    input sys_clk,
    input sys_rst_n,
    input  signed [11:0]indata,
    output clkp,clkn,
    output reg signed [15:0]outdata
);
    assign clkp = sys_clk;
    assign clkn = ~sys_clk;
  
    always @(posedge sys_clk) begin
        if (!sys_rst_n) begin
            outdata<=0;
        end
        else begin
            outdata <= {indata,{4'b0}};
        end
       
        
    end
     

endmodule