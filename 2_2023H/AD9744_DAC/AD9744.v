`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2023/05/05 19:56:56
// Design Name: 
// Module Name: AD9744
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module AD9744(
    input sys_clk,
    input sys_rst_n,
    input signed [15:0]data_in,
    output reg signed [13:0]DAC_out,
    output  clk_p,clk_n
    );
    initial begin
        DAC_out <=0;
    end

    assign clk_p = sys_clk;
    assign clk_n = ~sys_clk;

    always @(negedge sys_clk) begin
        if (!sys_rst_n) begin
            DAC_out<=0;
        end
        else begin
           DAC_out<=data_in[15:2];  
        end

    end
endmodule
