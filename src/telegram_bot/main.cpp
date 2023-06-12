
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>
#include <time.h>

#include <signal.h>
#include <stdint.h>
#include <mosquitto.h>

#define mqtt_host "127.0.0.1"
#define mqtt_port 1883

#define client_id "telegram_bot"

#define input_topic "/robot_mai/order_manager/ch2"
#define output_topic "/robot_mai/order_manager/ch1"

#include <tgbot/tgbot.h>

using namespace std;
using namespace TgBot;

enum EStatus {ORDER_EXIST, ORDER_OK};
EStatus status = ORDER_OK; 
bool flag_read = false;
int64_t currentChatId;

struct mosquitto *mosq;

//----------Telegram-----------

string token = "YOUR_TOKEN";
Bot bot(token);

ReplyKeyboardRemove::Ptr hideKeyboard(new ReplyKeyboardRemove);
ReplyKeyboardMarkup::Ptr startKeyboard(new ReplyKeyboardMarkup);
ReplyKeyboardMarkup::Ptr waitKeyboard(new ReplyKeyboardMarkup);

void createOneColumnKeyboard(const vector<string>& buttonStrings, ReplyKeyboardMarkup::Ptr& kb)
{
  for (size_t i = 0; i < buttonStrings.size(); ++i) {
    vector<KeyboardButton::Ptr> row;
    KeyboardButton::Ptr button(new KeyboardButton);
    button->text = buttonStrings[i];
    row.push_back(button);
    kb->keyboard.push_back(row);
  }
}

void createKeyboard(const vector<vector<string>>& buttonLayout, ReplyKeyboardMarkup::Ptr& kb)
{
  for (size_t i = 0; i < buttonLayout.size(); ++i) {
    vector<KeyboardButton::Ptr> row;
    for (size_t j = 0; j < buttonLayout[i].size(); ++j) {
      KeyboardButton::Ptr button(new KeyboardButton);
      button->text = buttonLayout[i][j];
      row.push_back(button);
    }
    kb->keyboard.push_back(row);
  }
}    

void anyMessageReply(Message::Ptr message){
        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }
        if(message->text == "Хочу напиток"){
            char data[] = "new_order";
            mosquitto_publish( mosq, NULL, output_topic, strlen(data), data, 0, 0 );
            clock_t start = clock();

            while((clock() - start)/CLOCKS_PER_SEC < 5){
                if(flag_read == true){
                    if(status == ORDER_EXIST)
                        bot.getApi().sendMessage(message->chat->id, "Заказ уже выполняется", false, 0);
                    if(status == ORDER_OK){
                        bot.getApi().sendMessage(message->chat->id, "Заказ принят, ожидайте", false, 0, hideKeyboard);
                        currentChatId = message->chat->id;
                    }
                    flag_read == false;
                    return;
                }
            }
            bot.getApi().sendMessage(message->chat->id, "Робот по доставке напитков недоступен, попробуйте позже", false, 0, hideKeyboard);
            return;
        }
        if(message->text == "Забрал напиток"){
            bot.getApi().sendMessage(message->chat->id, "Спасибо за заказ!", false, 0, startKeyboard);
            char data[] = "glass_ok";
            mosquitto_publish( mosq, NULL, output_topic, strlen(data), data, 0, 0 );
            return;
        }

        bot.getApi().sendMessage(message->chat->id, "Такой команды не существует");
}

void startMessageReply(Message::Ptr message){
        bot.getApi().sendMessage(message->chat->id, "Я робот по доставке напитков скажите 'Хочу напиток'", false, 0, startKeyboard);
}

//---------MQTT-----------



void connect_callback(struct mosquitto *mosq, void *obj, int result) {
    printf("connect callback(order_manager mqtt), rc=%d\n", result);
}

void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message) {
    printf("got message '%.*s' for topic '%s'\n", message->payloadlen, (char*) message->payload, message->topic);
    
    if (!strcmp((char*)(message->payload), "order_alredy_exist")){
        flag_read = true;
        status = ORDER_EXIST;
    }
    if (!strcmp((char*)(message->payload), "order_ok")){
        flag_read = true;
        status = ORDER_OK;
    }
    if (!strcmp((char*)(message->payload), "order_arrived")){
        bot.getApi().sendMessage(currentChatId, "Заказ прибыл, как заберете стакан скажите 'Забрал напиток'", false, 0, waitKeyboard);
    }
    if (!strcmp((char*)(message->payload), "glass_ok")){

            char data[] = "go_to_home";
            mosquitto_publish( mosq, NULL, output_topic, strlen(data), data, 0, 0 );
    }
}


int main() {

    //--------MQTT----------
    mosquitto_lib_init();
    mosq = mosquitto_new(client_id, true, 0);
    if (mosq){
        mosquitto_connect_callback_set(mosq, connect_callback);
        mosquitto_message_callback_set(mosq, message_callback);
        mosquitto_connect_async(mosq, mqtt_host, mqtt_port, 60);
        mosquitto_loop_start(mosq);
        mosquitto_subscribe(mosq, NULL, input_topic, 0);
    }


    //-------Telegram---------
    createOneColumnKeyboard({"Хочу напиток"}, startKeyboard);
    createOneColumnKeyboard({"Забрал напиток"}, waitKeyboard);

    bot.getEvents().onCommand("start", startMessageReply);
    bot.getEvents().onAnyMessage(anyMessageReply);

    signal(SIGINT, [](int s) {
        printf("SIGINT got\n");
        exit(0);
    });

    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        bot.getApi().deleteWebhook();

        TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    } catch (exception& e) {
        printf("error: %s\n", e.what());
    }

    return 0;
}