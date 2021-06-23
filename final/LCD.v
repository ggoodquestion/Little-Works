module lcd_Ui(
input CLK_50M,
input wire[3:0] pos,
input wire comf,
input wire reset,
input wire[3:0] show,
input wire sf,
output reg strataflash_ce,  // strataflash_ce = 1 to enable LCD for Spartan-3E
// output reg strataflash_ce, // strataflash_ce = 1 to enable
// LCD for Spartan-3E
output reg LCD_E, LCD_RS, LCD_RW,
output reg [7:0] LCD_DB, // LCD_DB[3:0] = 1 for Spartan-3AN
output reg[3:0] syl
);
reg [1:0] state_reg, state_next;
reg [23:0] count_next;
reg [23:0] count = 0; // 23-bit count, 0-(2^23-1), less than 0.2 s
reg [5:0] code; // {LCD_RS, LCD_RW, LCD_DB[7:4]}
reg refresh; // refresh LCD rate @ about 200Hz
reg swl, swr;
reg [5:0] cursor;
reg [11:0] res1, res2;
reg [1:0] num, num_next = 0;

localparam [1:0] in = 2'b00,
					  cal = 2'b01;

always @*
begin	
//syl = cursor;
count_next = count + 1;
cursor = {2'b00, pos[3:0]};
if(cursor > 6'h0d) cursor = 6'h0d;
syl = cursor;

refresh = count[17];

case (show)
4'h0:	res1 = {6'h23, 6'h20};
4'h1:	res1 = {6'h23, 6'h21};
4'h2:	res1 = {6'h23, 6'h22};
4'h3: res1 = {6'h23, 6'h23};
4'h4: res1 = {6'h23, 6'h24};
4'h5: res1 = {6'h23, 6'h25};
4'h6: res1 = {6'h23, 6'h26};
4'h7: res1 = {6'h23, 6'h27};
4'h8: res1 = {6'h23, 6'h28};
4'h9: res1 = {6'h23, 6'h29};
4'ha: res1 = {6'h22, 6'h2b};
4'hb: res1 = {6'h22, 6'h2d};
4'hc: res1 = {6'h22, 6'h2a};
4'hd: res1 = {6'h2f, 6'h2d};
endcase


case (count [23:18]) // every 2^19 clocks = 10 ms (0 for 5 ms, 1 for 5 ms)
// 5 ms > 4.1 ms
0: code = 6'h03; // power-on initializatoin sequence
1: code = 6'h03;
2: code = 6'h03;
3: code = 6'h02;
4: code = 6'h02; // configuration
5: code = 6'h08;
6: code = 6'h00;
7: code = 6'h06;
8: code = 6'h00;
9: code = 6'h0C;
10: code = 6'h00;
11: code = 6'h01;
12: code = 6'h23; // wirte data, 0 or +
13: code = 6'h20;
14: code = 6'h23; // 1 or -
15: code = 6'h21;
16: code = 6'h23; // 2 or *
17: code = 6'h22;
18: code = 6'h23; // 3 or /
19: code = 6'h23;
20: code = 6'h23; // 4
21: code = 6'h24;
22: code = 6'h23; // 5
23: code = 6'h25;
24: code = 6'h23; // 6
25: code = 6'h26;
26: code = 6'h23; // 7
27: code = 6'h27;
28: code = 6'h23; // 8
29: code = 6'h28;
30: code = 6'h23; // 9
31: code = 6'h29;
32: code = 6'h22; // +
33: code = 6'h2b;
34: code = 6'h22; // -
35: code = 6'h2d;
36: code = 6'h22; // *
37: code = 6'h2a;
38: code = 6'h2f; // /
39: code = 6'h2d;
40: code = 6'h0c; // change cursor
41: code = cursor;
42: code = 6'h2f; // show cursor
43: code = 6'h2f;
44: code = 6'h0c; // change cursor
45: code = 6'h0f;
46: code = res1[11:6]; // change cursor
47: code = res1[5:0];
endcase
end

always @ (posedge CLK_50M, posedge reset)
begin
if (reset)
begin
count <= 0;
state_reg <= in;
//cursor <= 0;
num <= 0;
end
else
begin
count <= count_next;
state_reg <= state_next;
num <= num_next;
strataflash_ce <= 1; // for Spartan 3E
LCD_E <= refresh;
LCD_RS <= code[5];
LCD_RW <= code[4];
LCD_DB[7] <= code[3];
LCD_DB[6] <= code[2];
LCD_DB[5] <= code[1];
LCD_DB[4] <= code[0];
LCD_DB[3] <= 1;
LCD_DB[2] <= 1;
LCD_DB[1] <= 1;
LCD_DB[0] <= 1;
end

end
endmodule