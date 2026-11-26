`timescale 1ns/1ps
module tb_alu;
 reg [15:0] a,b; reg [3:0] opcode;
 wire [15:0] result; wire zero,carry;
 sac16_alu dut(.a(a),.b(b),.opcode(opcode),.result(result),.zero(zero),.carry(carry));
 task check(input [3:0] op,input [15:0] av,bv,expected,input z,c);
 begin
  opcode=op; a=av; b=bv; #1;
  if(result!==expected || zero!==z || carry!==c)
   $fatal(1,"ALU op=%h a=%h b=%h result=%h z=%b c=%b expected=%h z=%b c=%b",op,av,bv,result,zero,carry,expected,z,c);
 end endtask
 initial begin
  check(4'h3,16'hffff,16'h0001,16'h0000,1'b1,1'b1); // ADD overflow
  check(4'h4,16'h000a,16'h0005,16'h0005,1'b0,1'b1); // SUB no borrow
  check(4'h4,16'h0005,16'h000a,16'hfffb,1'b0,1'b0); // SUB borrow
  check(4'h5,16'hffff,16'h0002,16'hfffe,1'b0,1'b1); // MUL high half
  check(4'h6,16'hf0f0,16'h0ff0,16'h00f0,1'b0,1'b0); // AND
  check(4'h7,16'hf000,16'h000f,16'hf00f,1'b0,1'b0); // OR
  check(4'h8,16'hffff,16'h0000,16'h0000,1'b1,1'b0); // NOT
  $display("ALL ALU TESTS PASSED"); $finish;
 end
endmodule
