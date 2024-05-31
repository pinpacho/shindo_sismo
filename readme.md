
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
EL MPU6050 es una unidad de medición inercial o IMU (Inertial Measurment Units) de 6 grados de libertad (DoF) pues combina un acelerómetro de 3 ejes y un giroscopio de 3 ejes.[4]

Los rangos de inicializacion del acelerometro es de $\pm2g$ en cada uno de los ejes. Teniendo en cuenta que la resolución de las lecturas es de 16 bits por lo que el rango de lectura es de -32768 a 32767.[5] 
Al calibrar el MPU6050 este tienden a los siguientes valores:

|**$a_x$**|**$a_y$**|**$a_z$**|
|----------|----------|----------|
|0|0|16384|

Con el objetivo de poder eliminar el valor de la gravedad ($980 gal$),se utiliza un filtro de paso alto en software a 0.2Hz y un filtro de paso bajo a 5Hz con un intervalo de muestreo de 10Hz.[6]

Para normalizar el valor de la aceleración del sensor MPU6050, después de haber usado el filtro, se utiliza la siguiente expresión:
$$a_f=\frac{980.0 }{16384}\sqrt{a_x²+a_y+a_z²} gal$$

Luego se calcula el $I_f$ con la aceleración normalizada.

En este programa, el $I_f$ se mantiene cada 100ms durante 3 segundos en el búfer deslizante (30 muestras) y se ordena cada 100ms para encontrar el tercer valor más alto, correspondiente a la regla de 0.3 segundos.
