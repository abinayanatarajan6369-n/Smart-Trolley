#include <xc.h>
#include <string.h>

#define _XTAL_FREQ 8000000

#pragma config FOSC = INTRC_NOCLKOUT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config MCLRE = ON
#pragma config CP = OFF
#pragma config CPD = OFF
#pragma config BOREN = OFF
#pragma config LVP = OFF

#define ADD_BTN    RA0
#define REMOVE_BTN RA1
#define BILL_BTN   RA2


char tag[13];

unsigned char sugar_qty=0;
unsigned char biscuit_qty=0;
unsigned char oil_qty=0;

unsigned int total=0;
signed char active_item=-1;

void delay(unsigned int t)
{
    while(t--);
}

void enable()
{
    RE1=1;
    delay(50);
    RE1=0;
    delay(50);
}

void lcd(char rs,char data)
{
    RE0=rs;
    PORTD=data;
    enable();
}

void str(char *s)
{
    while(*s)
        lcd(1,*s++);
}

void lcd_init()
{
    lcd(0,0x38);
    lcd(0,0x0C);
    lcd(0,0x06);
    lcd(0,0x01);
}

void uart_init()
{
    BRGH=1;
    SPBRG=25;

    SYNC=0;
    SPEN=1;
    CREN=1;
    TXEN=1;
}

char uart_read()
{
    while(!RCIF);

    if(OERR)
    {
        CREN=0;
        CREN=1;
    }

    return RCREG;
}

void main()
{
    unsigned char i;

    OSCCON=0x61;

    ANSEL=0x00;
    ANSELH=0x00;

    TRISA=0x07;
    TRISC=0x80;
    TRISD=0x00;
    TRISE=0x00;

    PORTA=0x00;
    PORTC=0x00;
    PORTD=0x00;
    PORTE=0x00;

    uart_init();
    lcd_init();
    lcd(0,0x01);
    lcd(0,0x80);
    str(" SMART TROLLEY");

    lcd(0,0xC0);
    str(" SCAN RFID");

    while(1)
    {
        if(RCIF)
        {
            for(i=0;i<12;i++)
            {
                tag[i]=uart_read();
            }

            tag[12]='\0';

            if(strcmp(tag,"480074582044")==0)
            {
                active_item=1;

                lcd(0,0x01);
                lcd(0,0x80);
                str(" SUGAR");
                lcd(0,0xC0);
                str(" PRESS ADD");
            }
            else if(strcmp(tag,"48007DCE7D86")==0)
            {
                active_item=2;

                lcd(0,0x01);
                lcd(0,0x80);
                str(" BISCUIT");
                lcd(0,0xC0);
                str(" PRESS ADD");
            }
            else if(strcmp(tag,"48007D7FDD97")==0)
            {
                active_item=3;

                lcd(0,0x01);
                lcd(0,0x80);
                str(" OIL");
                lcd(0,0xC0);
                str(" PRESS ADD");
            }
            else
            {
                lcd(0,0x01);
                lcd(0,0x80);
                str(" INVALID CARD");
            }
        }

        if(ADD_BTN==1)
        {
            __delay_ms(200);

            if(active_item==1)
            {
                sugar_qty++;
                total+=50;
            }
            else if(active_item==2)
            {
                biscuit_qty++;
                total+=10;
            }
            else if(active_item==3)
            {
                oil_qty++;
                total+=90;
            }

            lcd(0,0x01);
            lcd(0,0x80);
            str(" ITEM ADDED");

            while(ADD_BTN);
        }

        if(REMOVE_BTN==1)
        {
            __delay_ms(200);

            if(active_item==1 && sugar_qty>0)
            {
                sugar_qty--;
                total-=50;
            }
            else if(active_item==2 && biscuit_qty>0)
            {
                biscuit_qty--;
                total-=10;
            }
            else if(active_item==3 && oil_qty>0)
            {
                oil_qty--;
                total-=90;
            }

            lcd(0,0x01);
            lcd(0,0x80);
            str(" ITEM REMOVED");

            while(REMOVE_BTN);
        }

        if(BILL_BTN==1)
        {
            __delay_ms(200);

            lcd(0,0x01);
            lcd(0,0x80);
            str(" TOTAL BILL");

            lcd(0,0xC0);

            if(total<100)
            {
                lcd(1,(total/10)+'0');
                lcd(1,(total%10)+'0');
            }
            else
            {
                lcd(1,(total/100)+'0');
                lcd(1,((total/10)%10)+'0');
                lcd(1,(total%10)+'0');
            }

            while(BILL_BTN);
        }
    }
}
