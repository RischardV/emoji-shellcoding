# RISC-V: Emoji Shellcoding 🙂

```
By Hаdrien Ваrrаl and Georges-Axel Jaloyan
```

This work has been presented at
 [DEFCON30](https://defcon.org/html/defcon-30/dc-30-index.html) ([slides](https://î.fr/defcon/DEF%20CON%2030%20-%20Hadrien%20Barral%20-%20Emoji%20Shellcoding:%20%F0%9F%9B%A0%EF%B8%8F,%20%F0%9F%A7%8C,%20and%20%F0%9F%A4%AF%20-%20Presentation.pdf))

## Overview

This tool ⚒️ helps design RISC-V (both 32-bit and 64-bit) shellcodes capable of running arbitrary code, whose ASCII binary representation
use only [Unicode UTF-8](https://unicode.org/emoji/charts/full-emoji-list.html) emojis 🤯.

It consists of an emoji unpacker. For any target shellcode (non-emoji),
the tool will produce an emoji shellcode with the unpacker and the packed version of your shellcode.
Run it on a RISC-V simulator/cpu and enjoy!

For a general introduction on RISC-V shellcoding, you may read the [blog post by Thomas Karpiniec](https://thomask.sdf.org/blog/2018/08/25/basic-shellcode-in-riscv-linux.html).

You can find our previous work on RISC-V alphanumeric shellcoding here:
 [https://github.com/RischardV/riscv-alphanumeric-shellcoding](https://github.com/RischardV/riscv-alphanumeric-shellcoding).

Folder contents:
- `qemu`: Full source code and prebuilt binary for the baremetal QEMU demo
- `qemu_short`: Same as `qemu`, but a shorter version
- `esp32`: Demos running on the Espressif ESP32-C3 board
- `hifiveu`: Demos running on the HiFive-Unleashed board
- `payload`: Source code of the payloads used
- `block`: How we generated the available instructions
- `nopsled`: The source code of our ⛔🛷 nopsled

## Quick-try

Building the shellcodes requires a RISC-V toolchain.
We only provide easy-to-test pre-built QEMU baremetal shellcodes.

The only prerequisite is having a RISC-V QEMU v6.0.0 or newer [https://www.qemu.org/](https://www.qemu.org/).
On Ubuntu 22.04, you can install it using `apt install qemu-system-misc`.

Then:
  - `cd emoji-shellcoding/paper_hello_world`
  - `cat qemu_miniclog_small.bin`
    (optional, to print the shellcode. Open it in your favorite editor if your console does not support emojis)
  - `./launch_prebuilt`   use Ctrl+C to exit

You can modify `launch_prebuilt` to run `qemu_miniclog_medium.bin` or `qemu_miniclog_large.bin` instead.

## Building && Testing

Start by cloning the repository:
```
git clone https://github.com/RischardV/emoji-shellcoding.git
cd emoji-shellcoding
```

#### Prerequisites:

- Python 3.10 or later [https://www.python.org/](https://www.python.org/)
- RISC-V toolchain
- RISC-V QEMU: install QEMU v6.0 or newer [https://www.qemu.org/](https://www.qemu.org/)
- GNU m4

On Ubuntu 22.04:
```
apt install build-essential gcc-riscv64-linux-gnu gcc-riscv64-unknown-elf qemu-system-misc m4
```

### QEMU bare-metal shellcodes

- Build the instructions lists (takes some time)
  - `cd emoji-shellcoding/block`
  - `make`
- Build the shellcodes
  - `cd emoji-shellcoding/qemu`
  - `make`
- Run the shellcodes
  - `cd emoji-shellcoding/qemu`
  - `cat out/shellcode.bin` (optional, to print the shellcode)
  - `./l`   use Ctrl+C to exit
Expected results:
  The string "Hello, world!" should print on the screen.

### Espressif ESP32-C3 shellcodes

Prerequisites:
 - An Espressif ESP32-C3 board: [https://www.espressif.com/en/products/socs/esp32-c3](https://www.espressif.com/en/products/socs/esp32-c3)
   (esp32-c3-devkitm-1 or esp32-c3-devkitc-02 are easily available for <10$)
 - The ESP-IDF SDK: [https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/get-started/index.html](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/get-started/index.html)

- Build the shellcodes
  - `cd emoji-shellcoding/esp32`
  - `make -j`

Running instructions are very similar to QEMU Linux shellcodes above. Refer to them.

### HiFive Unleashed Linux shellcodes

Prerequisites:
 - A HiFive-Unleashed board: [https://www.sifive.com/boards/hifive-unleashed](https://www.sifive.com/boards/hifive-unleashed)
   (note: this board is not sold anymore)

- Build the shellcodes
  - `cd emoji-shellcoding/hifiveu`
  - `make -j`

Running instructions are very similar to QEMU Linux shellcodes above. Refer to them.

## License

This tool is released under Apache license. See `LICENSE` file.
