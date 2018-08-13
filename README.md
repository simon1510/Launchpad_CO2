# Launchpad_CO2

<p>Environment sensor with CO2, temperature and relative humidity measurement.
The Board is based on TI's Launchpad MSP-EXP430FR4133 and includes the sensors SHT21 (temperature and humidity) and SGP30 (CO2).<br>
This code is written for IDE Energia (by TI). If you want to use Arduino for this project, you'll have to add the MSP-libraries.</p>

Following libraries are used, they'll eventually have to be installed manually:
- LCD_Launchpad (credits: Energia):<br>
  https://github.com/energia/Energia/tree/master/hardware/msp430/libraries/LCD_Launchpad
- I2C_SoftwareLibrary (credits: rei-vilo):<br>
  https://github.com/rei-vilo/I2C_Software_Library/tree/master/src

<p>Note:
SGP30 gets corrupted after switching off power supply, so that no communication is possible. You'll need to do a software reset after powering up the system.</p>
