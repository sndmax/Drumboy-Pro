#include "Controller.h"

////////////////////////////////////////////////////////////////////////////////
/* Lifecycle Functions -------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////

Controller::Controller()
    : lcd(LANDSCAPE_1, WHITE, BLACK, FONT_07x09),
      dac(),
      sd(),

      sdBusy(false),
      sdInsertCheck(false),

      power(false),

      lcdTest(false) {
}

Controller::~Controller() {}

void Controller::initialize() {
    /// @brief Initializes boot workflow and attempts firmware update before app jump.

    // 1. BOOT STATUS INDICATOR
    // Light boot LED while update/startup flow is running.
    LED0_ON;

    // 2. FIRMWARE UPDATE FLOW
    // Mount SD and process firmware package if present.
    sd_initialize();
    updateFirmware();

    // 3. APPLICATION JUMP
    // Jump to the application image after boot sequence.
    startApplication();
}

void Controller::updateFirmware() {
    /// @brief Scans SD card for firmware package and performs in-place flash update.

    // 1. DEFAULT RESULT STATE
    // Assume failure until the full validation/programming pipeline succeeds.
    FwResult fwResult = FW_ERROR;

    // 2. SD DETECT + MOUNT
    // Continue only when card is present and filesystem is mounted.
    if ((sd_detect() == SD_OK) && (sd_mount() == SD_OK)) {
        sd.fresult = f_findfirst(&sd.dir, &sd.fileInfo, "", "Drumboy_Pro_Fw?*.bin");

        // 3. FIRMWARE FILE DISCOVERY
        // Search expected firmware naming pattern and open the first candidate.
        if (sd.fileInfo.fname[0] != '\0') {
            if (sd_checkFileExist(sd.fileInfo.fname) == SD_OK) {
                if (f_open(&sd.file, sd.fileInfo.fname, FA_READ) == FR_OK) {
                    char data[40] = "";
                    char headerTitle[] = "RW_DRUMBOY_PRO_FIRMWARE ";

                    // 4. HEADER VALIDATION
                    // Verify file signature before reading metadata.
                    if (f_read(&sd.file, data, 24, &sd.bytesread) == FR_OK) {
                        lcd.initialize();
                        lcd.displayOn();

                        HAL_Delay(500);

                        if (strcmp(data, headerTitle) == 0) {
                            memset(data, 0x00, sizeof(data));

                            // 5. METADATA LOAD + RANGE CHECKS
                            // Read version/size/CRC and validate compatibility/limits.
                            if (f_read(&sd.file, data, 12, &sd.bytesread) == FR_OK) {
                                uint16_t *versionMajor = (uint16_t *)&(data[0]);
                                uint16_t *versionMinor = (uint16_t *)&(data[2]);
                                uint32_t *firmwareSize = (uint32_t *)&(data[4]);
                                uint32_t *crcRef = (uint32_t *)&(data[8]);

                                uint32_t readSize = 10240;
                                uint32_t fileSize = f_size(&sd.file);
                                uint16_t readCount = *firmwareSize / readSize;
                                uint16_t remainder = *firmwareSize % readSize;

                                if ((*versionMajor < 10) && (*versionMinor < 100)) {
                                    if (((*firmwareSize + 36) == fileSize) && (*firmwareSize > 0) && (*firmwareSize < 917504)) {

                                        // 6. PAYLOAD READ INTO SDRAM
                                        // Copy firmware payload from file to RAM staging area.
                                        for (uint16_t i = 0; i < (readCount); i++) {
                                            f_read(&sd.file, (uint8_t *)(RAM_FIRMWARE_ADDRESS + (i * readSize)), readSize, &sd.bytesread);
                                        }
                                        f_read(&sd.file, (uint8_t *)(RAM_FIRMWARE_ADDRESS + (readCount * readSize)), remainder, &sd.bytesread);

                                        // 7. CRC VALIDATION
                                        // Reject the staged payload before touching flash if its CRC doesn't match the header.
                                        uint32_t crcValue = crc_update(0, (const void *)(RAM_FIRMWARE_ADDRESS), *firmwareSize);

                                        if (crcValue == (*crcRef)) {

                                            // 8. FLASH PROGRAM
                                            // Erase target sectors and write staged payload to flash.
                                            lcd.drawSdFirmwareAlert(true);

                                            lcd.setForeColor(GREEN);
                                            lcd.setBackColor(BLACK);
                                            lcd.setFont(FONT_07x09);
                                            lcd.setAlignment(CENTER);
                                            uint16_t xPos = 380;
                                            uint16_t yPos = 412;
                                            lcd.drawHLine(xPos, yPos, 200);
                                            lcd.drawHLine(xPos, yPos + 24, 200);
                                            lcd.drawHalfCircle(xPos - 1, yPos + 12, 12, 1);
                                            lcd.drawHalfCircle(xPos + 200, yPos + 12, 12, 3);

                                            lcd.drawText("UPDATING FIRMWARE", 17, 480, 420);

                                            lcd.setForeColor(GRAY_25);
                                            lcd.drawHLine(380, 365, 200);

                                            lcd.setForeColor(GREEN);
                                            lcd.setBackColor(BLACK);
                                            lcd.setFont(FONT_05x07);
                                            lcd.setAlignment(CENTER);
                                            char textVersion[7];
                                            sprintf(textVersion, "V%01d.%02d", *versionMajor, *versionMinor);
                                            lcd.drawText(textVersion, strlen(textVersion), 480, 338);

                                            if (flash_eraseSector(FLASH_FIRMWARE_ADDRESS, fileSize / 4) == HAL_OK) {
                                                HAL_Delay(250);
                                                if (flash_writeData(FLASH_FIRMWARE_ADDRESS, (uint32_t *)(RAM_FIRMWARE_ADDRESS), (*firmwareSize) / 4) == HAL_OK) {

                                                    // 8b. POST-WRITE VERIFICATION
                                                    // Re-read the programmed flash region and recheck CRC before declaring success.
                                                    uint32_t verifyCrc = crc_update(0, (const void *)(FLASH_FIRMWARE_ADDRESS), *firmwareSize);

                                                    if (verifyCrc == (*crcRef)) {
                                                        fwResult = FW_OK;
                                                        lcd.setForeColor(GREEN);
                                                        lcd.setBackColor(BLACK);
                                                        for (uint8_t i = 0; i < 200; i++) {
                                                            lcd.drawPixel(380 + i, 365);
                                                            HAL_Delay(5);
                                                        }
                                                        HAL_Delay(500);
                                                    } else {
                                                        fwResult = FW_ERROR_VERIFY;
                                                    }
                                                }
                                            }
                                        } else {
                                            fwResult = FW_ERROR_CRC;
                                        }
                                    } else {
                                        fwResult = FW_ERROR_SIZE;
                                    }
                                } else {
                                    fwResult = FW_ERROR_VERSION;
                                }
                            } else {
                                fwResult = FW_ERROR_FORMAT;
                            }
                        }

                        // 9. RESULT FEEDBACK UI
                        // Show success or detailed failure message.
                        if (fwResult == FW_OK) {
                            lcd.setForeColor(GREEN);
                            lcd.setBackColor(BLACK);
                            lcd.setFont(FONT_07x09);
                            lcd.setAlignment(CENTER);
                            lcd.drawText(" FIRMWARE UPDATED ", 18, 480, 420);
                        } else {
                            lcd.drawSdFirmwareAlert(false);

                            lcd.setForeColor(RED);
                            lcd.setBackColor(BLACK);
                            lcd.setFont(FONT_07x09);
                            lcd.setAlignment(CENTER);
                            uint16_t xPos = 380;
                            uint16_t yPos = 412;
                            lcd.drawHLine(xPos, yPos, 200);
                            lcd.drawHLine(xPos, yPos + 24, 200);
                            lcd.drawHalfCircle(xPos - 1, yPos + 12, 12, 1);
                            lcd.drawHalfCircle(xPos + 200, yPos + 12, 12, 3);
                            lcd.drawText("  FIRMWARE ERROR  ", 18, 480, 420);

                            lcd.setFont(FONT_05x07);
                            switch (fwResult) {
                            case FW_ERROR:
                                lcd.drawText("FORMAT-CHECK", 12, 480, 360);
                                break;

                            case FW_ERROR_FORMAT:
                                lcd.drawText("FORMAT-CHECK", 12, 480, 360);
                                break;

                            case FW_ERROR_VERSION:
                                lcd.drawText("VERSION-CHECK", 13, 480, 360);
                                break;

                            case FW_ERROR_SIZE:
                                lcd.drawText("SIZE-CHECK", 10, 480, 360);
                                break;

                            case FW_ERROR_CRC:
                                lcd.drawText("CRC-CHECK", 9, 480, 360);
                                break;

                            case FW_ERROR_VERIFY:
                                lcd.drawText("VERIFY-CHECK", 12, 480, 360);
                                break;

                            default:
                                break;
                            }
                        }
                    }

                    // 10. FILE FINALIZATION
                    // Move successful package into archive folder, otherwise delete it.
                    f_close(&sd.file);
                    if (fwResult == FW_OK) {
                        sd_deleteDirectory("System/Firmware");
                        sd_createDirectory("System/Firmware");
                        char fName[sizeof(sd.fileInfo.fname) + 16];
                        strcpy(fName, "System/Firmware/");
                        strcat(fName, sd.fileInfo.fname);
                        f_rename(sd.fileInfo.fname, fName);
                    } else {
                        f_unlink(sd.fileInfo.fname);
                    }
                    HAL_Delay(2500);
                    lcd.clearSdFirmwareAlert();
                }
            }
        }
    }
}

void Controller::startApplication() {
    /// @brief Transfers execution to the application vector table entry point.

    void (*SysMemBootJump)(void);
    SysMemBootJump = (void (*)(void))(*((uint32_t *)(FLASH_FIRMWARE_ADDRESS + 4)));

    // 1. INTERRUPT DISABLE
    // Prevent any IRQ from firing between MSP update and the jump.
    // SysTick is stopped explicitly to avoid a tick interrupt during the transition.
    __disable_irq();
    SysTick->CTRL = 0;

    // 2. STACK POINTER UPDATE + JUMP
    __set_MSP(*(uint32_t *)FLASH_FIRMWARE_ADDRESS);
    SysMemBootJump();
}

////////////////////////////////////////////////////////////////////////////////
/* Crc Functions ---------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////

uint32_t Controller::crc_update(uint32_t initial, const void *buf, uint32_t len) {
    // 1. CRC-32 (zlib/ISO-3309 polynomial) over an arbitrary buffer.
    // Pass initial=0 for a standalone checksum, or a prior return value to continue across chunks.
    uint32_t c = initial ^ 0xFFFFFFFF;
    const uint8_t *u = static_cast<const uint8_t *>(buf);
    for (uint32_t i = 0; i < len; ++i) {
        c = kCrcTable[(c ^ u[i]) & 0xFF] ^ (c >> 8);
    }
    return c ^ 0xFFFFFFFF;
}

////////////////////////////////////////////////////////////////////////////////
/* Flash Functions -----------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////

uint32_t Controller::flash_getSector(uint32_t address) {
    // 1. ADDRESS-TO-SECTOR MAPPING
    // Resolve STM32H7 flash sector from absolute address.
    uint32_t sector = 0;

    if ((address >= 0x08000000) && (address < 0x08020000)) {
        sector = FLASH_SECTOR_0;
    }

    else if ((address >= 0x08020000) && (address < 0x08040000)) {
        sector = FLASH_SECTOR_1;
    }

    else if ((address >= 0x08040000) && (address < 0x08060000)) {
        sector = FLASH_SECTOR_2;
    }

    else if ((address >= 0x08060000) && (address < 0x08080000)) {
        sector = FLASH_SECTOR_3;
    }

    else if ((address >= 0x08080000) && (address < 0x080A0000)) {
        sector = FLASH_SECTOR_4;
    }

    else if ((address >= 0x080A0000) && (address < 0x080C0000)) {
        sector = FLASH_SECTOR_5;
    }

    else if ((address >= 0x080C0000) && (address < 0x080E0000)) {
        sector = FLASH_SECTOR_6;
    }

    else if ((address >= 0x080E0000) && (address < 0x08100000)) {
        sector = FLASH_SECTOR_7;
    }

    return sector;
}

uint32_t Controller::flash_eraseSector(uint32_t startSectorAddress, uint32_t wordSize) {
    // 1. ERASE DESCRIPTOR SETUP
    // Compute erase bounds and configure HAL erase struct.
    static FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SECTORError;

    // 2. FLASH UNLOCK + ERASE
    // Perform sector erase transaction.
    HAL_FLASH_Unlock();

    uint32_t startSector = flash_getSector(startSectorAddress);
    uint32_t endSectorAddress = startSectorAddress + (wordSize * 4);
    uint32_t endSector = flash_getSector(endSectorAddress);

    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    EraseInitStruct.Banks = FLASH_BANK_1;
    EraseInitStruct.Sector = startSector;
    EraseInitStruct.NbSectors = (endSector - startSector) + 1;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK) {
        HAL_FLASH_Lock();
        return HAL_FLASH_GetError();
    }

    // 3. FLASH RELOCK
    // Re-lock flash after erase operation.
    HAL_FLASH_Lock();
    return 0;
}

uint32_t Controller::flash_writeData(uint32_t startSectorAddress, uint32_t *data, uint32_t wordSize) {
    // 1. PROGRAM STRIDE SETUP
    // 256 bits for STM32H72x/3X devices (8x 32-bit words).

    int sofar = 0;
    uint8_t flashWord = 8;

    // 2. FLASHWORD PROGRAM LOOP
    // Program payload in flashword chunks.
    HAL_FLASH_Unlock();

    while (sofar < wordSize) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, startSectorAddress, (uint32_t)&data[sofar]) == HAL_OK) {
            startSectorAddress += 4 * flashWord;
            sofar += flashWord;
        } else {
            HAL_FLASH_Lock();
            return HAL_FLASH_GetError();
        }
    }

    // 3. FLASH RELOCK
    // Re-lock flash after write operation.
    HAL_FLASH_Lock();
    return 0;
}

void Controller::flash_readData(uint32_t StartSectorAddress, uint32_t *data, uint32_t wordSize) {
    // 1. LINEAR READBACK LOOP
    // Copy word-by-word from flash to destination buffer.
    for (uint32_t i = 0; i < wordSize; i++) {
        *data++ = *(__IO uint32_t *)StartSectorAddress;
        StartSectorAddress += 4;
    }
}

////////////////////////////////////////////////////////////////////////////////
/* Dac Functions -------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////

void Controller::dac_initialize() {
    // 1. DAC INITIALIZATION
    // Initialize audio output peripheral.
    dac.initialize();
}

////////////////////////////////////////////////////////////////////////////////
/* Sd Functions --------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////

void Controller::sd_initialize() {
    // 1. SDMMC BOOTSTRAP
    // Initialize SDMMC peripheral only when card-detect is valid.
    if (sd_detect() == SD_OK) {
        MX_SDMMC2_SD_Init();
    }
}

SdResult Controller::sd_detect() {
    // 1. CARD-DETECT PIN READ
    // Translate hardware detect level to SdResult.
    SdResult sdResult;
    (HAL_GPIO_ReadPin(SDMMC2_DETECT_GPIO_Port, SDMMC2_DETECT_Pin)) ? sdResult = SD_ERROR : sdResult = SD_OK;
    return sdResult;
}

SdResult Controller::sd_mount() {
    // 1. FILESYSTEM MOUNT
    // Mount FATFS at SDPath.
    SdResult sdResult;
    (f_mount(&sd.fs, SDPath, 0) == FR_OK) ? sdResult = SD_OK : sdResult = SD_ERROR;
    return sdResult;
}

SdResult Controller::sd_unmount() {
    // 1. FILESYSTEM UNMOUNT
    // Unmount FATFS from SDPath.
    SdResult sdResult;
    (f_mount(0, SDPath, 0) == FR_OK) ? sdResult = SD_OK : sdResult = SD_ERROR;
    return sdResult;
}

SdResult Controller::sd_getSpace() {
    // 1. CAPACITY QUERY
    // Compute total/free/used size from FAT cluster data.
    SdResult sdResult;
    FATFS *fs_ptr = &sd.fs;
    uint32_t freeCluster;
    if (f_getfree(SDPath, (DWORD *)&freeCluster, &fs_ptr) == FR_OK) {
        uint32_t totalBlocks = (sd.fs.n_fatent - 2) * sd.fs.csize;
        uint32_t freeBlocks = freeCluster * sd.fs.csize;
        sd.totalSpace = totalBlocks / 2000;
        sd.freeSpace = freeBlocks / 2000;
        sd.usedSpace = sd.totalSpace - sd.freeSpace;
        sdResult = SD_OK;
    } else {
        sd.totalSpace = 0;
        sd.freeSpace = 0;
        sd.usedSpace = 0;
        sdResult = SD_ERROR;
    }
    return sdResult;
}

SdResult Controller::sd_checkFileExist(const char *fileAddress) {
    // 1. FILE TYPE CHECK
    // Verify path exists and is not a directory.
    SdResult sdResult;
    if ((f_stat(fileAddress, &sd.fileInfo) == FR_OK) && ((sd.fileInfo.fattrib & AM_DIR) == false)) {
        sdResult = SD_OK;
    } else {
        sdResult = SD_ERROR;
    }
    return sdResult;
}

SdResult Controller::sd_checkFolderExist(const char *folderAddress) {
    // 1. DIRECTORY TYPE CHECK
    // Verify path exists and is a directory.
    SdResult sdResult;
    if ((f_stat(folderAddress, &sd.fileInfo) == FR_OK) && (sd.fileInfo.fattrib & AM_DIR)) {
        sdResult = SD_OK;
    } else {
        sdResult = SD_ERROR;
    }
    return sdResult;
}

FRESULT Controller::sd_createDirectory(const char *path) {
    // 1. MKDIR WRAPPER
    // Create directory at the given path.
    FRESULT res;
    res = f_mkdir(path);
    return res;
}

FRESULT Controller::sd_deleteDirectory(const char *path) {
    // 1. RECURSIVE DELETE WALK
    // Delete all files/subdirs, then remove the root directory.
    FRESULT res;
    DIR dir;
    FILINFO fileInfo;
    char file[64] = "";
    bool listFile = true;

    res = f_opendir(&dir, path);

    while (listFile) {
        res = f_readdir(&dir, &fileInfo);
        if ((res == FR_OK) && (fileInfo.fname[0] != 0)) {
            memset(file, 0x00, strlen(file));
            sprintf((char *)file, "%s/%s", path, fileInfo.fname);
            (fileInfo.fattrib & AM_DIR) ? sd_deleteDirectory(file) : f_unlink(file);
        } else {
            listFile = false;
        }
    }

    f_closedir(&dir);
    f_unlink(path);
    return res;
}

////////////////////////////////////////////////////////////////////////////////
/* Lcd Functions -------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////

void Controller::lcd_initialize() {
    // 1. LCD CORE INIT
    // Initialize display controller and bus state.
    lcd.initialize();
}