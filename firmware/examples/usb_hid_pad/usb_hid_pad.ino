/* Please make sure your touch IC model. */
// 定义一个触摸型号，如果不清楚你使用的触摸型号，可以任意定义一个触摸型号，将下面两行中的一个取消掉，如果触摸不正常，那就换另一个试试
// 如果不定义,则禁用触摸功能
//!!!If you are unsure about the drive model, please switch the definition to try to see if the touch is normal

// #define TOUCH_MODULES_CST_MUTUAL    //Early use of CST328
#define TOUCH_MODULES_CST_SELF        //Use CST816 by default

#include "TouchLib.h"


#if ARDUINO_USB_MODE
#warning This sketch should be used when USB is in OTG mode
void setup() {}
void loop() {}
#else

#include "Arduino.h"
#include "OneButton.h"
#include "TFT_eSPI.h" /* Please use the TFT library provided in the library. */
#include "Wire.h"
#include "img_logo.h"
#include "pin_config.h"

#include "USB.h"
#include "USBHID.h"
#include "USBHIDMouse.h"

/* The product now has two screens, and the initialization code needs a small change in the new version. The LCD_MODULE_CMD_1 is used to define the
 * switch macro. */
#define LCD_MODULE_CMD_1

#warning Please confirm that you have purchased a display screen with a touch chip, otherwise the touch routine cannot be implemented.
#if defined(TOUCH_MODULES_CST_MUTUAL)
TouchLib touch(Wire, PIN_IIC_SDA, PIN_IIC_SCL, CTS328_SLAVE_ADDRESS, PIN_TOUCH_RES);
#elif defined(TOUCH_MODULES_CST_SELF)
TouchLib touch(Wire, PIN_IIC_SDA, PIN_IIC_SCL, CTS820_SLAVE_ADDRESS, PIN_TOUCH_RES);
#else
#error "Please choose the correct touch driver model!"
#endif

#define TOUCH_GET_FORM_INT 0

USBHIDMouse Mouse;
TFT_eSPI tft = TFT_eSPI();

#if TOUCH_GET_FORM_INT
bool get_int = false;
#endif
#if defined(LCD_MODULE_CMD_1)
typedef struct {
    uint8_t cmd;
    uint8_t data[14];
    uint8_t len;
} lcd_cmd_t;

lcd_cmd_t lcd_st7789v[] = {
    {0x11, {0}, 0 | 0x80},
    {0x3A, {0X05}, 1},
    {0xB2, {0X0B, 0X0B, 0X00, 0X33, 0X33}, 5},
    {0xB7, {0X75}, 1},
    {0xBB, {0X28}, 1},
    {0xC0, {0X2C}, 1},
    {0xC2, {0X01}, 1},
    {0xC3, {0X1F}, 1},
    {0xC6, {0X13}, 1},
    {0xD0, {0XA7}, 1},
    {0xD0, {0XA4, 0XA1}, 2},
    {0xD6, {0XA1}, 1},
    {0xE0, {0XF0, 0X05, 0X0A, 0X06, 0X06, 0X03, 0X2B, 0X32, 0X43, 0X36, 0X11, 0X10, 0X2B, 0X32}, 14},
    {0xE1, {0XF0, 0X08, 0X0C, 0X0B, 0X09, 0X24, 0X2B, 0X22, 0X43, 0X38, 0X15, 0X16, 0X2F, 0X37}, 14},
};
#endif
OneButton left_button(PIN_BUTTON_1, true);
OneButton right_button(PIN_BUTTON_2, true);
void setup()
{
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);

    pinMode(PIN_TOUCH_RES, OUTPUT);
    digitalWrite(PIN_TOUCH_RES, LOW);
    delay(500);
    digitalWrite(PIN_TOUCH_RES, HIGH);

    Serial.begin(115200);
    Serial.println("Hello T-Display-S3");

    tft.begin();

#if defined(LCD_MODULE_CMD_1)
    for (uint8_t i = 0; i < (sizeof(lcd_st7789v) / sizeof(lcd_cmd_t)); i++) {
        tft.writecommand(lcd_st7789v[i].cmd);
        for (int j = 0; j < lcd_st7789v[i].len & 0x7f; j++) {
            tft.writedata(lcd_st7789v[i].data[j]);
        }

        if (lcd_st7789v[i].len & 0x80) {
            delay(120);
        }
    }
#endif
    tft.setRotation(3);
    tft.pushImage(0, 0, 320, 170, (uint16_t *)img_logo);

    ledcSetup(0, 2000, 8);
    ledcAttachPin(PIN_LCD_BL, 0);
    ledcWrite(0, 255);

    delay(2000);

    tft.setTextSize(2);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);

    Wire.begin(PIN_IIC_SDA, PIN_IIC_SCL);
    if (!touch.init()) {
        Serial.println("Touch IC not found");
    }
    touch.setRotation(1);

    left_button.attachClick([] { Mouse.click(MOUSE_LEFT); });
    right_button.attachClick([] { Mouse.click(MOUSE_RIGHT); });
    tft.drawString("Please touch the screen to ", 0, 15);
    tft.drawString("simulate mouse sliding.", 0, 40);
#if TOUCH_GET_FORM_INT
    attachInterrupt(
        PIN_TOUCH_INT, [] { get_int = true; }, FALLING);
#endif

    Mouse.begin();
    USB.begin();
}

void loop()
{
    char str_buf[50];
    static bool first_touch;
    static uint16_t last_x, last_y;
    static uint32_t Mill;
#if TOUCH_GET_FORM_INT
    if (get_int) {
        get_int = 0;
        touch.read();
#else
    if (touch.read()) {
#endif
        uint8_t n = touch.getPointNum();
        TP_Point t = touch.getPoint(0);
        if (!first_touch) {
            Mouse.move(int(t.x - last_x), int(t.y - last_y));
        }
        last_x = t.x;
        last_y = t.y;
        first_touch = 0;
        Mill = millis();
    } else {
        if (millis() - Mill > 50)
            first_touch = 1;
    }

    left_button.tick();
    right_button.tick();
    delay(5);
}

#endif



// TFT Pin check
#if PIN_LCD_WR  != TFT_WR || \
    PIN_LCD_RD  != TFT_RD || \
    PIN_LCD_CS    != TFT_CS   || \
    PIN_LCD_DC    != TFT_DC   || \
    PIN_LCD_RES   != TFT_RST  || \
    PIN_LCD_D0   != TFT_D0  || \
    PIN_LCD_D1   != TFT_D1  || \
    PIN_LCD_D2   != TFT_D2  || \
    PIN_LCD_D3   != TFT_D3  || \
    PIN_LCD_D4   != TFT_D4  || \
    PIN_LCD_D5   != TFT_D5  || \
    PIN_LCD_D6   != TFT_D6  || \
    PIN_LCD_D7   != TFT_D7  || \
    PIN_LCD_BL   != TFT_BL  || \
    TFT_BACKLIGHT_ON   != HIGH  || \
    170   != TFT_WIDTH  || \
    320   != TFT_HEIGHT
#error  "Error! Please make sure <User_Setups/Setup206_LilyGo_T_Display_S3.h> is selected in <TFT_eSPI/User_Setup_Select.h>"
#error  "Error! Please make sure <User_Setups/Setup206_LilyGo_T_Display_S3.h> is selected in <TFT_eSPI/User_Setup_Select.h>"
#error  "Error! Please make sure <User_Setups/Setup206_LilyGo_T_Display_S3.h> is selected in <TFT_eSPI/User_Setup_Select.h>"
#error  "Error! Please make sure <User_Setups/Setup206_LilyGo_T_Display_S3.h> is selected in <TFT_eSPI/User_Setup_Select.h>"
#endif

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5,0,0)
#error  "The current version is not supported for the time being, please use a version below Arduino ESP32 3.0"
#endif