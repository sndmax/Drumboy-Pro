#ifndef __SGTL5000_H
#define __SGTL5000_H

// clang-format off

#define SGTL5000_I2C_ADDRESS                         0x14

/* ---------------------------------------------------------------- */

#define ID_REG                                       0x0000
                                                                        // 15:8
#define ID_PARTID_MASK                               0xFF00             // 0XA0 - 8 bit identifier for SGTL5000
#define ID_PARTID_SHIFT                              0x8
                                                                        // 7:0
#define ID_REVID_MASK                                0xFF               // 0X00 - revision number for SGTL5000.
#define ID_REVID_SHIFT                               0x0

/* ---------------------------------------------------------------- */

#define DIG_POWER_REG                                0x0002
                                                                        // 6
#define DIG_POWER_ADC_POWERUP_MASK                   0x40               // 1=Enable, 0=disable the ADC block, both digital & analog,
#define DIG_POWER_ADC_POWERUP_SHIFT                  0x6
                                                                        // 5
#define DIG_POWER_DAC_POWERUP_MASK                   0x20               // 1=Enable, 0=disable the DAC block, both analog and digital
#define DIG_POWER_DAC_POWERUP_SHIFT                  0x5
                                                                        // 4
#define DIG_POWER_DAP_POWERUP_MASK                   0x10               // 1=Enable, 0=disable the DAP block
#define DIG_POWER_DAP_POWERUP_SHIFT                  0x4
                                                                        // 1
#define DIG_POWER_I2S_OUT_POWERUP_MASK               0x2                // 1=Enable, 0=disable the I2S data output
#define DIG_POWER_I2S_OUT_POWERUP_SHIFT              0x1
                                                                        // 0
#define DIG_POWER_I2S_IN_POWERUP_MASK                0x1                // 1=Enable, 0=disable the I2S data input
#define DIG_POWER_I2S_IN_POWERUP_SHIFT               0x0

/* ---------------------------------------------------------------- */

#define CLK_CTRL_REG                                 0x0004
                                                                        // 5:4
#define CLK_CTRL_RATE_MODE_MASK                      0x30               // Sets the sample rate mode. MCLK_FREQ is still specified
#define CLK_CTRL_RATE_MODE_SHIFT                     0x4
                                                                        // Relative to the rate in SYS_FS
                                                                        // 0X0 = SYS_FS specifies the rate
                                                                        // 0X1 = Rate is 1/2 of the SYS_FS rate
                                                                        // 0X2 = Rate is 1/4 of the SYS_FS rate
                                                                        // 0X3 = Rate is 1/6 of the SYS_FS rate
                                                                        // 3:2
#define CLK_CTRL_SYS_FS_MASK                         0xC                // Sets the internal system sample rate (default=2)
#define CLK_CTRL_SYS_FS_SHIFT                        0x2
                                                                        // 0X0 = 32 kHz
                                                                        // 0X1 = 44.1 kHz
                                                                        // 0X2 = 48 kHz
                                                                        // 0X3 = 96 kHz
                                                                        // 1:0
#define CLK_CTRL_MCLK_FREQ_MASK                      0x3                // Identifies incoming SYS_MCLK frequency and if the PLL should be used
#define CLK_CTRL_MCLK_FREQ_SHIFT                     0x0
                                                                        // 0X0 = 256*Fs
                                                                        // 0X1 = 384*Fs
                                                                        // 0X2 = 512*Fs
                                                                        // 0X3 = Use PLL
                                                                        // The 0x3 (Use PLL) setting must be used if the SYS_MCLK is not
                                                                        // A standard multiple of Fs (256, 384, or 512). This setting can
                                                                        // Also be used if SYS_MCLK is a standard multiple of Fs.
                                                                        // Before this field is set to 0x3 (Use PLL), the PLL must be
                                                                        // Powered up by setting CHIP_ANA_POWER->PLL_POWERUP and
                                                                        // CHIP_ANA_POWER->VCOAMP_POWERUP.  Also, the PLL dividers must
                                                                        // Be calculated based on the external MCLK rate and
                                                                        // CHIP_PLL_CTRL register must be set (see CHIP_PLL_CTRL register
                                                                        // Description details on how to calculate the divisors).

/* ---------------------------------------------------------------- */

#define I2S_CTRL_REG                                 0x0006
                                                                        // 8
#define I2S_CTRL_SCLKFREQ_MASK                       0x100              // Sets frequency of I2S_SCLK when in master mode (MS=1). When in slave
#define I2S_CTRL_SCLKFREQ_SHIFT                      0x8
                                                                        // Mode (MS=0), this field must be set appropriately to match SCLK
                                                                        // Input rate.
                                                                        // 0X0 = 64Fs
                                                                        // 0X1 = 32Fs - Not supported for RJ mode (I2S_MODE = 1)
                                                                        // 7
#define I2S_CTRL_MS_MASK                             0x80               // Configures master or slave of I2S_LRCLK and I2S_SCLK.
#define I2S_CTRL_MS_SHIFT                            0x7
                                                                        // 0X0 = Slave: I2S_LRCLK an I2S_SCLK are inputs
                                                                        // 0X1 = Master: I2S_LRCLK and I2S_SCLK are outputs
                                                                        // NOTE: If the PLL is used (CHIP_CLK_CTRL->MCLK_FREQ==0x3),
                                                                        // The SGTL5000 must be a master of the I2S port (MS==1)
                                                                        // 6
#define I2S_CTRL_SCLK_INV_MASK                       0x40               // Sets the edge that data (input and output) is clocked in on for I2S_SCLK
#define I2S_CTRL_SCLK_INV_SHIFT                      0x6
                                                                        // 0X0 = data is valid on rising edge of I2S_SCLK
                                                                        // 0X1 = data is valid on falling edge of I2S_SCLK
                                                                        // 5:4
#define I2S_CTRL_DLEN_MASK                           0x30               // I2S data length (default=1)
#define I2S_CTRL_DLEN_SHIFT                          0x4
                                                                        // 0X0 = 32 bits (only valid when SCLKFREQ=0),
                                                                        // Not valid for Right Justified Mode
                                                                        // 0X1 = 24 bits (only valid when SCLKFREQ=0)
                                                                        // 0X2 = 20 bits
                                                                        // 0X3 = 16 bits
                                                                        // 3:2
#define I2S_CTRL_MODE_MASK                           0x0C               // Sets the mode for the I2S port
#define I2S_CTRL_MODE_SHIFT                          0x2
                                                                        // 0X0 = I2S mode or Left Justified (Use LRALIGN to select)
                                                                        // 0X1 = Right Justified Mode
                                                                        // 0X2 = PCM Format A/B
                                                                        // 0X3 = RESERVED
                                                                        // 1
#define I2S_CTRL_LRALIGN_MASK                        0x02               // I2S_LRCLK Alignment to data word. Not used for Right Justified mode
#define I2S_CTRL_LRALIGN_SHIFT                       0x1
                                                                        // 0X0 = Data word starts 1 I2S_SCLK delay after I2S_LRCLK
                                                                        // Transition (I2S format, PCM format A)
                                                                        // 0X1 = Data word starts after I2S_LRCLK transition (left
                                                                        // Justified format, PCM format B)
                                                                        // 0
#define I2S_CTRL_LRPOL_MASK                          0x01               // I2S_LRCLK Polarity when data is presented.
#define I2S_CTRL_LRPOL_SHIFT                         0x0
                                                                        // 0X0 = I2S_LRCLK = 0 - Left, 1 - Right
                                                                        // 1X0 = I2S_LRCLK = 0 - Right, 1 - Left
                                                                        // The left subframe should be presented first regardless of
                                                                        // The setting of LRPOL.

/* ---------------------------------------------------------------- */

#define SSS_CTRL_REG                                 0x000A
                                                                        // 14
#define SSS_CTRL_DAP_MIX_LRSWAP_MASK                 0x4000             // DAP Mixer Input Swap
#define SSS_CTRL_DAP_MIX_LRSWAP_SHIFT                0xE
                                                                        // 0X0 = Normal Operation
                                                                        // 0X1 = Left and Right channels for the DAP MIXER Input are swapped.
                                                                        // 13
#define SSS_CTRL_DAP_LRSWAP_MASK                     0x2000             // DAP Mixer Input Swap
#define SSS_CTRL_DAP_LRSWAP_SHIFT                    0xD
                                                                        // 0X0 = Normal Operation
                                                                        // 0X1 = Left and Right channels for the DAP Input are swapped
                                                                        // 12
#define SSS_CTRL_DAC_LRSWAP_MASK                     0x1000             // DAC Input Swap
#define SSS_CTRL_DAC_LRSWAP_SHIFT                    0xC
                                                                        // 0X0 = Normal Operation
                                                                        // 0X1 = Left and Right channels for the DAC are swapped
                                                                        // 10
#define SSS_CTRL_I2S_LRSWAP_MASK                     0x400              // I2S_DOUT Swap
#define SSS_CTRL_I2S_LRSWAP_SHIFT                    0xA
                                                                        // 0X0 = Normal Operation
                                                                        // 0X1 = Left and Right channels for the I2S_DOUT are swapped
                                                                        // 9:8
#define SSS_CTRL_DAP_MIX_SELECT_MASK                 0x300              // Select data source for DAP mixer
#define SSS_CTRL_DAP_MIX_SELECT_SHIFT                0x8
                                                                        // 0X0 = ADC
                                                                        // 0X1 = I2S_IN
                                                                        // 0X2 = Reserved
                                                                        // 0X3 = Reserved
                                                                        // 7:6
#define SSS_CTRL_DAP_SELECT_MASK                     0xC0               // Select data source for DAP
#define SSS_CTRL_DAP_SELECT_SHIFT                    0x6
                                                                        // 0X0 = ADC
                                                                        // 0X1 = I2S_IN
                                                                        // 0X2 = Reserved
                                                                        // 0X3 = Reserved
                                                                        // 5:4
#define SSS_CTRL_DAC_SELECT_MASK                     0x30               // Select data source for DAC (default=1)
#define SSS_CTRL_DAC_SELECT_SHIFT                    0x4
                                                                        // 0X0 = ADC
                                                                        // 0X1 = I2S_IN
                                                                        // 0X2 = Reserved
                                                                        // 0X3 = DAP
                                                                        // 1:0
#define SSS_CTRL_I2S_OUT_SELECT_MASK                 0x3                // Select data source for I2S_DOUT
#define SSS_CTRL_I2S_OUT_SELECT_SHIFT                0x0
                                                                        // 0X0 = ADC
                                                                        // 0X1 = I2S_IN
                                                                        // 0X2 = Reserved
                                                                        // 0X3 = DAP

/* ---------------------------------------------------------------- */

#define ADCDAC_CTRL_REG                              0x000E
                                                                        // 13
#define ADCDAC_CTRL_VOL_BUSY_DAC_RIGHT_MASK          0x2000             // Volume Busy DAC Right
#define ADCDAC_CTRL_VOL_BUSY_DAC_RIGHT_SHIFT         0xD
                                                                        // 0X0 = Ready
                                                                        // 0X1 = Busy - This indicates the channel has not reached its
                                                                        // Programmed volume/mute level
                                                                        // 12
#define ADCDAC_CTRL_VOL_BUSY_DAC_LEFT_MASK           0x1000             // Volume Busy DAC Left
#define ADCDAC_CTRL_VOL_BUSY_DAC_LEFT_SHIFT          0xC
                                                                        // 0X0 = Ready
                                                                        // 0X1 = Busy - This indicates the channel has not reached its
                                                                        // Programmed volume/mute level
                                                                        // 9
#define ADCDAC_CTRL_VOL_RAMP_EN_MASK                 0x200              // Volume Ramp Enable (default=1)
#define ADCDAC_CTRL_VOL_RAMP_EN_SHIFT                0x9
                                                                        // 0X0 = Disables volume ramp. New volume settings take immediate
                                                                        // Effect without a ramp
                                                                        // 0X1 = Enables volume ramp
                                                                        // This field affects DAC_VOL. The volume ramp effects both
                                                                        // Volume settings and mute When set to 1 a soft mute is enabled.
                                                                        // 8
#define ADCDAC_CTRL_VOL_EXPO_RAMP_MASK               0x100              // Exponential Volume Ramp Enable
#define ADCDAC_CTRL_VOL_EXPO_RAMP_SHIFT              0x8
                                                                        // 0X0 = Linear ramp over top 4 volume octaves
                                                                        // 0X1 = Exponential ramp over full volume range
                                                                        // This bit only takes effect if VOL_RAMP_EN is 1.
                                                                        // 3
#define ADCDAC_CTRL_DAC_MUTE_RIGHT_MASK              0x8                // DAC Right Mute (default=1)
#define ADCDAC_CTRL_DAC_MUTE_RIGHT_SHIFT             0x3
                                                                        // 0X0 = Unmute
                                                                        // 0X1 = Muted
                                                                        // If VOL_RAMP_EN = 1, this is a soft mute.
                                                                        // 2
#define ADCDAC_CTRL_DAC_MUTE_LEFT_MASK               0x4                // DAC Left Mute (default=1)
#define ADCDAC_CTRL_DAC_MUTE_LEFT_SHIFT              0x2
                                                                        // 0X0 = Unmute
                                                                        // 0X1 = Muted
                                                                        // If VOL_RAMP_EN = 1, this is a soft mute.
                                                                        // 1
#define ADCDAC_CTRL_ADC_HPF_FREEZE_MASK              0x2                // ADC High Pass Filter Freeze
#define ADCDAC_CTRL_ADC_HPF_FREEZE_SHIFT             0x1
                                                                        // 0X0 = Normal operation
                                                                        // 0X1 = Freeze the ADC high-pass filter offset register.  The
                                                                        // Offset continues to be subtracted from the ADC data stream.
                                                                        // 0
#define ADCDAC_CTRL_ADC_HPF_BYPASS_MASK              0x1                // ADC High Pass Filter Bypass
#define ADCDAC_CTRL_ADC_HPF_BYPASS_SHIFT             0x0
                                                                        // 0X0 = Normal operation
                                                                        // 0X1 = Bypassed and offset not updated

/* ---------------------------------------------------------------- */

#define DAC_VOL_REG                                  0x0010
                                                                        // 15:8
#define DAC_VOL_RIGHT_MASK                           0xFF00             // DAC Right Channel Volume.  Set the Right channel DAC volume
#define DAC_VOL_RIGHT_SHIFT                          0x8
                                                                        // With 0.5017 dB steps from 0 to -90 dB
                                                                        // 0X3B and less = Reserved
                                                                        // 0X3C = 0 dB
                                                                        // 0X3D = -0.5 dB
                                                                        // 0XF0 = -90 dB
                                                                        // 0XFC and greater = Muted
                                                                        // If VOL_RAMP_EN = 1, there is an automatic ramp to the
                                                                        // New volume setting.
                                                                        // 7:0
#define DAC_VOL_LEFT_MASK                            0x00FF             // DAC Left Channel Volume.  Set the Left channel DAC volume
#define DAC_VOL_LEFT_SHIFT                           0x0
                                                                        // With 0.5017 dB steps from 0 to -90 dB
                                                                        // 0X3B and less = Reserved
                                                                        // 0X3C = 0 dB
                                                                        // 0X3D = -0.5 dB
                                                                        // 0XF0 = -90 dB
                                                                        // 0XFC and greater = Muted
                                                                        // If VOL_RAMP_EN = 1, there is an automatic ramp to the
                                                                        // New volume setting.

/* ---------------------------------------------------------------- */

#define PAD_STRENGTH_REG                             0x0014
                                                                        // 9:8
#define PAD_STRENGTH_I2S_LRCLK_MASK                  0x300              // I2S LRCLK Pad Drive Strength (default=1)
#define PAD_STRENGTH_I2S_LRCLK_SHIFT                 0x8
                                                                        // Sets drive strength for output pads per the table below.
                                                                        // VDDIO    1.8 V     2.5 V     3.3 V
                                                                        // 0X0 = Disable
                                                                        // 0X1 =    1.66 mA   2.87 mA   4.02 mA
                                                                        // 0X2 =    3.33 mA   5.74 mA   8.03 mA
                                                                        // 0X3 =    4.99 mA   8.61 mA   12.05 mA
                                                                        // 7:6
#define PAD_STRENGTH_I2S_SCLK_MASK                   0xC0               // I2S SCLK Pad Drive Strength (default=1)
#define PAD_STRENGTH_I2S_SCLK_SHIFT                  0x6
                                                                        // 5:4
#define PAD_STRENGTH_I2S_DOUT_MASK                   0x30               // I2S DOUT Pad Drive Strength (default=1)
#define PAD_STRENGTH_I2S_DOUT_SHIFT                  0x4
                                                                        // 3:2
#define PAD_STRENGTH_CTRL_DATA_MASK                  0xC                // I2C DATA Pad Drive Strength (default=3)
#define PAD_STRENGTH_CTRL_DATA_SHIFT                 0x2
                                                                        // 1:0
#define PAD_STRENGTH_CTRL_CLK_MASK                   0x3                // I2C CLK Pad Drive Strength (default=3)
#define PAD_STRENGTH_CTRL_CLK_SHIFT                  0x0
                                                                        // (All use same table as I2S_LRCLK)

/* ---------------------------------------------------------------- */

#define ANA_ADC_CTRL_REG                             0x0020
                                                                        // 8
#define ANA_ADC_CTRL_ADC_VOL_M6DB_MASK               0x100              // ADC Volume Range Reduction
#define ANA_ADC_CTRL_ADC_VOL_M6DB_SHIFT              0x8
                                                                        // This bit shifts both right and left analog ADC volume
                                                                        // Range down by 6.0 dB.
                                                                        // 0X0 = No change in ADC range
                                                                        // 0X1 = ADC range reduced by 6.0 dB
                                                                        // 7:4
#define ANA_ADC_CTRL_ADC_VOL_RIGHT_MASK              0xF0               // ADC Right Channel Volume
#define ANA_ADC_CTRL_ADC_VOL_RIGHT_SHIFT             0x4
                                                                        // Right channel analog ADC volume control in 1.5 dB steps.
                                                                        // 0X0 = 0 dB
                                                                        // 0X1 = +1.5 dB
                                                                        // ...
                                                                        // 0XF = +22.5 dB
                                                                        // This range is -6.0 dB to +16.5 dB if ADC_VOL_M6DB is set to 1.
                                                                        // 3:0
#define ANA_ADC_CTRL_ADC_VOL_LEFT_MASK               0xF                // ADC Left Channel Volume
#define ANA_ADC_CTRL_ADC_VOL_LEFT_SHIFT              0x0
                                                                        // (Same scale as ADC_VOL_RIGHT)

/* ---------------------------------------------------------------- */

#define ANA_HP_CTRL_REG                              0x0022
                                                                        // 14:8
#define ANA_HP_CTRL_HP_VOL_RIGHT_MASK                0x7F00             // Headphone Right Channel Volume  (default 0x18)
#define ANA_HP_CTRL_HP_VOL_RIGHT_SHIFT               0x8
                                                                        // Right channel headphone volume control with 0.5 dB steps.
                                                                        // 0X00 = +12 dB
                                                                        // 0X01 = +11.5 dB
                                                                        // 0X18 = 0 dB
                                                                        // ...
                                                                        // 0X7F = -51.5 dB
                                                                        // 6:0
#define ANA_HP_CTRL_HP_VOL_LEFT_MASK                 0x7F               // Headphone Left Channel Volume  (default 0x18)
#define ANA_HP_CTRL_HP_VOL_LEFT_SHIFT                0x0
                                                                        // (Same scale as HP_VOL_RIGHT)

/* ---------------------------------------------------------------- */

#define ANA_CTRL_REG                                 0x0024
                                                                        // 8
#define ANA_CTRL_MUTE_LO_MASK                        0x100              // LINEOUT Mute, 0 = Unmute, 1 = Mute  (default 1)
#define ANA_CTRL_MUTE_LO_SHIFT                       0x8
                                                                        // 6
#define ANA_CTRL_SELECT_HP_MASK                      0x40               // Select the headphone input, 0 = DAC, 1 = LINEIN
#define ANA_CTRL_SELECT_HP_SHIFT                     0x6
                                                                        // 5
#define ANA_CTRL_EN_ZCD_HP_MASK                      0x20               // Enable the headphone zero cross detector (ZCD)
#define ANA_CTRL_EN_ZCD_HP_SHIFT                     0x5
                                                                        // 0X0 = HP ZCD disabled
                                                                        // 0X1 = HP ZCD enabled
                                                                        // 4
#define ANA_CTRL_MUTE_HP_MASK                        0x10               // Mute the headphone outputs, 0 = Unmute, 1 = Mute (default)
#define ANA_CTRL_MUTE_HP_SHIFT                       0x4
                                                                        // 2
#define ANA_CTRL_SELECT_ADC_MASK                     0x4                // Select the ADC input, 0 = Microphone, 1 = LINEIN
#define ANA_CTRL_SELECT_ADC_SHIFT                    0x2
                                                                        // 1
#define ANA_CTRL_EN_ZCD_ADC_MASK                     0x2                // Enable the ADC analog zero cross detector (ZCD)
#define ANA_CTRL_EN_ZCD_ADC_SHIFT                    0x1
                                                                        // 0X0 = ADC ZCD disabled
                                                                        // 0X1 = ADC ZCD enabled
                                                                        // 0
#define ANA_CTRL_MUTE_ADC_MASK                       0x1                // Mute the ADC analog volume, 0 = Unmute, 1 = Mute (default)
#define ANA_CTRL_MUTE_ADC_SHIFT                      0x0

/* ---------------------------------------------------------------- */

#define LINREG_CTRL_REG                              0x0026
                                                                        // 6
#define LINREG_CTRL_VDDC_MAN_ASSN_MASK               0x40               // Determines chargepump source when VDDC_ASSN_OVRD is set.
#define LINREG_CTRL_VDDC_MAN_ASSN_SHIFT              0x6
                                                                        // 0X0 = VDDA
                                                                        // 0X1 = VDDIO
                                                                        // 5
#define LINREG_CTRL_VDDC_ASSN_OVRD_MASK              0x20               // Charge pump Source Assignment Override
#define LINREG_CTRL_VDDC_ASSN_OVRD_SHIFT             0x5
                                                                        // 0X0 = Charge pump source is automatically assigned based
                                                                        // On higher of VDDA and VDDIO
                                                                        // 0X1 = the source of charge pump is manually assigned by
                                                                        // VDDC_MAN_ASSN If VDDIO and VDDA are both the same
                                                                        // And greater than 3.1 V, VDDC_ASSN_OVRD and
                                                                        // VDDC_MAN_ASSN should be used to manually assign
                                                                        // VDDIO as the source for charge pump.
                                                                        // 3:0
#define LINREG_CTRL_D_PROGRAMMING_MASK               0xF                // Sets the VDDD linear regulator output voltage in 50 mV steps.
#define LINREG_CTRL_D_PROGRAMMING_SHIFT              0x0
                                                                        // Must clear the LINREG_SIMPLE_POWERUP and STARTUP_POWERUP bits
                                                                        // In the 0x0030 (CHIP_ANA_POWER) register after power-up, for
                                                                        // This setting to produce the proper VDDD voltage.
                                                                        // 0X0 = 1.60
                                                                        // 0XF = 0.85

/* ---------------------------------------------------------------- */

#define REF_CTRL_REG                                 0x0028             // Bandgap reference bias voltage and currents
                                                                        // 8:4
#define REF_CTRL_VAG_VAL_MASK                        0x1F0              // Analog Ground Voltage Control
#define REF_CTRL_VAG_VAL_SHIFT                       0x4
                                                                        // These bits control the analog ground voltage in 25 mV steps.
                                                                        // This should usually be set to VDDA/2 or lower for best
                                                                        // Performance (maximum output swing at minimum THD). This VAG
                                                                        // Reference is also used for the DAC and ADC voltage reference.
                                                                        // So changing this voltage scales the output swing of the DAC
                                                                        // And the output signal of the ADC.
                                                                        // 0X00 = 0.800 V
                                                                        // 0X1F = 1.575 V
                                                                        // 3:1
#define REF_CTRL_BIAS_CTRL_MASK                      0xE                // Bias control
#define REF_CTRL_BIAS_CTRL_SHIFT                     0x1
                                                                        // These bits adjust the bias currents for all of the analog
                                                                        // Blocks. By lowering the bias current a lower quiescent power
                                                                        // Is achieved. It should be noted that this mode can affect
                                                                        // Performance by 3-4 dB.
                                                                        // 0X0 = Nominal
                                                                        // 0X1-0x3=+12.5%
                                                                        // 0X4=-12.5%
                                                                        // 0X5=-25%
                                                                        // 0X6=-37.5%
                                                                        // 0X7=-50%
                                                                        // 0
#define REF_CTRL_SMALL_POP_MASK                      0x1                // VAG Ramp Control
#define REF_CTRL_SMALL_POP_SHIFT                     0x0
                                                                        // Setting this bit slows down the VAG ramp from ~200 to ~400 ms
                                                                        // To reduce the startup pop, but increases the turn on/off time.
                                                                        // 0X0 = Normal VAG ramp
                                                                        // 0X1 = Slow down VAG ramp

/* ---------------------------------------------------------------- */

#define MIC_CTRL_REG                                 0x002A             // Microphone gain & internal microphone bias
                                                                        // 9:8
#define MIC_CTRL_BIAS_RESISTOR_MASK                  0x300              // MIC Bias Output Impedance Adjustment
#define MIC_CTRL_BIAS_RESISTOR_SHIFT                 0x8
                                                                        // Controls an adjustable output impedance for the microphone bias.
                                                                        // If this is set to zero the micbias block is powered off and
                                                                        // The output is highZ.
                                                                        // 0X0 = Powered off
                                                                        // 0X1 = 2.0 kohm
                                                                        // 0X2 = 4.0 kohm
                                                                        // 0X3 = 8.0 kohm
                                                                        // 6:4
#define MIC_CTRL_BIAS_VOLT_MASK                      0x70               // MIC Bias Voltage Adjustment
#define MIC_CTRL_BIAS_VOLT_SHIFT                     0x4
                                                                        // Controls an adjustable bias voltage for the microphone bias
                                                                        // Amp in 250 mV steps. This bias voltage setting should be no
                                                                        // More than VDDA-200 mV for adequate power supply rejection.
                                                                        // 0X0 = 1.25 V
                                                                        // ...
                                                                        // 0X7 = 3.00 V
                                                                        // 1:0
#define MIC_CTRL_GAIN_MASK                           0x3                // MIC Amplifier Gain
#define MIC_CTRL_GAIN_SHIFT                          0x0
                                                                        // Sets the microphone amplifier gain. At 0 dB setting the THD
                                                                        // Can be slightly higher than other paths- typically around
                                                                        // ~65 DB. At other gain settings the THD are better.
                                                                        // 0X0 = 0 dB
                                                                        // 0X1 = +20 dB
                                                                        // 0X2 = +30 dB
                                                                        // 0X3 = +40 dB

/* ---------------------------------------------------------------- */

#define LINE_OUT_CTRL_REG                            0x002C
                                                                        // 11:8
#define LINE_OUT_CTRL_OUT_CURRENT_MASK               0xF00              // Controls the output bias current for the LINEOUT amplifiers.  The
#define LINE_OUT_CTRL_OUT_CURRENT_SHIFT              0x8
                                                                        // Nominal recommended setting for a 10 kohm load with 1.0 nF load cap
                                                                        // Is 0x3. There are only 5 valid settings.
                                                                        // 0X0=0.18 mA
                                                                        // 0X1=0.27 mA
                                                                        // 0X3=0.36 mA
                                                                        // 0X7=0.45 mA
                                                                        // 0XF=0.54 mA
                                                                        // 5:0
#define LINE_OUT_CTRL_LO_VAGCNTRL_MASK               0x3F               // LINEOUT Amplifier Analog Ground Voltage
#define LINE_OUT_CTRL_LO_VAGCNTRL_SHIFT              0x0
                                                                        // Controls the analog ground voltage for the LINEOUT amplifiers
                                                                        // In 25 mV steps. This should usually be set to VDDIO/2.
                                                                        // 0X00 = 0.800 V
                                                                        // ...
                                                                        // 0X1F = 1.575 V
                                                                        // ...
                                                                        // 0X23 = 1.675 V
                                                                        // 0X24-0x3F are invalid

/* ---------------------------------------------------------------- */

#define LINE_OUT_VOL_REG                             0x002E
                                                                        // 12:8
#define LINE_OUT_VOL_LO_VOL_RIGHT_MASK               0x1F00             // LINEOUT Right Channel Volume (default=4)
#define LINE_OUT_VOL_LO_VOL_RIGHT_SHIFT              0x8
                                                                        // Controls the right channel LINEOUT volume in 0.5 dB steps.
                                                                        // Higher codes have more attenuation.
                                                                        // 4:0
#define LINE_OUT_VOL_LO_VOL_LEFT_MASK                0x001F             // LINEOUT Left Channel Output Level (default=4)
#define LINE_OUT_VOL_LO_VOL_LEFT_SHIFT               0x0
                                                                        // Used to normalize the output level of the left line output
                                                                        // To full scale based on the values used to set
                                                                        // LINE_OUT_CTRL->LO_VAGCNTRL and CHIP_REF_CTRL->VAG_VAL.
                                                                        // In general this field should be set to:
                                                                        // 40*Log((VAG_VAL)/(LO_VAGCNTRL)) + 15
                                                                        // Suggested values based on typical VDDIO and VDDA voltages.
                                                                        // VDDA  VAG_VAL VDDIO  LO_VAGCNTRL LO_VOL_*
                                                                        // 1.8 V    0.9   3.3 V     1.55      0x06
                                                                        // 1.8 V    0.9   1.8 V      0.9      0x0F
                                                                        // 3.3 V   1.55   1.8 V      0.9      0x19
                                                                        // 3.3 V   1.55   3.3 V     1.55      0x0F
                                                                        // After setting to the nominal voltage, this field can be used
                                                                        // To adjust the output level in +/-0.5 dB increments by using
                                                                        // Values higher or lower than the nominal setting.

/* ---------------------------------------------------------------- */

#define ANA_POWER_REG                                0x0030             // Power down controls for the analog blocks.
                                                                        // The only other power-down controls are BIAS_RESISTOR in the MIC_CTRL register
                                                                        // And the EN_ZCD control bits in ANA_CTRL.
                                                                        // 14
#define ANA_POWER_DAC_MONO_MASK                      0x4000             // While DAC_POWERUP is set, this allows the DAC to be put into left only
#define ANA_POWER_DAC_MONO_SHIFT                     0xE                // Mono operation for power savings. 0=mono, 1=stereo (default)
                                                                        // 13
#define ANA_POWER_LREG_SIMPLE_POWERUP_MASK           0x2000             // Power up the simple (low power) digital supply regulator.
#define ANA_POWER_LREG_SIMPLE_POWERUP_SHIFT          0xD
                                                                        // After reset, this bit can be cleared IF VDDD is driven
                                                                        // Externally OR the primary digital linreg is enabled with
                                                                        // LINREG_D_POWERUP
                                                                        // 12
#define ANA_POWER_STARTUP_POWERUP_MASK               0x1000             // Power up the circuitry needed during the power up ramp and reset.
#define ANA_POWER_STARTUP_POWERUP_SHIFT              0xC
                                                                        // After reset this bit can be cleared if VDDD is coming from
                                                                        // An external source.
                                                                        // 11
#define ANA_POWER_VDDC_CHRGPMP_POWERUP_MASK          0x800              // Power up the VDDC charge pump block. If neither VDDA or VDDIO
#define ANA_POWER_VDDC_CHRGPMP_POWERUP_SHIFT         0xB
                                                                        // Is 3.0 V or larger this bit should be cleared before analog
                                                                        // Blocks are powered up.
                                                                        // 10
#define ANA_POWER_PLL_POWERUP_MASK                   0x400              // PLL Power Up, 0 = Power down, 1 = Power up
#define ANA_POWER_PLL_POWERUP_SHIFT                  0xA
                                                                        // When cleared, the PLL is turned off. This must be set before
                                                                        // CHIP_CLK_CTRL->MCLK_FREQ is programmed to 0x3. The
                                                                        // CHIP_PLL_CTRL register must be configured correctly before
                                                                        // Setting this bit.
                                                                        // 9
#define ANA_POWER_LINREG_D_POWERUP_MASK              0x200              // Power up the primary VDDD linear regulator, 0 = Power down, 1 = Power up
#define ANA_POWER_LINREG_D_POWERUP_SHIFT             0x9
                                                                        // 8
#define ANA_POWER_VCOAMP_POWERUP_MASK                0x100              // Power up the PLL VCO amplifier, 0 = Power down, 1 = Power up
#define ANA_POWER_VCOAMP_POWERUP_SHIFT               0x8
                                                                        // 7
#define ANA_POWER_VAG_POWERUP_MASK                   0x80               // Power up the VAG reference buffer.
#define ANA_POWER_VAG_POWERUP_SHIFT                  0x7
                                                                        // Setting this bit starts the power up ramp for the headphone
                                                                        // And LINEOUT. The headphone (and/or LINEOUT) powerup should
                                                                        // Be set BEFORE clearing this bit. When this bit is cleared
                                                                        // The power-down ramp is started. The headphone (and/or LINEOUT)
                                                                        // Powerup should stay set until the VAG is fully ramped down
                                                                        // (200 To 400 ms after clearing this bit).
                                                                        // 0X0 = Power down, 0x1 = Power up
                                                                        // 6
#define ANA_POWER_ADC_MONO_MASK                      0x40               // While ADC_POWERUP is set, this allows the ADC to be put into left only
#define ANA_POWER_ADC_MONO_SHIFT                     0x6
                                                                        // Mono operation for power savings. This mode is useful when
                                                                        // Only using the microphone input.
                                                                        // 0X0 = Mono (left only), 0x1 = Stereo
                                                                        // 5
#define ANA_POWER_REFTOP_POWERUP_MASK                0x20               // Power up the reference bias currents
#define ANA_POWER_REFTOP_POWERUP_SHIFT               0x5
                                                                        // 0X0 = Power down, 0x1 = Power up
                                                                        // This bit can be cleared when the part is a sleep state
                                                                        // To minimize analog power.
                                                                        // 4
#define ANA_POWER_HEADPHONE_POWERUP_MASK             0x10               // Power up the headphone amplifiers
#define ANA_POWER_HEADPHONE_POWERUP_SHIFT            0x4
                                                                        // 0X0 = Power down, 0x1 = Power up
                                                                        // 3
#define ANA_POWER_DAC_POWERUP_MASK                   0x8                // Power up the DACs
#define ANA_POWER_DAC_POWERUP_SHIFT                  0x3
                                                                        // 0X0 = Power down, 0x1 = Power up
                                                                        // 2
#define ANA_POWER_C_HEADPHONE_POWERUP_MASK           0x4                // Power up the capless headphone mode
#define ANA_POWER_C_HEADPHONE_POWERUP_SHIFT          0x2
                                                                        // 0X0 = Power down, 0x1 = Power up
                                                                        // 1
#define ANA_POWER_ADC_POWERUP_MASK                   0x2                // Power up the ADCs
#define ANA_POWER_ADC_POWERUP_SHIFT                  0x1
                                                                        // 0X0 = Power down, 0x1 = Power up
                                                                        // 0
#define ANA_POWER_LINEOUT_POWERUP_MASK               0x1                // Power up the LINEOUT amplifiers
#define ANA_POWER_LINEOUT_POWERUP_SHIFT              0x0
                                                                        // 0X0 = Power down, 0x1 = Power up

/* ---------------------------------------------------------------- */

#define PLL_CTRL_REG                                 0x0032
                                                                        // 15:11
#define PLL_CTRL_INT_DIVISOR_MASK                    0xF800
#define PLL_CTRL_INT_DIVISOR_SHIFT                   0xB
                                                                        // 10:0
#define PLL_CTRL_FRAC_DIVISOR_MASK                   0x7FF
#define PLL_CTRL_FRAC_DIVISOR_SHIFT                  0x0

/* ---------------------------------------------------------------- */

#define CLK_TOP_CTRL_REG                             0x0034
                                                                        // 11
#define CLK_TOP_CTRL_ENABLE_INT_OSC_MASK             0x800              // Setting this bit enables an internal oscillator to be used for the
#define CLK_TOP_CTRL_ENABLE_INT_OSC_SHIFT            0xB
                                                                        // Zero cross detectors, the short detect recovery, and the
                                                                        // Charge pump. This allows the I2S clock to be shut off while
                                                                        // Still operating an analog signal path. This bit can be kept
                                                                        // On when the I2S clock is enabled, but the I2S clock is more
                                                                        // Accurate so it is preferred to clear this bit when I2S is present.
                                                                        // 3
#define CLK_TOP_CTRL_INPUT_FREQ_DIV2_MASK            0x8                // SYS_MCLK divider before PLL input
#define CLK_TOP_CTRL_INPUT_FREQ_DIV2_SHIFT           0x3
                                                                        // 0X0 = pass through
                                                                        // 0X1 = SYS_MCLK is divided by 2 before entering PLL
                                                                        // This must be set when the input clock is above 17 Mhz. This
                                                                        // Has no effect when the PLL is powered down.

/* ---------------------------------------------------------------- */

#define ANA_STATUS_REG                               0x0036
                                                                        // 9
#define ANA_STATUS_LRSHORT_STS_MASK                  0x200              // This bit is high whenever a short is detected on the left or right
#define ANA_STATUS_LRSHORT_STS_SHIFT                 0x9                // Channel headphone drivers.
                                                                        // 8
#define ANA_STATUS_CSHORT_STS_MASK                   0x100              // This bit is high whenever a short is detected on the capless headphone
#define ANA_STATUS_CSHORT_STS_SHIFT                  0x8                // Common/center channel driver.
                                                                        // 4
#define ANA_STATUS_PLL_IS_LOCKED_MASK                0x10               // This bit goes high after the PLL is locked.
#define ANA_STATUS_PLL_IS_LOCKED_SHIFT               0x4

/* ---------------------------------------------------------------- */

#define ANA_TEST1_REG                                0x0038             // Intended only for debug.
#define ANA_TEST2_REG                                0x003A             // Intended only for debug.
                                                                        // 14
#define ANA_TEST2_LINEOUT_TO_VDDA_MASK               0x4000             // Changes the LINEOUT amplifier power supply from VDDIO to VDDA. Typically
                                                                        // LINEOUT should be on the higher power supply. This bit is useful when VDDA is
                                                                        // ~3.3 V and VDDIO is ~1.8 V.
                                                                        // 13
#define ANA_TEST2_SPARE_MASK                         0x2000             // Spare registers to analog
                                                                        // 12
#define ANA_TEST2_MONOMODE_DAC_MASK                  0x1000             // Copy the left channel DAC data to the right channel. This allows both left and right to
                                                                        // Play from MONO dac data.
                                                                        // 11
#define ANA_TEST2_VCO_TUNE_AGAIN_MASK                0x800              // When toggled high then low forces the PLL VCO to retune the number of inverters in
                                                                        // The ring oscillator loop.
                                                                        // 10
#define ANA_TEST2_LO_PASS_MASTERVAG_MASK             0x400              // Tie the main analog VAG to the LINEOUT VAG. This can improve SNR for the
                                                                        // LINEOUT when both are the same voltage.
                                                                        // 9
#define ANA_TEST2_INVERT_DAC_SAMPLE_CLOCK_MASK       0x200              // Change the clock edge used for the DAC output sampling.
                                                                        // 8
#define ANA_TEST2_INVERT_DAC_DATA_TIMING_MASK        0x100              // Change the clock edge used for the digital to analog DAC data crossing.
                                                                        // 7
#define ANA_TEST2_DAC_EXTEND_RTZ_MASK                0x80               // Extend the return-to-zero time for the DAC.
                                                                        // 6
#define ANA_TEST2_DAC_DOUBLE_I_MASK                  0x40               // Double the output current of the DAC amplifier when it is in classA mode.
                                                                        // 5
#define ANA_TEST2_DAC_DIS_RTZ_MASK                   0x20               // Turn off the return-to-zero in the DAC. In mode cases, this hurts the SNDR of the DAC.
                                                                        // 4
#define ANA_TEST2_DAC_CLASSA_MASK                    0x10               // Turn off the classAB mode in the DAC amplifier. This mode should normally not be
                                                                        // Used. The output current is not high enough to support a full scale signal in this mode.
                                                                        // 3
#define ANA_TEST2_INVERT_ADC_SAMPLE_CLOCK_MASK       0x8                // Change the clock edge used for the ADC sampling.
                                                                        // 2
#define ANA_TEST2_INVERT_ADC_DATA_TIMING_MASK        0x4                // Change the clock edge used for the analog to digital ADC data crossing
                                                                        // 1
#define ANA_TEST2_ADC_LESSI_MASK                     0x2                // Drops ADC bias currents by 20%
                                                                        // 0
#define ANA_TEST2_ADC_DITHEROFF_MASK                 0x1                // Turns off the ADC dithering.

/* ---------------------------------------------------------------- */

#define SHORT_CTRL_REG                               0x003C
                                                                        // 14:12
#define SHORT_CTRL_LVLADJR_MASK                      0x7000             // Right channel headphone short detector in 25 mA steps.
#define SHORT_CTRL_LVLADJR_SHIFT                     0xC
                                                                        // 0X3=25 mA
                                                                        // 0X2=50 mA
                                                                        // 0X1=75 mA
                                                                        // 0X0=100 mA
                                                                        // 0X4=125 mA
                                                                        // 0X5=150 mA
                                                                        // 0X6=175 mA
                                                                        // 0X7=200 mA
                                                                        // This trip point can vary by ~30% over process so leave plenty
                                                                        // Of guard band to avoid false trips.  This short detect trip
                                                                        // Point is also effected by the bias current adjustments made
                                                                        // By CHIP_REF_CTRL->BIAS_CTRL and by CHIP_ANA_TEST1->HP_IALL_ADJ.
                                                                        // 10:8
#define SHORT_CTRL_LVLADJL_MASK                      0x700              // Left channel headphone short detector in 25 mA steps.
#define SHORT_CTRL_LVLADJL_SHIFT                     0x8
                                                                        // (Same scale as LVLADJR)
                                                                        // 6:4
#define SHORT_CTRL_LVLADJC_MASK                      0x70               // Capless headphone center channel short detector in 50 mA steps.
#define SHORT_CTRL_LVLADJC_SHIFT                     0x4
                                                                        // 0X3=50 mA
                                                                        // 0X2=100 mA
                                                                        // 0X1=150 mA
                                                                        // 0X0=200 mA
                                                                        // 0X4=250 mA
                                                                        // 0X5=300 mA
                                                                        // 0X6=350 mA
                                                                        // 0X7=400 mA
                                                                        // 3:2
#define SHORT_CTRL_MODE_LR_MASK                      0xC                // Behavior of left/right short detection
#define SHORT_CTRL_MODE_LR_SHIFT                     0x2
                                                                        // 0X0 = Disable short detector, reset short detect latch,
                                                                        // Software view non-latched short signal
                                                                        // 0X1 = Enable short detector and reset the latch at timeout
                                                                        // (Every ~50 ms)
                                                                        // 0X2 = This mode is not used/invalid
                                                                        // 0X3 = Enable short detector with only manual reset (have
                                                                        // To return to 0x0 to reset the latch)
                                                                        // 1:0
#define SHORT_CTRL_MODE_CM_MASK                      0x3                // Behavior of capless headphone central short detection
#define SHORT_CTRL_MODE_CM_SHIFT                     0x0
                                                                        // (Same settings as MODE_LR)

/* ---------------------------------------------------------------- */

#define DAP_CONTROL_REG                              0x0100
                                                                        // 4
#define DAP_CONTROL_MIX_EN_MASK                      0x10               // Enable/Disable the DAP mixer path
#define DAP_CONTROL_MIX_EN_SHIFT                     0x4
                                                                        // 0X0 = Disable
                                                                        // 0X1 = Enable
                                                                        // When enabled, DAP_EN must also be enabled to use the mixer.

                                                                        // 0
#define DAP_CONTROL_DAP_EN_MASK                      0x1                // Enable/Disable digital audio processing (DAP)
#define DAP_CONTROL_DAP_EN_SHIFT                     0x0
                                                                        // 0X0 = Disable. When disabled, no audio passes through.
                                                                        // 0X1 = Enable. When enabled, audio can pass through DAP even if none of the DAP
                                                                        // Functions are enabled.

/* ---------------------------------------------------------------- */

#define DAP_PEQ_REG                                  0x0102
                                                                        // 2:0
#define DAP_PEQ_EN_MASK                              0x7                // Set to Enable the PEQ filters
#define DAP_PEQ_EN_SHIFT                             0x0
                                                                        // 0X0 = Disabled
                                                                        // 0X1 = 1 Filter Enabled
                                                                        // 0X2 = 2 Filters Enabled
                                                                        // .....
                                                                        // 0X7 = Cascaded 7 Filters
                                                                        // DAP_AUDIO_EQ->EN bit must be set to 1 in order to enable the PEQ

#define DAP_PEQ_PN_CFG_REG                           0x0124             // PEQ Filtre Tipleri
#define DAP_PEQ_PN_IDX_REG                           0x0125             // PEQ Band Index (0-6)
#define DAP_PEQ_PN_COEF_0_REG                        0x0126             // B0 Katsayısı
#define DAP_PEQ_PN_COEF_1_REG                        0x0127             // B1 Katsayısı
#define DAP_PEQ_PN_COEF_2_REG                        0x0128             // B2 Katsayısı
#define DAP_PEQ_PN_COEF_3_REG                        0x0129             // A1 Katsayısı
#define DAP_PEQ_PN_COEF_4_REG                        0x012A             // A2 Katsayısı

/* ---------------------------------------------------------------- */

#define DAP_BASS_ENHANCE_REG                         0x0104
                                                                        // 8
#define DAP_BASS_ENHANCE_BYPASS_HPF_MASK             0x100              // Bypass high pass filter
#define DAP_BASS_ENHANCE_BYPASS_HPF_SHIFT            0x8
                                                                        // 0X0 = Enable high pass filter
                                                                        // 0X1 = Bypass high pass filter
                                                                        // 6:4
#define DAP_BASS_ENHANCE_CUTOFF_MASK                 0x70               // Set cut-off frequency
#define DAP_BASS_ENHANCE_CUTOFF_SHIFT                0x4
                                                                        // 0X0 = 80 Hz
                                                                        // 0X1 = 100 Hz
                                                                        // 0X2 = 125 Hz
                                                                        // 0X3 = 150 Hz
                                                                        // 0X4 = 175 Hz
                                                                        // 0X5 = 200 Hz
                                                                        // 0X6 = 225 Hz
                                                                        // 0
#define DAP_BASS_ENHANCE_EN_MASK                     0x1                // Enable/Disable Bass Enhance
#define DAP_BASS_ENHANCE_EN_SHIFT                    0x0
                                                                        // 0X0 = Disable
                                                                        // 0X1 = Enable

/* ---------------------------------------------------------------- */

#define DAP_BASS_ENHANCE_CTRL_REG                    0x0106
                                                                        // 13:8
#define DAP_BASS_ENHANCE_CTRL_LR_LEVEL_MASK          0x3F00             // Left/Right Mix Level Control
#define DAP_BASS_ENHANCE_CTRL_LR_LEVEL_SHIFT         0x8
                                                                        // 0X00= +6.0 dB for Main Channel
                                                                        // ......
                                                                        // 0X3F= Least L/R Channel Level
                                                                        // 6:0
#define DAP_BASS_ENHANCE_BASS_LEVEL_MASK             0x7F               // Bass Harmonic Level Control
#define DAP_BASS_ENHANCE_BASS_LEVEL_SHIFT            0x0
                                                                        // 0X00= Most Harmonic Boost
                                                                        // ......
                                                                        // 0X7F=Least Harmonic Boost

/* ---------------------------------------------------------------- */

#define DAP_AUDIO_EQ_REG                             0x0108
                                                                        // 1:0
#define DAP_AUDIO_EQ_EN_MASK                         0x3                // Selects between PEQ/GEQ/Tone Control and Enables it.
#define DAP_AUDIO_EQ_EN_SHIFT                        0x0
                                                                        // 0X0 = Disabled.
                                                                        // 0X1 = Enable PEQ. NOTE: DAP_PEQ->EN bit must also be set to the desired number
                                                                        // Of filters (bands) in order for the PEQ to be enabled.
                                                                        // 0X2 = Enable Tone Control
                                                                        // 0X3 = Enable 5 Band GEQ

/* ---------------------------------------------------------------- */

#define DAP_SGTL_SURROUND_REG                        0x010A
                                                                        // 6:4
#define DAP_SGTL_SURROUND_WIDTH_CONTROL_MASK         0x70               // Freescale Surround Width Control -
#define DAP_SGTL_SURROUND_WIDTH_CONTROL_SHIFT        0x4
                                                                        // The width control changes the perceived width of
                                                                        // The sound field.
                                                                        // 0X0 = Least Width
                                                                        // ......
                                                                        // 0X7 = Most Width
                                                                        // 1:0
#define DAP_SGTL_SURROUND_SELECT_MASK                0x3                // Freescale Surround Selection
#define DAP_SGTL_SURROUND_SELECT_SHIFT               0x0
                                                                        // 0X0 = Disabled
                                                                        // 0X1 = Disabled
                                                                        // 0X2 = Mono input Enable
                                                                        // 0X3 = Stereo input Enable

/* ---------------------------------------------------------------- */

#define DAP_FILTER_COEF_ACCESS_REG                   0x010C
                                                                        // 8
#define DAP_FILTER_COEF_ACCESS_WR_MASK               0x100              // When set, the coefficients written in the ten coefficient data registers are loaded into the filter specified by INDEX
#define DAP_FILTER_COEF_ACCESS_WR_SHIFT              0x8
                                                                        // 7:0
#define DAP_FILTER_COEF_ACCESS_INDEX_MASK            0xFF               // Specifies the index for each of the seven bands of the filter coefficient that needs to be
#define DAP_FILTER_COEF_ACCESS_INDEX_SHIFT           0x0
                                                                        // Written to. Each filter has 5 coefficients that need to be loaded into the 10 coefficient
                                                                        // Registers (MSB, LSB) before setting the index and WR bit.
                                                                        // Steps to write coefficients:
                                                                        // 1. Write the five 20-bit coefficient values to DAP_COEF_WR_XX_MSB and
                                                                        // DAP_COEF_WR_XX_LSB registers (XX= B0,B1,B2,A1,A2)
                                                                        // 2. Set INDEX of the coefficient from the table below.
                                                                        // 3. Set the WR bit to load the coefficient.
                                                                        // NOTE: Steps 2 and 3 can be performed with a single write to
                                                                        // DAP_FILTER_COEF_ACCESS register.
                                                                        // Coefficient address:
                                                                        // Band 0 = 0x00
                                                                        // Band 1 = 0x01
                                                                        // Band 2 = 0x02
                                                                        // Band 3 = 0x03
                                                                        // Band 4 = 0x04
                                                                        // ...
                                                                        // Band 7 = 0x06

/* ---------------------------------------------------------------- */

#define DAP_COEF_WR_B0_MSB_REG                       0x010E
                                                                        // 15:0
#define DAP_COEF_WR_B0_MSB_MS16BITS_MASK             0xFFFF             // Most significant 16-bits of the 20-bit filter coefficient that needs to be written
#define DAP_COEF_WR_B0_MSB_MS16BITS_SHIFT            0x0

/* ---------------------------------------------------------------- */

#define DAP_COEF_WR_B0_LSB_REG                       0x0110
                                                                        // 3:0
#define DAP_COEF_WR_B0_LSB_LS4BITS_MASK              0xF                // Least significant 4 bits of the 20-bit filter coefficient that needs to be written.
#define DAP_COEF_WR_B0_LSB_LS4BITS_SHIFT             0x0

/* ---------------------------------------------------------------- */

#define DAP_AUDIO_EQ_BAND0_REG                       0x0116             // 115 Hz
                                                                        // 6:0
#define DAP_AUDIO_EQ_BAND0_VOLUME_MASK               0x7F               // Sets Tone Control Bass/GEQ Band0
#define DAP_AUDIO_EQ_BAND0_VOLUME_SHIFT              0x0
                                                                        // 0X5F = sets to 12 dB
                                                                        // 0X2F = sets to 0 dB
                                                                        // 0X00 = sets to -11.75 dB
                                                                        // Each LSB is 0.25 dB

/* ---------------------------------------------------------------- */

#define DAP_AUDIO_EQ_BAND1_REG                       0x0118             // 330 Hz
                                                                        // 6:0
#define DAP_AUDIO_EQ_BAND1_VOLUME_MASK               0x7F               // Sets GEQ Band1
#define DAP_AUDIO_EQ_BAND1_VOLUME_SHIFT              0x0
                                                                        // 0X5F = sets to 12 dB
                                                                        // 0X2F = sets to 0 dB
                                                                        // 0X00 = sets to -11.75 dB
                                                                        // Each LSB is 0.25 dB

/* ---------------------------------------------------------------- */

#define DAP_AUDIO_EQ_BAND2_REG                       0x011A             // 990 Hz
                                                                        // 6:0
#define DAP_AUDIO_EQ_BAND2_VOLUME_MASK               0x7F               // Sets GEQ Band2
#define DAP_AUDIO_EQ_BAND2_VOLUME_SHIFT              0x0
                                                                        // 0X5F = sets to 12 dB
                                                                        // 0X2F = sets to 0 dB
                                                                        // 0X00 = sets to -11.75 dB
                                                                        // Each LSB is 0.25 dB

/* ---------------------------------------------------------------- */

#define DAP_AUDIO_EQ_BAND3_REG                       0x011C             // 3000 Hz
                                                                        // 6:0
#define DAP_AUDIO_EQ_BAND3_VOLUME_MASK               0x7F               // Sets GEQ Band3
#define DAP_AUDIO_EQ_BAND3_VOLUME_SHIFT              0x0
                                                                        // 0X5F = sets to 12 dB
                                                                        // 0X2F = sets to 0 dB
                                                                        // 0X00 = sets to -11.75 dB
                                                                        // Each LSB is 0.25 dB

/* ---------------------------------------------------------------- */

#define DAP_AUDIO_EQ_BAND4_REG                       0x011E             // 9900 Hz
                                                                        // 6:0
#define DAP_AUDIO_EQ_BAND4_VOLUME_MASK               0x7F               // Sets GEQ Band4
#define DAP_AUDIO_EQ_BAND4_VOLUME_SHIFT              0x0
                                                                        // 0X5F = sets to 12 dB
                                                                        // 0X2F = sets to 0 dB
                                                                        // 0X00 = sets to -11.75 dB
                                                                        // Each LSB is 0.25 dB

/* ---------------------------------------------------------------- */

#define DAP_MAIN_CHAN_REG                            0x0120
                                                                        // 15:0
#define DAP_MAIN_CHAN_VOL_MASK                       0xFFFF             // DAP Main Channel Volume
#define DAP_MAIN_CHAN_VOL_SHIFT                      0x0
                                                                        // 0XFFFF = 200%
                                                                        // 0X8000 (default) = 100%
                                                                        // 0X0000 = 0%

/* ---------------------------------------------------------------- */

#define DAP_MIX_CHAN_REG                             0x0122
                                                                        // 15:0
#define DAP_MIX_CHAN_VOL_MASK                        0xFFFF             // DAP Mix Channel Volume
#define DAP_MIX_CHAN_VOL_SHIFT                       0x0
                                                                        // 0XFFFF = 200%
                                                                        // 0X8000 (default) = 100%
                                                                        // 0X0000 = 0%

/* ---------------------------------------------------------------- */

#define DAP_AVC_CTRL_REG                             0x0124
                                                                        // 13:12
#define DAP_AVC_CTRL_MAX_GAIN_MASK                   0x3000             // Maximum gain that can be applied by the AVC in expander mode.
#define DAP_AVC_CTRL_MAX_GAIN_SHIFT                  0xC
                                                                        // 0X0 = 0 dB gain
                                                                        // 0X1 = 6.0 dB of gain
                                                                        // 0X2 = 12 dB of gain
                                                                        // 9:8
#define DAP_AVC_CTRL_LBI_RESPONSE_MASK               0x300              // Integrator Response
#define DAP_AVC_CTRL_LBI_RESPONSE_SHIFT              0x8
                                                                        // 0X0 = 0 mS LBI
                                                                        // 0X1 = 25 mS LBI
                                                                        // 0X2 = 50 mS LBI
                                                                        // 0X3 = 100 mS LBI
                                                                        // 5
#define DAP_AVC_CTRL_HARD_LIMIT_EN_MASK              0x20               // Enable Hard Limiter Mode
#define DAP_AVC_CTRL_HARD_LIMIT_EN_SHIFT             0x5
                                                                        // 0X0 = Hard limit disabled. AVC Compressor/Expander is enabled.
                                                                        // 0X1 = Hard limit enabled. The signal is limited to the programmed threshold. (Signal
                                                                        // Saturates at the threshold)
                                                                        // 0
#define DAP_AVC_CTRL_EN_MASK                         0x1                // Enable/disable AVC
#define DAP_AVC_CTRL_EN_SHIFT                        0x0
                                                                        // 0X0 = Disable
                                                                        // 0X1 = Enable

/* ---------------------------------------------------------------- */

#define DAP_AVC_THRESHOLD_REG                        0x0126
                                                                        // 15:0
#define DAP_AVC_THRESHOLD_THRESH_MASK                0xFFFF             // AVC Threshold Value
#define DAP_AVC_THRESHOLD_THRESH_SHIFT               0x0
                                                                        // Threshold is programmable. Use the following formula to calculate hex value:
                                                                        // Hex Value = ((10^(THRESHOLD_dB/20))*0.636)*2^15
                                                                        // Threshold can be set in the range of 0 dB to -96 dB
                                                                        // Example Values:
                                                                        // 0X1473 = Set Threshold to -12 dB
                                                                        // 0X0A40 = Set Threshold to -18 dB

/* ---------------------------------------------------------------- */

#define DAP_AVC_ATTACK_REG                           0x0128
                                                                        // 11:0
#define DAP_AVC_ATTACK_RATE_MASK                     0xFFF              // AVC Attack Rate
#define DAP_AVC_ATTACK_RATE_SHIFT                    0x0
                                                                        // This is the rate at which the AVC applies attenuation to the signal to bring it to the
                                                                        // Threshold level. AVC Attack Rate is programmable. To use a custom rate, use the
                                                                        // Formula below to convert from dB/S to hex value:
                                                                        // Hex Value = (1 - (10^(-(Rate_dBs/(20*SYS_FS)))) * 2^19
                                                                        // Where, SYS_FS is the system sample rate configured in CHIP_CLK_CTRL register.
                                                                        // Example values:
                                                                        // 0X28 = 32 dB/s
                                                                        // 0X10 = 8.0 dB/s
                                                                        // 0X05 = 4.0 dB/s
                                                                        // 0X03 = 2.0 dB/s

/* ---------------------------------------------------------------- */

#define DAP_AVC_DECAY_REG                            0x012A
                                                                        // 11:0
#define DAP_AVC_DECAY_RATE_MASK                      0xFFF              // AVC Decay Rate
#define DAP_AVC_ATTACK_RATE_SHIFT                    0x0
                                                                        // This is the rate at which the AVC releases the attenuation previously applied to the
                                                                        // Signal during attack. AVC Decay Rate is programmable. To use a custom rate, use the
                                                                        // Formula below to convert from dB/S to hex value:
                                                                        // Hex Value = (1 - (10^(-(Rate_dBs/(20*SYS_FS)))) * 2^23
                                                                        // Where, SYS_FS is the system sample rate configured in CHIP_CLK_CTRL register.
                                                                        // Example values:
                                                                        // 0X284 = 32 dB/s
                                                                        // 0XA0 = 8.0 dB/s
                                                                        // 0X50 = 4.0 dB/s
                                                                        // 0X28 = 2.0 dB/s

/* ---------------------------------------------------------------- */

#define DAP_COEF_WR_B1_MSB_REG                       0x012C
                                                                        // 15:0
#define DAP_COEF_WR_B1_MSB_MSB_MASK                  0xFFFF             // Most significant 16-bits of the 20-bit filter coefficient that needs to be written
#define DAP_COEF_WR_B1_MSB_MSB_SHIFT                 0x0

/* ---------------------------------------------------------------- */

#define DAP_COEF_WR_B1_LSB_REG                       0x012E
                                                                        // 3:0
#define DAP_COEF_WR_B1_LSB_LSB_MASK                  0xF                // Least significant 4 bits of the 20-bit filter coefficient that needs to be written.
#define DAP_COEF_WR_B1_LSB_LSB_SHIFT                 0x0

/* ---------------------------------------------------------------- */

#define DAP_COEF_WR_B2_MSB_REG                       0x0130
                                                                        // 15:0
#define DAP_COEF_WR_B2_MSB_MSB_MASK                  0xFFFF             // Most significant 16-bits of the 20-bit filter coefficient that needs to be written
#define DAP_COEF_WR_B2_MSB_MSB_SHIFT                 0x0

/* ---------------------------------------------------------------- */

#define DAP_COEF_WR_B2_LSB_REG                       0x0132
                                                                        // 3:0
#define DAP_COEF_WR_B2_LSB_LSB_MASK                  0xF                // Least significant 4 bits of the 20-bit filter coefficient that needs to be written.
#define DAP_COEF_WR_B2_LSB_LSB_SHIFT                 0x0

/* ---------------------------------------------------------------- */

#define DAP_COEF_WR_A1_MSB_REG                       0x0134
                                                                        // 15:0
#define DAP_COEF_WR_A1_MSB_MSB_MASK                  0xFFFF             // Most significant 16-bits of the 20-bit filter coefficient that needs to be written
#define DAP_COEF_WR_A1_MSB_MSB_SHIFT                 0x0

/* ---------------------------------------------------------------- */

#define DAP_COEF_WR_A1_LSB_REG                       0x0136
                                                                        // 3:0
#define DAP_COEF_WR_A1_LSB_LSB_MASK                  0xF                // Least significant 4 bits of the 20-bit filter coefficient that needs to be written.
#define DAP_COEF_WR_A1_LSB_LSB_SHIFT                 0x0

/* ---------------------------------------------------------------- */

#define DAP_COEF_WR_A2_MSB_REG                       0x0138
                                                                        // 15:0
#define DAP_COEF_WR_A2_MSB_MSB_MASK                  0xFFFF             // Most significant 16-bits of the 20-bit filter coefficient that needs to be written
#define DAP_COEF_WR_A2_MSB_MSB_SHIFT                 0x0

/* ---------------------------------------------------------------- */

#define DAP_COEF_WR_A2_LSB_REG                       0x013A
                                                                        // 3:0
#define DAP_COEF_WR_A2_LSB_LSB_MASK                  0xF                // Least significant 4 bits of the 20-bit filter coefficient that needs to be written.
#define DAP_COEF_WR_A2_LSB_LSB_SHIFT                 0x0

// clang-format on

#endif
