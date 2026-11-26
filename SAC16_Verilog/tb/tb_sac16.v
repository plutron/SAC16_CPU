`timescale 1ns/1ps
module tb_sac16;
 reg clk=0,reset=1; wire halted,error; wire [7:0] error_code;
 wire [11:0] pc; wire [15:0] sp,ir,acc; wire zero_flag,carry_flag;
 wire [31:0] cycle_count; wire [15:0] r0,r1,r2,r3,r4,r5,r6,r7; wire [3:0] debug_state;
 always #5 clk=~clk;
 sac16_cpu #(.IMEM_FILE("program.hex"),.DMEM_FILE("data.hex")) dut(
 .clk(clk),.reset(reset),.halted(halted),.error(error),.error_code(error_code),
 .pc(pc),.sp(sp),.ir(ir),.acc(acc),.zero_flag(zero_flag),.carry_flag(carry_flag),
 .cycle_count(cycle_count),.r0(r0),.r1(r1),.r2(r2),.r3(r3),.r4(r4),.r5(r5),.r6(r6),.r7(r7),.debug_state(debug_state));
 initial begin
  $dumpfile("build/sac16.vcd"); $dumpvars(0,tb_sac16);
  repeat(4) @(posedge clk); reset=0; wait(halted||error); repeat(2) @(posedge clk);
  $display("halt=%0d error=%0d code=%0d PC=%h SP=%h ACC=%h",halted,error,error_code,pc,sp,acc);
  $display("R0=%h R1=%h R2=%h R3=%h R4=%h R5=%h cycles=%0d",r0,r1,r2,r3,r4,r5,cycle_count);
  if(error) $fatal(1,"CPU error %0d",error_code);
  if(r0!==10 || r1!==20 || r2!==16'h20 || r3!==20) $fatal(1,"LD/ST/register test failed");
  if(r4!==16'h55 || r5!==0 || sp!==16'hffff) $fatal(1,"branch/CALL/RET test failed");
  if(acc!==0 || zero_flag!==1) $fatal(1,"ALU/zero flag test failed");
  if(dut.dmem.mem[16'h20]!==20) $fatal(1,"ST memory value mismatch");
  if(dut.dmem.mem[16'hffff]!==16) $fatal(1,"CALL return address mismatch");
  $display("ALL SAC16 INTEGRATION TESTS PASSED"); $finish;
 end
 initial begin #100000; $fatal(1,"timeout"); end
endmodule
