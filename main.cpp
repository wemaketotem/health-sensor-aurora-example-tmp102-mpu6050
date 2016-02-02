//--------------------------------------------------------------------------Benodigde Classes
#include "mbed.h"                                                           //mbed lib voor hoofdonderdelen
#include "MPU6050.h"                                                        //MPU class voor aansturing MPU6050 chip


//--------------------------------------------------------------------------Debug configuratie
I2C TMP102(P0_3, P0_2);                                                     //I2C verbinding
DigitalOut debugLed(P0_21);                                                 //Debug ledje knippert bij iedere loop

Serial pc(USBTX, USBRX);                                                    //RX TX voor monitoren pc

//--------------------------------------------------------------------------Initialiseren en declareren temp variabelen
const int addr = 0x90;                                                      //sensor adres 0x90 wanneer add0 laag is!!!!!!!!!!
char regByte[3];                                                            //config bytes decl                                    
char temp_read[2];                                                          //rouwe sensor waarde 2 bytes breed
float temp;                                                                 //sensor waarde

//--------------------------------------------------------------------------Initialisatie en declaratie MPU6050 vars en setup
InterruptIn MPU_INTERRUPT(P0_29);
MPU6050 mpu(MPU6050_ADDRESS_AD0_HIGH);                                      //Adres selectie: AD0 hoog is 0x69 

int16_t ax, ay, az, gx, gy, gz;
void fifo_overflow(); 
volatile bool new_data=false;

//==========================================================================Begin main
int main() {
    regByte[0] = 0x01;                                                      //configureer register
    regByte[1] = 0x60;                                                      //configureer byte 1
    regByte[2] = 0xA0;                                                      //configureer byte 2
    
    TMP102.write(addr, regByte, 3);                                         //schrijf config-data naar temp sensor
    regByte[0] = 0x00;                                                      //pointer register 
    TMP102.write(addr, regByte, 1);                                         //pointer naar lees data 
    
    mpu.reset();
    wait(0.2);
    
    pc.printf("MPU6050 test\n\n");
    pc.printf("MPU6050 initialize \n");
    
    mpu.initialize();                                                       //Initialisatie van de MPU6050
    wait(0.2);
    pc.printf("MPU6050 testConnection \n");
    
    bool mpu6050TestResult = mpu.testConnection();
    
    if(!mpu6050TestResult)                                                  //Test resultaat. Als ID niet gevonden is start de health patch opnieuw op
    {
        pc.printf("Kan de MPU6050 niet opstarten, geen device Id aanwezig");
        while(1){
            debugLed = 1;
            wait(0.2);
            debugLed = 0;
            wait(0.2);
            pc.printf("Herstart Totem Healt Patch");
        }
    } else 
    
    pc.printf("Health Patch is gestart\r\n");                       

//##########################################################################Oneidige hoofdloop
    while(1) {
        wait(1);
        TMP102.read(addr, temp_read, 2);                                    //lees de twee bytes
        temp = 0.0625 * (((temp_read[0] << 8) + temp_read[1]) >> 4);        //convert eer data naar float
        pc.printf("Temperatuur = %.2fC\n\r", temp);                         //schrijf float temp naar serieele monitor
        
        mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);                       //writing current accelerometer and gyro position 

        pc.printf("Accelero = %d;%d;%d\n\r",ax,ay,az);                      //writing current accelerometer and gyro position 
       
        pc.printf("Gyro = %d;%d;%d\n\r",gx,gy,gz);                          //writing current accelerometer and gyro position 
    }  
}
//--------------------------------------------------------------------------Einde programma