
### **Задачи проекта:**
 - создание системы управления AlphaBot  на базе KasperskyOS;
 - разделение системы управления на независимые сущности;
 - настройка политик безопасности KasperskyOS  для каждой сущности;
 
### **Результатом работы являются**
Программное решение состоит из трех основных сущностей:
- **Клиент** (Client) – сущность, обеспечивающая коммуникацию с сервером управления по протоколу MQTT.
 - **Сервер конфигурации** (Config  Server) – сущность, в которой защищенно хранится конфигурация для всех модулей в составе системы управления. Получение конфигурации ограниченно только для определенных сущностей, например для сущности Клиент:
```
request src = echo.Client, dst = echo.ConfigurationServer
{
	grant()
}

response src = echo.ConfigurationServer, dst = echo.Client
{
	grant()
}
```

 - **Система навигации** (Navigation  Controller) – сущность, обеспечивающая контроль движения бота.

#### **Описание системы:**

Сущность **Клиент** – обеспечивает подключение по MQTT  к серверу управления. Для авторизации на сервере используется логин и пароль. После подключения, **Клиент** высылает контрольное сообщение на указанный топик. Логин, пароль, адрес сервера управления, топик **Клиент** запрашивает у **Сервера Конфигурации** через IPC канал.   

В составе модуля **Клиент** находятся два подмодуля - **Валидатор** (Validator) – проверят формат и содержание сообщения пришедшего от сервера управления, и **Маршрутизатор Команд** (Comm  Router) – обеспечивает посылку соответствующего сообщения на предоставленный для него IPC  канал.   

**Клиент** принимает от сервера управления два основных вида команд:
- Движение по направлению с указанием скорости и времени выполнения маневра. Возможно указание следующих направлений: вперед (forward), назад (back), разворот налево (left), разворот направо (right). Скорость движения (spd) задается числом с плавающей запятой в пределах от (0, 1]. Время выполнения маневра (value)  задается числом с плавающей запятой.   
- Остановка выполнения текущей команды (stop). Прерывает выполнение текущей команды, останавливая робота.   
Команды принимаются в формате JSON. Шаблон  команды:
```
{
	“cmd” : “<forward, left, right, back, stop>”,
	“value”: 1.4 (double),
	“spd”: 0.6 (double [0, 1.0]) - optional
}
```
Сущность **Система Навигации** обеспечивает расчет текущего и оставшегося времени, направления маневра, необходимую мощность каждого из двигателей. Расчет текущего маневра происходит в отдельном потоке, с частотой опроса 100Гц. Поэтому точность остановки составляет до 1/100 секунды.   
Мощность двигателя регулируется с помощью ШИМ контроллера, реализованного программными средствами. Частота работы программного ШИМ котроллера 200Гц. ШИМ контроллер работает постоянно в выделенном потоке, просыпаясь не чаще чем 400 раз в секунду.   
ШИМ контроллер и расчёт текущего маневра являются частью системы управления, поэтому они работают в отдельных программных потоках в рамках одной сущности **Системы Навигации**, а их взаимодействие реализуется с помощью общей памяти и mutex.   
Политики безопасности решения настроены таким образом, что каждая сущность имеет свои права доступа к системе. Например, только сущность **Система Навигации** имеет доступ к GPIO:
```
request src=echo.NavigationController, dst=kl.drivers.GPIO
{
	grant()
}
```

А доступ к VfsNet имеет только сущность **Клиент**:

request src = echo.Client, dst = kl.VfsNet
```
{
	grant()
}
```

### **Проверка работоспособности решения в проекте**

1. Установить SDK  для Kaspersky  OS  CE  [https://os.kaspersky.ru/development/](https://os.kaspersky.ru/development/).   

2. Прошить SD  карту, которая будет использоваться в AplhaBot. Инструкция для этого представлена по ссылке: [https://support.kaspersky.ru/help/KCE/1.1/ru-RU/preparing_sd_card_rpi.htm](https://support.kaspersky.ru/help/KCE/1.1/ru-RU/preparing_sd_card_rpi.htm).   

3. Запустить в локальной сети MQTT  брокер (установить в нем логин и пароль).   

4. Узнать ip  адрес машины, на которой запущен брокер   
`$ ip a`    

5. Указать данные для доступа в сеть на точке доступа, закрепленной на верхней крышке предоставленного робота AlphaBot.   

6. Извлечь исходный код решения   
`$ git clone --recurse-submodules https://github.com/MrQuaters/kos-abot.git`   

7. Указать  адрес  машины  с MQTT брокером, топик, логин  и  пароль  в  файле _configuration_server/src/server.c_ 

8. Собрать образ с помощью команды:   
`$ ./image-build`

9. Скопировать полученный образ на SD  карту. Инструкция:  [https://support.kaspersky.ru/help/KCE/1.1/ru-RU/running_sample_programs_rpi.htm](https://support.kaspersky.ru/help/KCE/1.1/ru-RU/running_sample_programs_rpi.htm).   

10.  Включить AlphaBot. Результат: на сабтопик /ctl  топика, указанного в конфигурации, AlphaBot  опубликует сообщение “alive”, сигнализирующие об успешном включении устройства.

11. Опубликуйте команду `{“cmd”:“forward”, “value”:1.0,“spd”:0.8}` на топик, указанный в конфигурации. Результат: робот должен ехать вперед 1 секунду с 80% скоростью.

12. Опубликуйте команду `{“cmd”:“back”, “value”:2.0,“spd”:0.5}` на топик, указанный в конфигурации. Результат: робот должен ехать назад 2 секунды с 50% скоростью.

13. Опубликуйте команду `{“cmd”:“right”, “value”:1.1,“spd”:0.9}` на топик, указанный в конфигурации. Результат: поворачивать направо в течение 1.1 секунды с 90% скоростью.

14. Опубликуйте команду `{“cmd”:“left”, “value”:0.5}` на топик, указанный в конфигурации. Результат: робот должен поворачивать налево в течение 0.5 секунд.

15. Опубликуйте команду `{“cmd”:“forward”, “value”:100.0,“spd”:0.5}` на топик, указанный в конфигурации. Во время выполнения этой команды опубликуйте команду `{“cmd”:“stop”}`. Результат: после получения команды stop  робот должен остановиться.


### **Дальнейшие планы работ по проекту:**

- Разработка целей и предположений безопасности для бота и конфигурирование соответствующих им политик безопасности решения.   
- Разработка сценариев тестирования системы защиты безопасности решения.   
- Добавление возможности удаленного конфигурирования бота.   
- Добавление команды движения по заданному маршруту.   
- Переход на защищенное соединение с MQTT на MQTTs.   