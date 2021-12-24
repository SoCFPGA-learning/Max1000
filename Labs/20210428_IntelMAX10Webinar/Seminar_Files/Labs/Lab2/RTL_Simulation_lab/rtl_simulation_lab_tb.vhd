----------------------------------------------------------------
--Design Name:		RTL Simulation Lab
--Module Name:		Testbench
--Version:			1.0
--Created Date:	15 January 2019
--
--Company:			Arrow Electronics
--Author:			Szabolcs Bondor
--
--Configuration:
--Target Device:	Altera Max10 10M08SAU169C8G
--Target Board:	Arrow MAX1000 Board
--Software:			Quartus Prime Lite Edition Version 18.0.0
--
--Description:		Testbench file for rtl_simulation_lab.vhd
--Note:				
--Revision history:
----------------------------------------------------------------


---------------------------[LIBRARY]----------------------------

library ieee;
use ieee.std_logic_1164.all;

---------------------------[ENTITY]-----------------------------

entity rtl_simulation_lab_tb is
end entity rtl_simulation_lab_tb;

------------------------[ARCHITECTURE]--------------------------

architecture Behavioral of rtl_simulation_lab_tb is


signal CLK	: std_logic;
signal LED	: std_logic_vector(7 downto 0);


component rtl_simulation_lab
port(	CLK	: in std_logic;
		LED	: out std_logic_vector(7 downto 0));
end component;


begin


	toplevel : rtl_simulation_lab
	port map(CLK => CLK,
				LED => LED);

				
	clk12mhz : process	-- 12MHz clock
	begin
		CLK<='0';
		wait for 41.6667 ns;
		CLK<='1';
		wait for 41.6667 ns;
	end process;

	
end architecture Behavioral;

-----------------------------[END]------------------------------
