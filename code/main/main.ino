
1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21
22
23
24
25
26
27
28
29
30
31
32
33
34
35
36
37
38
39
40
41
42
43
44
45
46
47
48
49
50
51
52
53
54
55
56
57
58
59
60
61
62
63
64
65
66
67
68
69
70
71
72
73
74
75
76
77
78
79
80
81
82
83
84
85
86
87
88
89
90
91
92
93
94
95
96
97
98
99
100
101
102
103
104
105
106
107
108
109
110
111
112
113
114
115
116
117
118
119
120
121
122
123
124
125
126
127
128
129
130
131
132
133
134
135
136
137
138
139
140
141
142
143
144
145
146
147
148
149
150
151
152
153
154
155
156
157
158
159
160
161
162
163
164
165
166
167
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SPI.h>
#include <Wire.h>
#include "MQ135.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
 
char auth[] = "**************************";    // You should get Auth Token in the Blynk App.
char ssid[] = "**************************";                           // Your WiFi credentials.
char pass[] = "**************************";
 
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
 
#define DHTTYPE DHT11     // DHT 11
#define DHTPIN D4
 
#define relay_fan D5
#define relay_light D6
#define relay_fridge D7
#define relay_oven D8
 
#define buzzer_alarm D0
#define pir_human D3
 
 
int alarm_status;
int pir_status = 0;
 
DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
 
 
void setup()
{
  Serial.begin(115200);
  dht.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //initialize with the I2C addr 0x3C (128x64)
  Blynk.begin(auth, ssid, pass);
 
  pinMode(pir_human, INPUT);
  pinMode(buzzer_alarm, OUTPUT);
 
  pinMode(relay_fan, OUTPUT);
  pinMode(relay_light, OUTPUT);
  pinMode(relay_fridge, OUTPUT);
  pinMode(relay_oven, OUTPUT);
 
  digitalWrite(buzzer_alarm, LOW);
 
  digitalWrite(relay_fan, HIGH);
  digitalWrite(relay_light, HIGH);
  digitalWrite(relay_fridge, HIGH);
  digitalWrite(relay_oven, HIGH);
  delay(100);
}
 
 
void loop()
{
  Blynk.run();
  MQ135 gasSensor = MQ135(A0);
  float air_quality = gasSensor.getPPM();
 
  float t = dht.readTemperature();
  float h = dht.readHumidity();
 
  pir_status = digitalRead(pir_human);
  alarm_status = digitalRead(buzzer_alarm);
 
 
  if (pir_status == 1)
  {
    Serial.println("Person Detected");
  }
  else if (pir_status == 0)
  {
    Serial.println("No One in Room");
  }
 
 
  if (air_quality > 150)
  {
    digitalWrite(buzzer_alarm, HIGH);
    digitalWrite(relay_fan, LOW);
    Serial.println("Buzzer Status: ON");
    Serial.println("Exhaust Fan: ON");
  }
  else
  {
    digitalWrite(buzzer_alarm, LOW);
    digitalWrite(relay_fan, HIGH);
    Serial.println("Buzzer Status: OFF");
    Serial.println("Exhaust Fan: OFF");
  }
 
  Serial.print("Air Quality: ");
  Serial.print(air_quality);
  Serial.println(" PPM");
 
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C");
 
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" %");
 
  Serial.println();
  Serial.println("****************************");
  Serial.println();
 
  Blynk.virtualWrite(V1, t);             // For Temperature
  Blynk.virtualWrite(V2, h);             // For Humidity
  Blynk.virtualWrite(V3, air_quality);   // For Gas
  Blynk.virtualWrite(V4, alarm_status);  // For Alarm & Exhaust Fan
  Blynk.virtualWrite(V5, pir_status);    // For Human Detection
 
  display.clearDisplay();
  display.setCursor(0, 0); //oled display
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println("Air Quality Index");
 
  display.setCursor(0, 20); //oled display
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print(air_quality);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println(" PPM");
  display.display();
  delay(1500);
 
  display.clearDisplay();
 
  // display temperature
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Temperature: ");
  display.setTextSize(2);
  display.setCursor(0, 10);
  display.print(t);
  display.print(" ");
  display.setTextSize(1);
  display.cp437(true);
  display.write(167);
  display.setTextSize(2);
  display.print("C");
 
  // display humidity
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print("Humidity: ");
  display.setTextSize(2);
  display.setCursor(0, 45);
  display.print(h);
  display.print(" %");
 
  display.display();
  delay(1500);
}