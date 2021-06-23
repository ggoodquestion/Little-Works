module control(
input CLK_50M, 
input wire reset,
input wire [3:0] pos,
input wire comfirm,
input wire cal,
input wire read,
output wire strataflash_ce,  // strataflash_ce = 1 to enable LCD for Spartan-3E
output wire LCD_E, LCD_RS, LCD_RW,
output wire [7:0] LCD_DB, // LCD_DB[3:0] = 1 for Spartan-3AN
output wire [3:0] led,
output wire [3:0] op_data,
output wire op_emp, op_full
);
//LCD control input
wire op_cal;
wire op_rd, op_wr, sta_rd, sta_wr, out_rd, out_wr;
reg op_rd_reg, op_wr_reg, sta_rd_reg, sta_wr_reg, out_rd_reg, out_wr_reg;

//wire [3:0] op_data, op_data_next;
wire [3:0] sta_data, sta_data_next;
wire [3:0] out_data, out_data_next;

wire [3:0] op_in;
wire [3:0] sta_in;
wire[3:0] out_in;
reg[3:0] op_id, sta_id, out_id;
wire [7:0] data;

//Calculate stat
localparam [2:0] idle = 3'b000,
					  n1 = 3'b001,
					  op = 3'b010,
					  n2 = 3'b011,
					  caln = 3'b100,
					  fin = 3'b101;
reg [2:0] state_reg, state_next;

reg [6:0] a, b, opd;
reg [6:0] res, res_next;
reg ls = 0;
wire sf;
reg sf_reg = 0;

wire dp_comfirm, db_cal;
//wire op_emp, op_full;
wire [3:0] syl;

//assign op_rd = op_rd_reg;
assign sf = sf_reg;
assign data = {ls, res};

debounce_explicit db_exp(CLK_50M, reset, comfirm, , db_comfirm);
debounce_explicit db_rd_exp(CLK_50M, reset, read, , op_rd);
debounce_explicit db_cal_exp(CLK_50M, reset, cal, , db_cal);

lcd_Ui ui(CLK_50M, pos, db_comfirm, reset, op_data, op_rd, strataflash_ce, LCD_E, LCD_RS, LCD_RW, LCD_DB, syl);

fifo opp (CLK_50M, reset, op_rd, db_comfirm, syl, op_emp, op_full, op_data);


always @ (posedge CLK_50M, posedge reset)
begin
	if(reset)
	begin
		state_reg <= idle;
		res <= 0;
	end
	else
	begin
		state_reg <= state_next;
		res <= res_next;
	end
end

endmodule