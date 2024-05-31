/* verilator lint_off UNUSED */
/* verilator lint_off MULTIDRIVEN */
/* verilator lint_off WIDTH */
/* verilator lint_off BLKSEQ */

module gpioemu (
    input clk,
    input n_reset,
    input [15:0] saddress,
    input srd,
    input swr,
    input gpio_latch,
    input [31:0] sdata_in,
    input [31:0] gpio_in,
    output reg [31:0] gpio_out,
    output reg [31:0] gpio_in_s_insp,
    output reg [31:0] sdata_out
);

localparam SYKT_GPIO_ADDR_SPACE = 16'h0000;
localparam ADDR_A = SYKT_GPIO_ADDR_SPACE + 16'hC8;
localparam ADDR_S = SYKT_GPIO_ADDR_SPACE + 16'hE0;
localparam ADDR_W = SYKT_GPIO_ADDR_SPACE + 16'hD8;

reg [31:0] W;
reg [9:0] A;
reg [1:0] state;

parameter IDLE = 2'b00;
parameter COMPUTE = 2'b01;
parameter DONE = 2'b10;

reg [31:0] current_number;
reg [31:0] prime_count;
reg is_prime;
integer i;

reg [31:0] gpio_in_s;
reg [31:0] gpio_out_s;

initial begin
    A = 10'd0;
    W = 32'd0;
    state = IDLE;
    prime_count = 32'd1;
    current_number = 32'd2;
    gpio_out = 32'd0;
    gpio_in_s_insp = 32'd0;
    sdata_out = 32'd0;
end

always @(negedge n_reset) begin
    gpio_in_s <= 0;
    gpio_out_s <= 0;
    sdata_out <= 0;
end

always @(posedge gpio_latch) begin
    gpio_in_s <= gpio_in;
end

always @(posedge srd) begin
    case (saddress)
        ADDR_A: sdata_out <= {22'd0, A};
        ADDR_S: sdata_out <= {30'd0, state};
        ADDR_W: sdata_out <= W;
        default: sdata_out <= 32'd0;
    endcase
end

always @(posedge swr) begin
    if (saddress == ADDR_A) begin
        A <= sdata_in[9:0];
        state <= COMPUTE;
        current_number <= 32'd2;
        prime_count <= 32'd1;
    end
end

always @(posedge clk or negedge n_reset) begin
    if (!n_reset) begin
        A <= 10'd0;
        W <= 32'd0;
        state <= IDLE;
        prime_count <= 32'd1;
        current_number <= 32'd2;
        gpio_out <= 32'd0;
        gpio_in_s_insp <= 32'd0;
    end else begin
        if (state == COMPUTE) begin
            is_prime = 1;

            for (i = 2; i * i <= current_number; i = i + 1) begin: prime_check
                if (current_number % i == 0) begin
                    is_prime = 0;
                    disable prime_check;
                end
            end

            if (is_prime) begin
                prime_count <= prime_count + 1;
                if (prime_count == A) begin
                    W <= current_number;
                    state <= DONE;
                end
            end

            if (state == COMPUTE) begin
                current_number <= current_number + 1;
            end
        end
        gpio_out <= gpio_out_s;
        gpio_in_s_insp <= gpio_in_s;
    end
end

endmodule

