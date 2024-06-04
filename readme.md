# Medición de un Shindo en tiempo real con un ESP32

Acelerógrafo que calcula de la intensidad sísmica japonesa (Shindo) utilizando un ESP32 y un MPU6050.


## Escala sísmica japonesa (Shindo)

La escala sísmica japonesa es la escala proporcionada por la  Agencia Meteorológica Japonesa(JMA), la cual se utiliza principalmente en Japón  para indicar la intensidad de los terremotos [1]. Su unidad de medida se llama Shindo(震度 "grado de agitación"). A diferencia de la escala de Richter(donde se mide la magnitud total del terremoto), el Shindo describe el grado de agitación en un punto de la superficie terrestre.[2]


### Calculo de un Shindo [3]

1. Para calcular el Shindo, hay que buscar los pico de aceleración $a_f$ que provee el acelerógrafo en las tres dimensiones.
2. Luego se utiliza un filtro para corregir los efectos del periodo de la onda sísmica.
3. Encuentre un valor umbral $a_f$ tal que el tiempo total cuando el valor absoluto de la aceleración compuesta sea $a_f$ o más sea exactamente 0,3 segundos.
4. Al final, el valor de $a_f$ se lo ingresa en la siguiente formula:
$$I_f = 2\log_{10}{a_f} + 2.91 $$

Según la intensidad sísmica instrumental medida, se clasifica el valor de intensidad sísmica.


### Tabla de la escala de intensidad  sísmica

La escala Shindo se divide en 10 niveles, que se identifican con números del 0 al 7, y se incluyen dos subdivisiones (5- y 5+, 6- y 6+) para dar una mayor precisión en la descripción de la intensidad.[4]


| JMA Escala de intensidad(Shindo) | Descripción|Intensidad sísmica instrumental| Pico de aceleración (gal)[5] |
|----------|----------|----------|----------|
| **0**   | No sentido   | <0.5| < 0-8  |
| **1**   | Leve   |0.5 - 1.5| 0.8-2.5  |
| **2**    | Débil  |1.5-2.5| 2.5-8  |
| **3** | Levemente fuerte |2.5-3.5| 8 -25 |
| **4** | Fuerte |3.5-4.5|25-80|
|**5- Débil**| Muy fuerte |4.5-5.0|80-140|
|**5+ Fuerte** |Muy fuerte |5.0-5.5|140-250|
|**6- Débil**| Desastroso|5.5-6.0|250-315|
|**6+ Fuerte**| Desastroso| 6.0-6.5|315-400|
|**7**|Muy desastroso|>6.5|>400|


## Descripción del programa
Este programa utiliza un  MPU6050, el cual mide las aceleraciones en todos los ejes,los normaliza y calcula la intensidad sísmica japonesa (shindo) .

El programa simplifica los métodos que se utilizan para calcular el Shindo, ya que para esto se necesita el espectro de onda completo.[3]

Lo que realiza el programa es que proporciona IfF en tiempo real utilizando un búfer deslizante de 3 segundos (30 muestras) y un filtro digital simplificado de paso bajo/alto en el dominio del tiempo.
EL MPU6050 es una unidad de medición inercial o IMU (Inertial Measurment Units) de 6 grados de libertad (DoF) pues combina un acelerómetro de 3 ejes y un giroscopio de 3 ejes.[6]

Los rangos de inicializacion del acelerometro es de $\pm2g$ en cada uno de los ejes. Teniendo en cuenta que la resolución de las lecturas es de 16 bits por lo que el rango de lectura es de -32768 a 32767.[7] 
Al calibrar el MPU6050 este tienden a los siguientes valores:

|**$a_x$**|**$a_y$**|**$a_z$**|
|----------|----------|----------|
|0|0|16384|

Con el objetivo de poder eliminar el valor de la gravedad ($980 gal$),se utiliza un filtro de paso alto en software a 0.2Hz y un filtro de paso bajo a 5Hz con un intervalo de muestreo de 10Hz.[8][9]

Para normalizar el valor de la aceleración del sensor MPU6050, después de haber usado el filtro, se utiliza la siguiente expresión:
$$a_f=\frac{980.0 }{16384}\sqrt{a_x²+a_y²+a_z²} gal$$

Luego se calcula el $I_f$ con la aceleración normalizada.

En este programa, el $I_f$ se mantiene cada 100ms durante 3 segundos en el búfer deslizante (30 muestras) y se ordena cada 100ms para encontrar el tercer valor más alto, correspondiente a la regla de 0.3 segundos.


### shindo_sismo.ino (Código en Arduino)

El código para el ESP32 está escrito en Arduino y se encarga de leer los datos de los sensores y enviarlos a la base de datos Firebase[9]. 

####  Uso

1.  Configurar las credenciales de WiFi y Firebase.
2.  Conectar los sensores al ESP32 según sea necesario.
3.  Cargar el programa en el ESP32 utilizando el IDE de Arduino.
4.  El ESP32 comenzará a leer los datos de los sensores y enviarlos a la base de datos Firebase.

### Shindo_App

El $I_f$ mas alto obtenido durante los últimos 30 segundos se envía a la base de datos en Firebase, ademas se envía el valor de $a_f$ y el timestamp.

La interfaz web se utiliza para visualizar los datos almacenados en la base de datos Firebase en tiempo real[10]. A continuación se describe la estructura y funcionalidades principales del código HTML:

-   `index.html`: Este archivo contiene la estructura HTML de la interfaz web.
-   `style.css`: Este archivo contiene los estilos CSS aplicados a la interfaz web.
-   `scripts/auth.js`: Este archivo contiene el código JavaScript para la autenticación de usuarios.
-   `scripts/charts-definition.js`: Este archivo contiene el código JavaScript para definir los gráficos utilizados en la interfaz.
-   `scripts/index.js`: Este archivo contiene el código JavaScript principal para interactuar con la base de datos Firebase y actualizar la interfaz en tiempo real.

#### Uso

1.  Abrir el archivo `index.html` en un navegador web.
2.  Iniciar sesión con las credenciales de Firebase, si es necesario.
3.  La interfaz mostrará los datos almacenados en la base de datos Firebase, incluyendo gráficos y tablas interactivas.
4.  Los usuarios pueden interactuar con la interfaz para visualizar y manipular los datos según sea necesario.

## Simulaciones 

Se realizaron dos simulaciones de sismos para corroborar el funcionamiento del código y observar el índice de intensidad sísmica ($I_f$). A continuación, se presentan los resultados de cada simulación comparados con los datos reales.

### Primera Simulación: Datos del BCJ

Para la primera simulación, se utilizaron los datos del Building Center of Japan (BCJ), que proporciona una muestra de las aceleraciones en los ejes X, Y y Z (mitad de X) cada 10 milisegundos. Los resultados obtenidos se muestran en el siguiente cuadro.

| **Origen**       | **Intensidad Shindo** |
|------------------|-----------------------|
| **Simulación**   | 5.40                  |
| **BCJ\[12\]**    | **5.5**               |

*Comparación del If por la simulación y el BCJ*

### Segunda Simulación: Datos del SGC (Tumaco, Colombia)

Para la segunda simulación, se utilizaron los datos proporcionados por el Servicio Geológico Colombiano (SGC), específicamente del acelerógrafo ubicado en Tumaco, Nariño, que registró el sismo con epicentro en Guaitarilla, Nariño, el 9 de septiembre de 2013 a las 14:16\[13\]. La escala de intensidad utilizada por el SGC es la escala Mercalli modificada (EMS-98), en la cual este sismo tuvo un valor máximo de 7\[14\], equivalente a un Shindo de 5- (débil). Los resultados obtenidos se muestran en el cuadro V.

| **Origen**         | **Intensidad Shindo** |
|--------------------|-----------------------|
| **Simulación**     | 4.49                  |
| **SGC, Tumaco**    | **5-**                |

*Comparación del If por la simulación y el SGC Tumaco*


## Hardware

-   ESP32devkit
    
-   GY-521 MPU6050 or MPU9250 board wired on I2C bus.    
   
    ```
         SDA = GPIO_NUM_21 , SCL = GPIO_NUM_22   
    ```



## Librerias utilizadas

* https://github.com/JonHub/Filters
* https://github.com/emilv/ArduinoSort
* https://github.com/mobizt/Firebase-ESP-Client
* https://github.com/adafruit/Adafruit_Sensor``

## Referencias

1. https://www.jma.go.jp/jma/en/Activities/inttable.html
2. https://en.wikipedia.org/wiki/Japan_Meteorological_Agency_seismic_intensity_scale
3. https://www-data-jma-go-jp.translate.goog/eqev/data/kyoshin/kaisetsu/calc_sindo.html?_x_tr_sl=auto&_x_tr_tl=es&_x_tr_hl=es&_x_tr_pto=nui
4. https://www.hp1039.jishin.go.jp/eqchreng/at2-3.htm
5. https://web.archive.org/web/20060909043549/http://geoinfo.usc.edu/gees/Reports/Report3/japan/KOBE.HTML
6. https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf
7. https://naylampmechatronics.com/blog/45_tutorial-mpu6050-acelerometro-y-giroscopio.html
8. https://github.com/coniferconifer/ESP32-seismometer/tree/main
9. https://www.hackster.io/mircemk/sensitive-mpu6050-seismometer-with-data-logger-9e6bf5
10. https://randomnerdtutorials.com/esp32-data-logging-firebase-realtime-database/ 
11. https://randomnerdtutorials.com/esp32-esp8266-firebase-gauges-charts/
12. https://www.bousai.go.jp/jishin/syuto/denkikasaitaisaku/2/pdf/siryou3.pdf
13. http://bdrsnc.sgc.gov.co/sismologia1/HCG/acelerografos/consultas/Experta_RNAC/respuesta2.php?idsismo=20130209141606
14. http://sish.sgc.gov.co/visor/sesionServlet?metodo=irAInfoDetallada&idSismo=74