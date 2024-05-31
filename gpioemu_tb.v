module gpioemu_tb;

    reg clk;
    reg n_reset;
    reg [15:0] saddress;
    reg srd;
    reg swr;
    reg gpio_latch;
    reg [31:0] sdata_in;
    reg [31:0] gpio_in;
    wire [31:0] gpio_out;
    wire [31:0] gpio_in_s_insp;
    wire [31:0] sdata_out;

    reg [31:0] expected_result;
    reg [31:0] actual_result;

    initial begin
        clk = 0;
        forever #5 clk = ~clk;
    end

    gpioemu uut (
        .clk(clk),
        .n_reset(n_reset),
        .saddress(saddress),
        .srd(srd),
        .swr(swr),
        .sdata_in(sdata_in),
        .gpio_in(gpio_in),
        .gpio_out(gpio_out),
        .gpio_in_s_insp(gpio_in_s_insp),
        .sdata_out(sdata_out)
    );

    initial begin
        n_reset = 0;
        saddress = 0;
        srd = 0;
        swr = 0;
        gpio_latch = 0;
        sdata_in = 0;
        gpio_in = 0;

        #10;
        n_reset = 1;
        
        // Test 1: Write to wrong address
        #10;
        swr = 1;
        saddress = 16'h0010;
        sdata_in = 32'h12345678;
        #10;
        swr = 0;

        // Test 1: Write to correct address
        #10;
        swr = 1;
        saddress = 16'h00C8;
        sdata_in = 32'd5;
        #10;
        swr = 0;

        #2000; 

        // Test 1: Read from correct address
        #10;
        srd = 1;
        saddress = 16'h00D8;
        #10;
        actual_result = sdata_out;
        srd = 0;
        #10;

        expected_result = 32'd11;
        if (actual_result != expected_result) begin
            $display("Test 1 failed: Expected %d, got %d", expected_result, actual_result);
        end else begin
            $display("Test 1 passed: Correctly found 5th prime number %d", actual_result);
        end

        // Test 2: Write to correct address
        #10;
        swr = 1;
        saddress = 16'h00C8;
        sdata_in = 32'd10;
        #10;
        swr = 0;

        #2000;

        // Test 2: Read from correct address
        #10;
        srd = 1;
        saddress = 16'h00D8;
        #10;
        actual_result = sdata_out;
        srd = 0;
        #10;

        expected_result = 32'd29;
        if (actual_result != expected_result) begin
            $display("Test 2 failed: Expected %d, got %d", expected_result, actual_result);
        end else begin
            $display("Test 2 passed: Correctly found 10th prime number %d", actual_result);
        end

        // Test 3: Check 1st prime number
        #10;
        swr = 1;
        saddress = 16'h00C8;
        sdata_in = 32'd1;
        #10;
        swr = 0;

        #2000;

        #10;
        srd = 1;
        saddress = 16'h00D8;
        #10;
        actual_result = sdata_out;
        srd = 0;
        #10;

        expected_result = 32'd2;
        if (actual_result != expected_result) begin
            $display("Test 3 failed: Expected %d, got %d", expected_result, actual_result);
        end else begin
            $display("Test 3 passed: Correctly found 1st prime number %d", actual_result);
        end

        // Test 4: Check 1000th prime number
        #10;
        swr = 1;
        saddress = 16'h00C8;
        sdata_in = 32'd1000;
        #10;
        swr = 0;

        #500000;

        #10;
        srd = 1;
        saddress = 16'h00D8;
        #10;
        actual_result = sdata_out;
        srd = 0;
        #10;

        expected_result = 32'd7919;
        if (actual_result != expected_result) begin
            $display("Test 4 failed: Expected %d, got %d", expected_result, actual_result);
        end else begin
            $display("Test 4 passed: Correctly found 1000th prime number %d", actual_result);
        end

        $finish;
    end

    initial begin
        $monitor("Time = %0t, n_reset = %b, saddress = %h, srd = %b, swr = %b, sdata_in = %h, sdata_out = %h, gpio_in = %h, gpio_out = %h, gpio_in_s_insp = %h", 
                  $time, n_reset, saddress, srd, swr, sdata_in, sdata_out, gpio_in, gpio_out, gpio_in_s_insp);
    end

    initial begin
        $dumpfile("gpioemu.vcd");
        $dumpvars(0, gpioemu_tb);
    end

endmodule
