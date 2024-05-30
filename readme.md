
# Medición de un Shindo en tiempo real con un ESP32

Acelerógrafo que calcula de la intensidad sísmica japonesa (Shindo) utilizando un ESP32 y un MPU6050.

## Descripción del programa

Este programa utiliza un acelorometro MPU6050, el cual mide las aceleraciones en todos los ejes,los normaliza y calcula la intensidad sismica japonesa (shindo) simplificando los metodos que se utilizan para calcularlo.[1]

## Escala sísmica japonesa (Shindo)

La escala sísmica japonesa es la escala proporcionada por la  Agencia Meteorológica Japonesa(JMA), la cual se utiliza principalmente en Japón y Taiwan para indicar la intensidad de los terremotos. Su unidad de medida se llama Shindo(震度 "grado de agitación"). A diferencia de la escala de Richter(donde se mide la magnitud total del terremoto), el Shindo describe el grado de agitación en un punto de la superficie terrestre.[2]

### Tabla de la escala sísmica

La escala shindo se divide en 10 niveles, que se identifican con números del 0 al 7, y se incluyen dos subdivisiones (5- y 5+, 6- y 6+) para dar una mayor precisión en la descripción de la intensidad.


| JMA Escala de intensidad(Shindo) | Descripción| Pico de aceleración (gal) |
|----------|----------|----------|
| **0**   | No sentido   | < 0-8  |
| **1**   | Leve   | 0.8-2.5  |
| **2**    | Débil  | 2.5-8  |
| **3** | Levemente fuerte | 8 -25 |
| **4** | Fuerte |25-80|
|**5- Débil**| Muy fuerte |80-140|
|**5+ Fuerte** |Muy fuerte |140-250|
|**6- Débil**| Desastroso|250-315|
|**6+ Fuerte**| Desastroso| 315-400|
|**7**|Muy desastroso|>400|






