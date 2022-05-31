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

const char* Message1 = "GAME";
const char* Message2 = "END";
//-----------------------------------------------------------------------------
// 自作関数
//-----------------------------------------------------------------------------
// バッファの初期化
void buffer_all_clear()
{
  for(int i = 0;i < 8;i ++){
    for (int j = 0; j < 4; j++){
      Buffer[i][j] = B00000000;
    }
  }
}

// 任意の座標を光らせる
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

// 任意の座標のみ消す
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

// 2点間の距離を求める
double calcDistance(int x1, int y1, int x2, int y2)
{
  double distance;
  distance = sqrt((double)((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)));
  return distance;
}

// 鬼の行先を検証する
int decideDirection(int p_x, int p_y, int e_x, int e_y)
{
  // 右→0, 下→1, 左→2, 上→3
  int direction = 0;

  // まずは右に行った場合
  int x_r = e_x + 1;
  int y_r = e_y;
  double min_distance = calcDistance(p_x,p_y,x_r,y_r);

  // 下に行った場合
  int x_d = e_x;
  int y_d = e_y + 1;
  double distance_d = calcDistance(p_x,p_y,x_d,y_d);
  if(min_distance > distance_d){
    min_distance = distance_d;
    direction = 1;
  }

  // 左に行った場合
  int x_l = e_x - 1;
  int y_l = e_y;
  double distance_l = calcDistance(p_x,p_y,x_l,y_l);
  if(min_distance > distance_l){
    min_distance = distance_l;
    direction = 2;
  }

  // 上に行った場合
  int x_u = e_x;
  int y_u = e_y - 1;
  double distance_u = calcDistance(p_x,p_y,x_u,y_u);
  if(min_distance > distance_u){
    min_distance = distance_u;
    direction = 3;
  }

  return direction;
}

// プレイヤーが敵に捕まったか判定する関数
int is_catched(int p_x, int p_y, int e_x, int e_y, int flag){
  double distance = calcDistance(p_x,p_y,e_x,e_y);
  if(distance <= 1){
    flag = 1;
  }
  return flag;
}
//-----------------------------------------------------------------------------
// 自作パラメータ
//-----------------------------------------------------------------------------
// プレイヤーの座標
int player_x = 20, player_y = 4;

// 敵の座標
int enemy_x = 24, enemy_y = 2;

// 敵の移動間隔
int enemy_speed = 30;

// 敵の速度調整用のカウンター
int enemy_counter = enemy_speed;

// プレイヤーが敵に捕まったかどうか (0→まだ捕まってない,1→捕まった)
int game_flag = 0;

//-----------------------------------------------------------------------------
void setup()
{
  Serial.begin(9600);             // シリアル通信の準備をする
  while (!Serial);                  // 準備が終わるのを待つ
  Serial.println("start");    // シリアル通信でメッセージをPCに送信

  MyString.set( Message1, MatrixString::FILL );
  LedMatrix.clear();
  delay( 1000 );
  LedMatrix.show();
  delay( 3000 );
  LedMatrix.clear();
}
//-----------------------------------------------------------------------------
void loop()
{
  // バッファの初期化
  buffer_all_clear();

  // プレイヤーの座標変更
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

  //プレイヤーの描画
  drawPixel(player_x,player_y);

  enemy_counter -= 1;
  if(enemy_counter <= 0){
    // 敵の座標変更
    int direction = decideDirection(player_x,player_y,enemy_x,enemy_y);
    switch (direction)
    {
      case 0:  // 右
        if (enemy_x < 31){
          enemy_x += 1;
        }
        break;  

      case 1:  // 下
        if (enemy_y < 7){
          enemy_y += 1;
        }
        break;
      
      case 2:  // 左
        if(enemy_x > 0){
          enemy_x -= 1;
        }
        break;

      case 3:  // 上
        if(enemy_y > 0){
          enemy_y -= 1;
        }
        break;
    }
      enemy_counter = enemy_speed;
  }

  //敵の描画
  drawPixel(enemy_x,enemy_y);

  // 捕まったかどうか
  int is_end = is_catched(player_x,player_y,enemy_x,enemy_y,game_flag);
  if(is_end == 1){
    MyString.set( Message2, MatrixString::FILL );
  }
  
  LedMatrix.show();



  delay( 30 );
  
}
//-----------------------------------------------------------------------------
// Sketch end
//-----------------------------------------------------------------------------