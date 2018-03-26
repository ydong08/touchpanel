/** 
 **	A Template for developing new terminal application
 **/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
/** These two files are necessary for calling CTOS API **/
#include <ctosapi.h>
#include<linux/input.h>
#include <unistd.h>
/** 
 ** The main entry of the terminal application 
 **/
#define RGB(r,g,b)          ((DWORD)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))

int main(int argc, char *argv[]) 
{
    BYTE key;
    int fd;
    BYTE str[256],size = 5;
    ULONG rb;
    BYTE count = 0;
    struct input_event ev[size];
    // TODO: Add your program here //
    CTOS_LCDTClearDisplay();
    
    BOOL bPenDown = FALSE;
    
//    CTOS_LCDForeGndColor( RGB(0, 0, 0) );
//    CTOS_LCDBackGndColor(RGB(212,208, 200) );
    CTOS_LCDForeGndColor(RGB(212,208, 200) );
    CTOS_LCDGSetBox(0, 0, 320, 480, 1);
    
    CTOS_LCDGShowBMPPic(134, 398, (BYTE*) "./res/EBtn.bmp"); 
    CTOS_LCDGShowBMPPic(134-70, 398, (BYTE*) "./res/Y_Close.bmp");
    

    CTOS_LCDForeGndColor( RGB(0, 0, 0) );
    CTOS_LCDBackGndColor(RGB(212,208, 200) );
    
    //CTOS_LCDTSelectFontSize(0x101E);
    CTOS_LCDTPrintXY(1, 1, "Touch_Test");
    CTOS_LCDTPrintXY(1, 2, "   OPEN   ");
    fd = open("/dev/input/event0",O_RDONLY | O_NONBLOCK ); //To open the touch panel's event and set non-block
    if(fd < 0)
    {
        printf("open fail\n");
        exit(0);
    }
    CTOS_LCDTPrintXY(1, 2, "  IOCTRL  ");
    if(ioctl(fd,EVIOCGNAME(sizeof(str)), str) < 0) //To get the name of this input event
    {
        printf("ioctl fail\n");
        exit(0);
    }
    CTOS_LCDTPrintXY(1, 7, str);
    while(1)
    {
        CTOS_KBDHit(&key);
        if(key == d_KBD_CANCEL)
            goto EXIT;
        
        CTOS_LCDTPrintXY(1, 2, "  Read  ");
        rb = read(fd, ev, sizeof(struct input_event) * size); 
        if (rb == -1)
            continue;
        if ( rb < sizeof(struct input_event) )
        {
            printf("read fail\n");
            goto EXIT;
        }
        
        int x, y;
        for (count = 0; count < rb/sizeof(struct input_event); count++)
        {
            switch (ev[count].type)
            {
            case EV_ABS:    // == ev[count].type) //absolute values to the touch panel 
                {
                    if(ev[count].code == 0) // code 0 is equal to X-axis's coordinate
                    {
                        sprintf(str,"X-value:%04d ", ev[count].value);
                        CTOS_LCDTPrintXY(1,4,str);
                        x = ev[count].value;
                    }
                    else if(ev[count].code == 1) //code 1 is equal to Y-axis's coordinate
                    {
                        sprintf(str,"Y-value:%04d ", ev[count].value);
                        CTOS_LCDTPrintXY(1,5,str);
                        y = ev[count].value;
                    }

                    if((x>134) & (x<310) & (y>398)&(y<470))
                    {
                        if(bPenDown)
                        {
                            CTOS_LCDGShowBMPPic(134, 398, (BYTE*) "./res/ClickBtn.bmp");
                            CTOS_LCDGShowBMPPic(134-70, 398, (BYTE*) "./res/Y_Open.bmp");                            
                        }
                        else
                        {
                            CTOS_LCDGShowBMPPic(134, 398, (BYTE*) "./res/EBtn.bmp"); 
                            CTOS_LCDGShowBMPPic(134-70, 398, (BYTE*) "./res/Y_Close.bmp");
                        }
                    }
                    else
                    {
                        if(!bPenDown)
                        {
                            CTOS_LCDGShowBMPPic(134-70, 398, (BYTE*) "./res/Y_Close.bmp");
                            CTOS_LCDGShowBMPPic(134, 398, (BYTE*) "./res/EBtn.bmp"); 
                            
                        }
                    }
                }
                break;
                
            case EV_KEY:
                    if (ev[count].code == BTN_TOUCH) 
                    {
                        if (ev[count].value == 1 ) 
                             bPenDown = TRUE;
                        else
                             bPenDown = FALSE;
                    }
                break;
            }            
            //for
        }
    }
EXIT:    
    close(fd);
    exit(0);
}
