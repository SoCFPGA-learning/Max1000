--------------------------------------------------------------------------------
--
--	MAX1000 Workshop
--   
--	FileName:		top.vhd
--	Description:	Toplevel of MAX1000 Workshop Design Part 2
--
--	HDL CODE IS PROVIDED "AS IS."  ARROW EXPRESSLY DISCLAIMS ANY
--	WARRANTY OF ANY KIND, WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT
--	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
--	PARTICULAR PURPOSE, OR NON-INFRINGEMENT. IN NO EVENT SHALL ARROW
--	BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR CONSEQUENTIAL
--	DAMAGES, LOST PROFITS OR LOST DATA, HARM TO YOUR EQUIPMENT, COST OF
--	PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
--	BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF),
--	ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER SIMILAR COSTS.
--
--	Version History
--		Version 1.0 10.03.2019 Bernd Rautmann
--
--------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity top is
	port(
		clk12m			  : in    std_logic;								                  --system clock 12MHz
		reset_button	  : in    std_logic;								                  --asynchronous reset, low active
		led				  : out   std_logic_vector(7 downto 0);				            --LEDs, high active
		sw				     : in    std_logic;									               --user button, low active, shout be schmitt trigger input
		ls3dh_MISO       : in    std_logic                     := '0';             --     ls3dh.MISO
		ls3dh_MOSI       : out   std_logic;                                        --          .MOSI
		ls3dh_SCLK       : out   std_logic;                                        --          .SCLK
		ls3dh_SS_n       : out   std_logic;                                        --          .SS_n
		qspi_pins_dclk   : out   std_logic;                                        -- qspi_pins.dclk
		qspi_pins_ncs    : out   std_logic;                                        --          .ncs
		qspi_pins_data   : inout std_logic_vector(3 downto 0)  := (others => '0'); --          .data
		rs232_rxd        : in    std_logic                     := '0';             --     rs232.rxd
		rs232_txd        : out   std_logic;                                        --          .txd
		sdram_addr       : out   std_logic_vector(11 downto 0);                    --     sdram.addr
		sdram_ba         : out   std_logic_vector(1 downto 0);                     --          .ba
		sdram_cas_n      : out   std_logic;                                        --          .cas_n
		sdram_cke        : out   std_logic;                                        --          .cke
		sdram_cs_n       : out   std_logic;                                        --          .cs_n
		sdram_dq         : inout std_logic_vector(15 downto 0) := (others => '0'); --          .dq
		sdram_dqm        : out   std_logic_vector(1 downto 0);                     --          .dqm
		sdram_ras_n      : out   std_logic;		--          .ras_n
		sdram_clk		  : out   std_logic;
		sdram_we_n       : out   std_logic                                         --          .we_n

	);
end top;

architecture logic of top is
component main_pll IS
	PORT
	(
		inclk0		: IN STD_LOGIC  := '0';
		c0		: OUT STD_LOGIC ;
		c1		: OUT STD_LOGIC ;
		locked		: OUT STD_LOGIC 
	);
END component;

	component nios is
		port (
		clk_clk                 : in    std_logic                     := '0';             --              clk.clk
		key_pio_external_export : in    std_logic                     := '0';             -- key_pio_external.export
		led_pio_external_export : out   std_logic_vector(7 downto 0);                     -- led_pio_external.export
		ls3dh_MISO              : in    std_logic                     := '0';             --            ls3dh.MISO
		ls3dh_MOSI              : out   std_logic;                                        --                 .MOSI
		ls3dh_SCLK              : out   std_logic;                                        --                 .SCLK
		ls3dh_SS_n              : out   std_logic;                                        --                 .SS_n
		qspi_pins_dclk          : out   std_logic;                                        --        qspi_pins.dclk
		qspi_pins_ncs           : out   std_logic;                                        --                 .ncs
		qspi_pins_data          : inout std_logic_vector(3 downto 0)  := (others => '0'); --                 .data
		reset_reset_n           : in    std_logic                     := '0';             --            reset.reset_n
		rs232_rxd               : in    std_logic                     := '0';             --            rs232.rxd
		rs232_txd               : out   std_logic;                                        --                 .txd
		sdram_addr              : out   std_logic_vector(11 downto 0);                    --            sdram.addr
		sdram_ba                : out   std_logic_vector(1 downto 0);                     --                 .ba
		sdram_cas_n             : out   std_logic;                                        --                 .cas_n
		sdram_cke               : out   std_logic;                                        --                 .cke
		sdram_cs_n              : out   std_logic;                                        --                 .cs_n
		sdram_dq                : inout std_logic_vector(15 downto 0) := (others => '0'); --                 .dq
		sdram_dqm               : out   std_logic_vector(1 downto 0);                     --                 .dqm
		sdram_ras_n             : out   std_logic;                                        --                 .ras_n
		sdram_we_n              : out   std_logic                                         --                 .we_n
		);
	end component nios;
	
	signal	clk_50MHz	: std_logic;
	signal	pll_locked	: std_logic;
	signal	reset_nios	: std_logic;
	
begin

	reset_nios	<= reset_button and pll_locked;
	
	main_pll_inst : main_pll
		port map (
			inclk0	 => clk12m,
			c0		 => clk_50MHz,
			c1     => SDRAM_clk,
			locked	 => pll_locked
		);


	u0 : nios
		port map (
			key_pio_external_export => sw,			-- key_pio_external.export
			led_pio_external_export => led,			-- led_pio_external.export
			reset_reset_n           => reset_nios,	-- reset.reset_n
			clk_clk                 => clk_50MHz,	-- clk.clk
			ls3dh_MISO              => ls3dh_MISO,    
			ls3dh_MOSI              => ls3dh_MOSI,    
			ls3dh_SCLK              => ls3dh_SCLK,    
			ls3dh_SS_n              => ls3dh_SS_n, 
			qspi_pins_dclk          => qspi_pins_dclk,
			qspi_pins_ncs           => qspi_pins_ncs, 
			qspi_pins_data          => qspi_pins_data,
			rs232_rxd               => rs232_rxd,     
			rs232_txd               => rs232_txd,     
			sdram_addr              => sdram_addr,    
			sdram_ba                => sdram_ba,      
			sdram_cas_n             => sdram_cas_n,   
			sdram_cke               => sdram_cke,     
			sdram_cs_n              => sdram_cs_n,    
			sdram_dq                => sdram_dq,      
			sdram_dqm               => sdram_dqm,     
			sdram_ras_n             => sdram_ras_n,   
			sdram_we_n              => sdram_we_n    
		);                          
                                  
end logic;
