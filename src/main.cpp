#include "mbed.h"
#include "mlx90614.h"

#define HEAT_DETECT_OVER_IN_CELSIUS 50.0f

DigitalOut ledStatus(LED1);
DigitalOut ledDetectFire(p8);

int SAddr = DEFAULT_SADDR;
float temp = 0;
char c;

int main() {
    if (MLX90614_EEPROM_Read(SAddr,EEPROM_T0MAX) != TOBJ_P60) {
        MLX90614_EEPROM_Write(SAddr, EEPROM_T0MAX, TOBJ_P60);
    }

    if (MLX90614_EEPROM_Read(SAddr,EEPROM_T0MIN) != TOBJ_PM0) {
        MLX90614_EEPROM_Write(SAddr, EEPROM_T0MIN, TOBJ_PM0);
    }

    if (MLX90614_EEPROM_Read(SAddr,EEPROM_PWMCTRL) != DEFAULT_PWMCTRL) {
        MLX90614_EEPROM_Write(SAddr, EEPROM_PWMCTRL, DEFAULT_PWMCTRL);
    }
    if (MLX90614_EEPROM_Read(SAddr,EEPROM_TARANGE) != (uint16_t)(TAMB_P50 << 8 | TAMB_PM0)) {
        MLX90614_EEPROM_Write(SAddr, EEPROM_TARANGE, (uint16_t)(TAMB_P50 << 8 | TAMB_PM0));
    }

    if (MLX90614_EEPROM_Read(SAddr,EEPROM_CONFIGREG1) != DEFAULT_CONFIGREG1) {
        MLX90614_EEPROM_Write(SAddr, EEPROM_CONFIGREG1, DEFAULT_CONFIGREG1);
    }

    printf(" EEPROM_T0MAX = 0x%X\r\n", MLX90614_EEPROM_Read(SAddr,EEPROM_T0MAX));
    printf(" EEPROM_T0MIN = 0x%X\r\n", MLX90614_EEPROM_Read(SAddr,EEPROM_T0MIN));
    printf(" EEPROM_PWMCTRL = 0x%X\r\n", MLX90614_EEPROM_Read(SAddr,EEPROM_PWMCTRL));
    printf(" EEPROM_TARANGE = 0x%X\r\n", MLX90614_EEPROM_Read(SAddr,EEPROM_TARANGE));
    printf(" EEPROM_ECCOEF = 0x%X\r\n", MLX90614_EEPROM_Read(SAddr,EEPROM_ECCOEF));
    printf(" EEPROM_CONFIGREG1 = 0x%X\r\n", MLX90614_EEPROM_Read(SAddr,EEPROM_CONFIGREG1));

    SAddr = MLX90614_SA_Read();
    printf(" EEPROM_SMBUSADDR = 0x%X. \n\r", SAddr);
    if (SAddr == ERROR_ADDR_CRC8) {
        SAddr = MLX90614_SA_Scan();
        printf(" MLX90614 Slave Address Find = 0x%X. \n\r", SAddr);
    }

    printf("Start detecting heat over %.2f in celsius\r\n", HEAT_DETECT_OVER_IN_CELSIUS);

    while(1) {
        ledStatus = true;

//        printf("\033[2J\033[1;1H"); fflush(stdout);
//        do temp = getTempC_Ambi(SAddr); while(temp == ERROR_TEMP_CRC8);
//        printf("Amb temp:%.2f\r\n", temp);

        // wait for temp read
        do {
            temp = getTempC_Obj1(SAddr);
            wait_ms(0.1);
        } while(temp == ERROR_TEMP_CRC8);

        // flash led if dtect at 50C or higher
        ledDetectFire = (temp > HEAT_DETECT_OVER_IN_CELSIUS) ? true : false;
        printf("Obj Temp:%.2f\r\n", temp);

//        do temp = getTempC_Obj2(SAddr);  while(temp == ERROR_TEMP_CRC8);
//        printf("Obj#2 temp:%.2f\r\n", temp);

        ledStatus = false;
        wait(0.1);
    }
}
