# MP3 player firmware

Side project where I try to make a mp3 player. This repository contains the firmware I'm flashing
onto an ESP32 S-3.

## WSL USB device passthrough setup

Run the following in a windows administrator terminal.

To find which `<bus-id>` the ESP32 (`USB-Enhanced-SERIAL CH343 (COM5)`) is connected to, run:

```bash
usbipd list
```

Then run the following.

```bash
# Bind and attach to the WSL instance
usbipd bind --busid <bus-id>
usbipd attach --wsl --busid <bus-id> --auto-attach
```
