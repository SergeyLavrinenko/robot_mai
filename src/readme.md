# Robot MAI

## Описание проекта:
Приложение позволяет управлять роботом под управлением [Kaspersky OS](https://os.kaspersky.ru/) посредством распознования его по видеопотоку.
Управления роботом происходит путем распознавания точки базирования робота, точки назаначения, а также самого робота. На основе этих данных формируются и отправляются команды управления роботом по каналу **MQTT** для достижения им заданной цели.
Начало движения робота инициализируется по команде пользователя посланную через Telegram бота.

## Структура проекта:

```
+---------------+              +---------------+             +---------------+
|               |              |               |             |               |
|   kos_abot    |  <- MQTT ->  |    control    | <- MQTT ->  |  telegrab_bot |
|               |              |               |             |               |
+---------------+              +---------------+             +---------------+
```

### 1. KOS-ABOT
Модуль по убравлению роботом разработанный [MrQuaters](https://github.com/MrQuaters/) 
Проект [kos-abot](https://github.com/MrQuaters/kos-abot) написан для операционной системы [Kaspersky OS](https://os.kaspersky.ru/). 
#### Данный проект был склонирован за исключением следующих изменений:
<br/>  

* В файле [pwm_controller.cpp](https://github.com/SergeyLavrinenko/robot_mai/blob/master/src/kos-abot/navigation/src/pwm_controller.cpp):

```C++
void pwm_controller::goLeft(int perc) {
    set_pins(perc, 1, 0, 1, 0);
}

void pwm_controller::goRight(int perc) {
    set_pins(perc, 0, 1, 0, 1);
}

void pwm_controller::goForward(int perc) {
    set_pins(perc, 1, 0, 0, 1);
}

void pwm_controller::goBack(int perc) {
    set_pins(perc, 0, 1, 1, 0);
}

```

+ В файле [server.c](https://github.com/SergeyLavrinenko/robot_mai/blob/master/src/kos-abot/configuration_server/src/server.c) изменены конфигурационные данные для подключения к **MQTT** брокеру.
  
<br><br/>

### 2. Control
Модуль осуществляющий распознования робота в пространстве по изображеию с камеры, путем его обработки алгритмами содержащимися в библиотеке **OpenCV**. После распознования формируются команды для управления роботом (вперед, влево, вправо) передаваемые модулю kos-abot по протоколу **MQTT**.
<br>

Распознование ключевых точек таких как: робот, точка назначени, точка базирования. Осузествляются путем распознавания контрастных однотонных цветов, расположенных в данных ключевых точках.
<br>

Цвета заданы в формате **HSV** и задаются файле [main.cpp](https://github.com/SergeyLavrinenko/robot_mai/blob/master/src/control/main.cpp):
```C++
det->set_color_bot_rear(0, 0, 0);
det->set_color_bot_front(0, 0, 0);
det->set_color_target(0, 0, 0);
det->set_color_home(0, 0, 0);
```
Конфигруационные данные для подключения к **MQTT** брокеру расположены также в файле [main.cpp](https://github.com/SergeyLavrinenko/robot_mai/blob/master/src/control/main.cpp):
```C++
#define mqtt_host "192.168.1.4"
#define mqtt_port 1883
``` 
<br><br/>
### 3. Telegram bot
Telegram бот необходим для взаимодействия пользователя и модуля Control. У пользователя есть возможность передать команду для движения робота к цели и команду для отправления его обратно на базу.

Конфигруационные данные для подключения к **MQTT** брокеру расположены в файле [main.cpp](https://github.com/SergeyLavrinenko/robot_mai/blob/master/src/telegram_bot/main.cpp) модуля telegram_bot:
```c++
#define mqtt_host "192.168.1.4"
#define mqtt_port 1883
```
Также в этом файле необходимо изменить токен телеграм бота:
```c++
string token = "YOUR_TOKEN";
```
