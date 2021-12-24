set_location_assignment PIN_H6 -to clk12m
set_location_assignment PIN_D8 -to led[7]
set_location_assignment PIN_C10 -to led[6]
set_location_assignment PIN_C9 -to led[5]
set_location_assignment PIN_B10 -to led[4]
set_location_assignment PIN_A10 -to led[3]
set_location_assignment PIN_A11 -to led[2]
set_location_assignment PIN_A9 -to led[1]
set_location_assignment PIN_A8 -to led[0]
set_location_assignment PIN_E7 -to reset_button
set_instance_assignment -name IO_STANDARD "3.3 V SCHMITT TRIGGER" -to reset_button
set_location_assignment PIN_E6 -to sw
set_instance_assignment -name IO_STANDARD "3.3 V SCHMITT TRIGGER" -to sw
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to clk12m
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to led[7]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to led[6]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to led[5]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to led[4]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to led[3]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to led[2]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to led[1]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to led[0]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to led
set_location_assignment PIN_L5 -to ls3dh_SS_n
set_location_assignment PIN_J6 -to ls3dh_SCLK
set_location_assignment PIN_J7 -to ls3dh_MOSI
set_location_assignment PIN_K5 -to ls3dh_MISO
set_location_assignment PIN_B3 -to qspi_pins_ncs
set_location_assignment PIN_A3 -to qspi_pins_dclk
set_location_assignment PIN_D11 -to sdram_dq[0]
set_location_assignment PIN_G10 -to sdram_dq[1]
set_location_assignment PIN_F10 -to sdram_dq[2]
set_location_assignment PIN_F9 -to sdram_dq[3]
set_location_assignment PIN_E10 -to sdram_dq[4]
set_location_assignment PIN_D9 -to sdram_dq[5]
set_location_assignment PIN_G9 -to sdram_dq[6]
set_location_assignment PIN_F8 -to sdram_dq[7]
set_location_assignment PIN_F13 -to sdram_dq[8]
set_location_assignment PIN_E12 -to sdram_dq[9]
set_location_assignment PIN_E13 -to sdram_dq[10]
set_location_assignment PIN_D12 -to sdram_dq[11]
set_location_assignment PIN_C12 -to sdram_dq[12]
set_location_assignment PIN_B12 -to sdram_dq[13]
set_location_assignment PIN_B13 -to sdram_dq[14]
set_location_assignment PIN_A12 -to sdram_dq[15]
set_location_assignment PIN_M8 -to sdram_cke
set_location_assignment PIN_N6 -to sdram_ba[0]
set_location_assignment PIN_K8 -to sdram_ba[1]
set_location_assignment PIN_K6 -to sdram_addr[0]
set_location_assignment PIN_M5 -to sdram_addr[1]
set_location_assignment PIN_N5 -to sdram_addr[2]
set_location_assignment PIN_J8 -to sdram_addr[3]
set_location_assignment PIN_N10 -to sdram_addr[4]
set_location_assignment PIN_M11 -to sdram_addr[5]
set_location_assignment PIN_N9 -to sdram_addr[6]
set_location_assignment PIN_L10 -to sdram_addr[7]
set_location_assignment PIN_M13 -to sdram_addr[8]
set_location_assignment PIN_N8 -to sdram_addr[9]
set_location_assignment PIN_N4 -to sdram_addr[10]
set_location_assignment PIN_M10 -to sdram_addr[11]
set_location_assignment PIN_N7 -to sdram_cas_n
set_location_assignment PIN_M4 -to sdram_cs_n
set_location_assignment PIN_F12 -to sdram_dqm[1]
set_location_assignment PIN_E9 -to sdram_dqm[0]
set_location_assignment PIN_M7 -to sdram_ras_n
set_location_assignment PIN_K7 -to sdram_we_n
set_location_assignment PIN_A4 -to rs232_rxd
set_location_assignment PIN_B4 -to rs232_txd
set_location_assignment PIN_A2 -to qspi_pins_data[0]
set_location_assignment PIN_B2 -to qspi_pins_data[1]
set_location_assignment PIN_C4 -to qspi_pins_data[3]
set_location_assignment PIN_B9 -to qspi_pins_data[2]
set_location_assignment PIN_M9 -to sdram_clk
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to qspi_pins_ncs
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to ls3dh_MISO
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to ls3dh_MOSI
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to ls3dh_SCLK
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to ls3dh_SS_n
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to qspi_pins_data[3]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to qspi_pins_data[2]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to qspi_pins_data[1]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to qspi_pins_data[0]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to qspi_pins_dclk
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to rs232_rxd
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to rs232_txd
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_addr[11]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_addr[10]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_addr[9]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_addr[8]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_addr[7]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_addr[6]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_addr[5]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_addr[4]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_addr[3]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_addr[2]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_addr[1]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_addr[0]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_ba[1]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_ba[0]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_cas_n
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_cke
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_clk
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_cs_n
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_dq[15]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_dq[14]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_dq[13]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_dq[12]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_dq[11]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_dq[10]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_dq[9]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_dq[8]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_dq[7]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_dq[6]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_dq[5]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_dq[4]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_dq[3]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_dq[2]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_dq[1]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_dq[0]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_dqm[1]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_dqm[0]
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_ras_n
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to sdram_we_n
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to altera_reserved_tck
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to altera_reserved_tdi
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to altera_reserved_tdo
set_instance_assignment -name IO_STANDARD "3.3-V LVCMOS" -to altera_reserved_tms
