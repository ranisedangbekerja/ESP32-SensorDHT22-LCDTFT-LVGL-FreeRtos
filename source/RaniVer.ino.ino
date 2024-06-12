//Rani Nirmala Prakoso
//22/493982/TK/54153

//Pembuatan Program ESP32 Menggunakan Sensor DHT dan Layar LCD TFT
// Tugas ini dibuat untuk memenuhi Tugas Individu dalam Mata kuliah Sistem Berbasis Microkontroler

#include <lvgl.h>
#include <TFT_eSPI.h>
#include <DHT.h>

#define TFT_HOR_RES 320
#define TFT_VER_RES 240

void *draw_buf_1;
unsigned long lastTickMillis = 0;
unsigned long lastAnalogReadMillis = 0;
const unsigned long analogReadInterval = 500;
const int analogPin = 34;
const int tempSensorPin = 32;
const int ledPin = 21; // LED pin
#define DHTTYPE DHT22


DHT dht(tempSensorPin, DHTTYPE);

TFT_eSPI tft = TFT_eSPI();

float tempValue;

static lv_display_t *disp;
static lv_obj_t *mainPage, *tempPage;
static lv_obj_t *temp_arc, *temp_label;
static lv_obj_t *led_btn; // Button for controlling LED

#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))

void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data) {
    uint16_t x = 0, y = 0;
    bool pressed = tft.getTouch(&x, &y);

    if (pressed) {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = x;
        data->point.y = y;
        Serial.print("Data x ");
        Serial.println(x);
        Serial.print("Data y ");
        Serial.println(y);
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

static void temp_btn_event_cb(lv_event_t *e) {
    lv_scr_load(tempPage);
}

// Callback function for controlling LED
static void led_btn_event_cb(lv_event_t *e) {
    static bool ledState = false; // State of the LED
    ledState = !ledState; // Toggle LED state
    digitalWrite(ledPin, ledState ? HIGH : LOW); // Turn LED on or off

    // Update button label based on LED state
    lv_obj_t *btn = (lv_obj_t *)lv_event_get_target(e);
    if (btn) {
        lv_obj_t *label = lv_obj_get_child(btn, NULL);
        if (label) {
            lv_label_set_text(label, ledState ? "Turn Off LED" : "Turn On LED");
        }
    }
}

static void back_to_main_event_cb(lv_event_t *e) {
    lv_scr_load(mainPage);
}

void create_temp_page() {
    tempPage = lv_obj_create(NULL);
    if (!tempPage) {
        Serial.println("Failed to create tempPage");
        return;
    }

    temp_arc = lv_arc_create(tempPage);
    lv_obj_set_size(temp_arc, 150, 150);
    lv_obj_align(temp_arc, LV_ALIGN_CENTER, 0, 0);
    lv_arc_set_rotation(temp_arc, 270);
    lv_arc_set_bg_angles(temp_arc, 0, 360);
    lv_arc_set_angles(temp_arc, 0, 0);
    lv_arc_set_range(temp_arc, 0, 50);

    temp_label = lv_label_create(tempPage);
    lv_obj_align(temp_label, LV_ALIGN_CENTER, 0, 20);

    lv_obj_t *temp_back_btn = lv_btn_create(tempPage);
    if (!temp_back_btn) {
        Serial.println("Failed to create tempPage back button");
        return;
    }
    lv_obj_set_size(temp_back_btn, 100, 50);
    lv_obj_align(temp_back_btn, LV_ALIGN_CENTER, 0, 120);

    lv_obj_t *temp_back_label = lv_label_create(temp_back_btn);
    if (!temp_back_label) {
        Serial.println("Failed to create tempPage back button label");
        return;
    }
    lv_label_set_text(temp_back_label, "Back");
    lv_obj_center(temp_back_label);
    lv_obj_add_event_cb(temp_back_btn, back_to_main_event_cb, LV_EVENT_CLICKED, NULL);
}

void lv_example_event_1(void) {
    mainPage = lv_obj_create(NULL);
    if (!mainPage) {
        Serial.println("Failed to create mainPage");
        return;
    }

    static lv_style_t style_bg;
    lv_style_init(&style_bg);
    lv_style_set_bg_opa(&style_bg, LV_OPA_COVER);
    lv_style_set_bg_color(&style_bg, lv_color_hex(0xFDB0C0));
    lv_style_set_bg_grad_color(&style_bg, lv_color_hex(0xFDB0C0));
    lv_style_set_bg_grad_dir(&style_bg, LV_GRAD_DIR_VER);

    lv_obj_t * bg = lv_obj_create(mainPage);
    lv_obj_add_style(bg, &style_bg, 0);
    lv_obj_set_size(bg, LV_HOR_RES, LV_VER_RES);
    lv_obj_center(bg);

    static lv_style_t style_btn;
    lv_style_init(&style_btn);
    lv_style_set_radius(&style_btn, 10);
    lv_style_set_bg_opa(&style_btn, LV_OPA_COVER);
    lv_style_set_bg_color(&style_btn, lv_color_hex(0xFF0000));
    lv_style_set_border_color(&style_btn, lv_color_hex(0xFDB0C0));
    lv_style_set_border_width(&style_btn, 2);
    lv_style_set_text_color(&style_btn, lv_color_hex(0x000000));
    lv_style_set_shadow_width(&style_btn, 10);
    lv_style_set_shadow_color(&style_btn, lv_color_hex(0xFFC0CB));


    static lv_style_t style_text;
    lv_style_init(&style_text);
    lv_style_set_text_color(&style_text, lv_color_hex(0x000000)); 

    lv_obj_t * text_1 = lv_label_create(mainPage);
    lv_obj_add_style(text_1, &style_text, 0);
    lv_label_set_text(text_1, "Welcome to Rani's Program");
    lv_obj_align(text_1, LV_ALIGN_TOP_MID, 0, 10);
    create_temp_page();

    lv_obj_t * text = lv_label_create(mainPage);
    lv_obj_add_style(text, &style_text, 0);
    lv_label_set_text(text, "22/493982/TK/54153");
    lv_obj_align(text, LV_ALIGN_TOP_MID, 0, 30);
    create_temp_page(); 



    lv_scr_load(mainPage);

    lv_obj_t *temp_btn = lv_btn_create(mainPage);
    if (!temp_btn) {
        Serial.println("Failed to create temp_btn");
        return;
    }
    lv_obj_set_size(temp_btn, 150, 50);
    lv_obj_align(temp_btn, LV_ALIGN_CENTER, 0, -30);

    lv_obj_t *temp_label = lv_label_create(temp_btn);
    if (!temp_label) {
        Serial.println("Failed to create temp_label");
        return;
    }
    lv_label_set_text(temp_label, "Show Temperature");
    lv_obj_center(temp_label);
    lv_obj_add_event_cb(temp_btn, temp_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_style(temp_btn, &style_btn, 0);

    led_btn = lv_btn_create(mainPage);
    if (!led_btn) {
        Serial.println("Failed to create LED button");
        return;
    }
    lv_obj_set_size(led_btn, 150, 50);
    lv_obj_align(led_btn, LV_ALIGN_CENTER, 0, 30);

    lv_obj_t *led_label = lv_label_create(led_btn);
    if (!led_label) {
        Serial.println("Failed to create LED button label");
        return;
    }
    lv_label_set_text(led_label, "Turn On LED");
    lv_obj_center(led_label);
    lv_obj_add_event_cb(led_btn, led_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_style(led_btn, &style_btn, 0);
}

void update_temp_arc(int value){
    lv_arc_set_value(temp_arc, (int)value);
    char buf[32];

    if (value < 16) {
        lv_obj_set_style_arc_color(temp_arc, lv_color_make(0, 0, 255), LV_PART_INDICATOR);
        snprintf(buf, sizeof(buf), "%d%s", value, "°");
        lv_label_set_text(temp_label, buf);
    } else if (value < 29){
        snprintf(buf, sizeof(buf), "%d%s", value, "°");
        lv_obj_set_style_arc_color(temp_arc, lv_color_make(0, 255, 0), LV_PART_INDICATOR);
        lv_label_set_text(temp_label, buf);
    }  else{
        snprintf(buf, sizeof(buf), "%d%s", value, "°");
        lv_obj_set_style_arc_color(temp_arc, lv_color_make(255, 0, 0), LV_PART_INDICATOR);
        lv_label_set_text(temp_label, buf);
    }
}

void dht_sensor_task(void *pyParameters)
{
    while(true)
    {
        tempValue = dht.readTemperature();
        Serial.println(tempValue);
        vTaskDelay(pdMS_TO_TICKS(analogReadInterval));
    }
}

void ui_task(void *pyParameters)
{
    while(true)
    {
            // LVGL Tick Interface
    unsigned int tickPeriod = millis() - lastTickMillis;
    lv_tick_inc(tickPeriod);
    lastTickMillis = millis();

    // LVGL Task Handler
    lv_task_handler();

    if (millis() - lastAnalogReadMillis >= analogReadInterval) {
        int analogValue = analogRead(analogPin);


        // Update the temperature arc
        if (tempPage && temp_arc) {
            update_temp_arc((int)tempValue);
        }

        lastAnalogReadMillis = millis();
    }
    vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void setup() {
    Serial.begin(115200);

    // Initialize LVGL and DHT sensor
    lv_init();
    dht.begin();

    // Initialize TFT display
    draw_buf_1 = heap_caps_malloc(DRAW_BUF_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    if (!draw_buf_1) {
        Serial.println("Failed to allocate draw buffer");
        return;
    }
    disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf_1, DRAW_BUF_SIZE);
    if (!disp) {
        Serial.println("Failed to create display");
        return;
    }

    // Initialize touchpad input device
    lv_indev_t *indev = lv_indev_create();
    if (!indev) {
        Serial.println("Failed to create input device");
        return;
    }
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touchpad_read);
    
    // Set up LED pin
    pinMode(ledPin, OUTPUT);

    // Create UI
    lv_example_event_1();
    bool ledstate = false;
    xTaskCreate(dht_sensor_task, "Temp Task", 4096, NULL, 2, NULL);
    xTaskCreate(ui_task, "UI Task", 4096, NULL, 2, NULL);
}


void loop() {}

