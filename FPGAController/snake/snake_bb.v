
module snake (
	accelerometer_spi_external_interface_I2C_SDAT,
	accelerometer_spi_external_interface_I2C_SCLK,
	accelerometer_spi_external_interface_G_SENSOR_CS_N,
	accelerometer_spi_external_interface_G_SENSOR_INT,
	altpll_areset_conduit_export,
	altpll_locked_conduit_export,
	button_external_connection_export,
	clk_clk,
	clk_sdram_clk,
	hardware_clocks_external_connection_export,
	hardware_in_x_external_connection_export,
	hardware_in_y_external_connection_export,
	hardware_in_z_external_connection_export,
	hardware_out_x_external_connection_export,
	hardware_out_y_external_connection_export,
	hardware_out_z_external_connection_export,
	hex0_external_connection_export,
	hex1_external_connection_export,
	hex2_external_connection_export,
	hex3_external_connection_export,
	hex4_external_connection_export,
	hex5_external_connection_export,
	led_external_connection_export,
	reset_reset_n,
	sdram_wire_addr,
	sdram_wire_ba,
	sdram_wire_cas_n,
	sdram_wire_cke,
	sdram_wire_cs_n,
	sdram_wire_dq,
	sdram_wire_dqm,
	sdram_wire_ras_n,
	sdram_wire_we_n,
	switch_external_connection_export);	

	inout		accelerometer_spi_external_interface_I2C_SDAT;
	output		accelerometer_spi_external_interface_I2C_SCLK;
	output		accelerometer_spi_external_interface_G_SENSOR_CS_N;
	input		accelerometer_spi_external_interface_G_SENSOR_INT;
	input		altpll_areset_conduit_export;
	output		altpll_locked_conduit_export;
	input	[1:0]	button_external_connection_export;
	input		clk_clk;
	output		clk_sdram_clk;
	output	[3:0]	hardware_clocks_external_connection_export;
	input	[30:0]	hardware_in_x_external_connection_export;
	input	[30:0]	hardware_in_y_external_connection_export;
	input	[30:0]	hardware_in_z_external_connection_export;
	output	[30:0]	hardware_out_x_external_connection_export;
	output	[30:0]	hardware_out_y_external_connection_export;
	output	[30:0]	hardware_out_z_external_connection_export;
	output	[7:0]	hex0_external_connection_export;
	output	[7:0]	hex1_external_connection_export;
	output	[7:0]	hex2_external_connection_export;
	output	[7:0]	hex3_external_connection_export;
	output	[7:0]	hex4_external_connection_export;
	output	[7:0]	hex5_external_connection_export;
	output	[9:0]	led_external_connection_export;
	input		reset_reset_n;
	output	[11:0]	sdram_wire_addr;
	output	[1:0]	sdram_wire_ba;
	output		sdram_wire_cas_n;
	output		sdram_wire_cke;
	output		sdram_wire_cs_n;
	inout	[15:0]	sdram_wire_dq;
	output	[1:0]	sdram_wire_dqm;
	output		sdram_wire_ras_n;
	output		sdram_wire_we_n;
	input	[9:0]	switch_external_connection_export;
endmodule
