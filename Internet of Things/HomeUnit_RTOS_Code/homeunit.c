/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== uartecho.c ========
 */

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/cfg/global.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
#include <ti/drivers/I2C.h>

#include <ti/drivers/PIN.h>
// #include <ti/drivers/SPI.h>
#include <ti/drivers/UART.h>

// #include <ti/drivers/Watchdog.h>

/* Board Header files */
#include "Board.h"
#include <stdint.h>
#include <stdlib.h>
#define Board_initI2C() I2C_init();



/* Pin driver handle */
static PIN_Handle ledPinHandle;
static PIN_State ledPinState;

static PIN_State buttonPinState;
static PIN_Handle buttonPinHandle;


    uint32_t currVal = 0;
    uint8_t user_present=0;
    uint8_t motion_present=0;
    char motion_char[]="0";
    char user_char[]="0";
    int a=0;
    unsigned int    i=0;
    int64_t     temp_in_celcius=0;
    int64_t     temp_in_far=0;
    char        temp_char[]="0";
    uint32_t temp_read = 0x0000;
    uint8_t rx_buffer_data[305];
    UART_Handle uart;
    UART_Handle uart1;
    UART_Handle uart2;
    UART_Params uartParams;
    uint8_t         bulb1int=0;
    uint8_t         bulb2int=0;
    uint8_t         getbulb1=0;
    uint8_t         getbulb2=0;
    char            bulb1char[]="0";
    char            bulb2char[]="0";
    uint8_t         txBuffer[1];
    uint8_t         rxBuffer[2];
    I2C_Handle      i2c;
    I2C_Params      i2cParams;
    I2C_Transaction i2cTransaction;

            char AT_connect[] = "AT+CWJAP_CUR=\"HouseLANister\",\"royalsquash711\"\r\n";

            char AT_cipstart[] = "AT+CIPSTART=\"TCP\",\"www.arcane-sierra-84196.herokuapp.com\",80,5\r\n";
            //char AT_cipmode[] = "AT+CIPMODE=1\r\n";
            char AT_cipsend[] = "AT+CIPSEND=157\r\n";
            char AT_cipsendget[] = "AT+CIPSEND=92\r\n";
            char AT_cwlif[] = "AT+CWLIF\r\n";
            char post[]= "POST /boarddata HTTP/1.1\r\nHost: arcane-sierra-84196.herokuapp.com\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 15\r\n\r\n";
            char get[]= "GET /boarddata?temp=";
            char get1[]= " HTTP/1.1\r\nHost: arcane-sierra-84196.herokuapp.com\r\n\r\n";
            char motion[]="&motion=";
            char user[]="&user=";
            char databulb1[]= "bulb1=";
            char databulb2[]= "&bulb2=";
            char eod[]="\r\n\r\n";
            //char endofdata[] = "+++";
            char AT_cipclose[]="AT+CIPCLOSE\r\n";

PIN_Config ledPinTable[] = {
    Board_LED0 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    Board_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    IOID_12 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    IOID_15 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

PIN_Config buttonPinTable[] = {
    //Board_BUTTON0  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    IOID_22  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    IOID_21  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    IOID_0  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    PIN_TERMINATE
    //PIN_NOPULL
};


/*
 *  ======== echoFxn ========
 *  Task for this function is created statically. See the project's .cfg file.
 */
//Void echoFxn(UArg arg0, UArg arg1)
void buttonCallbackFxn(PIN_Handle handle, PIN_Id pinId) {

    currVal = 0;
    /* Debounce logic, only toggle if the button is still pushed (low) */
    CPUdelay(8000*50);
    if (!PIN_getInputValue(pinId)) {
        /* Toggle LED based on the button pressed */
        switch (pinId) {
            case IOID_22:
                Swi_post(BULB_1);
                Hwi_disable();
                break;

            case IOID_0:
                Swi_post(BULB_2);
                Hwi_disable();
                break;

            default:
                /* Do nothing */
                break;
        }
    }
}

void bulb1()
{
    currVal =  PIN_getOutputValue(IOID_12);
                    PIN_setOutputValue(ledPinHandle, IOID_12, !currVal);
    bulb1int = currVal;
    sprintf(bulb1char, "%d", bulb1int);
                    CPUdelay(80000*50);
                    Hwi_enable();
                    Semaphore_post(bulbsem);

}

void bulb2()
{
    currVal =  PIN_getOutputValue(IOID_15);
                    PIN_setOutputValue(ledPinHandle, IOID_15, !currVal);
    bulb2int = currVal;
    sprintf(bulb2char, "%d", bulb2int);
                    CPUdelay(80000*50);
                    Hwi_enable();
                    Semaphore_post(bulbsem);
}

Void initialization()

{

    /* Create a UART with data processing off. */


        I2C_Params_init(&i2cParams);
        i2cParams.bitRate = I2C_400kHz;
        i2c = I2C_open(Board_I2C, &i2cParams);
        if (i2c == NULL) {
            System_abort("Error Initializing I2C\n");
        }     else {
            System_printf("I2C Initialized!\n");
        }
        System_flush();
        txBuffer[0] = 0x0E3;
        i2cTransaction.slaveAddress = 0x040;
        i2cTransaction.writeBuf = txBuffer;
        i2cTransaction.writeCount = 1;
        i2cTransaction.readBuf = rxBuffer;
        i2cTransaction.readCount = 2;





    UART_Params_init(&uartParams);
    uartParams.readMode = UART_MODE_BLOCKING;
    uartParams.writeMode = UART_MODE_BLOCKING;
    uartParams.writeDataMode = UART_DATA_TEXT;
        uartParams.readDataMode = UART_DATA_TEXT;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 115200;
    uart = UART_open(Board_UART0, &uartParams);

    if (uart == NULL) {
        System_abort("Error opening the UART");
    }
    else
                     {
                         System_printf("UART Initialized!\n");
                     }

        System_flush();


}

void tempreadingandconn()
{
                  UART_write(uart, AT_connect, sizeof(AT_connect));
                  UART_read(uart, &rx_buffer_data, 100);

                  if(rx_buffer_data[0]=='A' && rx_buffer_data[1]=='T')
                      {
                          System_printf("Connected to the Internet\n");
                      }
                          System_flush();




 while(1)
  {

     Task_sleep(2000000);
     temp_read = 0x0000;
     currVal =  PIN_getOutputValue(Board_LED1);
      PIN_setOutputValue(ledPinHandle, Board_LED1, !currVal);
      if (I2C_transfer(i2c, &i2cTransaction)) {
             temp_read = temp_read|rxBuffer[0];
             temp_read = (temp_read<<8)|rxBuffer[1];
             temp_in_celcius = ((175.72*temp_read)/65536)-46.85;
             temp_in_far= ((9*temp_in_celcius)/5) + 32;
             sprintf(temp_char, "%d", temp_in_far);
      }
         /*  System_printf("Sample in degree celcius %u: %d (C)\n", i, temp_in_celcius);

                   }         else {
                             System_printf("I2C Bus fault\n");
                                                        }
                        System_flush();*/
          UART_write(uart, AT_cwlif, sizeof(AT_cwlif));
          UART_read(uart, &rx_buffer_data, 11);
          user_present=0;
          motion_present=0;
          for(i=0;i<4;i++)
          {
              UART_read(uart, &rx_buffer_data, 6);


          if(rx_buffer_data[0]=='1' && rx_buffer_data[1]=='9' && rx_buffer_data[2]=='2')
          {
              UART_read(uart, &rx_buffer_data, 25);
              user_present=user_present+1;

              motion_present=1;
          }
          else
          {
              i=5;
             motion_present = PIN_getInputValue(IOID_21);
          }
          }

          sprintf(motion_char, "%d", motion_present);
          sprintf(user_char, "%d", user_present);

          UART_write(uart, AT_cipstart, sizeof(AT_cipstart));
          UART_read(uart, &rx_buffer_data, 80);

      /*    if(rx_buffer_data[65]=='C' && rx_buffer_data[66]=='O')
              {
                System_printf("Connected to the Website\n");
              }
               System_flush();  */
          UART_write(uart, AT_cipsendget, sizeof(AT_cipsendget));
          UART_read(uart, &rx_buffer_data, 21);



          UART_write(uart, get, sizeof(get)-1);
          UART_write(uart, temp_char, 2);
          UART_write(uart, motion, sizeof(motion)-1);
          UART_write(uart, motion_char, sizeof(motion_char)-1);
          UART_write(uart, user, sizeof(user)-1);
          UART_write(uart, user_char, sizeof(user_char)-1);
          UART_write(uart, get1, sizeof(get1)-1);
          UART_read(uart, &rx_buffer_data, 302);

          if(rx_buffer_data[286]=='t' && rx_buffer_data[287]=='a')
             {
               System_printf("Data Received\n");
             }
                         System_flush();
          getbulb1 = rx_buffer_data[289]-'0';
          getbulb2 = rx_buffer_data[301]-'0';
          PIN_setOutputValue(ledPinHandle, IOID_12, !getbulb1);
          PIN_setOutputValue(ledPinHandle, IOID_15, !getbulb2);

          UART_write(uart, AT_cipclose, sizeof(AT_cipclose));
          UART_read(uart, &rx_buffer_data, 28);




       currVal =  PIN_getOutputValue(Board_LED1);
         PIN_setOutputValue(ledPinHandle, Board_LED1, !currVal);

      }
}


void bulbdata()
{
    while(1)
    {
            Semaphore_pend(bulbsem,BIOS_WAIT_FOREVER);
            currVal =  PIN_getOutputValue(Board_LED1);
              PIN_setOutputValue(ledPinHandle, Board_LED1, !currVal);

              UART_write(uart, AT_cipstart, sizeof(AT_cipstart));
              UART_read(uart, &rx_buffer_data, 80);

          /*   if(rx_buffer_data[65]=='C' && rx_buffer_data[66]=='O')
              {
                  System_printf("Connected to the Website\n");
              }
              System_flush();  */


              UART_write(uart, AT_cipsend, sizeof(AT_cipsend));
              UART_read(uart, &rx_buffer_data, 22);


              UART_write(uart, post, sizeof(post)-1);
              UART_write(uart, databulb1, sizeof(databulb1)-1);
              UART_write(uart, bulb1char, sizeof(bulb1char)-1);
              UART_write(uart, databulb2, sizeof(databulb2)-1);
              UART_write(uart, bulb2char, sizeof(bulb2char)-1);
              UART_write(uart, eod, sizeof(eod)-1);
              UART_read(uart, &rx_buffer_data, 290);

              if(rx_buffer_data[279]=='D' && rx_buffer_data[280]=='a')
                           {
                               System_printf("Data posted\n");
                           }
                           System_flush();



              UART_write(uart, AT_cipclose, sizeof(AT_cipclose));
              UART_read(uart, &rx_buffer_data, 28);


              currVal =  PIN_getOutputValue(Board_LED1);
                       PIN_setOutputValue(ledPinHandle, Board_LED1, !currVal);




}
}

void ledblink()
{
    while(1){
    PIN_setOutputValue(ledPinHandle, Board_LED0,
                              !PIN_getOutputValue(Board_LED0));
           CPUdelay(80000*50);
    }
}

int main(void)
{


    Board_initGeneral();
    Board_initUART();
    Board_initI2C();



    ledPinHandle = PIN_open(&ledPinState, ledPinTable);
    if(!ledPinHandle) {
        System_abort("Error initializing board LED pins\n");
    }

    PIN_setOutputValue(ledPinHandle, Board_LED1, 1);

    buttonPinHandle = PIN_open(&buttonPinState, buttonPinTable);
            if(!buttonPinHandle) {
                System_abort("Error initializing button pins\n");
            }

       if (PIN_registerIntCb(buttonPinHandle, &buttonCallbackFxn) != 0) {
                    System_abort("Error registering button callback function");
            }


    BIOS_start();

    return (0);
}
