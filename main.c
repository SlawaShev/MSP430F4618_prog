#include <msp430xG46x.h>
#include <stdio.h>

#include "LCD.h"

#define DAC_V_REF 2500
#define VCC 2950

#define ADC_CYCLES 32

volatile int dac_v;
void setupADC();
void setupDAC();
void setupPorts();
volatile float result[32];
volatile float current[32];
volatile float voltage[32];
volatile float resistance[32];
volatile float test[4];
volatile float control_current[] = {0.15, 0.25, 0.37, 0.5, 0.62, 0.73, 0.83, 0.94, 1.02, 1.12, 1.2, 1.26, 1.34, 1.36, 0.97, 0.76, 0.81, 0.75, 0.81, 0.82, 0.85, 0.75, 0.75, 0.78, 0.85, 0.83, 0.84, 0.64, 0.65, 0.45, 0.47, 0.5};
volatile int control;
volatile unsigned int i;
volatile unsigned int num = 0;
volatile float test[4];
/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    setupADC();    //Initialize ADC
    setupDAC();    //Initialize DAC
    setupPorts();  //Initialize Ports

    __enable_interrupt();

    initLCD_A();   //Initialize LCD
    clrLCD();	   //Clear LCD

    while (1) {
        i = 0;

        __bis_SR_register(LPM0_bits);


        if (control == 1) {
            displayPass();
        } else {
            displayFail();
        }
    }
}

void setupDAC(void) {
    // dac0 DAC12.0
    DAC12_0CTL = DAC12IR + DAC12AMP_5 + DAC12ENC;   // Int ref gain 1.   DAC12IR -входной диапазон
   // DAC12_0DAT = dac_v * (4096. / DAC_V_REF);

}


void setupADC(void) {
    ADC12CTL0 = REF2_5V // set reference voltage
              | REFON   // enable reference voltage
              | ADC12ON // enable adc
              | MSC    // enable multi sample conversion
              | SHT0_12;
    ADC12CTL1 |= CONSEQ_0      //
              | SHP;   // multichannel

     // A0: OUT+ (A)
     P6SEL |= BIT0; // enable AO in P6.0
     //ADC12MCTL0 |= INCH_0 | SREF_1; // ADC12 Input Channel Select Bit 0 - A0
     ADC12MCTL0 |= INCH_0
                | SREF_1;

     ADC12CTL0 |= ENC;
     ADC12IE = BIT0;
}

void setupPorts() {
    // init button
    P1SEL &= ~BIT0;     // enable GPIO in P1.0
    P1IE |= BIT0;       // enable interrupt from P1.0
    P1IES |= BIT0;      // select interrupt-edge
    P1DIR &= ~BIT0;     // set P1.0 to input

    // init all ports
    P2SEL &= ~BIT0;
    P3SEL &= ~BIT0;
    P4SEL &= ~BIT0;
    P7SEL &= ~BIT0;
    P8SEL &= ~BIT0;
    P9SEL &= ~BIT0;
    P10SEL &= ~BIT0;
    PASEL &= ~BIT0;
    PBSEL &= ~BIT0;
}

#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR(void) {



        result[i] = ADC12MEM0 * (2500 / 4095.);
        voltage[i] = dac_v - result[i]; //get voltage on diod
        current[i] = result[i] / 50;    //get current on diod
        if ((current[i] > (control_current[i] * 1.2)) || (current[i] < (control_current[i] * 0.8))) {
            control = 0;
        }
        printf("%d\n", ADC12MEM0);
        printf("Reading...\n");
        resistance[i] = voltage[i]/current[i];   	//calculate resistance of diod 
        dac_v = dac_v + 10;			 	//increase DAC voltage on 10V
        DAC12_0DAT = dac_v * (4096. / DAC_V_REF);	//apply magnitude to DAC register     
        i++;
        if (i < ADC_CYCLES) {  				//restrain amount of cycles
            ADC12CTL0 |= ADC12SC;
        } 
        else {
            __bic_SR_register_on_exit(LPM0_bits);
        }
}


#pragma vector = PORT1_VECTOR
__interrupt void PORT1_ISR(void) {
    if (!(P1IN & BIT0)) {
        num = 0;
        dac_v = 10;
        DAC12_0DAT = dac_v * (4096. / DAC_V_REF);
        control = 1;
        printf("Reading...\n");
        displayWait();
        ADC12CTL0 |= ADC12SC;
    }

    P1IFG &= ~BIT0;
}
