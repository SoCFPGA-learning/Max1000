#
#
create_clock -name {master_clk} -period 83.334 [get_ports {CLK12M}] -add

derive_clock_uncertainty
derive_pll_clocks
set_false_path -from reset_button -to *
set_false_path -from sw -to *
set_false_path -from * -to [get_ports led*]