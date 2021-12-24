----------------------------------------------------------------
--Design Name:		RTL Simulation Lab
--Module Name:		Top module
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
--Description:		A simple binary counter for the RTL Simulation
--						lab. After the modification, this code will 
--						realize a simple running LED application.
--Note:				
--Revision history:
----------------------------------------------------------------


---------------------------[LIBRARY]----------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

---------------------------[ENTITY]-----------------------------

entity rtl_simulation_lab is
port(	CLK	: in std_logic;
		LED	: out std_logic_vector(7 downto 0));
end entity rtl_simulation_lab;

------------------------[ARCHITECTURE]--------------------------

architecture Behavioral of rtl_simulation_lab is


signal iCLK	: std_logic;
signal iCNT	: std_logic_vector(7 downto 0) := (others => '0');


component PLL
port(	inclk0	: in std_logic;
		c0			: out std_logic);
end component;


begin

	clock : PLL
	port map(inclk0 => CLK,
				c0 => iCLK);
			
			
	counter : process(iCLK)
	begin
		if(iCLK = '1' and iCLK'event) then
			iCNT <= iCNT + 1;
		end if;
	end process;

	
	LED <= iCNT;

	
end architecture Behavioral;

-----------------------------[END]------------------------------
