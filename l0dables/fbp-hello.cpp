#include <r0ketlib/config.h>
#include <r0ketlib/display.h>
#include <r0ketlib/fonts.h>
#include <r0ketlib/render.h>
#include <r0ketlib/print.h>
#include <r0ketlib/keyin.h>
#include <r0ketlib/image.h>

#include "usetable.h"

extern "C" {
    void ram(void);

    void _exit(void) {
        // do nothing
    }
    void _kill(void) {
        /// ff
    }
    void _getpid(void) {
        /// ff
    }
}

void ram(void){
  if (lcdShowImageFile("nick.lcd") != 0) {
    lcdClear();
    lcdNl();
    lcdPrintln("File nick.lcd");
    lcdPrintln("not present.");
    lcdNl();
    lcdDisplay();
  }
}

