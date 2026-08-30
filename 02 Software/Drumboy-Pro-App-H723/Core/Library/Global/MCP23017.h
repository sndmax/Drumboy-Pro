#ifndef __MCP23017_H_
#define __MCP23017_H_

#define MCP23017_I2C_ADDRESS                        0x20

// clang-format off

#define MCP23017_IODIRA                             0x00                // IO Direction
#define MCP23017_IODIRB                             0x01                // 0 = Output, 1 = Input

#define MCP23017_IPOLA                              0x02                // IO Polarity
#define MCP23017_IPOLB                              0x03                // 0 = Normal, 1 = Inverse

#define MCP23017_GPINTENA                           0x04                // Interrupt-on-Change
#define MCP23017_GPINTENB                           0x05                // 0 = Disable, 1 = Enable

#define MCP23017_DEFVALA                            0x06                // Default Value for Interrupt-on-Change
#define MCP23017_DEFVALB                            0x07                // (Interrupts on Opposite)

#define MCP23017_INTCONA                            0x08                // Interrupt Control
#define MCP23017_INTCONB                            0x09                // 0 = Interrupt-on-Change from Previous, 1 = Interrupt-on-Change from DEFVAL

#define MCP23017_IOCONA                             0x0A                // IO Configuration
#define MCP23017_IOCONB                             0x0B                // Bank / Mirror / Seqop / Disslw / Haen / Odr / Intpol / Notimp

#define MCP23017_GPPUA                              0x0C                // GPIO Pull-up Resistor
#define MCP23017_GPPUB                              0x0D                // 0 = Disable, 1 = Enable

#define MCP23017_INTFA                              0x0E                // Interrupt Flag
#define MCP23017_INTFB                              0x0F                // 0 = No Interrupt, 1 = Pin Caused Interrupt

#define MCP23017_INTCAPA                            0x10                // Interrupt Capture
#define MCP23017_INTCAPB                            0x11                // Value of GPIO at Time of Last Interrupt

#define MCP23017_GPIOA                              0x12                // Port Value
#define MCP23017_GPIOB                              0x13                // Write to Change, Read to Obtain Value

#define MCP23017_OLATA                              0x14                // Output Latch
#define MCP23017_OLATB                              0x15                // Write to Latch Output

#define MCP23017_INT_ERR                            255

// clang-format on

#endif
