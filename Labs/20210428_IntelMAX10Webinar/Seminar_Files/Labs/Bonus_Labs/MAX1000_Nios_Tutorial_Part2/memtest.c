/**************************************************************************
 * Copyright (c) 2004 Altera Corporation, San Jose, California, USA.      *
 * All rights reserved. All use of this software and documentation is     *
 * subject to the License Agreement located at the end of this file below.*
 *************************************************************************/
/**************************************************************************
 *
 * 
 * Description
 *************** 
 * This is a test program which tests RAM and flash memory. 
 * Modified by Bernd Rautmann to run on Max1000
 * 
 * Requirements
 ****************
 * This is a "Hosted" application. According to the ANSI C standard, hosted 
 * applications can rely on numerous system-services (including properly-
 * initialized device drivers and, in this case, STDOUT).  
 * 
 * When this program is compiled, code is added before main(), so that all 
 * devices are properly-initialized and all system-services (e.g. the <stdio>
 * library) are ready-to-use. In this hosted environment, all standard C 
 * programs will run.
 * 
 * A hosted application (like this example) does not need to concern itself 
 * with initializing devices. As long as it only calls C Standard Library 
 * functions, a hosted application can run "as if on a workstation."
 * 
 * An application runs in a hosted environment if it declares the function 
 * main(), which this application does.
 * 
 * This software example requires a STDOUT component such as a UART or 
 * JTAG UART, a SPI flash component, and a RAM component 
 * Therefore it can run on the following hardware examples:
 * 
  * MAX1000 Board:
 * -  Standard (DMA RAM test will not run)
 * -  Full Featured
 *
 * Note: This example will not run on the Nios II Instruction Set Simulator
 * 
 * Peripherals Exercised by SW
 *******************************
 * The example's purpose is to test RAM and flash, as well as demonstrate the 
 * use of the DMA controller and flash API in NiosII.
 * 
 * The RAM test routine performs the following operations:
 * 1.) Tests the address and data lines for shorts and opens. 
 * 2.) Tests byte and half-word access.
 * 3.) Tests every bit in the memory to store both '1' and '0'. 
 * 4.) Tests DMA access to the memory.
 *
 * IMPORTANT: The RAM test is destructive to the contents of the RAM.  For this
 * reason, you MUST assure that none of the software sections are located in 
 * the RAM being tested.  This requires that code, data, and exception 
 * locations must all be in a memory seperate from the one being tested.
 * These locations can be adjusted in Nios II IDE and SOPC Builder.
 *
 *  
 * The flash tests demonstrate the use of the generic serial flash IP.  
 * This part is modified by Bernd Rautmann to test the onboard W74M64FS SPI Flash. 
 * Because there's no API for this IP, tests are written using operation.c from
 * https://fpgacloud.intel.com/devstore/platform/18.0.0/Standard/generic-serial-flash-interface-intel-fpga-ip-core-reference-design/
 * Due to differences between Winbond Flash and Cypress/Micron, there are some modifications
 * done in operation.c.
 * The following tests are implemented:
 * 
 * - Reading Device ID
 *    In the example, the results of this query are compared to what
 *    is expected, and an error is reported in the event of a mismatch.
 * - simple read write test
 *    Data at address 0 is read, and if it's empty (all F), the former read Flash ID is written
 *    to this address. For verification a read back is done and printed out on the screen 
 * - protect sectors tests    
 *    the status of the protection is checked, and if it's not protected, sector protection is done
 *    for testing the protection a sector erase command is given and a read out of the address 0
 *    shows, whether the Flash ID is erased (hopefully not). The value is shown on the screen.
 *    Next test is to unprotect and erase. Same procedure, but at the end the value should be all F
 * - write and read back a specified number of Bytes
 * - erase chip
 *    This performs a chip erase on the flash device. This takes some time (about 15s).
 * 
 * During the test, status and error information is passed to the user via 
 * printf's.
 * 
 * Software Files
 ******************
 * memtest.c - Main C file that contains all memory testing code in this 
 *             example.
 * operation.c - commands to control SPI Flashes from different vendors
 *
 **************************************************************************/


#include <stdio.h>
#include <alt_types.h>
#include <io.h>
#include <system.h>
#include <string.h>

#include "sys/alt_dma.h"
#include "system.h"
#include "sys/alt_flash.h"
#include "sys/alt_flash_dev.h"

#include "altera_avalon_pio_regs.h"
#include "altera_avalon_spi.h"

/* Mode parameters for Flash Test */
#define TEST 1
#define SHOWMAP 2
#define CFI 3
#define EPCS 4
#define W74M 5
#define QUIT_WITHOUT_TESTING -1

/* One nice define for going to menu entry functions. */
#define MenuCase(letter,proc) case letter:proc; break;

/* Global DMA "transaction finished" flag */
#ifdef DMA_NAME  
static volatile int rx_done = 0;
#endif /* DMA_NAME */  

int count, value, flashaddress;

/******************************************************************
*  Function: MenuHeader
*
*  Purpose: Prints the menu header.
*
******************************************************************/
static void MenuHeader(void)
{
  printf("\n\n");
  printf("             <---->   Nios II Memory Test.   <---->\n");
  printf("This software example tests the memory in your system to assure it\n");
  printf("is working properly.  This test is destructive to the contents of\n");
  printf("the memory it tests. Assure the memory being tested does not contain\n");
  printf("the executable or data sections of this code or the exception address\n");
  printf("of the system.\n");
}

/******************************************************************
*  Function: MenuBegin
*
*  Purpose: Prints the top portion of the menu.
*
******************************************************************/
static void MenuBegin( char *title )
{
  printf("\n\n");
  printf("----------------------------------\n");
  printf("%s\n",title);
  printf("----------------------------------\n");
}

/******************************************************************
*  Function: MenuItem
*
*  Purpose: Prints selection items in the menu, enumerated by the 
*           specified letter.
*
******************************************************************/
static void MenuItem( char letter, char *name )
{
  printf("     %c:  %s\n" ,letter, name);
}

/******************************************************************
*  Function: GetInputString
*
*  Purpose: Parses an input string for the character '\n'.  Then
*           returns the string, minus any '\r' characters it 
*           encounters.
*
******************************************************************/
void GetInputString( char* entry, int size, FILE * stream )
{
  int i;
  int ch = 0;
  
  for(i = 0; (ch != '\n') && (i < size); )
  {
    if( (ch = getc(stream)) != '\r')
    {
      entry[i] = ch;
      i++;
    }
  }
}

/******************************************************************
*  Function: MenuEnd
*
*  Purpose: Prints the end of the menu, then captures and returns
*           the user's selection.
*
******************************************************************/
static int MenuEnd( char lowLetter, char highLetter )
{
  static char entry[4];
  static char ch;

  printf("     q:  Exit\n");
  printf("----------------------------------\n");
  printf("\nSelect Choice (%c-%c): [Followed by <enter>]",lowLetter,highLetter);

  GetInputString( entry, sizeof(entry), stdin );
  if(sscanf(entry, "%c\n", &ch))
  {
    if( ch >= 'A' && ch <= 'Z' )
      ch += 'a' - 'A';
    if( ch == 27 )
      ch = 'q';
  }
  return ch;
}

/******************************************************************
*  Function: MemGetAddressRange
*
*  Purpose: Gathers a range of memory from the user.
*
******************************************************************/
static int MemGetAddressRange(int* base_address, int* end_address)
{

  char line[12];

  while(1)
  {
    /* Get the base address */
    printf("Base address to start memory test: (i.e. 0x800000)\n");
    printf(">");

    GetInputString( line, sizeof(line), stdin );
   
    /* Check the format to make sure it was entered as hex */
    if(sscanf(line, "0x%X", base_address) != 1)
    {
      printf("%s\n", line);
      printf(" -ERROR: Invalid base address entered.  Address must be in the form '0x800000'\n\n");
      continue;
    }
    
    /* Get the end address */
    printf("End Address:\n");
    printf(">");

    GetInputString( line, sizeof(line), stdin );
    
    /* Check the format to make sure it was entered as hex */
    if(sscanf(line, "0x%X", end_address) != 1)
    {
      printf(" -ERROR: Invalid end address entered.  Address must be in the form '0x8FFFFF'\n\n");
      continue;
    }
    
    /* Make sure end address is greater than base address. */
    if (*end_address <= *base_address)
    {
      printf(" -ERROR: End address must be greater than the start address\n\n");

      continue;
    }
    break;
  }

  return(0);
}

/******************************************************************
*  Function: MemTestDataBus
*
*  Purpose: Tests that the data bus is connected with no 
*           stuck-at's, shorts, or open circuits.
*
******************************************************************/
static int MemTestDataBus(unsigned int address)
{
  unsigned int pattern;
  unsigned int ret_code = 0x0;

  /* Perform a walking 1's test at the given address. */
  for (pattern = 1; pattern != 0; pattern <<= 1)
  {
    /* Write the test pattern. */
    IOWR_32DIRECT(address, 0, pattern);

    /* Read it back (immediately is okay for this test). */
    if (IORD_32DIRECT(address, 0) != pattern)
    {
      ret_code = pattern;
      break;
    }
  }
  return ret_code;
}


/******************************************************************
*  Function: MemTestAddressBus
*
*  Purpose: Tests that the address bus is connected with no 
*           stuck-at's, shorts, or open circuits.
*
******************************************************************/
static int MemTestAddressBus(unsigned int memory_base, unsigned int nBytes)
{
  unsigned int address_mask = (nBytes - 1);
  unsigned int offset;
  unsigned int test_offset;

  unsigned int pattern     = 0xAAAAAAAA;
  unsigned int antipattern  = 0x55555555;

  unsigned int ret_code = 0x0;

  /* Write the default pattern at each of the power-of-two offsets. */
  for (offset = sizeof(unsigned int); (offset & address_mask) != 0; offset <<= 1)
  {
    IOWR_32DIRECT(memory_base, offset, pattern);
  }

  /* Check for address bits stuck high. */
  test_offset = 0;
  IOWR_32DIRECT(memory_base, test_offset, antipattern);
  for (offset = sizeof(unsigned int); (offset & address_mask) != 0; offset <<= 1)
  {
     if (IORD_32DIRECT(memory_base, offset) != pattern)
     {
        ret_code = (memory_base+offset);
        break;
     }
  }

  /* Check for address bits stuck low or shorted. */
  IOWR_32DIRECT(memory_base, test_offset, pattern);
  for (test_offset = sizeof(unsigned int); (test_offset & address_mask) != 0; test_offset <<= 1)
  {
    if (!ret_code)
    {
      IOWR_32DIRECT(memory_base, test_offset, antipattern);
      for (offset = sizeof(unsigned int); (offset & address_mask) != 0; offset <<= 1)
      {
        if ((IORD_32DIRECT(memory_base, offset) != pattern) && (offset != test_offset))
        {
          ret_code = (memory_base + test_offset);
          break;
        }
      }
      IOWR_32DIRECT(memory_base, test_offset, pattern);
    }
  }

  return ret_code;
}


/******************************************************************
*  Function: MemTest8_16BitAccess
*
*  Purpose: Tests that the memory at the specified base address
*           can be read and written in both byte and half-word 
*           modes.
*
******************************************************************/
static int MemTest8_16BitAccess(unsigned int memory_base)
{
  int ret_code = 0x0;

  /* Write 4 bytes */
  IOWR_8DIRECT(memory_base, 0, 0x0A);
  IOWR_8DIRECT(memory_base, 1, 0x05);
  IOWR_8DIRECT(memory_base, 2, 0xA0);
  IOWR_8DIRECT(memory_base, 3, 0x50);

  /* Read it back as one word */
  if(IORD_32DIRECT(memory_base, 0) != 0x50A0050A)
  {
    ret_code = memory_base;
  }

  /* Read it back as two half-words */
  if (!ret_code)
  {
    if ((IORD_16DIRECT(memory_base, 2) != 0x50A0) ||
        (IORD_16DIRECT(memory_base, 0) != 0x050A))
    {
      ret_code = memory_base;
    }
  }

  /* Read it back as 4 bytes */
  if (!ret_code)
  {
    if ((IORD_8DIRECT(memory_base, 3) != 0x50) ||
        (IORD_8DIRECT(memory_base, 2) != 0xA0) ||
        (IORD_8DIRECT(memory_base, 1) != 0x05) ||
        (IORD_8DIRECT(memory_base, 0) != 0x0A))
    {
    ret_code = memory_base;
    }
  }

  /* Write 2 half-words */
  if (!ret_code)
  {
    IOWR_16DIRECT(memory_base, 0, 0x50A0);
    IOWR_16DIRECT(memory_base, 2, 0x050A);

    /* Read it back as one word */
    if(IORD_32DIRECT(memory_base, 0) != 0x050A50A0)
    {
      ret_code = memory_base;
    }
  }

  /* Read it back as two half-words */
  if (!ret_code)
  {
    if ((IORD_16DIRECT(memory_base, 2) != 0x050A) ||
        (IORD_16DIRECT(memory_base, 0) != 0x50A0))
    {
      ret_code = memory_base;
    }
  }

  /* Read it back as 4 bytes */
  if (!ret_code)
  {
    if ((IORD_8DIRECT(memory_base, 3) != 0x05) ||
        (IORD_8DIRECT(memory_base, 2) != 0x0A) ||
        (IORD_8DIRECT(memory_base, 1) != 0x50) ||
        (IORD_8DIRECT(memory_base, 0) != 0xA0))
    {
      ret_code = memory_base;
    }
  }

  return(ret_code);
}


/******************************************************************
*  Function: MemTestDevice
*
*  Purpose: Tests that every bit in the memory device within the 
*           specified address range can store both a '1' and a '0'.
*
******************************************************************/
static int MemTestDevice(unsigned int memory_base, unsigned int nBytes)
{
  unsigned int offset;
  unsigned int pattern;
  unsigned int antipattern;
  unsigned int ret_code = 0x0;

  /* Fill memory with a known pattern. */
  for (pattern = 1, offset = 0; offset < nBytes; pattern++, offset+=4)
  {
    IOWR_32DIRECT(memory_base, offset, pattern);
  }

  printf(" .");

  /* Check each location and invert it for the second pass. */
  for (pattern = 1, offset = 0; offset < nBytes; pattern++, offset+=4)
  {
    if (IORD_32DIRECT(memory_base, offset) != pattern)
    {
      ret_code = (memory_base + offset);
      break;
    }
    antipattern = ~pattern;
    IOWR_32DIRECT(memory_base, offset, antipattern);
  }

  printf(" .");

  /* Check each location for the inverted pattern and zero it. */
  for (pattern = 1, offset = 0; offset < nBytes; pattern++, offset+=4)
  {
    antipattern = ~pattern;
    if (IORD_32DIRECT(memory_base, offset) != antipattern)
    {
      ret_code = (memory_base + offset);
      break;
    }
    IOWR_32DIRECT(memory_base, offset, 0x0);
  }
  return ret_code;
}

/******************************************************************
*  Function: dma_done
*
*  Purpose: Called when a DMA recieve transaction is complete.
*           Increments rx_done to signal to the main program that
*           the transaction is done.
*
******************************************************************/
#ifdef DMA_NAME  
static void dma_done (void* handle, void* data)
{
  rx_done++;
}
#endif /* DMA_NAME */  

/******************************************************************
*  Function: MemDMATest
*
*  Purpose: Tests every bit in the memory device within the 
*  specified address range using DMA.  The DMA controller provides 
*  a more rigourous test of the memory since it performs back-to-
*  back memory accesses at full system speed.
*
******************************************************************/
#ifdef DMA_NAME  
static int MemDMATest(unsigned int memory_base, unsigned int nBytes)
{
  int rc;
  int ret_code = 0;
  int pattern, offset;
  alt_dma_txchan txchan;
  alt_dma_rxchan rxchan;
  void* data_written;
  void* data_read;
  
  /* Get a couple buffers for the test */
  data_written = (void*)alt_uncached_malloc(0x1000);
  data_read = (void*)alt_uncached_malloc(0x1000);
  
  
  /* Fill write buffer with known values */
  for (pattern = 1, offset = 0; offset < 0x1000; pattern++, offset+=4)
  {
    IOWR_32DIRECT((int)data_written, offset, pattern);
  }

  /* Create the transmit channel */
  if ((txchan = alt_dma_txchan_open("/dev/dma")) == NULL)
  {
    printf ("Failed to open transmit channel\n");
    exit (1);
  }
  
  /* Create the receive channel */
  if ((rxchan = alt_dma_rxchan_open("/dev/dma")) == NULL)
  {
    printf ("Failed to open receive channel\n");
    exit (1);
  }
  
  for(offset = memory_base; offset < (memory_base + nBytes); offset += 0x1000)
  {
    /* Use DMA to transfer from write buffer to memory under test */
    /* Post the transmit request */
    if ((rc = alt_dma_txchan_send (txchan, data_written, 0x1000, NULL, NULL)) < 0)
    {
      printf ("Failed to post transmit request, reason = %i\n", rc);
      exit (1);
    }

    /* Post the receive request */
    if ((rc = alt_dma_rxchan_prepare (rxchan, (void*)offset, 0x1000, dma_done, NULL)) < 0)
    {
      printf ("Failed to post read request, reason = %i\n", rc);
      exit (1);
    }
  
    /* Wait for transfer to complete */
    while (!rx_done);
    rx_done = 0;
    
    /* Clear the read buffer before we fill it */
    memset(data_read, 0, 0x1000);
    
    /* Use DMA to read data back into read buffer from memory under test */
    /* Post the transmit request */
    if ((rc = alt_dma_txchan_send (txchan, (void*)offset, 0x1000, NULL, NULL)) < 0)
    {
      printf ("Failed to post transmit request, reason = %i\n", rc);
      exit (1);
    }

    /* Post the receive request */
    if ((rc = alt_dma_rxchan_prepare (rxchan, data_read, 0x1000, dma_done, NULL)) < 0)
    {
      printf ("Failed to post read request, reason = %i\n", rc);
      exit (1);
    }

    /* Wait for transfer to complete */
    while (!rx_done);
    rx_done = 0;
    
    if (memcmp(data_written, data_read, 0x1000))
    {
      ret_code = offset;
      break;
    }
  }
  alt_uncached_free(data_written);
  alt_uncached_free(data_read);
  return ret_code;
}
#endif /* DMA_NAME */  


/******************************************************************
*  Function: TestRam
*
*  Purpose: Performs a full-test on the RAM specified.  The tests
*           run are:
*             - MemTestDataBus
*             - MemTestAddressBus
*             - MemTest8_16BitAccess
*             - MemTestDevice
*             - MemDMATest
*
******************************************************************/
static void TestRam(void)
{
  
  int memory_base, memory_end, memory_size;
  int ret_code = 0x0;

  /* Find out what range of memory we are testing */
  MemGetAddressRange(&memory_base, &memory_end);
  memory_size = (memory_end - memory_base);

  printf("\n");
  printf("Testing RAM from 0x%X to 0x%X\n", memory_base, (memory_base + memory_size));

  /* Test Data Bus. */
  ret_code = MemTestDataBus(memory_base);

  if (ret_code)
   printf(" -Data bus test failed at bit 0x%X", (int)ret_code);
  else
    printf(" -Data bus test passed\n");

  /* Test Address Bus. */
  if (!ret_code)
  {
    ret_code  = MemTestAddressBus(memory_base, memory_size);
    if  (ret_code)
      printf(" -Address bus test failed at address 0x%X", (int)ret_code);
    else
      printf(" -Address bus test passed\n");
  }

  /* Test byte and half-word access. */
  if (!ret_code)
  {
    ret_code = MemTest8_16BitAccess(memory_base);
    if  (ret_code)
      printf(" -Byte and half-word access test failed at address 0x%X", (int)ret_code);
    else
      printf(" -Byte and half-word access test passed\n");
  }

  /* Test that each bit in the device can store both 1 and 0. */
  if (!ret_code)
  {
    printf(" -Testing each bit in memory device.");
    ret_code = MemTestDevice(memory_base, memory_size);
    if  (ret_code)
      printf("  failed at address 0x%X", (int)ret_code);
    else
      printf("  passed\n");
  }
  
  /* Test DMA access to the RAM if DMA exists */
#ifdef DMA_NAME  
  if (!ret_code)
  {
    printf(" -Testing memory using DMA.");
    ret_code = MemDMATest(memory_base, memory_size);
    if  (ret_code)
      printf("  failed at address 0x%X", (int)ret_code);
    else
      printf("  passed\n");
  }
#endif /* DMA_NAME */
      
  if (!ret_code)
    printf("Memory at 0x%X Okay\n", memory_base);
}





/******************************************************************
*  Function: GetFlashName
*
*  Purpose: Gets the name of the flash to test from the user
*           Defaults to "/dev/ext_flash", the name of the flash
*           component in the Nios II example designs.
* 
******************************************************************/
static int GetFlashName(char line[30], int flash_type)
{

  char ch = 0x0;
  int i;
  int winbond_flash = 0x1740ef;

  if (flash_type == CFI)
  { 
    printf("\nEnter the name of the CFI flash device to be opened,\n");
    printf("or just press <enter> to open \"/dev/ext_flash\"\n");
    printf(">");
  }
  else if (flash_type == EPCS)
  {
    printf("\nEnter the name of the EPCS flash device to be opened,\n");
    printf("or just press <enter> to open \"/dev/epcs_controller\"\n");
    printf(">");
  }
  else if (flash_type == W74M)
  {
    printf("\nOnboard flash W74M.. device will be tested.\n");
 //   printf("or just press <enter> to open \"/dev/epcs_controller\"\n");
    printf(">");
  	if (read_device_id()== winbond_flash)
  	{
 		printf("Flash Device found: Winbond flash W74M64FS\n");
		printf("Device ID: %x\n",read_device_id());
		int mask_wip = 0x00000001; //set mask to check Write in Progress (WIP) bit to determine device busy or ready
		int block_protect_cr = 0x00000020; //set BP starts at bottom
		int block_protect_sr = 0x0000001c; //set BP2-0 in status register to protect all sectors
		int mask_wel = 0x00000002; //set mask to check write enable latch
		int mask_sr = 0x000000e3; // set mask to check bit 4:2 of status register
		int mask_e_err = 0x00000020; //set mask to check erase error bit
		int empty_data = 0xFFFFFFFF;

		printf("Reading data at address 0...\n");
		printf("Memory content at address 0: %x\n",read_memory_3byte());
		if (read_memory_3byte()== empty_data)	// if memory is empty, write flash-ID to address 0
		{

		//WRITING DATA

		printf("Address 0 not containing data...\n");
		printf("Writing flash-ID to address 0...\n");
		flashaddress = 0x00000000;
		value = winbond_flash;
		write_enable();

		write_memory_3byte();

		printf("Reading data at address 0...\n");
		printf("Memory content at address 0: %x\n",read_memory_3byte());
		}
		if ((read_status_register()| mask_sr) == mask_sr)
		{

			printf("All sectors in this configuration device is not protected\n");
			printf("Now performing sector protection...\n");
			write_enable();
			write_register_for_sector_protect_cypress(); //sector protect all sector(BP2:BP1:BP0=1:1:1)

			printf("Write register for sector protect in progress...");
			while (read_status_register() & mask_wip == mask_wip)
			{
				printf(".");
				usleep(1000);
			}
			printf("\n");

			//Check Status Register and Configuration Register to see whether both of them set to perform sector protect

			if ( ((read_status_register()& block_protect_sr) != block_protect_sr) && ((read_config_register() & block_protect_cr)!= block_protect_cr))
			{
				printf("Setor protection failed due to error in setting status and configuration register");
				printf("Status Register: %08x\n",read_status_register());
				printf("Configuration Register: %08x\n",read_config_register());
				printf("Check datasheet to find out");
				return 0;
			}
		}

		printf("All sectors in this configuration device is now successfully protected\n");
		//PERFORM SECTOR ERASE ON PROTECTED SECTOR

		write_enable();

		if ((read_status_register()& mask_wel) != mask_wel)  //check if write enable latch is set
		{
			printf("Sector erase cannot be executed as write enable latch do not set\n");
			return 0;
		}

		printf("Trying to erase sector 0...\n");

		erase_sector_winbond();

//		if ((read_status_register()& mask_e_err)!= mask_e_err)
//		{
//			printf("ERASE ERROR do not occur. Check status register for more details.\n");
//			return 0;
//		}

//		printf("ERASE ERROR as sector is protected!\n");
//		clear_status_register(); //clear erase error bit

		//UNPROTECT ALL SECTORS IN CONFIGURATION DEVICE
		printf("Reading data at address 0...\n");
		printf("Memory content at address 0: %x\n",read_memory_3byte());
		printf("Now perform sector unprotect...\n");

		write_enable();

		if ((read_status_register()& mask_wel) != mask_wel)
		{ //check if write enable latch is set
			printf("Sector unprotect cannot be executed as write enable latch do not set\n");
			return 0;
		}

		write_register_for_sector_unprotect_cypress();


		while ((read_status_register() & mask_wip) == mask_wip)
		{
			usleep(1000);
		}

		if (read_status_register()| mask_sr != mask_sr)
		{
			printf("Sector unprotect not successful! :(\n");
			return 0;
		}

		printf("Sector unprotect successfully! :)\n");


		//READ AND WRITE DATA

//		printf("Reading data at address 0...\n");
//		printf("Memory content at address 0: %x\n",read_memory_3byte());
//		int empty_data = 0xffffffff;

		if (read_memory_3byte()!= empty_data)
		{

			//PERFORM SECTOR ERASE to clear sector 0

			write_enable();

			if ((read_status_register()& mask_wel) != mask_wel)
			{ //check if write enable latch is set
				printf("Sector erase cannot be executed as write enable latch do not set\n");
			}

			printf("Trying to erase sector 0...\n");
			erase_sector_winbond();
			printf("sector erase in progress...");
			while (read_status_register() & mask_wip == mask_wip)
			{
				printf(".");
				usleep(1000);
			}
			printf("\n");
//			erase_chip();
//			while (read_status_register() & mask_wip == mask_wip){
//				printf(".");
//				usleep(100000);
//			}

		}

		printf("Reading data at address 0...\n");
		printf("Memory content at address 0: %x\n",read_memory_3byte());

		if (read_memory_3byte()== empty_data)
		{

		//WRITING DATA

		printf("Address 0 not containing data...\n");
		printf("Writing data to address 0..15\n");
		flashaddress = 0x00000000;
		value = 0x00000100;
		while(flashaddress < 0x00000010)
		  {
		write_memory_3byte();

			while ((read_status_register() & mask_wip) == mask_wip)
			{
				printf("Write data in progress...\n");
				usleep(10000000);
			}
		flashaddress = flashaddress + 1;
		value = value + 1;
		  }
		}
		//READ BACK DATA
		flashaddress = 0x00000000;
		value = 0x00000100;
		while(flashaddress < 0x00000010)
		  {
//		    printf("Read back data from address %i...\n", flashaddress);
 		    int data1 = value;
		    if(read_memory_3byte() != data1)
		    {
			  printf("Current memory in address 0: %x\n",read_memory_3byte());
			  printf("Something is wrong...");
			  return 0;
		    }


		   printf("Current memory in address %i: %x\n",flashaddress,read_memory_3byte());
		  flashaddress = flashaddress + 1;
		  value = value + 1;
		  }
		printf("Read data match with data written. Write memory successful.\n");

		printf("Trying to erase chip\n");
		write_enable();

		erase_chip();
		while (read_status_register() & mask_wip == mask_wip)
		{
			printf(".");
			usleep(100000);
		}

		printf("\nReading data at address 0...\n");
		printf("Memory content at address 0: %x\n",read_memory_3byte());
		if (read_memory_3byte()== empty_data)
		printf("Chip erase successful.\n");
		return 0;
	}
	else
		{
				printf("No W74M64 Flash memory found.\n");
				printf("Something is wrong...");
				return 0;
		}

    printf("\nPress enter to continue...\n");

    while( (( ch = getc(stdin)) != '\n' ) && ( ch != EOF ));


  return (ch);
  }
  return 0;
}


/******************************************************************
*  Function: TestFlash
*
*  Purpose: Opens the specified flash device.  If the mode
*           parameter is TEST, the function finds an erased
*           block, then tests it.  If the mode parameter is
*           SHOWMAP, the function lists all blocks in the flash and
*           indicates which ones are erased.  The flash is closed
*           at the end of the function.
*
******************************************************************/
static void TestFlash(int mode, int flash_type)
{
  int ret_code = 0x0;
//  char entry[4];
  char flashname[30];

  ret_code = GetFlashName(flashname, flash_type);

}

/******************************************************************
*  Function: Spirit Level with onboard LEDs
*
*  Purpose: implements a spirit level logic
*
******************************************************************/
void init_gyro();

int spiritlevel()
{
	unsigned char wdata[1];
	unsigned char rdata[2];
	unsigned char led_out = 0x18;
	  char ch = 0x0;

	alt_8 x_value = 0;
	alt_8 x_value_1 = 0;
	alt_8 x_value_2 = 0;
	alt_8 x_value_3 = 0;
	alt_8 x_value_4 = 0;
	alt_8 x_value_5 = 0;

	init_gyro();

	wdata[0]= 0x8F;	 //read who am I
	alt_avalon_spi_command (LS3DH_BASE, 0, 1, wdata, 1, rdata, 0);
//    if (rdata[0] = 0x33)

	//	wdata[0]= 0x87;	 //read status register
	wdata[0]= 0xEA;       // read x-axis register


	while (x_value != -50)
	{
		// read x-axis data from gyro
		alt_avalon_spi_command (LS3DH_BASE, 0, 1, wdata, 2, rdata, 0);

		// calculate average
		x_value_5 = x_value_4;
		x_value_4 = x_value_3;
		x_value_3 = x_value_2;
		x_value_2 = x_value_1;
		x_value_1 = rdata[1];

		x_value = (x_value_1 + x_value_2 + x_value_3 + x_value_4 + x_value_5) / 5;


		// determine LED setting according to x-axis value
		if (x_value > -8 && x_value < 8)
			led_out = 0x18;
		if (x_value >= 8 && x_value < 15)
			led_out = 0x08;
		if (x_value >= 15 && x_value < 25)
			led_out = 0x04;
		if (x_value >= 25 && x_value < 35)
			led_out = 0x02;
		if (x_value >= 35)
			led_out = 0x01;
		if (x_value > -15 && x_value <= -8)
			led_out = 0x10;
		if (x_value > -25 && x_value <= -15)
			led_out = 0x20;
		if (x_value > -35 && x_value <= -25)
			led_out = 0x40;
		if (x_value <= -35)
			led_out = 0x80;

		// set LED
		IOWR_ALTERA_AVALON_PIO_DATA(LED_PIO_BASE, led_out);

		// wait 10ms
		usleep(10000);
//		if( (( ch = getc(stdin)) != '\n' ) && ( ch != EOF ))
//		return 0;
	}
	return 0;
}

void init_gyro()
{
	unsigned char wdata[3];
	unsigned char rdata[1];

//	wdata[0]=0x1E;		// write ctrl register
//	wdata[1]=0x10;		// initialize
//	wdata[2]=0x13;		// 2g, 100Hz

//	alt_avalon_spi_command (SPI_0_BASE, 0, 2, wdata, 0, rdata, 0);

	wdata[0]=0x20;		// write register
	wdata[1]=0x97;		//
//	wdata[2]=0x02;		// start measurement

	alt_avalon_spi_command (LS3DH_BASE, 0, 2, wdata, 0, rdata, 0);
}


/******************************************************************
*  Function: TopMenu
*
*  Purpose: Generates the top level menu.
* 
******************************************************************/
static int TopMenu( void )
{
  char ch;

  /* Print the top-level menu to stdout */
  while (1)
  {
    MenuBegin("      Memory Test Main Menu");
    MenuItem( 'a', "Test RAM" );
    MenuItem( 'b', "Test Flash");
    MenuItem( 'c', "Spirit Level");
#ifdef EPCS_CONTROLLER_NAME    
    MenuItem( 'c', "Test EPCS Serial Flash");
    ch = MenuEnd( 'a', 'c' );    
#else
    ch = MenuEnd( 'a', 'b' );
#endif /* EPCS_CONTROLLER_NAME */

    switch(ch)
    {
      MenuCase('a',TestRam());
      MenuCase('b',TestFlash(TEST, W74M));
      MenuCase('c',spiritlevel());       /* spirit level */
//      MenuCase('m',TestFlash(SHOWMAP, CFI)); /* hidden option */
#ifdef EPCS_CONTROLLER_NAME    
      MenuCase('c',TestFlash(TEST, EPCS));
      MenuCase('f',FlashErase(EPCS));       /* hidden option */
      MenuCase('s',TestFlash(SHOWMAP, EPCS)); /* hidden option */
#endif /* EPCS_CONTROLLER_NAME */
      case 'q':	break;
      default:	printf("\n -ERROR: %c is an invalid entry.  Please try again\n", ch); break;
    }
    if (ch == 'q')
      break;
    printf("\nPress enter to continue...\n");
    while( (( ch = getc(stdin)) != '\n' ) && ( ch != EOF ));

  }
  return (ch);
}


/******************************************************************
*  Function: main
*
*  Purpose: Continually prints the menu and performs the actions
*           requested by the user.
* 
******************************************************************/
int main(void)
{

  int ch;

  /* Print the Header */
  MenuHeader();
  /* Print the menu and do what the user requests, until they hit 'q' */
  while (1)
  {
    ch = TopMenu();
    if (ch == 'q')
    {
      printf( "\nExiting from Memory Test.\n");
      break;
    }
  }
  return (0);
}


/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2004 Altera Corporation, San Jose, California, USA.           *
* All rights reserved.                                                        *
*                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a     *
* copy of this software and associated documentation files (the "Software"),  *
* to deal in the Software without restriction, including without limitation   *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
* and/or sell copies of the Software, and to permit persons to whom the       *
* Software is furnished to do so, subject to the following conditions:        *
*                                                                             *
* The above copyright notice and this permission notice shall be included in  *
* all copies or substantial portions of the Software.                         *
*                                                                             *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
* DEALINGS IN THE SOFTWARE.                                                   *
*                                                                             *
* This agreement shall be governed in all respects by the laws of the State   *
* of California and by the laws of the United States of America.              *
* Altera does not recommend, suggest or require that this reference design    *
* file be used in conjunction or combination with any other product.          *
******************************************************************************/
