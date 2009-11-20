//Matthew Gay
//August 19, 2009
//Lee Lab - Yale School of Medicine


//This is a colletion of private inline functions.

#include "PictoBoxDaqBoard.h"

//! Private functions used in driving the DAQ board

/*!
 * These functions are used to set up the analog inputs on a National Instruments 
 * M-series data acquisition card. The functions are from the example code
 * provided for the M Series.  It should be noted that this is a very limited set
 * of functions, since they are really only designed to be used for continuous
 * analog input.  A good implementation would keep a shadow copy
 * of all the registers.  This implementation only keeps a copy of a few (frequently
 * changed) registers.  It would be relatively easy to break things if you didn't pay
 * attention, or changed the order in which these are called.  

 * The functions (and the way there are used), is best understood by looking at the
 * analog input examples here:
 *  http://digital.ni.com/express.nsf/bycode/exyv4w?opendocument&lang=en&node=seminar_US
 */


#ifndef PICTOBOXDAQBOARD_P_H
#define PICTOBOXDAQBOARD_P_H

#define CONVERT_OUTPUT_SELECT 3
#define kAI_START_SelectPulse 18
#define kAI_START_SelectSI_TC 0

#define AI_CONFIGURE_POLARITY 0
#define AI_CONFIGURE_GAIN 0

namespace Picto {

/************************************************/
// _ResetAI()
/************************************************/
//! Resets the analog input registers to a known state
/*! 
 * Resets a bunch of registers, and then puts the DAQ into a state from which
 * it can be programmed for analog input.
 *
 * This is a combination of a bunch of functions from ai.cpp
 */
/************************************************/
inline void PictoBoxDaqBoard::_ResetAI()
{
	DWORD bytesWritten;
	ULONG data;


	soft_AI_MODE_1_ = 0;
	soft_AI_MODE_2_ = 0;
	soft_AI_START_STOP_SELECT_ = 0;

	DeviceIoControl(hDAQ_,IOCTRL_USE_BAR1,NULL,0,NULL,0, NULL, NULL);

	//configureTimebase()
	//-------------------
	//board->Clock_and_FOUT.setSlow_Internal_Timebase(1);  //set bit 11
	SetFilePointer(hDAQ_,REG_CLOCK_AND_FOUT,NULL,FILE_BEGIN);
	data = 1 << 11;
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);


	//pllReset()
	//----------
	//board->Clock_And_Fout2.setTB1_Select(tMSeries::tClock_And_Fout2::kTB1_SelectSelect_OSC); //kTB1_SelectSelect_OSC = 0
	//board->Clock_And_Fout2.setTB3_Select(tMSeries::tClock_And_Fout2::kTB3_SelectSelect_OSC); //kTB3_SelectSelect_OSC = 0
	data = (0<<5) | (0<<6);
	SetFilePointer(hDAQ_,REG_CLOCK_AND_FOUT2,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

	//board->PLL_Control.setPLL_Enable (kFalse);
	data = 0 <<12;
	SetFilePointer(hDAQ_,REG_PLL_CONTROL,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

	//analogTriggerReset()
	//---------------------
	//board->Analog_Trigger_Etc.setAnalog_Trigger_Reset(1);
    //board->Analog_Trigger_Etc.setAnalog_Trigger_Mode(tMSeries::tAnalog_Trigger_Etc::kAnalog_Trigger_ModeLow_Window);  //kAnalog_Trigger_ModeLow_Window = 0
	//Set bit 5 to 1, 
	//set bits <0:2> to zero
	SetFilePointer(hDAQ_,REG_ANALOG_TRIGGER_ETC,NULL,FILE_BEGIN);
	data = (1<<5) | (0<<0);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

    //board->Analog_Trigger_Control.setAnalog_Trigger_Select(tMSeries::tAnalog_Trigger_Control::kAnalog_Trigger_SelectGround);
	//set bits<0:1> to 3
	SetFilePointer(hDAQ_,REG_ANALOG_TRIGGER_CONTROL,NULL,FILE_BEGIN);
	data = 3;
	WriteFile(hDAQ_,&data, ONE_BYTE, &bytesWritten,NULL);

	//board->Gen_PWM[0].writeRegister(0);
    //board->Gen_PWM[1].writeRegister(0);
	//these guys are located at 0x44 and 0x46 (at least that's where they're initialized...)
	//Write zero to both
	data = 0;
	SetFilePointer(hDAQ_,REG_GEN_PWM_0,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);
	SetFilePointer(hDAQ_,REG_GEN_PWM_1,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

	//board->Analog_Trigger_Etc.setAnalog_Trigger_Enable(tMSeries::tAnalog_Trigger_Etc::kAnalog_Trigger_EnableDisabled);
	//This means to set bit 3 to 0.  It should already be that way, but just to be safe, 
	//we'll set the whole thing to zero.  Note that the previously set bit 5 in this register gets 
	//automatically cleared, so we don't need to worry about it.
	data = 0;
	SetFilePointer(hDAQ_,REG_ANALOG_TRIGGER_ETC,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

	//aiReset(hDAQ_);
	//--------------
	//board->Joint_Reset.writeAI_Reset(1);
	data = 1;
	SetFilePointer(hDAQ_,REG_JOINT_RESET,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

    //board->Joint_Reset.setAI_Configuration_Start (1);
	data = (1<<4);
	SetFilePointer(hDAQ_,REG_JOINT_RESET,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);
    
    //board->Interrupt_A_Enable.setAI_SC_TC_Interrupt_Enable (0);
    //board->Interrupt_A_Enable.setAI_START1_Interrupt_Enable (0);
    //board->Interrupt_A_Enable.setAI_START2_Interrupt_Enable (0);
    //board->Interrupt_A_Enable.setAI_START_Interrupt_Enable (0);
    //board->Interrupt_A_Enable.setAI_STOP_Interrupt_Enable (0);
    //board->Interrupt_A_Enable.setAI_Error_Interrupt_Enable (0);
    //board->Interrupt_A_Enable.setAI_FIFO_Interrupt_Enable (0);
	//set everything to zero...
	data = 0;
	SetFilePointer(hDAQ_,REG_INTERRUPT_A_ENABLE,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

    
    //board->Interrupt_A_Ack.setAI_SC_TC_Error_Confirm (1);
    //board->Interrupt_A_Ack.setAI_SC_TC_Interrupt_Ack (1);
    //board->Interrupt_A_Ack.setAI_START1_Interrupt_Ack (1);
    //board->Interrupt_A_Ack.setAI_START2_Interrupt_Ack (1);
    //board->Interrupt_A_Ack.setAI_START_Interrupt_Ack (1);
    //board->Interrupt_A_Ack.setAI_STOP_Interrupt_Ack (1);
    //board->Interrupt_A_Ack.setAI_Error_Interrupt_Ack (1);
	//set bits: 7,8,9,10,11,12,13 to 1
	data = (1<<7) | (1<<8)| (1<<9)| (1<<10)| (1<<11)| (1<<12)| (1<<13);
	SetFilePointer(hDAQ_,REG_INTERRUPT_A_ACK,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

    
    //board->AI_Mode_1.setAI_Start_Stop (1);
	//Set bit 3 to 1
	data = 1<<3;
	soft_AI_MODE_1_ = data;
	SetFilePointer(hDAQ_,REG_AI_MODE_1,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

    //These resets are kind of uneccessary...
	//Really, ressetting the AI should have taken care of this.
	//However, these are added so that the soft copies
	//of the registers are cleared (except that I'm 
	//not keeping soft copies)
	
	//board->AI_Mode_2.writeRegister (0);
	data = 0;
	soft_AI_MODE_2_ = data;
	SetFilePointer(hDAQ_,REG_AI_MODE_2,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);


    //board->AI_Mode_3.writeRegister (0);
	data = 0;
	SetFilePointer(hDAQ_,REG_AI_MODE_3,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);


    //board->AI_Output_Control.writeRegister (0);
	data = 0;
	SetFilePointer(hDAQ_,REG_AI_OUTPUT_CONTROL,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

    //board->AI_Personal.writeRegister (0);
	data = 0;
	SetFilePointer(hDAQ_,REG_AI_PERSONAL,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

    //board->AI_START_STOP_Select.writeRegister (0);
	data = 0;
	soft_AI_START_STOP_SELECT_ = 0;
	SetFilePointer(hDAQ_,REG_AI_START_STOP_SELECT,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

    //board->AI_Trigger_Select.writeRegister (0);
 	data = 0;
	SetFilePointer(hDAQ_,REG_AI_TRIGGER_SELECT,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);
   
    //board->Joint_Reset.setAI_Configuration_End (1);
 	data = 1<<8;
	SetFilePointer(hDAQ_,REG_JOINT_RESET,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);


	//aiPersonalize();
	//----------------
	//board->Joint_Reset.setAI_Configuration_Start (1);
    data = 1<<4;
	SetFilePointer(hDAQ_,REG_JOINT_RESET,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

    //board->AI_Output_Control.setAI_CONVERT_Output_Select (convertOutputSelect); //convertOutputSelect = kAI_CONVERT_Output_SelectActive_Low = 2
    //^^^^^^ This didn't work, so we're going to set it to kAI_CONVERT_Output_SelectActive_High (=3) instead, as advised here:
	//http://forums.ni.com/ni/board/message?board.id=90&message.id=1041&query.id=1471127#M1041
	//board->AI_Output_Control.setAI_SCAN_IN_PROG_Output_Select (tMSeries::tAI_Output_Control::kAI_SCAN_IN_PROG_Output_SelectActive_High); //kAI_SCAN_IN_PROG_Output_SelectActive_High = 3
	data = (CONVERT_OUTPUT_SELECT & 0x3) | (3<<8);
	SetFilePointer(hDAQ_,REG_AI_OUTPUT_CONTROL,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);
   
    //board->AI_Personal.setAI_CONVERT_Pulse_Width (tMSeries::tAI_Personal::kAI_CONVERT_Pulse_WidthAbout_1_Clock_Period); //kAI_CONVERT_Pulse_WidthAbout_1_Clock_Period = 1
    data = 1<<10;
	SetFilePointer(hDAQ_,REG_AI_PERSONAL,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

    //board->Joint_Reset.setAI_Configuration_End (1);
	data = 1<<8;
	SetFilePointer(hDAQ_,REG_JOINT_RESET,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

	//aiClearFifo()
	//-------------
	data = 1;
	SetFilePointer(hDAQ_,REG_AI_FIFO_CLEAR,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);
}


/************************************************/
// _ConfigureAI()
/************************************************/
//! Configures the daq for analog input using the list of AI channels
/*! 
 * This first fills the configuration FIFO on the DAQ, with all of the channels
 * from the AI Channel list.  Then, it sets up the a bunch of other registers
 * that control the triggering functions, and start and stop controls (these depend on
 * whether we are running continuously).
 *
 * This is a combination of a bunch of functions from ai.cpp
 */
/************************************************/
inline void PictoBoxDaqBoard::_ConfigureAI(unsigned int frequency, unsigned int numberOfSamples, bool continuous)
{
	DWORD bytesWritten;
	ULONG data;

	DeviceIoControl(hDAQ_,IOCTRL_USE_BAR1,NULL,0,NULL,0, NULL, NULL);


	//aiDisarm()
	//----------
    //board->Joint_Reset.writeAI_Reset (1);
	//data = 1;
	//SetFilePointer(hDAQ_,REG_JOINT_RESET,NULL,FILE_BEGIN);
	//WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

	//The above code doesn't make sense.  It resets all of the registers
	//To disarm, the AI, all we need to do is set the disaarm bit.
	//I'm going to try that instead:
	data = 1<<13;
	SetFilePointer(hDAQ_,REG_AI_COMMAND_1,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

	
	//aiClearConfigurationMemory()
	//----------------------------
	//board->Configuration_Memory_Clear.writeRegister (1);
	data = 1;
	SetFilePointer(hDAQ_,REG_CONFIGURATION_MEMORY_CLEAR,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

    //board->AI_Config_FIFO_Bypass.setAI_Bypass_Config_FIFO (0);
	data = 0;
	SetFilePointer(hDAQ_,REG_AI_CONFIG_FIFO_BYPASS,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, FOUR_BYTES, &bytesWritten,NULL);


	//Fill configuration FIFO
	//-----------------------
	//Loop through the channel list
	for(int i=0; i<AIChannelList_.size(); i++)
	{
		bool lastChannel;

		if(i == AIChannelList_.size()-1)
			lastChannel = TRUE;
		else
			lastChannel = FALSE;

	   //board->AI_Config_FIFO_Data.setAI_Config_Polarity (polarity);
		data = (AI_CONFIGURE_POLARITY & 0x1) << 12;
		//board->AI_Config_FIFO_Data.setAI_Config_Gain (gain);  
		data |= (AI_CONFIGURE_GAIN & 0x7) << 9;
		//board->AI_Config_FIFO_Data.setAI_Config_Channel (channel & 0xF);
		//board->AI_Config_FIFO_Data.setAI_Config_Bank ((channel & 0x30) >> 4);
		data |= AIChannelList_[i].channelNum & 0x3F;
		//board->AI_Config_FIFO_Data.setAI_Config_Channel_Type (channelType);
		data |= (AIChannelList_[i].chanType & 0x7) << 6;
		//board->AI_Config_FIFO_Data.setAI_Config_Dither (0);
		data |= 0<<13;
		//board->AI_Config_FIFO_Data.setAI_Config_Last_Channel (lastChannel);
		if(lastChannel)
			data |= 1 << 14;
			

		SetFilePointer(hDAQ_,REG_AI_CONFIG_FIFO_DATA,NULL,FILE_BEGIN);
		WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

		if(lastChannel)
		{
			//board->AI_Command_1.setAI_LOCALMUX_CLK_Pulse (kTrue);
			data = 1<<2;
			SetFilePointer(hDAQ_,REG_AI_COMMAND_1,NULL,FILE_BEGIN);
			WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);
		}


	}

	//aiSetFifoRequestMode()
	//----------------------
	//board->Joint_Reset.setAI_Configuration_Start (1);
	data = 1<<4;
	SetFilePointer(hDAQ_,REG_JOINT_RESET,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);


	//board->AI_Mode_3.setAI_FIFO_Mode (tMSeries::tAI_Mode_3::kAI_FIFO_ModeNot_Empty);  //kAI_FIFO_ModeNot_Empty = 0
	data = 0<<6;
	SetFilePointer(hDAQ_,REG_AI_MODE_3,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

	//board->Joint_Reset.setAI_Configuration_End (1);
	data = 1<<8;
	SetFilePointer(hDAQ_,REG_JOINT_RESET,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

	//aiEnvironmentalize()
	//--------------------
	//board->Joint_Reset.setAI_Configuration_Start (1);
	data = 1<<4;
	SetFilePointer(hDAQ_,REG_JOINT_RESET,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

    
    //board->AI_Mode_2.setAI_External_MUX_Present (tMSeries::tAI_Mode_2::kAI_External_MUX_PresentEvery_Convert);  //kAI_External_MUX_PresentEvery_Convert = 0
	data = soft_AI_MODE_2_;
	data |= 0<<12;
	soft_AI_MODE_2_ = data;
	SetFilePointer(hDAQ_,REG_AI_MODE_2,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);
    
    //board->AI_Output_Control.setAI_EXTMUX_CLK_Output_Select (tMSeries::tAI_Output_Control::kAI_EXTMUX_CLK_Output_SelectGround);  //kAI_EXTMUX_CLK_Output_SelectGround=1
	data = (CONVERT_OUTPUT_SELECT & 0x3) | (3<<8);  //from earlier commands
	data |= 1<<6;
	SetFilePointer(hDAQ_,REG_AI_OUTPUT_CONTROL,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);
    
    //board->Joint_Reset.setAI_Configuration_End (1);
	data = 1<<8;
	SetFilePointer(hDAQ_,REG_JOINT_RESET,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

	//aiHardwareGating()
	//------------------
	// board->Joint_Reset.setAI_Configuration_Start (1);
	data = 1<<4;
	SetFilePointer(hDAQ_,REG_JOINT_RESET,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

    
	//board->AI_Mode_3.setAI_External_Gate_Select (tMSeries::tAI_Mode_3::kAI_External_Gate_SelectDisabled);  //kAI_External_Gate_SelectDisabled=0
	data = 0<<6;  //from earlier commands
	data |= 0<<0;
	SetFilePointer(hDAQ_,REG_AI_MODE_3,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);
 
    //board->Joint_Reset.setAI_Configuration_End (1);
	data = 1<<8;
	SetFilePointer(hDAQ_,REG_JOINT_RESET,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

	//aiTrigger()
	//-----------
		// board->Joint_Reset.setAI_Configuration_Start (1);
	data = 1<<4;
	SetFilePointer(hDAQ_,REG_JOINT_RESET,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

	//board->AI_Mode_1.setAI_Trigger_Once (1);
	data = soft_AI_MODE_1_; //from earlier command
	data |= 1<<0;  //since ai_continous is set to 1, this should be zero! (according to documentation)
	//data |= 0<<0;  //since ai_continous is set to 1, this should be zero! (according to documentation)
	soft_AI_MODE_1_ = data;
	SetFilePointer(hDAQ_,REG_AI_MODE_1,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

    
    // Start trigger signal
    //board->AI_Trigger_Select.setAI_START1_Select (startSource);  //startSource = kAI_START1_SelectPulse = 0
    //board->AI_Trigger_Select.setAI_START1_Polarity (startPolarity);  /startPolarity = kAI_START1_PolarityRising_Edge = 0
    //board->AI_Trigger_Select.setAI_START1_Edge (1);
    //board->AI_Trigger_Select.setAI_START1_Sync (1);
	data = 0 | (0<<15) | (1<<5) | (1<<6);
	SetFilePointer(hDAQ_,REG_AI_TRIGGER_SELECT,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

    
    //board->AI_Mode_2.setAI_Pre_Trigger (0);
	data = soft_AI_MODE_2_; //from earlier command
	data |= 0<<13;
	soft_AI_MODE_2_ = data;
	SetFilePointer(hDAQ_,REG_AI_MODE_2,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);
    
    //board->AI_Trigger_Select.setAI_START2_Select (refSource); //refSource = kAI_START2_SelectPulse = 0
    //board->AI_Trigger_Select.setAI_START2_Polarity (refPolarity);  //refPolarity = kAI_START2_PolarityRising_Edge = 0
    //board->AI_Trigger_Select.setAI_START2_Edge (1);
    //board->AI_Trigger_Select.setAI_START2_Sync (1);
	data = 0 | (0<<15) | (1<<5) | (1<<6); //from earlier command
	data |= (0<<7) | (0<<14) | (1<<12) | (1<<13);
	SetFilePointer(hDAQ_,REG_AI_TRIGGER_SELECT,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);


	//board->Joint_Reset.setAI_Configuration_End (1);
	data = 1<<8;
	SetFilePointer(hDAQ_,REG_JOINT_RESET,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

	//aiSampleStop()
	//--------------
	// board->Joint_Reset.setAI_Configuration_Start (1);
	data = 1<<4;
	SetFilePointer(hDAQ_,REG_JOINT_RESET,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

    //board->AI_START_STOP_Select.setAI_STOP_Select (tMSeries::tAI_START_STOP_Select::kAI_STOP_SelectIN); //kAI_STOP_SelectIN = 19
    //board->AI_START_STOP_Select.setAI_STOP_Polarity (tMSeries::tAI_START_STOP_Select::kAI_STOP_PolarityActive_High); //kAI_STOP_PolarityActive_High =0
    //board->AI_START_STOP_Select.setAI_STOP_Sync (1);
    //board->AI_START_STOP_Select.setAI_STOP_Edge (multiChannel);  //multiChannel = 1 if >1 channels involved
	data = (19<<7) | (0<<14) | (1<<13);
	if(AIChannelList_.size()>1)
		data |= (1<<12);
	else
		data |= (0<<12);
	soft_AI_START_STOP_SELECT_ = data;
	SetFilePointer(hDAQ_,REG_AI_START_STOP_SELECT,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);


	//board->Joint_Reset.setAI_Configuration_End (1);
	data = 1<<8;
	SetFilePointer(hDAQ_,REG_JOINT_RESET,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);


	//aiNumberOfSamples()
	//-------------------
	//we assume zero pretrigger samples, and a coninuous
	//request overrides the number of samples.
	
	// board->Joint_Reset.setAI_Configuration_Start (1);
	data = 1<<4;
	SetFilePointer(hDAQ_,REG_JOINT_RESET,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

	//board->AI_Mode_1.setAI_Continuous (continuous); 
	data = soft_AI_MODE_1_; //from earlier command
	if(continuous)
		data |= 1<<1;
	else
		data |= 0<<1;
	soft_AI_MODE_1_ = data;
	SetFilePointer(hDAQ_,REG_AI_MODE_1,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);
    
    //board->AI_SC_Load_A.writeRegister (postTriggerSamples-1);  
    data = numberOfSamples-1;
	SetFilePointer(hDAQ_,REG_AI_SC_LOAD_A,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, FOUR_BYTES, &bytesWritten,NULL);


	//assuming no pretrigger samples

    //board->AI_Mode_2.setAI_SC_Reload_Mode (tMSeries::tAI_Mode_2::kAI_SC_Reload_ModeNo_Change);  //kAI_SC_Reload_ModeNo_Change = 0
    //board->AI_Mode_2.setAI_SC_Initial_Load_Source (tMSeries::tAI_Mode_2::kAI_SC_Initial_Load_SourceLoad_A);  //kAI_SC_Initial_Load_SourceLoad_A = 0
	data = soft_AI_MODE_2_;
	data |= (0<<1) | (0<<2);
	soft_AI_MODE_2_ = data;
	SetFilePointer(hDAQ_,REG_AI_MODE_2,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

	//board->AI_SC_Load_B.setRegister (0);
	data = 0;
	SetFilePointer(hDAQ_,REG_AI_SC_LOAD_B,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, FOUR_BYTES, &bytesWritten,NULL);

    //board->AI_Command_1.setAI_SC_Load (1);
	data = 1<<5;
	SetFilePointer(hDAQ_,REG_AI_COMMAND_1,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

	//board->Joint_Reset.setAI_Configuration_End (1);
	data = 1<<8;
	SetFilePointer(hDAQ_,REG_JOINT_RESET,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

	//aiSampleStart (hDAQ_,numChannels,samplePeriodDivisor,3,kAI_START_SelectSI_TC);
	//----------------------------------------------------------------

	//calculate the sample period divisor (timebase/samplerate)
	unsigned int periodDivisor = 20000000/frequency; //timebase = 20MHz
	unsigned int delayDivisor = 3;

	//We are assuming that the "source" is always kAI_START_SelectSI_TC
	//This means that you can't really collect samples "on demand", but
	//that doesn't seem to be a great loss...

	// board->Joint_Reset.setAI_Configuration_Start (1);
	data = 1<<4;
	SetFilePointer(hDAQ_,REG_JOINT_RESET,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

    //board->AI_START_STOP_Select.setAI_START_Select (source);
    //board->AI_START_STOP_Select.setAI_START_Polarity (polarity);  //polarity = kAI_START_PolarityRising_Edge = 0
    //board->AI_START_STOP_Select.setAI_START_Sync (1);
    //board->AI_START_STOP_Select.setAI_START_Edge (1);
	data = soft_AI_START_STOP_SELECT_;
	data |= (kAI_START_SelectSI_TC<<0) | (0<<15) | (1<<5) | (1<<6);
	soft_AI_START_STOP_SELECT_ = data;
	SetFilePointer(hDAQ_,REG_AI_START_STOP_SELECT,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);
  
    //if (source == kAI_START_SelectSI_TC) 
	//^^^^ always true

	//board->AI_Mode_1.setAI_SI_Source_Polarity (tMSeries::tAI_Mode_1::kAI_SI_Source_PolarityRising_Edge); //kAI_SI_Source_PolarityRising_Edge = 0
    //board->AI_Mode_1.setAI_SI_Source_Select (tMSeries::tAI_Mode_1::kAI_SI_Source_SelectINTIMEBASE1); //kAI_SI_Source_SelectINTIMEBASE1 = 28
    //board->AI_Mode_1.flush ();
	data = soft_AI_MODE_1_;
	data |= (0<<4) | (28<<6);
	soft_AI_MODE_1_ = data;
	SetFilePointer(hDAQ_,REG_AI_MODE_1,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

    //board->AI_SI_Load_B.writeRegister (delayDivisor-1);
	data = delayDivisor-1;
	SetFilePointer(hDAQ_,REG_AI_SI_LOAD_B,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, FOUR_BYTES, &bytesWritten,NULL);

    //board->AI_SI_Load_A.writeRegister (periodDivisor-1);
	data = periodDivisor-1;
	SetFilePointer(hDAQ_,REG_AI_SI_LOAD_A,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, FOUR_BYTES, &bytesWritten,NULL);

    //board->AI_Mode_2.setAI_SI_Initial_Load_Source (tMSeries::tAI_Mode_2::kAI_SI_Initial_Load_SourceLoad_B);     //kAI_SI_Initial_Load_SourceLoad_B = 1   
    //board->AI_Mode_2.flush ();
	data = soft_AI_MODE_2_;
	data |= (1<<7);
	soft_AI_MODE_2_ = data;
	SetFilePointer(hDAQ_,REG_AI_MODE_2,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

    //board->AI_Command_1.writeAI_SI_Load (1);
	data = 1<<9;
	SetFilePointer(hDAQ_,REG_AI_COMMAND_1,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

    //board->AI_Mode_2.setAI_SI_Reload_Mode (tMSeries::tAI_Mode_2::kAI_SI_Reload_ModeAlternate_First_Period_Every_SCTC); //kAI_SI_Reload_ModeAlternate_First_Period_Every_SCTC = 6
    //board->AI_Mode_2.setAI_SI_Initial_Load_Source (tMSeries::tAI_Mode_2::kAI_SI_Initial_Load_SourceLoad_A);
    //board->AI_Mode_2.flush ();
	data = soft_AI_MODE_2_;
	data |= (6<<4);
	data &= ~(1<<7); //clear initial load source
	soft_AI_MODE_2_ = data;
	SetFilePointer(hDAQ_,REG_AI_MODE_2,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);


	//board->Joint_Reset.setAI_Configuration_End (1);
	data = 1<<8;
	SetFilePointer(hDAQ_,REG_JOINT_RESET,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

	//aiConvert()
	//-----------
	DWORD convertDelay = 3;

	// board->Joint_Reset.setAI_Configuration_Start (1);
	data = 1<<4;
	SetFilePointer(hDAQ_,REG_JOINT_RESET,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

	//board->AI_Mode_1.setAI_CONVERT_Source_Select (tMSeries::tAI_Mode_1::kAI_CONVERT_Source_SelectSI2TC); //kAI_CONVERT_Source_SelectSI2TC = 0
    //board->AI_Mode_1.setAI_CONVERT_Source_Polarity (tMSeries::tAI_Mode_1::kAI_CONVERT_Source_PolarityFalling_Edge); //kAI_CONVERT_Source_PolarityFalling_Edge = 0
	data = soft_AI_MODE_1_;
	data |= (0<<11) | (0<<5);
	soft_AI_MODE_1_ = data;
	SetFilePointer(hDAQ_,REG_AI_MODE_1,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

    //board->AI_Mode_2.setAI_SC_Gate_Enable (0);
    //board->AI_Mode_2.setAI_Start_Stop_Gate_Enable (0);
    //board->AI_Mode_2.setAI_SI2_Reload_Mode (tMSeries::tAI_Mode_2::kAI_SI2_Reload_ModeAlternate_First_Period_Every_STOP);  //kAI_SI2_Reload_ModeAlternate_First_Period_Every_STOP = 1
	data = soft_AI_MODE_2_;
	data |= (0<<15) | (0<<14) | (1<<8);
	soft_AI_MODE_2_ = data;
	SetFilePointer(hDAQ_,REG_AI_MODE_2,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

    //if (externalSampleClock)
        //board->AI_Mode_3.setAI_SI2_Source_Select (tMSeries::tAI_Mode_3::kAI_SI2_Source_SelectINTIMEBASE1);
   // else
        //board->AI_Mode_3.setAI_SI2_Source_Select (tMSeries::tAI_Mode_3::kAI_SI2_Source_SelectSame_As_SI); //kAI_SI2_Source_SelectSame_As_SI=0
	//no external sample clock
	data = 0<<6;  //from earlier commands
	data |= 0<<0;  //from earlier commands
	data |= 0<<11;
	SetFilePointer(hDAQ_,REG_AI_MODE_3,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

    //board->AI_SI2_Load_A.writeRegister (delayDivisor); //delayDivisor = 280
	data = convertDelay;
	SetFilePointer(hDAQ_,REG_AI_SI2_LOAD_A,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, FOUR_BYTES, &bytesWritten,NULL);

    //board->AI_SI2_Load_B.writeRegister (periodDivisor); //periodDivisor = 280
	data = 280;
	SetFilePointer(hDAQ_,REG_AI_SI2_LOAD_B,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, FOUR_BYTES, &bytesWritten,NULL);
    
    //board->AI_Mode_2.setAI_SI2_Initial_Load_Source (tMSeries::tAI_Mode_2::kAI_SI2_Initial_Load_SourceLoad_A); //kAI_SI2_Initial_Load_SourceLoad_A=0
	data = soft_AI_MODE_2_; //from earlier command
	data |= (0<<9);
	soft_AI_MODE_2_ = data;
 	SetFilePointer(hDAQ_,REG_AI_MODE_2,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);
   
    //board->AI_Command_1.setAI_SI2_Load (1);
	data |= (1<<11);
 	SetFilePointer(hDAQ_,REG_AI_COMMAND_1,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);
    
	//board->AI_Mode_2.setAI_SI2_Initial_Load_Source (tMSeries::tAI_Mode_2::kAI_SI2_Initial_Load_SourceLoad_B); //kAI_SI2_Initial_Load_SourceLoad_B = 1
	data = soft_AI_MODE_2_; //from earlier command
	data |= (1<<9);
	soft_AI_MODE_2_ = data;
 	SetFilePointer(hDAQ_,REG_AI_MODE_2,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

	//board->Joint_Reset.setAI_Configuration_End (1);
	data = 1<<8;
	SetFilePointer(hDAQ_,REG_JOINT_RESET,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

	//aiClearFifo()
	//-------------
	data = 1;
	SetFilePointer(hDAQ_,REG_AI_FIFO_CLEAR,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);


	//aiArm()
	//board->AI_Command_1.setAI_SC_Arm (1);
    //board->AI_Command_1.setAI_SI2_Arm (1);  
	//board->AI_Command_1.setAI_SI_Arm (1);
	data = (1<<6) | (1<<12);
	data |= (1<<10);
	SetFilePointer(hDAQ_,REG_AI_COMMAND_1,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

}
/************************************************/
// _StartAI()
/************************************************/
//! Starts the DAQ collecting AI data
/*! 
 * This starts the AI running (either continuously, or for a given number of samples)
 */
/************************************************/

inline void PictoBoxDaqBoard::_StartAI()
{
	DWORD bytesWritten;
	ULONG data;

	DeviceIoControl(hDAQ_,IOCTRL_USE_BAR1,NULL,0,NULL,0, NULL, NULL);

    //board->AI_Command_2.setAI_START1_Pulse (1);
	data = (1<<0);
	SetFilePointer(hDAQ_,REG_AI_COMMAND_2,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

}





/*! 
 * This is a selection of functions used for scaling the digital
 * values created by a National Instruments M-Series data acquisition
 * board.
 * 
 * These functions have been tested on a PCI-6221 board, and may or may not
 * work on other boards.  
 * 
 * 
 * The functions (and the way there are used), is best understood by looking at the
 * analog input examples here:
 *   http://digital.ni.com/express.nsf/bycode/exyv4w?opendocument&lang=en&node=seminar_US
 */




//!Used for converting raw binary to floating point
typedef union {
    UCHAR  b[4];
    float f;
}eepromF32;

/************************************************/
// getFloatFromEeprom()
/************************************************/
//! Gets a floating point value from the eeprom memory
/*! 
 * This pulls a 4-byte floating point value out of the (previously downloaded)
 * eeprom memory.  The value is retrieved from the passed in offset.
 *
 * The function uses a simple union to perform the conversion
 *
 * \param _eepromMemory The EEEPROM memory downloaded from the DAQ.
 * \param _offset The offset (in bytes) from which to recover the floating point value
 * \returns The floating point value at the passed in offset
 *
 * \note This function will assumes little endian notation.
 */
/************************************************/
inline float getFloatFromEeprom (const UCHAR _eepromMemory[], ULONG _offset)
{
    eepromF32 value;
   
	//little endian   
    value.b[3] = _eepromMemory[_offset++];
    value.b[2] = _eepromMemory[_offset++];
    value.b[1] = _eepromMemory[_offset++];
    value.b[0] = _eepromMemory[_offset++];
   
   
    return value.f;
}
/************************************************/
// _getCalibrationValues()
/************************************************/
//! Recovers the calibration values from the DAQs EEPROM
/*! 
 * This grabs the calibration values from the DAQ's EEPROM, and then stores them
 * in a scalingCoeff structure.
 *
 * To understand this function, check out the scale.cpp files in the NI examples.
 *
 * \param _pScale Pointer to a scalingCoeff structure which will be filled with the scaling coefficients from the EEPROM
 *
 * \note This function assumes that we are using 0 gain (+/- 10V).  For other gains the function will require additional modifications.
 */
/************************************************/
inline void PictoBoxDaqBoard::_getCalibrationValues(scalingCoeff *_pScale)
{
	ULONG eepromInfo[2];
	UCHAR eepromData[1024];

	ULONG calOffset, modeOffset, intervalMemOffset;

	ULONG modeOrder;
	float modeC[4];

	float intervalGain, intervalOffset;


	//The calibration info is stored in the Eeprom from 0x400 to 0x800
	//To make life easier, we'll just read the whole block of memory into
	//an array and work with it there (since IOControls are expensive)
	eepromInfo[0] = 0x400;	//offset
	eepromInfo[1] = 0x400;	//buffer length
	//DeviceIoControl(hDAQ_,IOCTRL_READ_EEPROM,eepromInfo,8,eepromData,0x400, &bytesRead, NULL);
	_readEeprom(0x400,		//offset
			   eepromData,	//buffer
			   0x400);		//buffer length

	//next, we need to find the area in which the calibration constants are stored
	calOffset = (eepromData[24] << 8) | eepromData[25];
	
	//Now grab all the useful data from the EEPROM
	
	//u32 modeOffset = calOffset + kAiModeBlockStart + (modeIdx*kAiModeBlockSize);
	modeOffset = calOffset + 16 + (0*17);

	modeOrder = eepromData[modeOffset++];
	for(int i=0; i<4; i++)
	{
		modeC[i] = getFloatFromEeprom (eepromData, modeOffset);
		modeOffset+=4;
	}

	//u32 intervalOffset = calOffset + kAiIntervalBlockStart + (intervalIdx*kAiIntervalBlockSize);
	intervalMemOffset = calOffset + 84 + (0*8);
	intervalGain = getFloatFromEeprom (eepromData, intervalMemOffset);
	intervalMemOffset += 4;
	intervalOffset = getFloatFromEeprom (eepromData, intervalMemOffset);

	//Finally, we can store the data collected from the EEPROM
	//in the scaling structure
	_pScale->order = modeOrder;

	for(unsigned int i=0; i<modeOrder; i++)
	{
		_pScale->c[i] = modeC[i] * intervalGain;
		if(i==0)
			_pScale->c[i] += intervalOffset;
	}




}


/************************************************/
// _aiPolynomialScaler()
/************************************************/
//! Converts a raw binary value to a voltage
/*! 
 * This uses the passed in caling coefficients to convert a raw binary 
 * value into a scaled voltage.
 *
 * \param _raw The raw 32 bit value from the DAQ's data FIFO
 * \param _pScale Pointer to a scalingCoeff structure which was filled by calling _getCalibrationValues
 *
 * \returns A floating point value which is the exact voltage recorded by the DAQ.
 *
 * \note You must call _getCalibrationValues before using this.
 */
/************************************************/
inline float PictoBoxDaqBoard::_aiPolynomialScaler(int _raw, const scalingCoeff *_pScale)
{
	float scaledVal;
	ULONG order;

	order = _pScale->order;
	scaledVal = _pScale->c[order];

	for(int i=order-1; i>=0; i--)
	{
		scaledVal *= (float)_raw;
		scaledVal += _pScale->c[i];
	}

	return scaledVal;
}


/************************************************/
// _aiPolynomialScaler()
/************************************************/
//! Reads data from the DAQ's EEPROM and stores it in a buffer for later access.
/*! 
 * This reads the DAQs EEPROM and stores the values in a buffer.  The
 * process of reading from the EEPROM is a bit of black magic, The incantation
 * is found in the function readFromEeprom in scale.cpp in the examples folder.
 * 
 * \param _offset The starting offset for reading from the eeprom
 * \param _pBuffer The buffer to be filled with the eeprom data
 * \param _bufferLen The amount of data to be placed in the buffer
 */
/************************************************/
inline void PictoBoxDaqBoard::_readEeprom(unsigned int _offset, UCHAR *_pBuffer, unsigned int _bufferLen)
{
	ULONG iowcr1Initial;
    ULONG iowbsr1Initial;
    ULONG iodwbsrInitial; 
    ULONG bar1Value;

	DWORD bytesRead, bytesWritten;
	DWORD data;


	
	//switch over to bar0 (MITE registers)
	DeviceIoControl(hDAQ_,IOCTRL_USE_BAR0,NULL,0,NULL,0, NULL, NULL);

	//set up the eeprom for reading
	//-----------------------------
	SetFilePointer(hDAQ_,REG_IO_DEVICE_BASE_ADDRESS,NULL,FILE_BEGIN);
	ReadFile(hDAQ_,&iodwbsrInitial, FOUR_BYTES, &bytesRead,NULL); 

	data = 0;
	SetFilePointer(hDAQ_,REG_IO_DEVICE_BASE_ADDRESS,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data,FOUR_BYTES,&bytesWritten,NULL);

	SetFilePointer(hDAQ_,REG_IO_WINDOW_BASE_ADDRESS,NULL,FILE_BEGIN);
	ReadFile(hDAQ_,&iowbsr1Initial, FOUR_BYTES, &bytesRead,NULL); 

	SetFilePointer(hDAQ_,REG_BASE_ADDRESS_REGISTER_1,NULL,FILE_BEGIN);
	ReadFile(hDAQ_,&bar1Value, FOUR_BYTES, &bytesRead,NULL);

	data = 0x0000008B | bar1Value;
	SetFilePointer(hDAQ_,REG_IO_WINDOW_BASE_ADDRESS,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data,FOUR_BYTES,&bytesWritten,NULL);

	SetFilePointer(hDAQ_,REG_IO_WINDOW_CONTROL_1,NULL,FILE_BEGIN);
	ReadFile(hDAQ_,&iowcr1Initial, FOUR_BYTES, &bytesRead,NULL); 


	data = 0x00000001 | iowcr1Initial;
	SetFilePointer(hDAQ_,REG_IO_WINDOW_CONTROL_1,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data,FOUR_BYTES,&bytesWritten,NULL);

	data = 0xF;
	SetFilePointer(hDAQ_,REG_IO_WINDOW_CONTROL_1,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data,FOUR_BYTES,&bytesWritten,NULL);

	//Read from the eeprom
	//--------------------
	//Switch to BAR1
	DeviceIoControl(hDAQ_,IOCTRL_USE_BAR1,NULL,0,NULL,0, NULL, NULL);
	
	//move to the begining of the desired block of memory
	SetFilePointer(hDAQ_,_offset,NULL,FILE_BEGIN);

	for(unsigned int i=0; i<_bufferLen; i++)
	{
		//*pBufOut=READ_REGISTER_UCHAR((UCHAR*)(pDAQContext->bars[1]+eepromOffset+i));
		//pBufOut++;
		ReadFile(hDAQ_,_pBuffer,ONE_BYTE,&bytesRead,NULL);
		_pBuffer++;
	}

	//switch back to bar0
	DeviceIoControl(hDAQ_,IOCTRL_USE_BAR0,NULL,0,NULL,0, NULL, NULL);




	//return the eeprom to its orginal state
	//--------------------------------------
	//WRITE_REGISTER_ULONG((ULONG*)(pDAQContext->bars[0]+ 0xC4), iowbsr1Initial);
	data = iowbsr1Initial;
	SetFilePointer(hDAQ_,REG_IO_WINDOW_BASE_ADDRESS,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data,FOUR_BYTES,&bytesWritten,NULL);

	//WRITE_REGISTER_ULONG((ULONG*)(pDAQContext->bars[0]+ 0xC0), iodwbsrInitial);
	data = iodwbsrInitial;
	SetFilePointer(hDAQ_,REG_IO_DEVICE_BASE_ADDRESS,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data,FOUR_BYTES,&bytesWritten,NULL);

	//WRITE_REGISTER_ULONG((ULONG*)(pDAQContext->bars[0]+ 0xF4), iowcr1Initial);
	data = iowcr1Initial;
	SetFilePointer(hDAQ_,REG_IO_WINDOW_CONTROL_1,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data,FOUR_BYTES,&bytesWritten,NULL);

	//WRITE_REGISTER_ULONG((ULONG*)(pDAQContext->bars[0]+ 0x30), 0x00);
	data = 0x00;
	SetFilePointer(hDAQ_,REG_ADDRESS_MODE,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data,FOUR_BYTES,&bytesWritten,NULL);

	//Since most of the time, we'll be reading/writing BAR1, let's
	//leave the dAQ in that state.
	DeviceIoControl(hDAQ_,IOCTRL_USE_BAR1,NULL,0,NULL,0, NULL, NULL);




}

};  //end Picto namespace

#endif