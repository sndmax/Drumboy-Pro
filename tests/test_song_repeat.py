"""Run the real song-repeat, bar-edit, and fill code on an emulated Cortex-M7.

Requires arm-none-eabi-g++ (or ARM_CXX), unicorn, and pyelftools.
Run: python tests/test_song_repeat.py
Generated files stay in the ignored firmware Debug directory.
"""

from pathlib import Path
import os
import shutil
import subprocess

from elftools.elf.elffile import ELFFile
from unicorn import Uc, UC_ARCH_ARM, UC_MODE_THUMB, UC_MODE_MCLASS
from unicorn.arm_const import (
    UC_CPU_ARM_CORTEX_M7, UC_ARM_REG_C1_C0_2, UC_ARM_REG_FPEXC,
    UC_ARM_REG_SP, UC_ARM_REG_LR, UC_ARM_REG_R0, UC_ARM_REG_PC,
)

ROOT = Path(__file__).resolve().parents[1]
APP = ROOT / "02 Software/Drumboy-Pro-App-H723"
SOURCE = (APP / "Core/Library/Controller/Controller.cpp").read_text(encoding="utf-8")
OUTPUT = APP / "Debug/song-repeat-tests"
OUTPUT.mkdir(parents=True, exist_ok=True)


def method(name):
    start = SOURCE.index("void Controller::" + name + "(")
    opening = SOURCE.index("{", start)
    depth, end = 1, opening + 1
    while depth:
        depth += (SOURCE[end] == "{") - (SOURCE[end] == "}")
        end += 1
    return SOURCE[start:end]


NAMES = [
    "rhythm_setBar", "rhythm_menuUp", "layerSong_repeat", "adjustMeasureBarTiming",
    "layerSong_resetBeats", "layerSong_arrangeActiveBeats", "layerSong_calculatePlayBeatNum",
]
METHODS = [method(name) for name in NAMES]
DECLARATIONS = "\n".join(code[:code.index("{")].replace("Controller::", "") + ";" for code in METHODS)

# Exercise the exact keyboard press/release and confirmation dispatch statements.
KEYBOARD = method("keyboard_action")
COPY_PRESS = KEYBOARD.split("case KEY_COPY:", 1)[1].split("switch (menu)", 1)[0]
COPY_RELEASE = KEYBOARD.split("case KEY_COPY:", 2)[2].split("if (layerInstCopyKeyFlag)", 1)[0]
BAR_CONFIRM = KEYBOARD.split("case ALERT_BARUP:", 1)[1].split("break;", 1)[0]
ENCODER_BAR_UP = method("encoder_incValue").split("case ENC_RHYTHM_BAR:", 1)[1].split("break;", 1)[0]

HARNESS = r'''
#include "Global.h"
struct Controller {
    Rhythm rhythm;
    Keyboard keyboard;
    Layer layerLibrary[kLayerLibrarySize] = {Layer(0), Layer(1), Layer(2), Layer(3), Layer(4),
        Layer(5), Layer(6), Layer(7), Layer(8), Layer(9)};
    uint16_t songInterval = kMeasureInterval * kInitialMeasure * kInitialBar;
    uint16_t playInterval = 0;
    uint8_t activeSongBank = 2, menuTab = 2;
    Menu menu = RHYTHM_MENU;
    bool playActive = false, alertFlag = false, rhythmBarDuplicatePending = false;
    AlertType alertType = ALERT_BARUP;
    struct { struct { bool slaveMode = false; } sync; } io;
    unsigned resets = 0;
    void calculateSongInterval() { songInterval = kMeasureInterval * rhythm.measure * rhythm.bar; }
    void reset() {
        ++resets; playInterval = 0;
        for (uint8_t i = 0; i < kLayerLibrarySize; ++i)
            layerSong_calculatePlayBeatNum(i, activeSongBank, 0);
    }
    void rhythm_setTempo(uint8_t) {}
    void rhythm_setMeasure(uint8_t) {}
    void rhythm_setQuantize(uint8_t) {}
    void lcd_clearAlert() {}
    void lcd_drawAlert() {}
    void lcd_drawRhythm_BarData() {}
    void lcd_calculateSongX() {}
    void lcd_resetPlay() {}
    void lcd_drawMeasureBar() {}
    void lcd_drawSong(uint8_t, uint8_t) {}
'''
HARNESS += DECLARATIONS + "\n"
HARNESS += "void copyPress() {" + COPY_PRESS + "}\n"
HARNESS += "void copyRelease() {" + COPY_RELEASE + "}\n"
HARNESS += "void confirmBarUp() {" + BAR_CONFIRM + "}\n"
HARNESS += "void encoderBarUp() {" + ENCODER_BAR_UP + "}\n"
HARNESS += "};\n" + "\n".join(METHODS)
HARNESS += r'''
#define CHECK(condition) do { if (!(condition)) return __LINE__; } while (0)
void seed(Controller& c, uint8_t bars = 1) {
    c.rhythm.measure = 4; c.rhythm.bar = bars; c.rhythm.barLock = false;
    c.calculateSongInterval();
    for (uint8_t i = 0; i < kLayerLibrarySize; ++i) {
        Bank& bank = c.layerLibrary[i].bankLibrary[c.activeSongBank];
        const uint16_t start = i * 25; // Include instruments with initial silence.
        bank.setBeat(0, start, 800);
        bank.setBeatFill(0, 0, 2, 0, 0);
        bank.setBeat(1, 800, c.songInterval);
        bank.setBeatFill(1, 1, 1, 2, 3);
        bank.lastActiveBeatNum = 1;
        Bank& other = c.layerLibrary[i].bankLibrary[1];
        other.setBeat(0, 400, c.songInterval); other.lastActiveBeatNum = 0;
    }
}
extern "C" int run_tests() {
    Controller c{}; seed(c);
    c.copyPress(); c.rhythm_menuUp(); c.copyRelease();
    CHECK(c.rhythm.bar == 2 && c.resets == 1 && !c.keyboard.copyKeyPress);
    for (uint8_t i = 0; i < kLayerLibrarySize; ++i) {
        Bank& bank = c.layerLibrary[i].bankLibrary[c.activeSongBank];
        CHECK(bank.lastActiveBeatNum == 3);
        for (uint8_t j = 0; j < 2; ++j) {
            const Beat& a = bank.beatLibrary[j]; const Beat& b = bank.beatLibrary[j + 2];
            CHECK(a.getData() == b.getData());
            CHECK(b.getStartInterval() == a.getStartInterval() + 3200);
            CHECK(b.getEndInterval() == a.getEndInterval() + 3200);
        }
        const Beat& a = bank.beatLibrary[1]; const Beat& b = bank.beatLibrary[3];
        CHECK(a.getFillTarget() != b.getFillTarget());
        const BeatFill& af = bank.beatFillLibrary[a.getFillTarget()];
        const BeatFill& bf = bank.beatFillLibrary[b.getFillTarget()];
        CHECK(af.getActive() && bf.getActive() && af.getStepSize() == bf.getStepSize());
        for (uint8_t j = 0; j < af.getStepSize(); ++j) {
            CHECK(bf.beatMicroLibrary[j].getInterval() == af.beatMicroLibrary[j].getInterval() + 3200);
            CHECK(bf.beatMicroLibrary[j].getLevel() == af.beatMicroLibrary[j].getLevel());
        }
        CHECK(c.layerLibrary[i].bankLibrary[1].lastActiveBeatNum == 0);
        CHECK(bank.playBeatNum >= 0 && bank.playBeatNum <= bank.lastActiveBeatNum);
    }

    // Holding Copy is captured before the playback prompt and survives release.
    Controller pending{}; seed(pending, 2); pending.playActive = true;
    pending.copyPress(); pending.rhythm_menuUp(); pending.copyRelease();
    CHECK(pending.alertFlag && pending.rhythmBarDuplicatePending && pending.rhythm.bar == 2);
    pending.confirmBarUp();
    CHECK(pending.rhythm.bar == 4 && !pending.rhythmBarDuplicatePending);
    CHECK(pending.layerLibrary[9].bankLibrary[2].lastActiveBeatNum == 3);

    Controller encoder{}; seed(encoder, 2); encoder.copyPress(); encoder.encoderBarUp();
    CHECK(encoder.rhythm.bar == 4 && encoder.layerLibrary[9].bankLibrary[2].lastActiveBeatNum == 3);
    encoder.copyRelease(); encoder.encoderBarUp();
    CHECK(encoder.rhythm.bar == 5 && encoder.layerLibrary[9].bankLibrary[2].lastActiveBeatNum == 3);

    // Copy doubles the entire two-bar song, including hits in its second bar.
    Controller doubled{}; seed(doubled, 2);
    for (uint8_t i = 0; i < kLayerLibrarySize; ++i) {
        Bank& bank = doubled.layerLibrary[i].bankLibrary[2];
        bank.beatLibrary[1].setEndInterval(5000);
        bank.setBeatFill(1, 1, 1, 2, 3);
        bank.setBeat(2, 5000, 6400); bank.setBeatFill(2, 0, 1, 0, 0);
        bank.lastActiveBeatNum = 2;
    }
    doubled.copyPress(); doubled.rhythm_menuUp();
    CHECK(doubled.rhythm.bar == 4);
    for (uint8_t i = 0; i < kLayerLibrarySize; ++i) {
        Bank& bank = doubled.layerLibrary[i].bankLibrary[2];
        CHECK(bank.lastActiveBeatNum == 5);
        for (uint8_t j = 0; j < 3; ++j) {
            CHECK(bank.beatLibrary[j + 3].getData() == bank.beatLibrary[j].getData());
            CHECK(bank.beatLibrary[j + 3].getStartInterval() == bank.beatLibrary[j].getStartInterval() + 6400);
            CHECK(bank.beatLibrary[j + 3].getEndInterval() == bank.beatLibrary[j].getEndInterval() + 6400);
        }
    }
    doubled.rhythm_menuUp(); doubled.copyRelease();
    CHECK(doubled.rhythm.bar == 8 && doubled.layerLibrary[9].bankLibrary[2].lastActiveBeatNum == 11);

    Controller triple{}; seed(triple, 3); triple.copyPress(); triple.encoderBarUp();
    CHECK(triple.rhythm.bar == 6 && triple.layerLibrary[9].bankLibrary[2].lastActiveBeatNum == 3);
    triple.encoderBarUp(); CHECK(triple.rhythm.bar == 12);

    // Reject doubling beyond either limit without partially extending the song or prompting.
    Controller overLimit{}; seed(overLimit, 7); overLimit.copyPress();
    overLimit.rhythm_menuUp(); overLimit.encoderBarUp();
    CHECK(overLimit.rhythm.bar == 7 && overLimit.resets == 0);
    CHECK(overLimit.layerLibrary[9].bankLibrary[2].lastActiveBeatNum == 1);
    overLimit.playActive = true; overLimit.rhythm_menuUp(); CHECK(!overLimit.alertFlag);
    overLimit.rhythm.measure = 8; overLimit.rhythm.bar = 6; overLimit.calculateSongInterval();
    overLimit.rhythm_menuUp(); overLimit.encoderBarUp();
    CHECK(overLimit.rhythm.bar == 6 && overLimit.resets == 0 && !overLimit.alertFlag);

    // A later ordinary edit does not reuse a canceled Copy gesture.
    Controller canceled{}; seed(canceled); canceled.playActive = true;
    canceled.copyPress(); canceled.rhythm_menuUp(); canceled.copyRelease();
    canceled.alertFlag = false; canceled.rhythm_menuUp(); canceled.confirmBarUp();
    CHECK(canceled.layerLibrary[0].bankLibrary[2].lastActiveBeatNum == 1);

    // Ordinary growth, same size, and shrink never append beats.
    Controller ordinary{}; seed(ordinary); ordinary.rhythm_setBar(2, false);
    CHECK(ordinary.layerLibrary[0].bankLibrary[2].lastActiveBeatNum == 1);
    c.rhythm_setBar(2, true); CHECK(c.layerLibrary[0].bankLibrary[2].lastActiveBeatNum == 3);
    c.rhythm_setBar(1, true); CHECK(c.layerLibrary[0].bankLibrary[2].lastActiveBeatNum == 1);

    // Partial extension clips fills at the new end; multiple repetitions use original beats only.
    Controller partial{}; seed(partial, 2); partial.rhythm_setBar(3, true);
    CHECK(partial.layerLibrary[0].bankLibrary[2].beatLibrary[3].getEndInterval() == 9600);
    CHECK(partial.layerLibrary[0].bankLibrary[2].beatLibrary[3].getFillType() == 1);
    Controller multiple{}; seed(multiple); multiple.rhythm_setBar(4, true);
    CHECK(multiple.layerLibrary[0].bankLibrary[2].lastActiveBeatNum == 7);
    CHECK(multiple.layerLibrary[0].bankLibrary[2].beatLibrary[6].getStartInterval() == 9600);

    // Empty instruments, locks, song-length limits, and full banks remain safe.
    Controller empty{}; empty.rhythm.barLock = false; empty.copyPress(); empty.rhythm_menuUp();
    CHECK(empty.layerLibrary[0].bankLibrary[2].lastActiveBeatNum == -1);
    Controller locked{}; seed(locked); locked.rhythm.barLock = true;
    locked.copyPress(); locked.rhythm_menuUp(); CHECK(locked.rhythm.bar == 1);
    unsigned resets = multiple.resets; multiple.rhythm_setBar(13, true);
    CHECK(multiple.rhythm.bar == 4 && multiple.resets == resets);
    multiple.rhythm.measure = kMaxMeasure; multiple.rhythm_setBar(kMaxBar, true);
    CHECK(multiple.rhythm.bar == 4 && multiple.resets == resets);
    Controller full{}; seed(full);
    Bank& bank = full.layerLibrary[0].bankLibrary[2];
    for (uint8_t i = 0; i < 63; ++i) bank.setBeat(i, i * 40, (i + 1) * 40);
    bank.lastActiveBeatNum = 62; full.rhythm_setBar(2, true);
    CHECK(bank.lastActiveBeatNum == 63 && bank.beatLibrary[63].getStartInterval() == 3200);
    CHECK(full.layerLibrary[0].bankLibrary[3].lastActiveBeatNum == -1);
    full.rhythm_setBar(3, true); CHECK(bank.lastActiveBeatNum == 63);

    // No hits in the added region still preserves the ending fill of the original song.
    Controller silent{}; seed(silent, 2);
    Bank& silentBank = silent.layerLibrary[0].bankLibrary[2];
    silentBank.resetBeat(0); silentBank.resetBeat(1);
    silentBank.setBeat(0, 4000, 6400); silentBank.setBeatFill(0, 1, 1, 2, 3);
    silentBank.lastActiveBeatNum = 0; silent.rhythm_setBar(3, true);
    CHECK(silentBank.lastActiveBeatNum == 0 && silentBank.beatLibrary[0].getFillType() == 1);
    CHECK(silentBank.beatLibrary[0].getEndInterval() == 6400);

    // Exhausted fill storage uses the existing basic-beat fallback without sharing fill slots.
    Controller fills{}; seed(fills);
    Bank& fillBank = fills.layerLibrary[0].bankLibrary[2];
    for (uint8_t i = 0; i < 32; ++i) {
        fillBank.setBeat(i, i * 100, (i + 1) * 100);
        fillBank.setBeatFill(i, 1, 1, 2, 3);
    }
    fillBank.lastActiveBeatNum = 31; fills.rhythm_setBar(2, true);
    CHECK(fillBank.lastActiveBeatNum == 63);
    for (uint8_t i = 0; i < 32; ++i) {
        CHECK(fillBank.beatLibrary[i].getFillType() == 1);
        CHECK(fillBank.beatLibrary[i + 32].getFillTarget() == NULL_FILL_TARGET);
    }
    return 0;
}
'''

cpp, elf = OUTPUT / "song_repeat.cpp", OUTPUT / "song_repeat.elf"
cpp.write_text(HARNESS, encoding="utf-8")
compiler = os.environ.get("ARM_CXX") or shutil.which("arm-none-eabi-g++")
if not compiler:
    raise SystemExit("Set ARM_CXX or add arm-none-eabi-g++ to PATH")
includes = [
    "Core/Inc", "Core/Library/Global", "FATFS/Target", "FATFS/App",
    "Drivers/STM32H7xx_HAL_Driver/Inc", "Drivers/CMSIS/Device/ST/STM32H7xx/Include",
    "Drivers/CMSIS/Include", "Middlewares/Third_Party/FatFs/src",
]
subprocess.run([
    compiler, str(cpp), "-o", str(elf), "-std=gnu++14", "-O2", "-mcpu=cortex-m7",
    # Unicorn's M7 implements single precision; emit software double operations for fill math.
    "-mthumb", "-mfpu=fpv5-sp-d16", "-mfloat-abi=hard", "-fno-exceptions", "-fno-rtti",
    "-DUSE_HAL_DRIVER", "-DSTM32H723xx", "-ffunction-sections", "-fdata-sections",
    "-nostartfiles", "--specs=nosys.specs", "-Wl,--gc-sections", "-Wl,-e,run_tests",
    "-Wl,-Ttext=0x10000", "-Wl,-Tdata=0x200000",
    *["-I" + str(APP / path) for path in includes],
], check=True)

machine = Uc(UC_ARCH_ARM, UC_MODE_THUMB | UC_MODE_MCLASS)
machine.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_M7)
machine.mem_map(0, 16 * 1024 * 1024)
with elf.open("rb") as stream:
    binary = ELFFile(stream)
    for segment in binary.iter_segments():
        if segment["p_type"] == "PT_LOAD":
            machine.mem_write(segment["p_vaddr"], segment.data())
    entry = binary.header["e_entry"]
machine.reg_write(UC_ARM_REG_C1_C0_2, 0xF00000)
machine.reg_write(UC_ARM_REG_FPEXC, 0x40000000)
machine.reg_write(UC_ARM_REG_SP, 0x800000)
machine.reg_write(UC_ARM_REG_LR, 0xF00001)
try:
    machine.emu_start(entry | 1, 0xF00000, timeout=30_000_000)
except Exception:
    print(f"Emulator stopped at PC=0x{machine.reg_read(UC_ARM_REG_PC):x}")
    raise
if machine.reg_read(UC_ARM_REG_PC) != 0xF00000:
    raise TimeoutError("Song repeat regression did not return before the emulator timeout")
failure = machine.reg_read(UC_ARM_REG_R0)
if failure:
    raise AssertionError(f"ARM regression failed at {cpp}:{failure}: {HARNESS.splitlines()[failure - 1]}")
print("PASS: Copy doubles 1/2/3/4/6 bars, all instruments and fills, playback confirmation, ordinary edits, empty/locked/full banks, length limits")
