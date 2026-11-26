`timescale 1ns/1ps
module sac16_cpu #(parameter IMEM_FILE="program.hex",parameter DMEM_FILE="data.hex")(
 input wire clk,reset, output reg halted,error,output reg [7:0] error_code,
 output reg [11:0] pc,output reg [15:0] sp,ir,output wire [15:0] acc,
 output reg zero_flag,carry_flag,output reg [31:0] cycle_count,
 output wire [15:0] r0,r1,r2,r3,r4,r5,r6,r7,output wire [3:0] debug_state);
 localparam F=0,D=1,X=2,MR=3,MW=4,RR=5,RW=6,H=7,E=8;
 localparam BADOP=1,BADREG=2,BADFIELD=3,STACKERR=4;
 reg [3:0] state; reg [16:0] stack_depth;
 wire [15:0] inst,ra,rb,dmdata,alu_result; wire alu_zero,alu_carry;
 reg rfwe; reg [3:0] rfwa,rfra,rfrb; reg [15:0] rfwd;
 reg dmwe; reg [15:0] dma,dmwd;
 assign debug_state=state;
 sac16_imem #(.INIT_FILE(IMEM_FILE)) imem(.clk(clk),.addr(pc),.instruction(inst));
 sac16_dmem #(.INIT_FILE(DMEM_FILE)) dmem(.clk(clk),.we(dmwe),.addr(dma),.wdata(dmwd),.rdata(dmdata));
 sac16_regfile rf(.clk(clk),.reset(reset),.we(rfwe),.waddr(rfwa),.wdata(rfwd),
  .raddr_a(rfra),.raddr_b(rfrb),.rdata_a(ra),.rdata_b(rb),.acc(acc),
  .r0(r0),.r1(r1),.r2(r2),.r3(r3),.r4(r4),.r5(r5),.r6(r6),.r7(r7));
 sac16_alu alu(.a(acc),.b(ra),.opcode(ir[15:12]),.result(alu_result),.zero(alu_zero),.carry(alu_carry));
 always @* begin
  rfwe=0; rfwa=0; rfwd=0; rfra=ir[11:8]; rfrb=ir[7:4];
  if(state==X) case(ir[15:12])
   2: begin rfwe=1; rfwa=ir[11:8]; rfwd=rb; end
   3,4,5,6,7,8: begin rfwe=1; rfwa=8; rfwd=alu_result; end
   14: begin rfwe=1; rfwa=ir[11:8]; rfwd={8'h00,ir[7:0]}; end
   default: ;
  endcase
  else if(state==MW) begin rfwe=1; rfwa=ir[11:8]; rfwd=dmdata; end
 end
 always @* begin
  dmwe=0; dma=0; dmwd=0;
  if(state==X) case(ir[15:12])
   0: dma=rb;
   1: begin dmwe=1; dma=rb; dmwd=ra; end
   12: begin dmwe=1; dma=sp; dmwd={4'b0,pc}; end
   default: ;
  endcase
  else if(state==MR) dma=rb;
  else if(state==RR) dma=sp+16'd1;
 end
 always @(posedge clk) begin
  if(reset) begin state<=F; pc<=0; sp<=16'hffff; ir<=0; halted<=0; error<=0; error_code<=0;
   zero_flag<=0; carry_flag<=0; cycle_count<=0; stack_depth<=0; end
  else begin
   cycle_count<=cycle_count+1'b1;
   case(state)
    F: state<=D;
    D: begin ir<=inst; pc<=pc+1'b1; state<=X; end
    X: case(ir[15:12])
     0: if(ir[11:8]>8 || ir[7:4]>8) begin error<=1; error_code<=BADREG; state<=E; end else if(ir[3:0]!=0) begin error<=1; error_code<=BADFIELD; state<=E; end else state<=MR;
     1,2: if(ir[11:8]>8 || ir[7:4]>8) begin error<=1; error_code<=BADREG; state<=E; end else if(ir[3:0]!=0) begin error<=1; error_code<=BADFIELD; state<=E; end else state<=F;
     3,4,5,6,7: if(ir[11:8]>8) begin error<=1; error_code<=BADREG; state<=E; end else if(ir[7:4]!=0 || ir[3:0]!=0) begin error<=1; error_code<=BADFIELD; state<=E; end else begin zero_flag<=alu_zero; carry_flag<=alu_carry; state<=F; end
     8: if(ir[11:0]!=0) begin error<=1; error_code<=BADFIELD; state<=E; end else begin zero_flag<=alu_zero; carry_flag<=0; state<=F; end
     9: begin pc<=ir[11:0]; state<=F; end
     10: begin if(acc==0) pc<=ir[11:0]; state<=F; end
     11: begin if(acc!=0) pc<=ir[11:0]; state<=F; end
     12: if(sp==0 || stack_depth==17'h0ffff) begin error<=1; error_code<=STACKERR; state<=E; end else begin sp<=sp-1'b1; pc<=ir[11:0]; stack_depth<=stack_depth+1'b1; state<=F; end
     13: if(ir[11:0]!=0) begin error<=1; error_code<=BADFIELD; state<=E; end else if(stack_depth==0 || sp==16'hffff) begin error<=1; error_code<=STACKERR; state<=E; end else state<=RR;
     14: if(ir[11:8]>8) begin error<=1; error_code<=BADREG; state<=E; end else state<=F;
     15: if(ir[11:0]!=0) begin error<=1; error_code<=BADFIELD; state<=E; end else begin halted<=1; state<=H; end
     default: begin error<=1; error_code<=BADOP; state<=E; end
    endcase
    MR: state<=MW;
    MW: state<=F;
    RR: state<=RW;
    RW: begin pc<=dmdata[11:0]; sp<=sp+1'b1; stack_depth<=stack_depth-1'b1; state<=F; end
    H: state<=H;
    E: state<=E;
    default: begin error<=1; error_code<=BADOP; state<=E; end
   endcase
  end
 end
endmodule
