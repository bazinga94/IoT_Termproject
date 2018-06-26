#include <SoftwareSerial.h>
#include <Servo.h>  //라이브러리 불러오기
int pos = 0;
Servo servo_9;
Servo servo_10;
SoftwareSerial BTSerial(4, 5);
byte LEFT = 'a';
byte RIGHT = 'b';
byte distance_MODE = 'c';
byte button_MODE = 'd';

const int TriggerPin = 10; //Trig pin
const int EchoPin = 11; //Echo pin
long Duration = 0;
long Distance_temp = 0;
long Distance_mm = 0;

int mode = 1;
int flag1 = 0;
int flag2 = 0;

int left(Servo servo_9){ //-90도 만큼 회전하고 원래 자리로 되돌아 온다.
    for(pos = 90;pos>=0;pos-=1){
        servo_9.write(pos); //pos 각도만큼 이동시키고
        delay(2);  //약간의 delay를 준다 (ms)
    }
    for(pos = 0;pos<=90;pos+=1){
      servo_9.write(pos);
      delay(2);
    }


  }
int right(Servo servo_9){  //90도 만큼 회전하고 원래 자리로 되돌아 온다.
    for(pos = 90;pos<=180;pos+=1){
        servo_9.write(pos);
        delay(2);
    }
    for(pos = 180;pos>=90;pos-=1){
        servo_9.write(pos);
    }

  }
long Distance(long time)
{
  // Calculates the Distance in mm
  // ((time)*(Speed of sound))/ toward and backward of object) * 10

  long DistanceCalc; // Calculation variable
  DistanceCalc = ((time / 2.9) / 2); //초음차가 되돌아 오는데 걸린 시간을 거리로 환산해 준다.
  //DistanceCalc = time / 74 / 2; // inch로 계산하는 경우
  return DistanceCalc; // return calculated value
}   



void setup() {
  pinMode(TriggerPin, OUTPUT); // output TriggerPin
  pinMode(EchoPin, INPUT); // input EchoPin
  BTSerial.begin(9600);  // 블루투스 시리얼통신을 시작한다. 속도는 9600 same Serial3.begin(9600, SERIAL_8N1)
  Serial.begin(9600);  // PC의 시리얼통신을 시작한다. 
  servo_9.attach(9);   // 서보모터 9번 Pin

}
  

void loop() {

if (BTSerial.available()){   //블루투스 통신이 가능하면
    byte data = BTSerial.read();   //블루투스 시리얼 통신으로 데이터를 수신한다.
    if (data == distance_MODE){   //'c'를 수신하면 distance mode
      mode = 0;
    }
    if (data == button_MODE){   //'d'를 수신하면 button mode
      mode = 1;
    }
}
if (mode == 0){
  digitalWrite(TriggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(TriggerPin, HIGH); // Trigger pin to HIGH
  delayMicroseconds(10); // 10us high
  digitalWrite(TriggerPin, LOW); // Trigger pin to HIGH

  Duration = pulseIn(EchoPin, HIGH); // Waits for the echo pin to get high
  // Duration 을 microseconds 단위로 받는다.

  Distance_mm = Distance(Duration); // distance를 계산하는 함수


  Serial.print("Distance = "); // Output to serial
  Serial.print(Distance_mm);
  Serial.println(" mm");
 
  Serial.print("Distance_temp = "); // Output to serial
  Serial.print(Distance_temp);
  Serial.println(" mm");
  Serial.println("\n");
  Serial.println(Distance_temp - Distance_mm);

  if (Distance_temp ==0){ // 제일 처음 distance 센서 켰을때 모터가 돌아가는것 해결
    Distance_temp = Distance_mm; 
  }

  if( flag2 == 0 && flag1 == 0 && Distance_temp - Distance_mm >= 500){ // 인식하는 대상이 가까워질때
        left(servo_9);  //servo 모터를 왼쪽으로 회전시킨다.(-90)
        Distance_temp = Distance_mm;   //Distance_temp에는 이전에 측정한 거리를 저장한다. 
        delay(5000);
        flag1 = 1;
       
      }
  
   else if( flag2 == 0 && flag1 == 0 && Distance_temp - Distance_mm <= -500){ // 인식하는 대상이 멀어질때
        right(servo_9);  //servo 모터를 오른쪽으로 회전시킨다.(+90)
        Distance_temp = Distance_mm;
        delay(5000);
        flag2 = 1;
        
    }
    else {
      flag1 = 0;
      flag2 = 0;
      Distance_temp = Distance_mm;
    }
   delay(1000);
}
if (mode == 1){  //button으로 동작하는 mode일때 

  if (BTSerial.available()){  //블루투스 통신이 가능하면
    byte data = BTSerial.read();  //블루투스 시리얼 통신으로 데이터를 수신한다. 
    Serial.write(data);
    if ((data == LEFT)){   //'a' 수신시 
    left(servo_9);  //왼쪽으로 회전(-90)
    }
    else if ((data == RIGHT)){   //'b' 수신시
    right(servo_9);  //오른쪽으로 회전(90)
    }
    
  }
  }
}

