#include "msp.h"
void LCD_command(unsigned char);
void LCD_write(unsigned char);
void LCD_init(void);
// custom header
extern unsigned char RXdata[], TXdata[], RXcnt;
extern unsigned char LCD_data[];
void BH_EDU_SIM_init(void);
void BH_EDU_SIM_task(void);

unsigned int adc_raw[3];
float convert[3];

void main(void) {
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;  // hold watchdog time

    BH_EDU_SIM_init(); //  BH EDU sim init
    // Used: P1.2, P1.3 (UART)
    // used: (Output) P1.1
    __enable_irq(); // keep this line

    P2->DIR |= 0xFF;
    P2->OUT &= ~0xFF;
    P6->DIR |= 0xC1;
    P6->OUT &= ~0xC1;


    P1->DIR &= ~0xC0; //setup port 1 for input
    P1->REN |= 0xC0; //enabled resistors
    P1->OUT |= 0xC0; //pull-up resistor

    P3->DIR |= 0x01;
    P3->DIR &= ~0xC0; //setup port 3 input
    P3->REN |= 0xC0; //resistor enable
    P3->OUT |= 0xC0; //pullup config

    P1->DIR &= ~0x02;
    P1->REN |= 0x02; //enabled resistors
    P1->OUT |= 0x02; //pull-up resistor
    P5->DIR |= BIT6;
    P5->SEL1 &= ~BIT6;
    P5->SEL0 |= BIT6;

    P4->SEL1 |= BIT2;
    P4->SEL0 |= BIT2; //adc

    ADC14->CTL0 = ADC14_CTL0_SHT0_6 | ADC14_CTL0_SHP | ADC14_CTL0_ON;

    ADC14->CTL1 = ADC14_CTL1_RES_2;
    ADC14->CTL0 |= ADC14_CTL0_MSC | ADC14_CTL0_CONSEQ_1;

    ADC14->MCTL[0] = ADC14_MCTLN_INCH_11; //channel 11
    ADC14->MCTL[1] = ADC14_MCTLN_INCH_12; //channel 12
    ADC14->MCTL[2] = ADC14_MCTLN_INCH_13 | ADC14_MCTLN_EOS; //channel 13

    TIMER_A0->CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1 | TIMER_A_CTL_CLR;
    TIMER_A0->CCR[0] = 5000;

    TIMER_A2->CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1 | TIMER_A_CTL_CLR;
    TIMER_A2->CCR[0] = 60000;
    TIMER_A2->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;

    LCD_init();
    while(1) {
        BH_EDU_SIM_task();  // BH EDU sim task
        ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
        while ((ADC14->CTL0 & ADC14_CTL0_BUSY!=0));
        adc_raw[0]=ADC14->MEM[0];
        adc_raw[1]=ADC14->MEM[1];
        adc_raw[2]=ADC14->MEM[2];
        convert[0]=((float)adc_raw[0]/4095)*3.3*1000; //convert to V then multiply by 1000
        convert[1]=((float)adc_raw[1]/4095)*3.3*1000;
        convert[2]=((float)adc_raw[2]/4095)*3.3*1000;

        LCD_command(0xC0); //line 2
        LCD_write('A');
        LCD_write('l');
        LCD_write('e');
        LCD_write('x');
        LCD_write(' ');
        LCD_write('L');
        LCD_write('i');
        LCD_write('u');
        // Virtual LCD
        int i = 0;

        if (!(P1->IN & 0x40)){ //up
            LCD_command(0x80);
            LCD_write('U');
            LCD_write('p');
            for(i=0;i<50000;i++){ //play sound as long as button held
                if((TIMER_A0->CTL & TIMER_A_CTL_IFG)!=0){
                    P3->OUT ^= 0x01;
                    TIMER_A0->CTL &= ~TIMER_A_CTL_IFG;
                }
            }
        }
        if (!(P1->IN & 0x80)){ //down
            LCD_command(0x80);
            LCD_write('D');
            LCD_write('o');
            LCD_write('w');
            LCD_write('n');
            for(i=0;i<50000;i++){
                if((TIMER_A0->CTL & TIMER_A_CTL_IFG)!=0){
                    P3->OUT ^= 0x01;
                    TIMER_A0->CTL &= ~TIMER_A_CTL_IFG;
                }
            }
        }
        if (!(P3->IN & 0x40)){ //left
            LCD_command(0x80);
            LCD_write('L');
            LCD_write('e');
            LCD_write('f');
            LCD_write('t');
            for(i=0;i<50000;i++){
                if((TIMER_A0->CTL & TIMER_A_CTL_IFG)!=0){
                    P3->OUT ^= 0x01;
                    TIMER_A0->CTL &= ~TIMER_A_CTL_IFG;
                }
            }
        }
        if (!(P3->IN & 0x80)){ //right
            LCD_command(0x80);
            LCD_write('R');
            LCD_write('i');
            LCD_write('g');
            LCD_write('h');
            LCD_write('t');
            for(i=0;i<50000;i++){
                if((TIMER_A0->CTL & TIMER_A_CTL_IFG)!=0){
                    P3->OUT ^= 0x01;
                    TIMER_A0->CTL &= ~TIMER_A_CTL_IFG;
                }
            }
        }
        LCD_command(0x80);
        LCD_write(' ');
        LCD_write(' ');
        LCD_write(' ');
        LCD_write(' ');
        LCD_write(' ');

        if(!(P1->IN & 0x02)){
            TIMER_A2->CCR[1] = 7500;
            //__delay_cycles(500000);
        }
        else{
            //__delay_cycles(500000);
            TIMER_A2->CCR[1] = 4500;
        }

        LCD_data[0]='0'+(convert[0]/1000);
        LCD_data[1]='.';
        LCD_data[2]='0'+(((int)convert[0]/100)%10);
        LCD_data[3]='0'+(((int)convert[0]/10)%10);
        LCD_data[4]='0'+((int)convert[0]%10);

        LCD_data[7]='0'+(convert[1]/1000);
        LCD_data[8]='.';
        LCD_data[9]='0'+(((int)convert[1]/100)%10);
        LCD_data[10]='0'+(((int)convert[1]/10)%10);
        LCD_data[11]='0'+((int)convert[1]%10);
        LCD_data[0]='0'+(convert[0]/1000);

        LCD_data[16]='0'+(convert[2]/1000);
        LCD_data[17]='.';
        LCD_data[18]='0'+(((int)convert[2]/100)%10);
        LCD_data[19]='0'+(((int)convert[2]/10)%10);
        LCD_data[20]='0'+((int)convert[2]%10);
        LCD_data[21]='0'+(convert[2]/1000);

        __delay_cycles(5000); // keep this line, recommended minimum delay
    }
}

void LCD_command(unsigned char in) {
    P2OUT = in;
    P6OUT &= ~BIT7;  // clear RS
    P6OUT &= ~BIT6;  // clear R/W
    P6OUT |= BIT0;  // set E
    __delay_cycles(200);
    P6OUT &= ~BIT0;   // clear E
}

void LCD_write(unsigned char in) {
    P2OUT = in;
    P6OUT |= BIT7;  // set RS
    P6OUT &= ~BIT6;  // clear R/W
    P6OUT |= BIT0;  // set E
    __delay_cycles(200);
    P6OUT &= ~BIT0;   // clear E
}

void LCD_init(void) {
    P6OUT &= ~BIT0;  // clear E
    __delay_cycles(3000);
    LCD_command(0x30);  // Wake up
    __delay_cycles(400);
    LCD_command(0x30);
    __delay_cycles(400);
    LCD_command(0x30);
    __delay_cycles(400);
    LCD_command(0x38);  // Function set: 8 bit/2-line
    LCD_command(0x10);  // set cursor
    LCD_command(0x0F);  // Display on, cursor on
    LCD_command(0x06); // Entry mode set
    LCD_command(0x01);  // clear display
    __delay_cycles(5000);
}
