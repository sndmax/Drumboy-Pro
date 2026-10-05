# Drumboy Pro

📖 **[Developer Wiki](https://www.randomwaves.io/drumboy-pro/drumboy-pro-wiki.html)** — full hardware specs, schematics walkthrough, firmware architecture, DSP internals, and memory map.

**An open-source hardware groovebox** — sequencer, sampler, and DSP effects engine built around an STM32H723 (Cortex-M7 @ 550 MHz), with a 540×960 touch-free color display, 42 keys, 8 rotary encoders, and full MIDI/Sync I/O.

This repository holds the complete hardware design (KiCad) and firmware (STM32CubeIDE / C++) for Drumboy Pro, released by [Randomwaves](https://randomwaves.io) as an open hardware/software project.

![Drumboy Pro](<01 Hardware/Drawing/Drumboy Pro.png>)

---

## Overview

The MCU sits at the center of six subsystems: an NT35510 display driven over a 16-bit parallel GPIO bus, 16 MB of external SDRAM for sample streaming, an SGTL5000 audio codec over I2S/I2C, a microSD card for storage, a control surface fanned out over six MCP23017 I/O expanders, and external connectivity (MIDI, Sync, USB-C power).

| | |
|---|---|
| **MCU** | STM32H723ZGT6 — Cortex-M7 @ 550 MHz (VOS0), LQFP-144 |
| **Display** | NT35510, 540×960, 16-bit parallel |
| **SDRAM** | 16 MB (IS42S16800J-7TL-TR), FMC bus |
| **Audio codec** | SGTL5000 — I2S3 audio / I2C3 control |
| **Storage** | microSD (TF-01A), SDMMC2, FAT filesystem |
| **Control surface** | 42 keys, 42 LEDs, 8 rotary encoders — 6× MCP23017 over I2C |
| **Connectivity** | MIDI In/Out, Sync In/Out, USB-C power |
| **Flash** | 1024 KB, split between bootloader and application |

## Features

- **10 sample layers**, each independently modulated by a 10-slot LFO matrix (sample-and-hold at note-trigger time)
- **Per-layer routing** into a shared DSP chain: parametric EQ → 2× multimode filter → 2× send effect → stereo reverb
- **9 effect types** per send slot: Delay, Chorus, Flanger, Phaser, Compressor, Expander, Overdrive, Distortion, Bitcrusher
- **Freeverb/Schroeder-Moorer style reverb** with pre-delay, stereo surround derivation, and nine built-in profiles: Default, Tight, Snap, Chamber, Warehouse, Disco, Dub, Ghost, Texture
- **Sequencer** with 5 song banks per layer, 64 beats per bank, and algorithmic complex-fill generation (timing alignment × level curve templates)
- **MIDI In/Out** (opto-isolated input) and **Sync In/Out** for external clock/gate sync
- **SD-card firmware updates** via a dedicated bootloader with CRC-32 verified staged writes
- Fully open hardware (KiCad) and firmware (C/C++ against the STM32 HAL) — no CMSIS-DSP or external DSP library, all hand-rolled biquad/delay-line DSP on the Cortex-M7 FPU

Choose **Profile** with the second encoder in the Reverb menu or select the field with left/right and change it with up/down. Each profile enables reverb and applies its complete settings through a bypass transition. **Tight** is the starting profile at initialization and for new projects and drumkits. **Default** retains the original settings for an explicit reverb reset and recovery from invalid saved parameters.

| Profile | Intended use | Size | Decay | Damping | PreDelay (ms) | Surround (%) | Dry | Wet |
|---|---|---:|---:|---:|---:|---:|---:|---:|
| Default | Original reset settings | 25 | 50 | 30 | 5 | 25 | 100 | 50 |
| Tight | Techno/house grooves, hats and shakers | 25 | 32 | 65 | 8 | 25 | 100 | 18 |
| Snap | Breakbeat, fast snare/clap and percussion | 10 | 15 | 40 | 10 | 10 | 100 | 18 |
| Chamber | Dark hypnotic percussion, rims and toms | 38 | 55 | 70 | 3 | 30 | 100 | 25 |
| Warehouse | Sparse hits, rides and transitions | 68 | 40 | 60 | 20 | 40 | 100 | 25 |
| Disco | Brighter house/Italo claps, snares and stabs | 40 | 50 | 25 | 20 | 30 | 100 | 30 |
| Dub | Leftfield accents and delay into reverb | 52 | 70 | 75 | 30 | 40 | 100 | 35 |
| Ghost | Wet textures with a shorter tail | 72 | 80 | 70 | 4 | 65 | 0 | 100 |
| Texture | Long atmospheric tails and transitions | 100 | 100 | 65 | 40 | 80 | 0 | 100 |

**Ghost** and **Texture** use Dry 0% / Wet 100% on the shared reverb bus: every source routed into reverb loses its direct signal. For a dedicated texture, route only the intended sources into that bus. Manual edits display **Custom** unless all settings match a built-in profile; the global Size encoder assignment remains available. Projects and drumkits keep the existing file format and restore the profile name from the saved settings. Older Room, Hall, Dark and Ambient settings load as Custom with their parameters preserved.

## Repository structure

```
01 Hardware/
  Main/       KiCad project — main board (Drumboy_Main)
  Top/        KiCad project — top/control-surface board (Drumboy_Top)
  Drawing/    Mechanical drawings, enclosure models, graphics, fonts

02 Software/
  Drumboy-Pro-Boot-H723/   Bootloader — STM32CubeIDE project (own repo, linked below)
  Drumboy-Pro-App-H723/    Application firmware — STM32CubeIDE project (own repo, linked below)
  Firmware/                 Prebuilt firmware binaries
  Sd Card/                  Reference SD card contents (samples, presets, drumkits, system data)
```

### Related repositories

Firmware is split into two independently-built projects that share one flash device — a bad application build can never brick the update path:

| Project | Responsibility |
|---|---|
| [Drumboy-Pro-Boot-H723](https://github.com/Randomwaves-Team/Drumboy-Pro-Boot-H723) | Checks the SD card for a firmware package, validates and programs it, then jumps to the application |
| [Drumboy-Pro-App-H723](https://github.com/Randomwaves-Team/Drumboy-Pro-App-H723) | The sequencer, DSP engine, UI, and all runtime behavior |

## Hardware

Designed in **KiCad 7**. Open `01 Hardware/Main/Drumboy_Main.kicad_pro` for the main board, or `01 Hardware/Top/Drumboy_Top.kicad_pro` for the control-surface board. All third-party symbols, footprints, and 3D models are vendored under each project's `Library/` folder.

Mechanical design (enclosure and 3D model) lives in `01 Hardware/Drawing/` as **Rhinoceros 8** files — `Drumboy Pro 2D.3dm` and `Drumboy Pro 3D.zip` (zipped to keep it under GitHub's file size limit; unzip to get the `.3dm`).

A full BOM is generated from KiCad at `production/bom.csv`. See the [wiki](https://www.randomwaves.io/drumboy-pro/drumboy-pro-wiki.html) for the annotated schematic walkthrough, clock tree, power-domain isolation scheme, and key component reference.

## Firmware

Built with **STM32CubeIDE**, written in C/C++ against the STM32 HAL.

- Bootloader owns flash sector 0 (`0x08000000`–`0x0801FFFF`)
- Application owns sectors 1–7 (`0x08020000`–`0x080FFFFF`), with the vector table relocated at startup

To flash over SWD, wire an ST-Link V2 to the board's `SWDIO` (PA13), `SWCLK` (PA14), and `GND` pins, then build and flash from STM32CubeIDE. See the wiki's [Build, Flash & Debug](https://www.randomwaves.io/drumboy-pro/drumboy-pro-wiki.html) section for details.

## License

Released under the [MIT License](LICENSE).

## Community

- 💬 [Discord community](https://discord.gg/5atevNH6UU) — join the discussion
- 🌐 [randomwaves.io](https://www.randomwaves.io)
- ✉️ [hello@randomwaves.io](mailto:hello@randomwaves.io)
