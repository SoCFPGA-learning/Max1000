--------------------------------------------------------------------------------
--
--	MAX1000 Workshop
--   
--	FileName:		top.vhd
--	Description:	Toplevel of MAX1000 Workshop Design
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
--		Version 1.0 18.02.2019 Matthias Glattfelder
--
--------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity top is
	port(
		clk12m			: in  std_logic;								--system clock 12MHz
		reset_button	: in  std_logic;								--asynchronous reset, low active
		led				: out std_logic_vector(7 downto 0);				--LEDs, high active
		sw				: in  std_logic									--user button, low active, shout be schmitt trigger input
	);
end top;

architecture logic of top is

	component main_pll
		port
		(
			areset	: in std_logic  := '0';
			inclk0	: in std_logic  := '0';
			c0		: out std_logic ;
			locked	: out std_logic 
		);
	end component;

	component nios is
		port (
			key_pio_external_export : in  std_logic;					-- export
			led_pio_external_export : out std_logic_vector(7 downto 0);	-- export
			reset_reset_n           : in  std_logic;					-- reset_n
			clk_clk                 : in  std_logic						-- clk
		);
	end component nios;
	
	signal	clk_50MHz	: std_logic;
	signal	pll_locked	: std_logic;
	signal	reset_nios	: std_logic;
	
begin

	reset_nios	<= reset_button and pll_locked;
	
	main_pll_inst : main_pll
		port map (
			areset	 => '0',
			inclk0	 => clk12m,
			c0		 => clk_50MHz,
			locked	 => pll_locked
		);

	u0 : nios
		port map (
			key_pio_external_export => sw,			-- key_pio_external.export
			led_pio_external_export => led,			-- led_pio_external.export
			reset_reset_n           => reset_nios,	-- reset.reset_n
			clk_clk                 => clk_50MHz	-- clk.clk
		);

end logic;
