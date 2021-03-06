#include <stdint.h>
#include <string.h>

#include <r0ketlib/display.h>
#include <r0ketlib/fonts.h>
#include <r0ketlib/render.h>
//#include <r0ketlib/fonts/smallfonts.h>
#include <r0ketlib/keyin.h>
#include <r0ketlib/itoa.h>
#include <r0ketlib/config.h>
#include <r0ketlib/print.h>
#include <rad1olib/pins.h>
#include <rad1olib/light_ws2812_cortex.h>
#include <rad1olib/setup.h>
#include <r0ketlib/display.h>

#include "usetable.h"

#define one ((uint32_t)1)

typedef uint8_t uchar;

static unsigned long iter=0;


// NOTE: LED data orde is G R B

void dim(uint8_t *target, uint8_t *colours, size_t size, int dimmingfactor){
    for(int i= 0; i < size; i++){
        target[i] = colours[i]/dimmingfactor ;
    };

};

void ram(void)
{
    getInputWaitRelease();

    char template[256];
    static int nickx=2,nicky=10;
    static int nickwidth,nickheight;
    static int nickoff=10;
    static char delaytime=15;
    static char speedmode=0;
    static char movx=1;
    static char LCDSHIFTX_EVERY_N=1;
    static char __attribute__((unused)) LCDSHIFTY_EVERY_N=1;
    char *nick=GLOBAL(nickname);
    uint8_t larson_shift_every_n = 5;
    uint8_t larson_pos = 0;
    int8_t larson_dir = 1;
    uint8_t larson_buffer[5*3];
    uint8_t larson_len = sizeof(larson_buffer)/3;

    int dimmingfactor = 10;
    uint8_t rgbled_buffer[8*3];
    uint8_t* rgbled_ant_end = &rgbled_buffer[7*3];
    uint8_t* rgbled_left = &rgbled_buffer[0*3];
    uint8_t* rgbled_right = &rgbled_buffer[1*3];

    uint8_t dimmer[8*3];


    lcdClear();
    setExtFont(GLOBAL(nickfont));
    nicky=1;
    nickwidth=DoString(nickx,nicky,nick);
    if(nickwidth<50)nickoff=30;
    nickheight=getFontHeight();
    nicky=(RESY-getFontHeight())/2;
    // Adjust speed depending on rendered image size
    if(nickwidth>RESX && nickheight>14) { movx=2;}
    if(nickwidth>RESX && nickheight>40) { movx=3;}

    lcdClear();
    char stepmode=0;

    SETUPgout(RGB_LED);
    delayms_queue_plus(5,0);
    memset(rgbled_buffer, 0x0, sizeof(rgbled_buffer));
    dim(dimmer, rgbled_buffer, sizeof(dimmer), dimmingfactor);
    ws2812_sendarray(dimmer, sizeof(dimmer));

    while (1)
    {
        ++iter;

        if(iter % larson_shift_every_n == 0)
        {
            // bookkeepinb
            larson_pos += larson_dir;
            if (larson_pos < 1)
            {
                larson_dir = 1;
            }
            if (larson_pos > larson_len-2)
            {
                larson_dir = -1;
            }
            // clear
            memset(larson_buffer, 0x0, sizeof(larson_buffer));
            larson_buffer[larson_pos*3+1] = 0xff; // r
            larson_buffer[larson_pos*3]   = 0x00; // g
            larson_buffer[larson_pos*3+2] = 0x00; // b
            int8_t prev_pos = larson_pos - larson_dir;
            if (   prev_pos > -1
                && prev_pos < larson_len)
            {
                larson_buffer[prev_pos*3+1] = 0x40; // r
                larson_buffer[prev_pos*3]   = 0x00; // g
                larson_buffer[prev_pos*3+2] = 0x00; // b
            }
            memcpy(&rgbled_buffer[2*3], larson_buffer, sizeof(larson_buffer));
            dim(dimmer, rgbled_buffer, sizeof(dimmer), dimmingfactor);
            ws2812_sendarray(dimmer, sizeof(dimmer));
        }


        lcdDisplay();
        lcdClear();
        lcdFill(GLOBAL(nickbg));
        setTextColor(GLOBAL(nickbg),GLOBAL(nickfg));
        // Old shift code. Can't handle longer Nicks...
        // if(iter%LCDSHIFT_EVERY_N==0) lcdShift(1,-2,1);
        // if(iter%LCDSHIFT_EVERY_N==0) { nickx=(nickx+1)%100-nickwidth; nicky=(nicky+1)%50;}
        if(iter%LCDSHIFTX_EVERY_N==0) { nickx-=movx;
        if(nickx<=(-1*nickwidth-nickoff))nickx=0; }
        DoString(nickx,nicky,nick);
        DoString(nickx+nickwidth+nickoff,nicky,nick);
        if(nickwidth<RESX) DoString(nickx+2*(nickwidth+nickoff),nicky,nick);
        char key=stepmode?getInputWait():getInputRaw();
        stepmode=0;
        switch(key)
        {
            // Buttons: Right change speed, Up hold scrolling
            case BTN_ENTER:
                return;
            case BTN_RIGHT:
                getInputWaitRelease();
                speedmode=(speedmode+1)%6;
                delaytime=15;
                // speeds: normal, slow, sloooow, double, tripple...
                switch(speedmode)
                {
                  case 0:
                    movx=1; LCDSHIFTX_EVERY_N=1; LCDSHIFTY_EVERY_N=1; break;
                  case 1:
                    movx=1; LCDSHIFTX_EVERY_N=2; LCDSHIFTY_EVERY_N=2; break;
                  case 2:
                    movx=1; LCDSHIFTX_EVERY_N=3; LCDSHIFTY_EVERY_N=4; break;
                  case 4:
                    movx=2; LCDSHIFTX_EVERY_N=1; LCDSHIFTY_EVERY_N=1; break;
                  case 5:
                    movx=3; LCDSHIFTX_EVERY_N=1; LCDSHIFTY_EVERY_N=1; break;
                }
                break;
            case BTN_UP:
                getInputWaitRelease();
                dimmingfactor -= 2;
                if (dimmingfactor < 1)
                {
                    dimmingfactor = 1;
                }
                dim(dimmer, rgbled_buffer, sizeof(dimmer), dimmingfactor);
                ws2812_sendarray(dimmer, sizeof(dimmer));
                break;
            case BTN_LEFT:
                stepmode=1;
                getInputWaitRelease();
                break;
            case BTN_DOWN:
                getInputWaitRelease();
                dimmingfactor += 2;
                if (dimmingfactor > 255)
                {
                    dimmingfactor = 255;
                }
                dim(dimmer, rgbled_buffer, sizeof(dimmer), dimmingfactor);
                ws2812_sendarray(dimmer, sizeof(dimmer));
                break;
        }
        delayms_queue_plus(delaytime,0);
    }
    return;
}
