`timescale 1ns/1ps
module sac16_alu(input wire [15:0] a,b, input wire [3:0] opcode,
 output reg [15:0] result, output reg zero,carry);
 reg [16:0] ext; reg [31:0] product;
 always @* begin
  result=0; zero=0; carry=0; ext=0; product=0;
  case(opcode)
   4'h3: begin ext={1'b0,a}+{1'b0,b}; result=ext[15:0]; carry=ext[16]; end
   4'h4: begin result=a-b; carry=(a>=b); end
   4'h5: begin product=a*b; result=product[15:0]; carry=|product[31:16]; end
   4'h6: result=a&b;
   4'h7: result=a|b;
   4'h8: result=~a;
   default: result=0;
  endcase
  zero=(result==0);
 end
endmodule
