//Matthew Gay
//August 5, 2009
//Lee Lab - Yale School of Medicine

//! This is a list of the register addresses used in a National Instruments M-Series data acquisition board

/*!
 * The register map was pulled together from a number of different sources.
 * These included the tMSeries files in the mseries/ChipObjects diretory
 * (which can be downloaded from the NI DDK website), the tMite files which are
 * in the DMA package (not part of the DDK but located on the same page), and 
 * some random documentation that kinda, sorta explained how the PCI controller
 * worked:
 * http://www.ni.com/pdf/manuals/370595b.pdf
 * http://www.ni.com/pdf/manuals/370505b.pdf
 */


#ifndef REGISTERMAP_H
#define REGISTERMAP_H


//-------------------------------------------
// STC Registers
//-------------------------------------------
// These are the registers that control the timing and
// nature of the data acquisition.  In the sample code,
// these are usually accessed through BAR0, and can be found
// in the tMSeries files (.h .cpp & .ipp)
//-------------------------------------------

#define REG_STATIC_DIGITAL_OUTPUT 0x024
#define REG_STATIC_DIGITAL_INPUT 0x024
#define REG_DIO_DIRECTION 0x028
#define REG_IO_BIDIRECTION_PIN 0x172
#define REG_PFI_OUTPUT_SELECT_1 0x1D0
#define REG_PFI_OUTPUT_SELECT_2 0x1D2
#define REG_PFI_OUTPUT_SELECT_3 0x1D4
#define REG_PFI_OUTPUT_SELECT_4 0x1D6
#define REG_PFI_OUTPUT_SELECT_5 0x1D8
#define REG_PFI_OUTPUT_SELECT_6 0x1DA
#define PFI_DO 0x1DE
#define REG_CLOCK_AND_FOUT 0x170
#define REG_CLOCK_AND_FOUT2 0x1C4
#define REG_PLL_CONTROL 0x1C6
#define REG_ANALOG_TRIGGER_ETC 0x17A
#define REG_ANALOG_TRIGGER_CONTROL 0x08C
#define REG_GEN_PWM_0 0x044
#define REG_GEN_PWM_1 0x046
#define REG_JOINT_RESET 0x190
#define REG_JOINT_STATUS_1 0x136
#define REG_JOINT_STATUS_2 0x13A
#define REG_INTERRUPT_A_ENABLE 0x192
#define REG_INTERRUPT_A_ACK 0x104
#define REG_AI_MODE_1 0x118
#define REG_AI_MODE_2 0x11A
#define REG_AI_MODE_3 0x1AE
#define REG_AI_OUTPUT_CONTROL 0x178
#define REG_AI_PERSONAL 0x19A
#define REG_AI_START_STOP_SELECT 0x17C
#define REG_AI_TRIGGER_SELECT 0x17E
#define REG_AI_FIFO_CLEAR 0x1A6
#define REG_CONFIGURATION_MEMORY_CLEAR 0x1A4
#define REG_AI_CONFIG_FIFO_BYPASS 0x218
#define REG_AI_CONFIG_FIFO_DATA 0x05E
#define REG_AI_COMMAND_1 0x110
#define REG_AI_COMMAND_2 0x108
#define REG_AI_SC_LOAD_A 0x124
#define REG_AI_SC_LOAD_B 0x128
#define REG_AI_SI2_LOAD_A 0x12C
#define REG_AI_SI2_LOAD_B 0x130
#define REG_AI_STATUS_1 0x104
#define REG_AI_FIFO_DATA 0x01C
#define REG_SCRATCH_PAD 0x005
#define REG_AI_SI_LOAD_A 0x11C
#define REG_AI_SI_LOAD_B 0x120
#define REG_AI_AO_SELECT 0x009

//-------------------------------------------
// MITE Registers
//-------------------------------------------
// These are the registers that control the way the
// DAQ interfaces with the PCI bus.  They are described
// in the tMite files (.h .cpp & .ipp).  Those files are
// located the DMA examples (as opposed to the regular 
// example files), and must be downloaded seperately.
//
// The register adresses were found in the exampe code provided
// by NI.  The first five names are guesses assembled from these two documents:
//  http://www.ni.com/pdf/manuals/370595b.pdf
//  http://www.ni.com/pdf/manuals/370505b.pdf
//The rest of the names are from tMITE.h
//-------------------------------------------
#define REG_IO_DEVICE_BASE_ADDRESS 0xC0
#define REG_IO_WINDOW_BASE_ADDRESS 0xC4
#define REG_BASE_ADDRESS_REGISTER_1 0x314
#define REG_IO_WINDOW_CONTROL_1 0xF4
#define REG_ADDRESS_MODE 0x30

#define REG_CHANNEL_OPERATION 0x00
#define REG_CHANNEL_CONTROL 0x04
#define REG_BASE_COUNT 0x2C
#define REG_TRANSFER_COUNT 0x08
#define REG_MEMORY_CONFIG 0x0C
#define REG_DEVICE_CONFIG 0x14
#define REG_BASE_ADDRESS 0x28
#define REG_MEMORY_ADDRESS 0x10
#define REG_DEVICE_ADDRESS 0x18

#endif

