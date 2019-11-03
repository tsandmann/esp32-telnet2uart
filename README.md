# UART to Telnet Server for ESP32

This is a basic UART to telnet server for the [ESP32][esp32]. It adds a wireless control interface to the [c't-Bot][ctBot]. It's tested with an ESP32-WROOM-32, but should work with most ESP32 boards.
This implementation is based on the [*WiFiTelnetToSerial*][esp32Telnet] example of the [ESP32 arduino framework][esp32Arduino] and therefore licensed under the terms of the [LGPLv2.1 license](LICENSE.md).

## Preparation

1. install PlatformIO core as described [here][PIOInstall]
    * can be skipped, if using VS Code IDE with [PlatformIO extension][PlatformIOVSC]
1. clone this git repository: `git clone https://github.com/tsandmann/esp32-telnet2uart`
1. change to cloned repo: `cd esp32-telnet2uart`
1. initialize build system for...
    * commandline build: `platformio init`
    * [VS Code][VSCode] project: `platformio init --ide vscode`
    * [Eclipse CDT][EclipseCDT] project: `platformio init --ide eclipse`
    * any other environment supported by [PlatformIO][PlatformIOIDE]

## Setup

1. create a config file
    1. copy [config.h.in](src/config.h.in) to `config.h`
    1. fill in your wifi SSID in `static const char* ssid = "YOUR_SSID";`
    1. fill in your wifi password in `static const char* password = "YOUR_PASSWORD";`
1. build the project
    * commandline: `plaformio run`
    * VS Code: use “Build” button on the PlatformIO toolbar or shortcut (`ctrl/cmd+alt+b`)
    * Eclipse CDT: `Project` -> `Build Project` or shortcut (`ctrl/cmd+b`)
    * **note:** you may need to adjust `board = esp01` in platformio.ini](platformio.ini), if you don't use an esp-01 board
1. upload firmware image
    1. connect the ESP32 to a programmer (we assume an USB programmer)
    1. adjust `upload_port =` in [platformio.ini](platformio.ini) as used by your programmer
        * `ll /dev/cu*` may help to find the correct device entry on a POSIX compatible OS
    1. start programming by
        * on commandline: `platformio run -t upload`
        * with VS Code: use “Upload” button on the PlatformIO toolbar or shortcut (`ctrl/cmd+alt+t`) and select "PlatformIO: Upload"
1. use a telnet program to connect
    * `telnet [IP or HOSTNAME OF YOUR ESP32]`

## Notices

* conventions:
  * indentation is done by 4 (four) spaces for each level, **never** ever use tabs (`\t` | `HT`)
  * source code formatting is done with [clang-format], use [.clang-format](.clang-format) for style settings

[ctBot]: https://www.ct-bot.de
[esp32]: https://www.espressif.com/en/products/hardware/esp32/overview
[esp32Arduino]: https://github.com/espressif/arduino-esp32
[esp32Telnet]: https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WiFiTelnetToSerial/WiFiTelnetToSerial.ino
[PlatformIO]: https://platformio.org
[PIOGithub]: https://github.com/platformio/platformio-core
[PIOInstall]: http://docs.platformio.org/en/latest/installation.html
[PlatformIOVSC]: http://docs.platformio.org/en/latest/faq.html#faq-install-shell-commands
[VSCode]: https://github.com/Microsoft/vscode
[EclipseCDT]: https://eclipse.org
[PlatformIOIDE]: http://docs.platformio.org/en/latest/ide.html#ide
[clang-format]: https://clang.llvm.org/docs/ClangFormat.html
