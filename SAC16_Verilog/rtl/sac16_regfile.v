`timescale 1ns/1ps
module sac16_regfile(input wire clk,reset,we,input wire [3:0] waddr,
 input wire [15:0] wdata,input wire [3:0] raddr_a,raddr_b,
 output wire [15:0] rdata_a,rdata_b,output reg [15:0] acc,
 output wire [15:0] r0,r1,r2,r3,r4,r5,r6,r7);
 reg [15:0] regs[0:7]; integer i;
 assign rdata_a=(raddr_a<8)?regs[raddr_a[2:0]]:(raddr_a==8)?acc:16'h0000;
 assign rdata_b=(raddr_b<8)?regs[raddr_b[2:0]]:(raddr_b==8)?acc:16'h0000;
 assign r0=regs[0]; assign r1=regs[1]; assign r2=regs[2]; assign r3=regs[3];
 assign r4=regs[4]; assign r5=regs[5]; assign r6=regs[6]; assign r7=regs[7];
 always @(posedge clk) begin
  if(reset) begin for(i=0;i<8;i=i+1) regs[i]<=0; acc<=0; end
  else if(we) begin if(waddr<8) regs[waddr[2:0]]<=wdata; else if(waddr==8) acc<=wdata; end
 end
endmodule
