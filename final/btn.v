module BTN(
input wire clk, reset,
input wire sw,
output reg rw
);

localparam  [1:0]
               zero  = 2'b00,
               wait0 = 2'b01,
               one   = 2'b10;

   // signal declaration
   reg [1:0] state_reg, state_next;

    always @(posedge clk, posedge reset)
       if (reset)
          begin
             state_reg <= zero;
          end
       else
          begin
             state_reg <= state_next;
          end

   always @*
   begin
      state_next = state_reg;  // default state: the same
      case (state_reg)
         zero:
            begin
		rw = 1'b0;
               if (sw)
                  begin
                     state_next = one;
                  end
            end
         one:
            begin
		rw = 1'b1;
               if (~sw)
                  begin
                     state_next = zero;
                  end
		else state_next = wait0;
            end
         wait0:
            begin
               rw = 1'b0;
               if (~sw)
                  begin
                     state_next = zero;
                  end
               else // sw==1
                  state_next = wait0;
            end
         default: state_next = zero;
      endcase
   end

endmodule