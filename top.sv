`default_nettype none

module top (
  input wire clk,
  input wire d,
  output reg q = 0
);

always @(posedge clk) begin
  q <= d;
end

endmodule

`default_nettype wire
