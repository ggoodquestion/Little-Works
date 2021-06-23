// 4k x 4 Dual-Port Block-RAM-based FIFO
module fifo #( parameter B=4,W=4 )
(
input wire clk, reset,
input wire rd, wr,
input wire [B-1:0] w_data,
output wire empty, full,
output wire [B-1:0] r_data );

reg [W:0] w_ptr_reg, w_ptr_next, w_ptr_succ, w_ptr_mi;
reg [W:0] r_ptr_reg, r_ptr_next, r_ptr_succ, r_ptr_mi;
reg full_reg, empty_reg, full_next, empty_next;
reg [W:0] ds_reg, ds_next;
wire wr_en;
wire rd_en;
reg[11:0] ad_reg;
wire [11:0] addressA;
wire [11:0] addressB;

RAMB16_S4_S4 #(
      .INIT_A(4'h0),  // Value of output RAM registers on Port A at startup
      .INIT_B(4'h0),  // Value of output RAM registers on Port B at startup
      .SRVAL_A(4'h0), // Port A output value upon SSR assertion
      .SRVAL_B(4'h0), // Port B output value upon SSR assertion
      .WRITE_MODE_A("WRITE_FIRST"), // WRITE_FIRST, READ_FIRST or NO_CHANGE
      .WRITE_MODE_B("WRITE_FIRST"), // WRITE_FIRST, READ_FIRST or NO_CHANGE
      .SIM_COLLISION_CHECK("ALL"),  // "NONE", "WARNING_ONLY", "GENERATE_X_ONLY", "ALL" 

      // The forllowing INIT_xx declarations specify the initial contents of the RAM
      // Address 0 to 1023
      .INIT_00(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_01(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_02(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_03(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_04(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_05(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_06(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_07(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_08(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_09(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_0A(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_0B(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_0C(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_0D(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_0E(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_0F(256'h0000000000000000000000000000000000000000000000000000000000000000),
      // Address 1024 to 2047
      .INIT_10(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_11(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_12(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_13(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_14(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_15(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_16(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_17(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_18(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_19(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_1A(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_1B(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_1C(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_1D(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_1E(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_1F(256'h0000000000000000000000000000000000000000000000000000000000000000),
      // Address 2048 to 3071
      .INIT_20(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_21(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_22(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_23(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_24(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_25(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_26(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_27(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_28(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_29(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_2A(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_2B(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_2C(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_2D(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_2E(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_2F(256'h0000000000000000000000000000000000000000000000000000000000000000),
      // Address 3072 to 4095
      .INIT_30(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_31(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_32(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_33(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_34(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_35(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_36(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_37(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_38(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_39(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_3A(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_3B(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_3C(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_3D(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_3E(256'h0000000000000000000000000000000000000000000000000000000000000000),
      .INIT_3F(256'h0000000000000000000000000000000000000000000000000000000000000000)) 
      RAMB16_S4_S4_inst (
      .DOA(), // Port A 4-bit Data Output
      .DOB(r_data), // Port B 4-bit Data Output
      .ADDRA(addressA), // Port A 12-bit Address Input
      .ADDRB(addressB), // Port B 12-bit Address Input
      .CLKA(clk), // Port A Clock
      .CLKB(clk), // Port B Clock
      .DIA(w_data), // Port A 4-bit Data Input
      .DIB(), // Port B 4-bit Data Input
      .ENA(wr_en), // Port A RAM Enable Input for read, write, or reset
      .ENB(rd_en), // Port B RAM Enable Input for read, write, or reset
      .SSRA(reset), // Port A Synchronous Set/Reset Input
      .SSRB(reset), // Port B Synchronous Set/Reset Input
      .WEA(wr), // Port A Write Enable Input
      .WEB() // Port B Write Enable Input
);

assign addressA = {{(11-W){1'b0}}, w_ptr_reg}; // asssign address
assign addressB = {{(11-W){1'b0}}, r_ptr_reg}; // asssign address
assign wr_en = wr & ~full_reg; // when FIFO is not full
assign rd_en = rd & ~empty_reg; // when FIFO is not empty
always @(posedge clk, posedge reset)
if (reset)
begin
w_ptr_reg <= 1;
r_ptr_reg <= 0;
full_reg <= 1'b0;
empty_reg <= 1'b1;
ds_reg <= 5'b00000;
end
else
begin
w_ptr_reg <= w_ptr_next;
r_ptr_reg <= r_ptr_next;
full_reg <= full_next;
empty_reg <= empty_next;
ds_reg <= ds_next;
end

// next-state logic for read and write pointers
always @*
begin
w_ptr_succ = w_ptr_reg + 1; // successive pointer values
r_ptr_mi = r_ptr_reg - 1; // successive pointer values
r_ptr_succ = r_ptr_reg + 1;
w_ptr_next = w_ptr_reg; // default: keep old values
r_ptr_next = r_ptr_reg;
full_next = full_reg;
empty_next = empty_reg;
ds_next = ds_reg;
case ({wr, rd}) // 2'b00: no op
2'b01: // read
if (~empty_reg) // not empty
begin
r_ptr_next = r_ptr_mi;
w_ptr_next = r_ptr_mi + 1;
full_next = 1'b0;
ds_next = ds_reg - 1;
if (ds_next==5'b00000)
empty_next = 1'b1;
end

2'b10: // write
if (~full_reg) // not full
begin
r_ptr_next = r_ptr_succ;
w_ptr_next = r_ptr_succ + 1;
empty_next = 1'b0;
ds_next = ds_reg + 1;
if (ds_next==5'b10000) 
full_next = 1'b1;
end
2'b11: // write and read
begin
w_ptr_next = w_ptr_succ;
r_ptr_next = r_ptr_succ;
end
endcase
end
assign full = full_reg; // output
assign empty = empty_reg;
endmodule