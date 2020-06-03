#include <stdio.h>
#include <wiringPi.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <gfx.h>
#include <signal.h>
#include <unistd.h>

#define ROW_0 28
#define ROW_1 27
#define ROW_2 25
#define ROW_3 24
#define ROW_4 23
#define ROW_5 22
#define ROW_6 21

#define SERIAL_OUT 6
#define CLK_OUT 11
#define OUT_DISABLE 10

#define false 0
#define true 1

char frameBuffer[7][8];

void selectDisplayRow(int row)
{
  row = GFX_HEIGHT - 1 - row;
  int pins[] = { ROW_0, ROW_1, ROW_2, ROW_3, ROW_4, ROW_5, ROW_6 };
  for (int i = 0; i < 8; i++) {
    int val = false;
    if (i == row){ val = false; } else { val = true; }
    digitalWrite(pins[i], val);
  }
}

void delayNoUnits(int delayTime)
{
  for (int i = 0; i < delayTime; i++){;}
}

// This needs to be ran constantly (in a loop) or else the display will stop. Any delays will cause a dimmer display or tearing
void sendDisplayData()
{
       
  for (int row = 0; row < GFX_HEIGHT; row++)
  {
    selectDisplayRow(row);
    digitalWrite(OUT_DISABLE, HIGH);
    
    for (int column = GFX_WIDTH - 1; column >= 0; column--)
    {
        
      int xByte = (int)(column / 8);
      int xByteOffset = column % 8;
        
      digitalWrite(CLK_OUT, LOW);
      digitalWrite(SERIAL_OUT, ((frameBuffer[row][xByte] >> (7 - xByteOffset)) & 0x01));
          
      delayNoUnits(70);
                
      digitalWrite(CLK_OUT, HIGH);
          
      delayNoUnits(70);
    }
    
    digitalWrite(OUT_DISABLE, LOW);
    delayMicroseconds(2000);
  }
  
}

void alarmWakeup(int sig_num)
{
  if (sig_num == SIGALRM)
  {
    sendDisplayData();
  }
}

void setup()
{
  memset(&frameBuffer, 0x00, GFX_WIDTH * GFX_HEIGHT);
  
  wiringPiSetup();
  
  pinMode(SERIAL_OUT, OUTPUT);
  pinMode(CLK_OUT, OUTPUT);
  pinMode(OUT_DISABLE, OUTPUT);
  pinMode(ROW_0, OUTPUT);
  pinMode(ROW_1, OUTPUT);
  pinMode(ROW_2, OUTPUT);
  pinMode(ROW_3, OUTPUT);
  pinMode(ROW_4, OUTPUT);
  pinMode(ROW_5, OUTPUT);
  pinMode(ROW_6, OUTPUT);
  digitalWrite(OUT_DISABLE, LOW);
  
  gfx_init();
  
  
  signal(SIGALRM, alarmWakeup);   
  ualarm(20000, 20000);
}

void lcd_clear()
{
  memset(&frameBuffer, 0x00, GFX_WIDTH * GFX_HEIGHT);
}

void lcd_write_pixel(uint x, uint y, uint colour)
{
  if(x < 0 || x > GFX_WIDTH - 1 || y < 0 || y > GFX_HEIGHT - 1)
  {
    return;
  }
  
  int xByte = (int)(x / 8);
  int xByteOffset = x % 8;
  
  frameBuffer[y][xByte] = (frameBuffer[y][xByte] & (~(0x80 >> xByteOffset))) | (colour << 7 - xByteOffset);
  
}

int main()
{
  setup(); 
  
  gfx_setTextSize(1);
  gfx_setTextColor(1, 0);
  
  
  while (true)
  {
    for (int i = 0; i < 5*12; i++)
    {
      //lcd_clear();
      //gfx_setCursor(i - 30, 0);
      //gfx_puts("memes");
      gfx_fillRect(0, 0, i, 7, 1);
      delayNoUnits(200000);
      //delayMicroseconds(1000000000);
    }
    
    
  }
  

  
  return 0;
  
}



