#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

#include <spike/hub/system.h>

#include <linetrace.h>

#include "spike/pup/motor.h"
#include "spike/pup/colorsensor.h"
#include "spike/pup/forcesensor.h"
#include "spike/pup/ultrasonicsensor.h"

#include "spike/hub/battery.h"
#include "spike/hub/button.h"
#include "spike/hub/display.h"
#include "spike/hub/imu.h"
#include "spike/hub/light.h"
#include "spike/hub/speaker.h"

#include <pbio/color.h>

#include "kernel_cfg.h"
#include "syssvc/serial.h"

pup_device_t *colorC;

void Main(intptr_t exinf)
{
  pup_motor_t *motorA;
  pup_motor_t *motorB;
  pup_motor_t *motorE;
  pup_device_t* force1 = pup_force_sensor_get_device('D');
  colorC = pup_color_sensor_get_device(PBIO_PORT_ID_C);
  motorA = pup_motor_get_device('A');
  motorB = pup_motor_get_device('B');
  motorE = pup_motor_get_device('E');
  pup_motor_setup(motorA, PUP_DIRECTION_COUNTERCLOCKWISE, true);
  pup_motor_setup(motorB, PUP_DIRECTION_CLOCKWISE, true);
  pup_motor_setup(motorE, PUP_DIRECTION_CLOCKWISE, true);
  hub_speaker_set_volume(50);
  int target=55; 
  int base=40;
  float kp=0.5;
  int ref=0;
  float e=0;
  int dV=0;
  int VL=0;
  int VR=0;

  while(1){
    ref = pup_color_sensor_reflection(colorC);//ライトONで反射光の値を取得
    hub_display_number(ref);//ディスプレイに表示する
    e=target-ref; //目標値-反射光のデータ
    dV=kp*e;
    VL=base+dV;
    VR=base-dV;

    pup_motor_set_power(motorA, VL); // 比例制御
    pup_motor_set_power(motorB, VR); // 比例制御
    dly_tsk(10*1000);//0.1秒待機

    if (VL > VR){
      pup_motor_set_power(motorE, 30);
    }else{
      pup_motor_set_power(motorE, -30);
    }

    if (pup_force_sensor_touched(force1))//もしフォースセンサーが押されていたら
    {
      pup_motor_set_power(motorA, 0);
      pup_motor_set_power(motorB, 0);
      hub_speaker_play_tone(NOTE_C4,1000);//ドの音
      break;
    }
  }
  exit(0);
}
