/**
 * Originally this program has been created by Ronald Lee (Thanks!)
 * https://os.mbed.com/users/ronaldhklee/code/MLX90614/
 *
 * I made a few changes to make it work with LPC1768
 */

#include "mbed.h"
#include "mlx90614.h"

I2C MLX90614_I2C(p9, p10);              // SDA, SCL
Serial MLX90614_serial(USBTX, USBRX);   // TX, RX

uint8_t MLX90614_SA_Scan(void)
{
    int nofind;
    int addr = 0;
    char cmd = 0;

    for (int cnt = 1; cnt < 128; cnt++)
    {
        MLX90614_I2C.start();
        nofind = MLX90614_I2C.write((cnt << 1), &cmd, 1);
        MLX90614_I2C.stop();
        if (!nofind) { addr = cnt; break; }
    }
    return addr;
}

uint8_t MLX90614_SA_Read(uint8_t curAddr)
{
    char cmd[3] = { 0 };
    uint8_t crcbuf[6] = { 0 };
    uint8_t writeAddr, readAddr;

    writeAddr = (curAddr << 1) + 0;
    readAddr = (curAddr << 1) + 1;

    crcbuf[0] = writeAddr;       //write Address
    crcbuf[1] = cmd[0] = OPCODE_EEPROM | EEPROM_SMBUSADDR;
    crcbuf[2] = readAddr;       //read Address
    MLX90614_I2C.write(writeAddr, cmd, 1, true);
    MLX90614_I2C.read(readAddr, cmd, 3);
    for (uint8_t cnt = 0; cnt < 3; cnt++) crcbuf[cnt+3] = (uint8_t)cmd[cnt];

    if (MLX90614_CRC8(crcbuf, 6) == (uint8_t)0x00) return cmd[0];
    else return ERROR_ADDR_CRC8;
}

int MLX90614_EEPROM_Erase(int8_t curAddr, uint8_t eepromcmd)
{
    char cmd[4] = { 0 };
    uint8_t crcbuf[4] = { 0 };
    uint8_t writeAddr;

    writeAddr = (curAddr << 1) + 0;

    crcbuf[0] = writeAddr;       //write Address
    crcbuf[1] = cmd[0] = OPCODE_EEPROM | eepromcmd;
    crcbuf[2] = cmd[1] = 0x00;
    crcbuf[3] = cmd[2] = 0x00;
    cmd[3] = MLX90614_CRC8(crcbuf, 4);

    return MLX90614_I2C.write(writeAddr, cmd, 4, true);
}

int MLX90614_EEPROM_Write(int8_t curAddr, uint8_t eepromcmd, uint16_t data)
{
    char cmd[4] = { 0 };
    uint8_t crcbuf[4] = { 0 };
    uint8_t writeAddr;

    MLX90614_EEPROM_Erase(curAddr, eepromcmd);
    wait(0.1);

    writeAddr = (curAddr << 1) + 0;

    crcbuf[0] = writeAddr;       //write Address
    crcbuf[1] = cmd[0] = OPCODE_EEPROM | eepromcmd;
    crcbuf[2] = cmd[1] = (uint8_t)(data & 0x00ff);
    crcbuf[3] = cmd[2] = (uint8_t)((data & 0xff00) >> 8);
    cmd[3] = MLX90614_CRC8(crcbuf, 4);

    MLX90614_I2C.write(writeAddr, cmd, 4, true);
    wait(0.1);

    return 1;
}

uint16_t MLX90614_EEPROM_Read(uint8_t curAddr, uint8_t eepromcmd)
{
    char cmd[3] = { 0 };
    uint8_t crcbuf[6] = { 0 };
    uint8_t writeAddr, readAddr;

    writeAddr = (curAddr << 1) + 0;
    readAddr = (curAddr << 1) + 1;

    crcbuf[0] = writeAddr;       //write Address
    crcbuf[1] = cmd[0] = OPCODE_EEPROM | eepromcmd;
    crcbuf[2] = readAddr;       //read Address
    MLX90614_I2C.write(writeAddr, cmd, 1, true);
    MLX90614_I2C.read(readAddr, cmd, 3);
    for (uint8_t cnt = 0; cnt < 3; cnt++) crcbuf[cnt+3] = (uint8_t)cmd[cnt];

    if (MLX90614_CRC8(crcbuf, 6) == (uint8_t)0x00) return (cmd[1]<<8)|cmd[0];
    else return ERROR_ADDR_CRC8;
}

float getTempC_Ambi(uint8_t SAddr)
{
    char cmd[3] = { 0 };
    uint8_t crcbuf[6] = { 0 };
    uint8_t writeAddr, readAddr;

    writeAddr = (SAddr << 1) + 0;
    readAddr = (SAddr << 1) + 1;

    crcbuf[0] = writeAddr;       //write Address
    crcbuf[1] = cmd[0] = OPCODE_RAM | RAM_TA;
    crcbuf[2] = readAddr;       //read Address

    MLX90614_I2C.write(writeAddr, cmd, 1, true);
    MLX90614_I2C.read(readAddr, cmd, 3);

    for (uint8_t cnt = 0; cnt < 3; cnt++) crcbuf[cnt+3] = (uint8_t)cmd[cnt];

    if (MLX90614_CRC8(crcbuf, 6) == (uint8_t)0x00)
        return ((0.02*static_cast<float>((cmd[1]<<8)|cmd[0]))   //Kelvin
                -273.15                                        //C
                -0.18);                                        //(3.3V-3)*0.6/V
    else return ERROR_TEMP_CRC8;                                     //Error
}

float getTempC_Obj1(uint8_t SAddr)
{
    char cmd[3] = { 0 };
    uint8_t crcbuf[6] = { 0 };
    uint8_t writeAddr, readAddr;

    writeAddr = (SAddr << 1) + 0;
    readAddr = (SAddr << 1) + 1;

    crcbuf[0] = writeAddr;       //write Address
    crcbuf[1] = cmd[0] = OPCODE_RAM | RAM_TOBJ1;
    crcbuf[2] = readAddr;       //read Address

    MLX90614_I2C.write(writeAddr, cmd, 1, true);
    MLX90614_I2C.read(readAddr, cmd, 3);

    for (uint8_t cnt = 0; cnt < 3; cnt++) crcbuf[cnt+3] = (uint8_t)cmd[cnt];

    if (MLX90614_CRC8(crcbuf, 6) == (uint8_t)0x00)
        return ((0.02*static_cast<float>((cmd[1]<<8)|cmd[0]))   //Kelvin
                -273.15                                         //C
                -0.18);                                         //(3.3V-3)*0.6/V
    else return ERROR_TEMP_CRC8;                                //Error
}

float getTempC_Obj2(uint8_t SAddr)
{
    char cmd[3] = { 0 };
    uint8_t crcbuf[6] = { 0 };
    uint8_t writeAddr, readAddr;

    writeAddr = (SAddr << 1) + 0;
    readAddr = (SAddr << 1) + 1;

    crcbuf[0] = writeAddr;       //write Address
    crcbuf[1] = cmd[0] = OPCODE_RAM | RAM_TOBJ2;
    crcbuf[2] = readAddr;       //read Address

    MLX90614_I2C.write(writeAddr, cmd, 1, true);
    MLX90614_I2C.read(readAddr, cmd, 3);

    for (uint8_t cnt = 0; cnt < 3; cnt++) crcbuf[cnt+3] = (uint8_t)cmd[cnt];

    if (MLX90614_CRC8(crcbuf, 6) == (uint8_t)0x00)
        return ((0.02*static_cast<float>((cmd[1]<<8)|cmd[0]))   //Kelvin
                -273.15                                        //C
                -0.18);                                        //(3.3V-3)*0.6/V
    else return ERROR_TEMP_CRC8;                                     //Error
}

float getTempC(uint8_t SAddr, uint8_t memAddr)
{
    char cmd[3] = { 0 };
    uint8_t crcbuf[6] = { 0 };

    cmd[0] = OPCODE_RAM | memAddr;
    crcbuf[0] = (SAddr << 1) + 0;       //write Address
    crcbuf[1] = cmd[0];
    crcbuf[2] = (SAddr << 1) + 1;       //read Address

    MLX90614_I2C.write((SAddr<<1)+0, cmd, 1, true);
    MLX90614_I2C.read((SAddr<<1)+1, cmd, 3);

    for (uint8_t cnt = 0; cnt < 3; cnt++) crcbuf[cnt+3] = (uint8_t)cmd[cnt];

    if (MLX90614_CRC8(crcbuf, 6) == (uint8_t)0x00)
        return ((0.02*static_cast<float>((cmd[1]<<8)|cmd[0]))   //Kelvin
                -273.15                                        //C
                -0.18);                                        //(3.3V-3)*0.6/V
    else return ERROR_TEMP_CRC8;                                     //Error
}

float getTempF(uint8_t SAddr, uint8_t memAddr)
{
    char cmd[3] = { 0 };
    uint8_t crcbuf[6] = { 0 };

    cmd[0] = OPCODE_RAM | memAddr;
    crcbuf[0] = (SAddr << 1) + 0;       //write Address
    crcbuf[1] = cmd[0];
    crcbuf[2] = (SAddr << 1) + 1;       //read Address
    MLX90614_I2C.write(SAddr, cmd, 1, true);
    MLX90614_I2C.read(SAddr, cmd, 3);

    for (uint8_t cnt = 0; cnt < 3; cnt++) crcbuf[cnt+3] = (uint8_t)cmd[cnt];

    if (MLX90614_CRC8(crcbuf, 6) == (uint8_t)0x00)
        return (((0.02*static_cast<float>((cmd[1]<<8)|cmd[0]))  //Kelvin
                 -273.15                                       //C
                 -0.18)                                        //(3.3V-3)*0.6/V
                *1.8+32);                                     //F
    else return ERROR_TEMP_CRC8;                                     //Error
}

uint8_t MLX90614_CRC8(uint8_t *data, int length)
{
    uint8_t crc = 0;

    while(length--) crc = MLX90614_crc8_table[crc ^ *data++];

    return crc;
}