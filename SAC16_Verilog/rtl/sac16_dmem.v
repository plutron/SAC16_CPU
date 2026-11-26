`timescale 1ns/1ps
module sac16_dmem #(parameter integer DEPTH=65536,parameter INIT_FILE="data.hex")
 (input wire clk,input wire we,input wire [15:0] addr,wdata,output reg [15:0] rdata);
 reg [15:0] mem[0:DEPTH-1]; integer i;
 initial begin rdata=0; for(i=0;i<DEPTH;i=i+1) mem[i]=0; $readmemh(INIT_FILE,mem); end
 always @(posedge clk) begin if(we) mem[addr]<=wdata; rdata<=mem[addr]; end
endmodule
