# Modular-ESP-MQTT-System

This is a simple ESP32 program that is purposely built with modularity. It is meant to be a "copy and paste" style of script. Essentially, if you paste this code into a PlatformIO project (or Arduino IDE), all you will need to do is program in your custom actions in the empty/example functions given. You can choose what action that might be: turning on LEDs, moving a motor, etc.

## Architecture

The device is run with what are called states. There are four default states in the code (however, more custom states can be added with ease):
1. Idle
2. Main Action
3. Sub Action
4. Terminate

The device is always at a certain state at any given time. The **Idle** state is the default state. This is the "rest" mode where the device is awaiting to receive commands. After any command is executed, the device returns to the Idle state.

The other 3 states are technically the same, however they are named as such to make it simpler to understand and modify at baseline. They are all fully customizable to your own liking. For example, if the device receives the "MAIN_ACTION" command, it will execute the Main Action - which (depending on what you coded in to be the Main Action) could turn on some LEDs. On the other hand, you could progam the Terminate command to turn off the lights. These three commands, technically speaking, are only different by name to make it easier for you to add commands.


## Uses

The beauty of this architecture is that because I ran all my MQTT systems through a server [(a different project run on a Raspberry Pi, which is designed to be similar to Tony Stark's Jarvis AI)](https://github.com/Zeb152/AlfredAI-Public), I could simply paste this code onto a new ESP32 I bought to control a different area of LEDs. Once the ESP has a unique device name, the hostname of the server, and the port of the server, the server would be able to message it commands immediately and the ESP would be able to act on those commands after the action functions are filled in with your custom code. All you need for the server's MQTT code is a way of sending the commands to the specific device.*

*_This is true. However, the MQTT server I personally run is more complex as it is integrated with AI and a database so it's not exactly how my own server works._

