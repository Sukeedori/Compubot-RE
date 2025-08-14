# Compubot-RE

El **Compubot** es un recurso educativo innovador que integra robótica e inteligencia artificial para apoyar la enseñanza del tema **“Reconocimiento de dispositivos de entrada y salida de un computador”**.  
Este recurso está diseñado para dinamizar las clases de informática, permitiendo al estudiante identificar objetos físicos (mouse, teclado, monitor, etc.) mediante visión artificial con **ESP32-CAM** y un modelo entrenado en **Edge Impulse**.

---

## 1️⃣ Descargar y abrir el código del robot

1. Descarga el archivo `Compubot_Dispositivos_ES.ino` desde este repositorio.
2. Abre Arduino IDE.
3. Ve a **File → Open...** y selecciona `Compubot_Dispositivos_ES.ino`.

---

## 2️⃣ Instalar las librerías necesarias

Para el funcionamiento de la pantalla OLED y la cámara ESP32-CAM, instala las siguientes librerías en Arduino IDE:

- **Adafruit SSD1306** → para manejar la pantalla OLED.  
- **Adafruit GFX Library** → soporte gráfico para OLED.  
- **esp32** (Paquete de tarjetas de Espressif Systems) → incluye soporte para la cámara ESP32-CAM.


**Pasos de instalación:**
1. Ve a **Sketch → Include Library → Manage Libraries** y busca e instala las librerías **Adafruit SSD1306** y **Adafruit GFX Library**.
2. Para instalar el paquete de tarjetas **ESP32**:
   - Ve a **File → Preferences**.
   - En el campo *Additional Board Manager URLs*, agrega las siguientes direcciones (puedes separarlas con una coma o ponerlas en líneas diferentes):
     ```
     https://dl.espressif.com/dl/package_esp32_index.json
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Presiona **OK**.
3. Luego ve a **Tools → Board → Board Manager**, busca **esp32** e instálalo.
---

## OPCIONAL: Descargar la librería del modelo desde Edge Impulse si desea adaptar el modelo

1. Ingresa a [Edge Impulse Studio](https://studio.edgeimpulse.com/).
2. Inicia sesión con tu cuenta.
3. Abre el proyecto compartido del modelo entrenado para Compubot (https://studio.edgeimpulse.com/public/706244/live).
4. Ve a **Deployment** (Despliegue).
5. Selecciona **Arduino Library** como formato de exportación.
6. Haz clic en **Build** y descarga el archivo `.zip`.
7. En Arduino IDE, ve a **Sketch → Include Library → Add .ZIP Library** y selecciona el archivo descargado.

## 3️⃣ Ficha pedagógica del robot

Puedes consultar la ficha pedagógica de Compubot, con detalles y actividades sugeridas, en el siguiente enlace:  
📄 [Ficha pedagógica de Compubot](https://drive.google.com/file/d/1175LLox4lqw2HsK3wJkG9myr-nm7dDEZ/view?usp=sharing)

---

✏️ **Autor:** Kevin Encarnación 
📅 **Versión:** 1.0

## Contribución

Si quieres contribuir a este proyecto, siéntete libre de hacer un `fork` del repositorio y enviar un `pull request` con tus mejoras.
