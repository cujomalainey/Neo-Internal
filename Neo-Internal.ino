#include "Adafruit_NeoPixel.h"

// Which pin on the Arduino is connected to the NeoPixels?
#define LEDPIN         10

// Which pin on the Arduino is connected to the Power Line?
#define POWERPIN         12

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      60

// Define the fastest possible color change frequency
#define FREQ_MAX       150

// Change this number to get different flow patterns 
// you may also change this to a variable that is randomly generated to get a pattern each run
#define RNGSEED        665

// define the default maximum color brightness (between 2 and 128)
int amplitude = 128;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

uint32_t spot = 0;
int w_red,p_red,w_green,p_green,w_blue,p_blue;

void setup() {
  pixels.begin(); // This initializes the NeoPixel library.
  randomSeed(RNGSEED);
  w_red = random(FREQ_MAX);
  p_red = random();
  w_green = random(FREQ_MAX);
  p_green = random();
  w_blue = random(FREQ_MAX);
  p_blue = random();
  pinMode(POWERPIN, INPUT);
}

void loop() {
  if (digitalRead(POWERPIN) == HIGH) {
    for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i, 
        pixels.Color(
          (amplitude - 1)*sin((float)w_red*(float)(spot-i)/1800.0 + (float)p_red) + amplitude,
          (amplitude - 1)*sin((float)w_green*(float)(spot-i)/1800.0 + (float)p_green) + amplitude,
          (amplitude - 1)*sin((float)w_blue*(float)(spot-i)/1800.0 + (float)p_blue) + amplitude)
        ); 
    }
    pixels.show();
    spot++;
  }
  else {
    for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
    pixels.show();
    delay(1000);
  }
}

/*
#include "Adafruit_NeoPixel.h"

#define COLORPIN  6
#define NUMPIXELS 60
#define DISPLAYS  3

int current_color[DISPLAYS][3];
int target_color[DISPLAYS][3];
int display_pixels[DISPLAYS];
char message[60];
uint8_t str_postion = 0;
uint16_t red;
uint16_t green;
uint16_t blue;
uint16_t display;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, COLORPIN, NEO_GRB + NEO_KHZ800);

void setup_displays()
{
  int sections = NUMPIXELS/(DISPLAYS - 1);
  display_pixels[0] = 0;
  for (int i = 1; i < DISPLAYS; i++)
  {
    display_pixels[i] = display_pixels[i - 1] + sections;
  }
  display_pixels[DISPLAYS - 1] = NUMPIXELS - 1;
}

bool pixel_change(int d)
{
  return (current_color[d][0] != target_color[d][0] || current_color[d][1] != target_color[d][1] || current_color[d][2] != target_color[d][2]);
}

int get_display(int p)
{
  for (int i = 0; i < DISPLAYS; i++)
  {
    if (p == display_pixels[i])
      return i;
  }
  return -1;
}

void update_pixel(int d)
{
  for (int i; i < 3; i++)
  {
    if (target_color[d][i] > current_color[d][i])
    {
      current_color[d][i]++;
    } 
    else if (target_color[d][i] < current_color[d][i])
    {
      current_color[d][i]--;
    }
  }
}

bool is_pixel(int p)
{
  for (int i = 0; i < DISPLAYS; i++)
  {
    if (p == display_pixels[i])
    {
      return true;
    }
  }
  return false;
}

int get_leading_pixel(int p)
{
  for (int i = 0; i < DISPLAYS; i++)
  {
    if (p < display_pixels[i])
    {
      return i;
    }
  }
}

void setup() {
    Serial.begin(9600);
    pixels.begin();
    delay(5000);
    setup_displays();
}

void loop() {
  while (Serial.available() > 0)
  {
    message[str_postion] = Serial.read();
    if (message[str_postion] == '\n')
    {
      message[str_postion + 1] = '\0';
      str_postion = 0;
      if (message[0] == 'T')
      {
        Serial.print("GREEN!\n");
      }
      else if (sscanf(message, "(%u, %u, %u, %u)\n", &red, &green, &blue, &display) == 4)
      {
        target_color[display][0] = (uint8_t)red;
        target_color[display][1] = (uint8_t)green;
        target_color[display][2] = (uint8_t)blue;
      }
      else
      {
        Serial.print("PARSE\n");
      }
    }
    else
    {
      str_postion++;
    }
  }
  delay(10);
  if (DISPLAYS == 1)
  {
    if (pixel_change(0))
    {
      update_pixel(0);
      for (int i; i < NUMPIXELS; i++)
      {
        pixels.setPixelColor(i, pixels.Color(current_color[0][0], current_color[0][1], current_color[0][2]));
      }
      pixels.show();
    }
  }
  else 
  {
    bool change = false;
    for (int d = 0; d < DISPLAYS; d++)
    {
      if (pixel_change(d))
      {
        update_pixel(d);
        change = true;
      }
    }
    if (change)
    {
      for (int p = 0; p < NUMPIXELS; p++)
      {
        if (is_pixel(p))
        {
          int d = get_display(p);
          pixels.setPixelColor(p, pixels.Color(current_color[d][0], current_color[d][1], current_color[d][2]));
        }
        else
        {
          int d = get_leading_pixel(p);
          double leading;
          double trailing;
          double l_factor = 1 - ((double)display_pixels[d] - (double)p)/(double)NUMPIXELS;
          double t_factor = 1 - ((double)p - (double)display_pixels[d-1])/(double)NUMPIXELS;
          int colors[3];
          for (int c = 0; c < 3; c++)
          {
            colors[c] = (int)(l_factor*(double)current_color[d][c] + t_factor*(double)current_color[d - 1][c]);
          }
          pixels.setPixelColor(p, pixels.Color(colors[0], colors[1], colors[2]));
        }
      }
      pixels.show();
    }
  }
}*/