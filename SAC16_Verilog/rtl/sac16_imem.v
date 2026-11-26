`timescale 1ns/1ps
module sac16_imem #(parameter integer DEPTH=4096, parameter INIT_FILE="program.hex")
 (input wire clk,input wire [11:0] addr,output reg [15:0] instruction);
 reg [15:0] mem[0:DEPTH-1]; integer i;
 initial begin instruction=0; for(i=0;i<DEPTH;i=i+1) mem[i]=0; $readmemh(INIT_FILE,mem); end
 always @(posedge clk) instruction<=mem[addr];
endmodule
