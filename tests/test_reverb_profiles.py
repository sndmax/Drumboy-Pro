"""Compile the real reverb code for Cortex-M7 and execute it with Unicorn.

Requires arm-none-eabi-g++ on PATH and Python packages unicorn and pyelftools.
Run: python tests/test_reverb_profiles.py
Generated code and binaries stay in the ignored firmware Debug directory.
Only hardware, display, and unrelated routing inputs are stubbed.
"""

from pathlib import Path
import os
import re
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
GLOBAL = (APP / "Core/Library/Global/Global.h").read_text(encoding="utf-8")
SOURCE = (APP / "Core/Library/Controller/Controller.cpp").read_text(encoding="utf-8")
OUTPUT = APP / "Debug/reverb-tests"
OUTPUT.mkdir(parents=True, exist_ok=True)


def method(name):
    start = SOURCE.index("void Controller::" + name + "(")
    opening = SOURCE.index("{", start)
    depth = 1
    end = opening + 1
    while depth:
        depth += (SOURCE[end] == "{") - (SOURCE[end] == "}")
        end += 1
    return SOURCE[start:end]


NAMES = [
    "file_newAction", "drumkit_newAction",
    "reverb_reset", "reverb_setProfile", "reverb_stepProfile", "reverb_processProfile",
    "reverb_loadSettings", "reverb_saveSettings", "reverb_setActive", "reverb_setSize",
    "reverb_setDecay", "reverb_setDamping", "reverb_setPreDelay", "reverb_setSurround",
    "reverb_setDry", "reverb_setWet", "reverb_genTransition", "reverb_mixTransition",
    "reverb_calculateGenTransition", "reverb_calculateMixTransition", "processAudioReverb",
]
METHODS = [method(name) for name in NAMES]
DECLARATIONS = "\n".join(code[:code.index("{")].replace("Controller::", "") + ";" for code in METHODS)
NUMBERS = GLOBAL[GLOBAL.index("struct NumberData {"):]
NUMBERS = NUMBERS[:NUMBERS.index("};", NUMBERS.index("kNumberDataLibrary")) + 2]
REVERB = GLOBAL[GLOBAL.index("const uint8_t kMinReverbSize"):GLOBAL.index("/* Lfo Constants")]
ENCODER = GLOBAL[GLOBAL.index("typedef enum {", GLOBAL.index("// ENC_FNC_OFF")):]
ENCODER = ENCODER[:ENCODER.index("} EncoderFunc;") + len("} EncoderFunc;")]
TRANSITIONS = method("interruptTransition")
TRANSITIONS = TRANSITIONS[TRANSITIONS.index("    // 7. REVERB TRANSITIONS"):]

HARNESS = r'''
#include <stdint.h>
#include <string.h>
#define AUDIO_SAMPLE_RATE 44100.0f
#define I2S_BLOCK_SIZE 32
static uint32_t irqState;
uint32_t __get_PRIMASK() { return irqState; }
void __disable_irq() { irqState = 1; }
void __set_PRIMASK(uint32_t value) { irqState = value; }
'''
HARNESS += NUMBERS + "\n" + REVERB + "\n" + ENCODER
HARNESS += r'''
const int REVERB_MENU = 13, MIXER_IN_REVERB = 5, kLayerLibrarySize = 1;
const int kBankLibrarySize = 5, kLfoLibrarySize = 1;
struct Controller {
    Reverb reverb;
    bool loadAction = false, transitionClearFlag = false, transitionClearMenu = false;
    int menu = REVERB_MENU, transitionShowFlag = 0;
    int activeSongBank = 0;
    float audioReverb_L[32]{}, audioReverb_R[32]{}, audioEffect[2][32]{};
    float audioReceive[32]{}, audioMetronome[32]{}, audioLayer[1][32]{};
    struct { int liMixerInMode = 0; } mixer;
    struct { bool active = false; int mixerInMode = 0; } metroPlayData;
    struct { struct { int mixerInMode = 0; } activeBeatPlayData; } layerPlayData[1];
    void lcd_updateGlobalMenuData(EncoderFunc) {}
    void checkMenuTransition(int, bool) {}
    void checkEncoderTransition(EncoderFunc, bool) {}
    void lcd_drawReverb_ActiveData() {}
    void lcd_drawReverb_ProfileData() {}
    void lcd_drawReverb_DecayData() {}
    void lcd_drawReverb_DampingData() {}
    void lcd_drawReverb_PreDelayData() {}
    void lcd_drawReverb_SurroundData() {}
    void lcd_drawReverb_DryData() {}
    void lcd_drawReverb_WetData() {}
    void lcd_clearAlert() {}
    void reset() {}
    void lcd_drawSongBankTab(int, bool) {}
    void layerInst_reset(int) {}
    void layerSong_reset(int, int) {}
    void lfo_reset(int) {}
    void rhythm_reset() {}
    void eq_reset() {}
    void filter_reset(int) {}
    void effect_reset(int) {}
    void interruptTransition();
'''
HARNESS += DECLARATIONS + "\n};\n" + "\n".join(METHODS)
HARNESS += "\nvoid Controller::interruptTransition() { const float kChangeVal = 0.0002f;\n" + TRANSITIONS
HARNESS += r'''
#define CHECK(condition) do { if (!(condition)) return __LINE__; } while (0)
bool settle(Controller& c) {
    for (int tick = 0; tick < 60000; ++tick) {
        c.reverb_processProfile();
        c.interruptTransition();
        if (!c.reverb.profileBusy() && !c.reverb.genTransition.active && !c.reverb.mixTransition.active) return true;
    }
    return false;
}
extern "C" int run_tests() {
    Controller c{};
    c.reverb.initialize();
    CHECK(c.reverb.profile() == REV_PROFILE_TIGHT);
    CHECK(REV_PROFILE_COUNT == 9);
    CHECK(ENC_REVERB_SIZE == 0x3E && ENC_FNC_NONE == 0x61 && ENC_REVERB_PROFILE == 0x62);

    // New projects and drumkits start with Tight; explicit reset retains Default.
    c.reverb_reset(); CHECK(settle(c));
    CHECK(c.reverb.profile() == REV_PROFILE_DEFAULT);
    c.file_newAction(); CHECK(settle(c));
    CHECK(c.reverb.profile() == REV_PROFILE_TIGHT);
    c.reverb_reset(); CHECK(settle(c));
    c.drumkit_newAction(); CHECK(settle(c));
    CHECK(c.reverb.profile() == REV_PROFILE_TIGHT);

    // Full application, indices in real time tables, names, and storage round trips.
    const ReverbSettings expected[] = {
        {true, 25, 50, 30,  5,  5, 100,  50},
        {true, 25, 32, 65,  8,  5, 100,  18},
        {true, 10, 15, 40, 10,  2, 100,  18},
        {true, 38, 55, 70,  3,  6, 100,  25},
        {true, 68, 40, 60, 11,  8, 100,  25},
        {true, 40, 50, 25, 11,  6, 100,  30},
        {true, 52, 70, 75, 12,  8, 100,  35},
        {true, 72, 80, 70,  4, 13,   0, 100},
        {true,100,100, 65, 13, 16,   0, 100}
    };
    const char* names[] = {"   Default", "     Tight", "      Snap", "   Chamber",
        " Warehouse", "     Disco", "       Dub", "     Ghost", "   Texture"};
    const int predelayMs[] = {5, 8, 10, 3, 20, 20, 30, 4, 40};
    const int surroundPct[] = {25, 25, 10, 30, 40, 30, 40, 65, 80};
    uint8_t saved[180];
    for (uint8_t id = 0; id < REV_PROFILE_COUNT; ++id) {
        c.reverb_setProfile(id);
        CHECK(c.reverb.displayedProfile() == id);
        CHECK(settle(c));
        CHECK(c.reverb.settings() == expected[id]);
        CHECK(c.reverb.settings() == kReverbProfileLibrary[id].settings);
        CHECK(c.reverb.profile() == id && strlen(c.reverb.profileName()) == 10);
        CHECK(strcmp(c.reverb.profileName(), names[id]) == 0);
        CHECK(int(kReverbPreDelayDataLibrary[c.reverb.preDelay].data * 1000 + 0.5f) == predelayMs[id]);
        CHECK(c.reverb.surround * 5 == surroundPct[id]);
        memset(saved, 0xA5, sizeof(saved));
        c.reverb_saveSettings(saved + 167);
        CHECK(saved[166] == 0xA5 && saved[175] == 0xA5);
        c.reverb_setProfile(REV_PROFILE_DEFAULT);
        c.reverb_loadSettings(reinterpret_cast<char*>(saved + 167));
        CHECK(!c.reverb.profileBusy() && !c.reverb.genTransition.active);
        CHECK(c.reverb.profile() == id);
    }

    // Every profile is reachable in order in both directions, including wrapping.
    c.reverb_setProfile(REV_PROFILE_DEFAULT); CHECK(settle(c));
    for (uint8_t step = 1; step <= REV_PROFILE_COUNT; ++step) {
        c.reverb_stepProfile(true); CHECK(settle(c));
        CHECK(c.reverb.profile() == step % REV_PROFILE_COUNT);
    }
    for (uint8_t step = 1; step <= REV_PROFILE_COUNT; ++step) {
        c.reverb_stepProfile(false); CHECK(settle(c));
        CHECK(c.reverb.profile() == (REV_PROFILE_COUNT - step) % REV_PROFILE_COUNT);
    }
    c.reverb_setProfile(REV_PROFILE_TEXTURE); CHECK(settle(c));

    // Manual edits produce Custom, restore exact matches, and obey the mix limit.
    c.reverb_setDry(5);
    CHECK(c.reverb.profile() == REV_PROFILE_CUSTOM);
    CHECK(settle(c));
    c.reverb_setDry(0);
    CHECK(settle(c));
    CHECK(c.reverb.profile() == REV_PROFILE_TEXTURE);
    CHECK(c.reverb.dryFloat == 0 && c.reverb.wetFloat == 1);
    c.reverb_setDry(100);
    CHECK(c.reverb.dry + c.reverb.wet == 150);
    CHECK(settle(c));
    c.reverb_stepProfile(true);
    CHECK(c.reverb.requestedProfile == REV_PROFILE_DEFAULT);
    CHECK(settle(c));
    c.reverb_stepProfile(false);
    CHECK(c.reverb.requestedProfile == REV_PROFILE_TEXTURE);
    CHECK(settle(c));
    c.reverb_setDamping(64);
    c.reverb_stepProfile(false);
    CHECK(c.reverb.requestedProfile == REV_PROFILE_TEXTURE);
    CHECK(settle(c));

    // Existing generator/mix transitions must finish; only the latest request wins.
    c.reverb_setPreDelay(5);
    CHECK(c.reverb.genTransition.active);
    c.reverb_setDry(10);
    CHECK(c.reverb.mixTransition.active);
    c.reverb_setProfile(REV_PROFILE_TIGHT);
    c.reverb_setProfile(REV_PROFILE_WAREHOUSE);
    const ReverbSettings blocked = c.reverb.settings();
    c.reverb_setActive(false); c.reverb_setSize(0); c.reverb_setDecay(0);
    c.reverb_setDamping(0); c.reverb_setPreDelay(0); c.reverb_setSurround(0);
    c.reverb_setDry(0); c.reverb_setWet(0);
    CHECK(c.reverb.settings() == blocked);
    c.reverb_processProfile();
    CHECK(c.reverb.applyingProfile == REV_PROFILE_CUSTOM);
    CHECK(settle(c));
    CHECK(c.reverb.profile() == REV_PROFILE_WAREHOUSE);

    // Persist the old complete tuple before midpoint, the new one after midpoint.
    c.reverb_setProfile(REV_PROFILE_TIGHT);
    c.transitionClearMenu = true;
    c.reverb_processProfile();
    CHECK(!c.transitionClearMenu);
    c.reverb_saveSettings(saved + 167);
    CHECK(saved[168] == 68 && saved[169] == 40 && saved[174] == 25);
    c.reverb_setProfile(REV_PROFILE_TEXTURE);
    CHECK(c.reverb.applyingProfile == REV_PROFILE_TIGHT);
    while (c.reverb.genTransition.phase == REV_PHASE_A) c.interruptTransition();
    CHECK(c.reverb.profile() == REV_PROFILE_TIGHT);
    c.reverb_saveSettings(saved + 167);
    CHECK(saved[168] == 25 && saved[169] == 32 && saved[174] == 18);
    CHECK(settle(c));
    CHECK(c.reverb.profile() == REV_PROFILE_TEXTURE);
    c.reverb_saveSettings(saved + 167);
    CHECK(saved[173] == 0 && saved[174] == 100);
    // An explicit disable transition also precedes selection from bypass.
    c.reverb_setActive(false);
    CHECK(c.reverb.genTransition.active);
    c.reverb_setProfile(REV_PROFILE_TIGHT);
    CHECK(settle(c));
    CHECK(c.reverb.profile() == REV_PROFILE_TIGHT && c.reverb.active);
    // Restore Tight as the saved tuple used in the cancellation test below.
    c.reverb_saveSettings(saved + 167);

    // Load during a fade cancels it and also cancels any newer queued request.
    c.reverb_setProfile(REV_PROFILE_CHAMBER);
    c.reverb_processProfile();
    c.reverb_setProfile(REV_PROFILE_DUB);
    c.reverb_loadSettings(reinterpret_cast<char*>(saved + 167));
    CHECK(c.reverb.profile() == REV_PROFILE_TIGHT && !c.reverb.profileBusy());
    CHECK(!c.reverb.genTransition.active && !c.reverb.mixTransition.active);
    saved[167] = 0; saved[168] = 42;
    c.reverb_loadSettings(reinterpret_cast<char*>(saved + 167));
    CHECK(c.reverb.profile() == REV_PROFILE_CUSTOM && !c.reverb.active);
    c.reverb_setProfile(REV_PROFILE_TEXTURE);
    c.reverb_processProfile();
    CHECK(c.reverb.genTransition.phase == REV_PHASE_B);
    CHECK(c.reverb.active && c.reverb.dryFloat == 0 && c.reverb.wetFloat == 1);
    CHECK(settle(c));

    // Previous catalog tuples load unchanged; profile IDs were never serialized.
    const uint8_t legacy[][8] = {
        {1, 25, 50, 30,  5,  5, 100,  50}, // Default
        {1, 10, 15, 45,  2,  2, 100,  25}, // Room
        {1, 65, 80, 25, 11,  8, 100,  45}, // Hall
        {1, 45, 70, 90, 10,  5, 100,  40}, // Dark
        {1,100,100, 55, 13, 12,  70,  70}, // Ambient
        {1,100,100, 65, 13, 16,   0, 100}  // Texture
    };
    for (int id = 0; id < 6; ++id) {
        c.reverb_loadSettings(reinterpret_cast<const char*>(legacy[id]));
        CHECK(c.reverb.profile() == (id == 0 ? REV_PROFILE_DEFAULT :
            id == 5 ? REV_PROFILE_TEXTURE : REV_PROFILE_CUSTOM));
        c.reverb_saveSettings(saved + 167);
        CHECK(memcmp(saved + 167, legacy[id], 8) == 0);
    }
    // Invalid saves fall back to original Default, not the new starting Tight.
    saved[168] = 101;
    c.reverb_setProfile(REV_PROFILE_DUB);
    c.reverb_loadSettings(reinterpret_cast<char*>(saved + 167));
    CHECK(c.reverb.profile() == REV_PROFILE_DEFAULT && !c.reverb.profileBusy());
    CHECK(c.reverb.settings() == expected[REV_PROFILE_DEFAULT]);

    // Real DSP: both fully wet profiles suppress the direct impulse and produce stereo tails.
    for (uint8_t id = REV_PROFILE_GHOST; id <= REV_PROFILE_TEXTURE; ++id) {
        c.reverb_setProfile(id); CHECK(settle(c));
        c.reverb.cleanMemory();
        c.audioEffect[1][0] = 1.0f;
        c.processAudioReverb();
        CHECK(c.reverb.dry == 0 && c.reverb.wet == 100);
        for (int i = 0; i < 32; ++i) {
            CHECK(c.audioReverb_L[i] == 0 && c.audioReverb_R[i] == 0);
            c.audioEffect[1][i] = 0;
        }
        bool tailL = false, tailR = false;
        for (int block = 0; block < 400; ++block) {
            c.processAudioReverb();
            for (int i = 0; i < 32; ++i) {
                tailL |= c.audioReverb_L[i] > 0.00001f || c.audioReverb_L[i] < -0.00001f;
                tailR |= c.audioReverb_R[i] > 0.00001f || c.audioReverb_R[i] < -0.00001f;
            }
        }
        CHECK(tailL && tailR);
        c.reverb_saveSettings(saved + 167);
        CHECK(saved[173] == 0 && saved[174] == 100);
        c.reverb_loadSettings(reinterpret_cast<char*>(saved + 167));
        CHECK(c.reverb.profile() == id && c.reverb.dryFloat == 0 && c.reverb.wetFloat == 1);
    }
    c.reverb_reset(); CHECK(settle(c));
    CHECK(c.reverb.profile() == REV_PROFILE_DEFAULT);
    c.reverb_setProfile(255); CHECK(!c.reverb.profileBusy());
    CHECK(irqState == 0);
    return 0;
}
'''

# Check the integration paths as well as executing the extracted methods.
assert SOURCE.count("reverb_loadSettings(data + 167);") == 2
assert SOURCE.count("reverb_saveSettings(data + 167);") == 2
assert "{ENC_REVERB_ACTIVE, ENC_REVERB_PROFILE," in GLOBAL
assert "kMaxEncoderGlobalFunction             = 68;" in GLOBAL
metadata = GLOBAL[GLOBAL.index("const EncoderFuncData kEncoderFuncDataLibrary[]"):]
metadata = metadata[:metadata.index("};")]
entries = re.findall(r"\{(ENC_\w+),", metadata)
assert entries[0x3E] == "ENC_REVERB_SIZE"
assert entries[0x61:0x63] == ["ENC_FNC_NONE", "ENC_REVERB_PROFILE"]

cpp = OUTPUT / "reverb_profiles.cpp"
elf = OUTPUT / "reverb_profiles.elf"
cpp.write_text(HARNESS, encoding="utf-8")
compiler = os.environ.get("ARM_CXX") or shutil.which("arm-none-eabi-g++")
if not compiler:
    raise SystemExit("Set ARM_CXX or add arm-none-eabi-g++ to PATH")
subprocess.run([
    compiler, str(cpp), "-o", str(elf), "-std=gnu++14", "-O2", "-mcpu=cortex-m7",
    "-mthumb", "-mfpu=fpv5-d16", "-mfloat-abi=hard", "-fno-exceptions", "-fno-rtti",
    "-ffunction-sections", "-fdata-sections", "-nostartfiles", "--specs=nosys.specs",
    "-Wl,--gc-sections", "-Wl,-e,run_tests", "-Wl,-Ttext=0x10000", "-Wl,-Tdata=0x200000",
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
machine.emu_start(entry | 1, 0xF00000, timeout=30_000_000)
if machine.reg_read(UC_ARM_REG_PC) != 0xF00000:
    raise TimeoutError("ARM regression did not return before the emulator timeout")
failure = machine.reg_read(UC_ARM_REG_R0)
if failure:
    raise AssertionError(f"ARM regression failed at {cpp}:{failure}: {HARNESS.splitlines()[failure - 1]}")
print("PASS: 9 profiles, Tight startup/new files/kits, Custom/cycling, queued transitions, legacy storage/load, Default reset/fallback, Ghost/Texture DSP")
