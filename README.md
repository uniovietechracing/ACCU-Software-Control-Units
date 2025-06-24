<h1 align="center"> ACCU-Software-Control-Units </h1>
<p align="left">
   <img src="https://img.shields.io/badge/STATUS-EN%20DESAROLLO-green">
   </p>
   
## Table of Contents
1. [General Info](#general-info)
2. [STM32F4 Flash Memory Stack](#stm32f4-flash-memory-stack)

   
### General Info
***
Applications & Bootloader Software for the Electronic Control Units with STM32F4 listed below:

		➜ Rear Control Unit 1 Shutdown Cicruit Readings Unit.
    
		➜ Rear Control Power Distribution Cobtrol Unit.
    
		➜ Charger Control Unit.
    
		➜ Front Control Unit.
    
		➜ Battery Management System Master Unit.
    	


### STM32F4 Flash Memory Stack
***
		STM32F4 Flash memory Stack

		0x8000000 -> +-------------------------------------------------------------------------+
					 |  0x8000000  |	\
					 |      to     |	 \
					 |  0x8003FFF  |	  |- Contains the bootloader software
					 |    16 KB    |	 /
					 |  Bootloader |	/
		0x8004000 -> +-------------------------------------------------------------------------+
					 |  0x8004000  |	\
					 |      to     |	 \
					 |  0x8007FFF  |	  |- Contains the non volatile memory
					 |    16 KB    |	 /
					 |     NVM     |	/
		0x8008000 -> +-------------------------------------------------------------------------+
					 |             |	\
					 |             |	 \
					 |             |	  |
					 |             |	  |
					 |  0x8008000  |	  |
					 |     to      |	  |
					 |  0x80FFFF   |	  |- Contains the main program memory
					 |    64 KB    |	  |
					 |     	       |	  |
					 |             |	  |
					 |             |	  |
					 |             |	 /
					 |             |	/

