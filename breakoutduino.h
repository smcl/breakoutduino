// maybe can ditch and use width/height
#define MAX_X 128
#define MAX_Y 64

// bumper
#define bumper_y 56
unsigned bumper_x;
unsigned bumper_mul;

// setup display
#define OLED_DC     6
#define OLED_CS     7
#define OLED_RESET  8
Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);


