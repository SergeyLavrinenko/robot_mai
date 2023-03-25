## kos-abot   

Secured application based on KasperskyOS to control alpha bot.
[Kaspersky OS documentation](https://support.kaspersky.ru/help/KCE/1.1/en-US/whats_new.htm)
[AlphaBot documentation](https://www.mouser.com/pdfdocs/AlphaBot-User-Manual2.pdf)

### Application Structure
```
        +---------------+      +---------------+
        | communication | IPC  |   navigation  |
MQTT -> |     unit      | ---> |      unit     |
        +---------------+      +---------------+
               | IPC
               +->  +---------------+
                    | configuration |
                    |     unit      |
                    +---------------+
```
Links:   
  - [Communication Unit](/communication)
  - [Navigation Unit](/navigation)
  - [Configuration Unit](/configuration_server)

Communication unit receives commands via MQTT, validates it and then sends ones to Navigation Unit. Navigation Unit calculates required motor parameters based on required command.
Configuration unit used by Communication Unit to get MQTT config (server address, topic, passwords etc...).

### Secure features:
  - Secured MQTT connection (require username, password)
  - Message validation (validate all messages recieved via MQTT)
  - One action execution (only one action can be proccessed at time)
  - Policies
  
### Application features:
  - Application implements [Software PWM controller](/navigation/src/pwm_controller.h). Bot speed can be set.
### Configuration:   
To setup your MQTT broker config, change values in [configuration_server](/configuration_server/src/server.c).

### Build:
You can build this project with:
  - (qemu image) `./cross-build.sh` [qemu image](/cross-build.sh)
  - (kos image) `./image-build` [kos image](/image-build)

### Run:
To run QUEMU image run the following command:   
```/opt/KasperskyOS-Community-Edition-<your version>/toolchain/bin/qemu-system-aarch64 -m 2048 -machine vexpress-a15,secure=on -cpu cortex-a72 -nographic -monitor none -smp 4 -net nic -net user -serial stdio -kernel build/einit/kos-qemu-image```

To run on physical device you can follow [this manual](https://support.kaspersky.ru/help/KCE/1.1/en-US/preparing_sd_card_rpi.htm).   
**Note: use `rpi4_prepare_fs_image_rev1.5.sh`!!**

### Control:
Publish one of the following control commands on the topic AlphaBot is subscribed on:
```   
{
   “cmd” : “<forward, left, right, back, stop>”,
   “value”: 1.4 (double),
   “spd”: 0.6 (double [0, 1.0]) - optional
}
```
**forward** - move forward, *left* - rotate left, *right* - rotate right, *back* - go back.   
**stop** - any command can be stopped sending stop action.   
**value** - how many seconds command should be executed.   
**spd** - AlphaBot speed for this command *[optional]*.   

