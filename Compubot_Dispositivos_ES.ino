#include "esp_camera.h" // Librería para controlar la cámara ESP32-CAM
#include <Arduino.h>
#include "esp_heap_caps.h" //Acceso a funciones avanzadas de gestión de memoria ESP32-CAM
#include <Compubot_Entrada_Salida_inferencing.h>  // Librería externa generada por Edge Impulse (modelo int8)

// Pines para botones, LEDs y buzzer
#define BUTTON_ENTRADA 2
#define BUTTON_SALIDA  13
#define LED_VERDE      14
#define LED_ROJO       12
#define BUZZER_PIN     15  

bool yaProcesado = false;

int get_image_signal(ei::ei_signal_t *signal, camera_fb_t *fb); //Prototipo de función para preparar la imagen del modelo con la cámara ESP32-CAM

// Configuración e inicialización con la cámara ESP32-CAM (modelo AI Thinker)
bool initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = 5;
  config.pin_d1       = 18;
  config.pin_d2       = 19;
  config.pin_d3       = 21;
  config.pin_d4       = 36;
  config.pin_d5       = 39;
  config.pin_d6       = 34;
  config.pin_d7       = 35;
  config.pin_xclk     = 0;
  config.pin_pclk     = 22;
  config.pin_vsync    = 25;
  config.pin_href     = 23;
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
  config.pin_pwdn     = 32;
  config.pin_reset    = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_GRAYSCALE; // Imagen en escala de grises previamente configurada con Edge Impulse 
  config.frame_size   = FRAMESIZE_96X96; // Tamaño de la imagen
  config.fb_count     = 1;
  config.fb_location  = CAMERA_FB_IN_DRAM;   // Uso de RAM interna, no PSRAM
  config.grab_mode    = CAMERA_GRAB_WHEN_EMPTY;

  return esp_camera_init(&config) == ESP_OK; // Retorna si la cámara se inicializó correctamente
}

void setup() {
  Serial.begin(115200);

  // Configuración de pines y buzzer
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_ROJO, LOW);
  digitalWrite(BUZZER_PIN, LOW);  
  delay(50);

  // Configurar botones
  pinMode(BUTTON_ENTRADA, INPUT_PULLDOWN);
  pinMode(BUTTON_SALIDA, INPUT_PULLDOWN);

  // Encendido de cámara y para si hay algún fallo
  if (!initCamera()) {
    Serial.println("Error al inicializar la cámara");
    while (true) delay(1000);
  }

  Serial.println("ESP32-CAM lista");
}

void loop() {
  int seleccion = -1;
  // Detecta si se presionó uno de los botones
  if (digitalRead(BUTTON_ENTRADA) == HIGH) {
    seleccion = 0;
  } else if (digitalRead(BUTTON_SALIDA) == HIGH) {
    seleccion = 1;
  } else {
    yaProcesado = false;
    return;
  }

  if (yaProcesado) return;
  yaProcesado = true;
  // Captura imagen desde la cámara
  Serial.println("Capturando imagen...");
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Error al capturar imagen");
    return;
  }

  // Prepara la imagen capturada (modelo int8) (indispensable si no existirán fallos en el reconocimiento)
  ei::ei_signal_t signal;
  get_image_signal(&signal, fb);
  // Ejecuta el modelo
  ei_impulse_result_t result = { 0 };
  EI_IMPULSE_ERROR err = run_classifier(&signal, &result, false);
  esp_camera_fb_return(fb);

  if (err != EI_IMPULSE_OK) {
    Serial.printf("Error del clasificador: %d\n", err);
    return;
  }
  // Verifica si se detectaron objetos etiquetados como "Entrada" o "Salida"
  bool hayEntrada = false;
  bool haySalida = false;

  for (size_t i = 0; i < result.bounding_boxes_count; i++) {
    auto bb = result.bounding_boxes[i];
    if (bb.value == 0) continue;

    Serial.printf("Detectado: %s (%.2f%%) en [%d,%d,%d,%d]\n",
                  bb.label, bb.value * 100, bb.x, bb.y, bb.width, bb.height);
  //Configuración que los objetos dentro del modelo cuenten respectivamente con un string de 7 y 6 al inicio
    if (strncmp(bb.label, "Entrada", 7) == 0) hayEntrada = true;
    if (strncmp(bb.label, "Salida", 6) == 0)  haySalida = true;
  }
  // Determina si la selección del usuario fue correcta
  bool correcto = false;
  if (seleccion == 0 && hayEntrada) correcto = true;
  if (seleccion == 1 && haySalida)  correcto = true;
  // Muestra resultados con LEDs y buzzer según la situación
  if (!hayEntrada && !haySalida) {
    Serial.println("No se detectó ningún dispositivo."); // Ambos encendidos indican falta de detección
    digitalWrite(LED_VERDE, HIGH);
    digitalWrite(LED_ROJO, HIGH);
    digitalWrite(BUZZER_PIN, LOW);  
  } else if (correcto) {
    Serial.println("Correcto: El objeto coincide con la selección.");
    digitalWrite(LED_VERDE, HIGH);
    digitalWrite(LED_ROJO, LOW);
    digitalWrite(BUZZER_PIN, LOW);  
  } else {
    Serial.println("Incorrecto: El objeto no coincide con la selección.");
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_ROJO, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);  // Buzzer suena
  }

  delay(2000);
  // Apagar buzzer tras la espera
  digitalWrite(BUZZER_PIN, LOW);
}
// Convierte una parte de memoria de la ESP32-CAM de imagen en una señal interpretable para el modelo (configuración modelo Edge Impulse)
int get_image_signal(ei::ei_signal_t *signal, camera_fb_t *fb) {
  static uint8_t *image_data = nullptr;
  //Si la memoria interna de la cámara ESP32 está llena
  if (!image_data) {
    image_data = (uint8_t*)malloc(fb->len);
    if (!image_data) {
      Serial.println("Error: no se pudo asignar memoria.");
      return -1;
    }
  }

  memcpy(image_data, fb->buf, fb->len); // Copia la imagen capturada
  // Define cómo acceder a los datos
  signal->total_length = fb->len;
  signal->get_data = [](size_t offset, size_t length, float *out_ptr) -> int {
    for (size_t i = 0; i < length; i++) {
      out_ptr[i] = (float)image_data[offset + i];  
    }
    return 0;
  };

  return 0;
}