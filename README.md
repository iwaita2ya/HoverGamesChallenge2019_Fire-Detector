# Fire Detector for HoverGames Challenges 2019

This repository holds source code of fire detector developed for HoverGames Challenge 2019.
This program requires mbed LPC1768 dev board with MLX90614, an IR Thermometer module communicates in I2C protocol.

I believe this program works on any mbed board, as long as it has I2C interface.

mbed LPC1768 Dev board  
https://os.mbed.com/platforms/mbed-LPC1768/

Melexis Contact-less Infrared Sensor - MLX90614 5V  
https://www.adafruit.com/product/1748

You can find schematic diagram of this device here.  
https://bit.ly/2uYJf1b

## mbed-cli set up
If you prefer off-line compile environment (like me,) you need to configure 
mbed-cli environment on your system.

This programs is mbed OS2 based. However recently Arm [announced](https://os.mbed.com/blog/entry/Working-towards-the-next-version-of-Mbed/) they will no longer maintain OS2 environment, so this document
might not work in near future. I will keep it up as much as I can.

### fix pip version (may not required anymore)
In the past (or it might still going) `mbed deploy` command failed if you use pip 9.0.3 or higher.

https://github.com/ARMmbed/mbed-cli/issues/657

    $ mbed deploy
    [mbed] Updating library "mbed" to rev #e95d10626187
    [mbed] ERROR: Unknown Error: 'module' object has no attribute 'get_installed_distributions'

In order to avoid trouble, you need to fix pip version till the issue fixed.

    $ python -m pip install pip==9.0.3

### mbed-cli installation
again, you need [pip](https://pip.pypa.io/en/stable/installing/) for mbed-cli installation.

    $ pip install mbed-cli
    $ pip install jsonschema
    $ pip install pyelftools
    
### update gcc-arm-none-eabi

gcc-arm-none-eabi via apt is too old for mbed compile. You need to get latest binary from official site

https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads

https://os.mbed.com/docs/mbed-os/v5.15/tools/manual-installation.html

    e.g.) if you store binary at /home/iwait/opt/gcc-arm-none-eabi/bin
    $ mbed config -G GCC_ARM_PATH "/home/iwait/opt/gcc-arm-none-eabi/bin"

## Project Installation

You first create empty mbed OS2 project

    $ mbed new <project-name> --mbedlib
    $ cd <project-name>
    $ git clone https://github.com/iwaita2ya/HoverGamesChallenge2019_Fire-Detector
    
## compile
At project root directory, run following command.  
If you would like to use other mbed board. replace `lpc1768` with your board.

    $ mbed deploy
    $ mbed compile -m lpc1768 -t gcc_arm

If compile succeeded, a binary file will be gererated under `./BUILD/<board>/gcc_arm/`  
copy the binary into your mbed board, or simply grag & drop on to the device recognized flash drive.
