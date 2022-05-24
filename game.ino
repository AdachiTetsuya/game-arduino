//-----------------------------------------------------------------------------
#include <MaxMatrix2.h>
#include "Font7x5.h"
#include "Font5x3.h"
#include "MatrixString.h"
#include <ResKeypad.h> // library for keypad
//-----------------------------------------------------------------------------
// Arduino pin numbers
//-----------------------------------------------------------------------------
const int DATA_PIN = 8;
const int LOAD_PIN = 9;
const int CLOCK_PIN = 10;
int ain=A4;
ResKeypad keypad(ain,16,RESKEYPAD_4X4,RESKEYPAD_4X4_SIDE_A); // enable this line if SIDE A of 4X4 keypad is used
//-----------------------------------------------------------------------------
// Font setup
//-----------------------------------------------------------------------------
Font7x5 MyFont;  // <--- large font (7x5)
//Font5x3 MyFont ;  // <--- small font (5x3)
//-----------------------------------------------------------------------------
// Matrix buffer setup
//-----------------------------------------------------------------------------
const int NCELL = 4;
const int NDOT_LINE = NCELL * 8;
byte Buffer[ MaxMatrix2::NROW ][ NCELL ];  // byte型の 8✖︎4 の配列「Buffer」を定義
byte* Address[ MaxMatrix2::NROW ] =     
{
  &Buffer[0][0], &Buffer[1][0], &Buffer[2][0], &Buffer[3][0],
  &Buffer[4][0], &Buffer[5][0], &Buffer[6][0], &Buffer[7][0],
};                                           // byte*型の 長さ 8 の配列
//-----------------------------------------------------------------------------
// LED Matrix (MaxMatrix2) setup
//-----------------------------------------------------------------------------
MaxMatrix2 LedMatrix( DATA_PIN, LOAD_PIN, CLOCK_PIN, NCELL, Address );
//-----------------------------------------------------------------------------
// Matrix string setup
//-----------------------------------------------------------------------------
MatrixString MyString( MyFont, NCELL, MaxMatrix2::NROW, Address, NCELL );

const char* Message = "GAME";

//-----------------------------------------------------------------------------
// Sketch main
//-----------------------------------------------------------------------------

void buffer_all_clear()
{
  for(int i = 0;i < 8;i ++){
    for (int j = 0; j < 4; j++){
      Buffer[i][j] = B00000000;
    }
  }
}

void drawPixel(int x, int y)
{
  int cell=0,shift=0;

  if((x>=0 && y>=0)||(x<=31 && y<=31))
  {
    cell = x/8;
    shift = x%8;
    Buffer[y][cell] |= B10000000 >> shift;
  }
}

void erasePixel(int x,int y)
{
  int cell=0,shift=0;

  if((x>=0 && y>=0)||(x<=31 && y<=31))
  {
    cell = x/8;
    shift = x%8;
    Buffer[y][cell] &= ~(B10000000) >> shift;
  }
}
//-----------------------------------------------------------------------------
// プレイヤーの座標
int player_x = 20;
int player_y = 4;
//-----------------------------------------------------------------------------
void setup()
{
  Serial.begin(9600);             // シリアル通信の準備をする
  while (!Serial);                  // 準備が終わるのを待つ
  Serial.println("start");    // シリアル通信でメッセージをPCに送信

  MyString.set( Message, MatrixString::FILL );
  LedMatrix.clear();
  delay( 1000 );
  LedMatrix.show();
  delay( 3000 );
  LedMatrix.clear();
}
//-----------------------------------------------------------------------------
void loop()
{
  signed char key;
  key=keypad.GetKey();  // key=11:ボタン4,key=10:ボタン5,key=9:ボタン6,key=8:ボタン7に対応
  switch (key)
  {
    case 11:  // 左
      if (player_x > 0){
        player_x -= 1;
      }
      break;  

    case 10:  // 上
      if (player_y > 0){
        player_y -= 1;
      }
      break;
    
    case 9:  // 下
      if(player_y < 7){
        player_y += 1;
      }
      break;

    case 8:  // 左
      if(player_x < 31){
        player_x += 1;
      }
      break;
  }
  
  buffer_all_clear();

  drawPixel(player_x,player_y);

  LedMatrix.show();

  delay( 30 );
  
}
//-----------------------------------------------------------------------------
// Sketch end
//-----------------------------------------------------------------------------