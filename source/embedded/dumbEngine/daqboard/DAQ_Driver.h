/*	This is a near-copy of the same file from the actual driver code.
 *	The driver code uses an enum that we'll need to use in our code as well.
 *  
 *  This file MUST be changed if the original driver file is changed
 *
 *  This file MUST NOT be changed, unless the original driver file is changed
 * 
 *	Yes, this is a violation of DRY(Don't Repeat Yourself), but the original driver
 *	code is buried deeply inside a Windows CE platform builder project, so it isn't easily
 *	accessible.
 */

#ifndef DAQ_DRIVER_H
#define DAQ_DRIVER_H

// IO Control Codes
enum{
	IOCTRL_USE_BAR0,
	IOCTRL_USE_BAR1,
	IOCTRL_DATA_FIFO_DUMP
};

#endif