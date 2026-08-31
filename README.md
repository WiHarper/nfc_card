# NFC Energy-Harvesting PCB Business Card with an MCU

![image](https://wilsonharper.net/assets/img/nfc/hero-1400.webp)

Like most business cards, this one doesn’t have a battery. When it’s tapped against a phone, though, 21 LEDs start an animation, powered just by NFC.

Find more information at [wlsn.ee/nfc](https://wlsn.ee/nfc).

## Details

* MCU: ATtiny816 
* NFC Chip: NXP NTAG I2C Plus
* Antenna: Custom rectangular coil tuned to ~2.75 µH
* Display: 21 Charlieplexed LEDs
* Dimensions: Payment card form factor

## Repository Layout

* `hardware/` - KiCad 9.0 schematic files/PCB layout files and 3D models
* `software/` - firmware and KiPython script
* `gallery/` - photos/video of card

## Programming

The ATtiny is flashed over UPDI; its clock speed should be set to 1 MHz. The NFC chip is formatted and written using [NXP Tagwriter](https://play.google.com/store/apps/details?id=com.nxp.nfc.tagwriter&hl=en_US). Energy harvesting is enabled by default, and it is easy to add vCard records or a website link.