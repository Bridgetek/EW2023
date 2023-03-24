/**
* This source code ("the Software") is provided by Bridgetek Pte Ltd
* ("Bridgetek") subject to the licence terms set out
*   http://brtchip.com/BRTSourceCodeLicenseAgreement/ ("the Licence Terms").
* You must read the Licence Terms before downloading or using the Software.
* By installing or using the Software you agree to the Licence Terms. If you
* do not agree to the Licence Terms then do not download or use the Software.
*
* Without prejudice to the Licence Terms, here is a summary of some of the key
* terms of the Licence Terms (and in the event of any conflict between this
* summary and the Licence Terms then the text of the Licence Terms will
* prevail).
*
* The Software is provided "as is".
* There are no warranties (or similar) in relation to the quality of the
* Software. You use it at your own risk.
* The Software should not be used in, or for, any medical device, system or
* appliance. There are exclusions of Bridgetek liability for certain types of loss
* such as: special loss or damage; incidental loss or damage; indirect or
* consequential loss or damage; loss of income; loss of business; loss of
* profits; loss of revenue; loss of contracts; business interruption; loss of
* the use of money or anticipated savings; loss of information; loss of
* opportunity; loss of goodwill or reputation; and/or loss of, damage to or
* corruption of data.
* There is a monetary cap on Bridgetek's liability.
* The Software may have subsequently been amended by another user and then
* distributed by that other user ("Adapted Software").  If so that user may
* have additional licence terms that apply to those amendments. However, Bridgetek
* has no liability in relation to those amendments.
*/

#include "EVE_MediaFifo.h"
#include "EVE_Platform.h"

#ifdef EVE_SUPPORT_MEDIAFIFO

/* Set the media FIFO. 
Returns false in case a coprocessor fault occurred */
bool EVE_MediaFifo_set(EVE_HalContext *phost, uint32_t address, uint32_t size)
{
	bool res;

	if (!EVE_Hal_supportMediaFifo(phost))
	{
		eve_assert_ex(false, "EVE_MediaFifo_set is not available on the current graphics platform\n");
		return false;
	}

	if (phost->MediaFifoAddress != address || phost->MediaFifoSize != size)
	{
		EVE_Cmd_startFunc(phost);
		EVE_Cmd_wr32(phost, CMD_MEDIAFIFO);
		EVE_Cmd_wr32(phost, address);
		EVE_Cmd_wr32(phost, size);
		EVE_Cmd_endFunc(phost);

		/* Must flush for fifo pointers to be ready. */
		res = EVE_Cmd_waitFlush(phost);
	}
	else
	{
		res = true;
	}

	if (res)
	{
		eve_assert(EVE_Hal_rd32(phost, REG_MEDIAFIFO_READ) < size);
		eve_assert(EVE_Hal_rd32(phost, REG_MEDIAFIFO_WRITE) < size);
		phost->MediaFifoAddress = address;
		phost->MediaFifoSize = size;
	}
	else
	{
		phost->MediaFifoAddress = 0;
		phost->MediaFifoSize = 0;
	}

	return res;
}

void EVE_MediaFifo_close(EVE_HalContext *phost)
{
	phost->MediaFifoAddress = 0;
	phost->MediaFifoSize = 0;
}

/* Get the current read pointer. */
uint32_t EVE_MediaFifo_rp(EVE_HalContext *phost)
{
	return EVE_Hal_rd32(phost, REG_MEDIAFIFO_READ);
}

/* Get the current write pointer. */
uint32_t EVE_MediaFifo_wp(EVE_HalContext *phost)
{
	return EVE_Hal_rd32(phost, REG_MEDIAFIFO_WRITE);
}

/* Get the currently available space. */
uint32_t EVE_MediaFifo_space(EVE_HalContext *phost)
{
	if (!phost->MediaFifoSize)
		return 0;

	int32_t rp = EVE_Hal_rd32(phost, REG_MEDIAFIFO_READ);
	int32_t wp = EVE_Hal_rd32(phost, REG_MEDIAFIFO_WRITE);
#if 1
	return rp > wp
	    ? (rp - wp - 4)
	    : (rp + phost->MediaFifoSize - wp - 4);
#else
	int32_t diff = wp - rp;
	if (diff < 0)
		diff += phost->MediaFifoSize;
	eve_assert(diff >= 0 && diff < phost->MediaFifoSize);
	return phost->MediaFifoSize - diff - 4;
#endif
}

/* Write a buffer to the media FIFO. 
Waits if there is not enough space in the media FIFO. 
Returns false in case a coprocessor fault occurred */
bool EVE_MediaFifo_wrMem(EVE_HalContext *phost, const uint8_t *buffer, uint32_t size, uint32_t *transfered)
{
	if (!EVE_Hal_supportMediaFifo(phost))
	{
		eve_assert_ex(false, "EVE_MediaFifo_wrMem is not available on the current graphics platform\n");
		return false;
	}

	if (!phost->MediaFifoSize)
	{
		eve_printf_debug("EVE Media FIFO has not been set, cannot write\n");
		return false;
	}

#if 1
	/* Two strategies. 
	- Wait for entire space and write the entire buffer.
	- Wait for half the fifo to be available, and write in parts. */

	/* If the size is within the media fifo size, write the entire buffer at once */
	if (size <= phost->MediaFifoSize - 4)
	{
		uint32_t wp;
		int32_t overflow;

		if (!EVE_MediaFifo_waitSpace(phost, size, transfered))
			return false;

		wp = EVE_Hal_rd32(phost, REG_MEDIAFIFO_WRITE);
		overflow = (int32_t)(wp + size) - (int32_t)(phost->MediaFifoSize);
		if (overflow > 0)
		{
			eve_assert(phost->MediaFifoAddress + wp + size - overflow <= RAM_G_SIZE);
			eve_assert(phost->MediaFifoAddress + overflow <= RAM_G_SIZE);
			EVE_Hal_wrMem(phost, phost->MediaFifoAddress + wp, buffer, size - overflow);
			EVE_Hal_wrMem(phost, phost->MediaFifoAddress, &buffer[size - overflow], overflow);
		}
		else
		{
			eve_assert(phost->MediaFifoAddress + wp + size <= RAM_G_SIZE);
			EVE_Hal_wrMem(phost, phost->MediaFifoAddress + wp, buffer, size);
		}
		wp += size;
		if (wp >= phost->MediaFifoSize)
			wp -= phost->MediaFifoSize;
		eve_assert(wp < phost->MediaFifoSize);
		EVE_Hal_wr32(phost, REG_MEDIAFIFO_WRITE, wp);
		if (transfered)
			*transfered = size;
		return true;
	}
	else
	{
		/* Otherwise, write in parts. */
		uint32_t halfSize = ((phost->MediaFifoSize >> 3) << 2) - 4;
		uint32_t remaining = size;
		uint32_t done = 0;
		uint32_t wp;

		wp = EVE_Hal_rd32(phost, REG_MEDIAFIFO_WRITE);
		while (remaining)
		{
			uint32_t transfer = min(halfSize, remaining);
			int32_t overflow;
			if (!EVE_MediaFifo_waitSpace(phost, transfer, transfered))
				return false;

			overflow = (int32_t)(wp + transfer) - (int32_t)(phost->MediaFifoSize);
			if (overflow > 0)
			{
				eve_assert(phost->MediaFifoAddress + wp + transfer - overflow <= RAM_G_SIZE);
				eve_assert(phost->MediaFifoAddress + overflow <= RAM_G_SIZE);
				EVE_Hal_wrMem(phost, phost->MediaFifoAddress + wp, &buffer[done], transfer - overflow);
				EVE_Hal_wrMem(phost, phost->MediaFifoAddress, &buffer[done + transfer - overflow], overflow);
			}
			else
			{
				eve_assert(phost->MediaFifoAddress + wp + transfer <= RAM_G_SIZE);
				EVE_Hal_wrMem(phost, phost->MediaFifoAddress + wp, &buffer[done], transfer);
			}
			wp += transfer;
			done += transfer;
			remaining -= transfer;
			if (wp >= phost->MediaFifoSize)
				wp -= phost->MediaFifoSize;
			eve_assert(wp < phost->MediaFifoSize);
			EVE_Hal_wr32(phost, REG_MEDIAFIFO_WRITE, wp);

			if (transfered)
			{
				*transfered = done;
				if (EVE_Cmd_rp(phost) == EVE_Cmd_wp(phost))
					return true; /* Early exit, finished processing. */
			}
		}

		return true;
	}
#else
	scope
	{
		uint32_t halfSize = ((phost->MediaFifoSize >> 3) << 2) - 4;
		uint32_t remaining = size;
		uint32_t done = 0;
		uint32_t wp;

		/* Write to media FIFO as soon as space is available */
		wp = EVE_Hal_rd32(phost, REG_MEDIAFIFO_WRITE);
		while (remaining)
		{
			uint32_t transfer = min(
			    EVE_MediaFifo_waitSpace(phost, 4),
			    min(halfSize, remaining));
			if (!transfer)
				return false; // !phost->CmdFault;
			int32_t overflow = (int32_t)(wp + transfer) - (int32_t)(phost->MediaFifoSize);
			if (overflow > 0)
			{
				EVE_Hal_wrMem(phost, phost->MediaFifoAddress + wp, &buffer[done], transfer - overflow);
				EVE_Hal_wrMem(phost, phost->MediaFifoAddress, &buffer[done + transfer - overflow], overflow);
			}
			else
			{
				EVE_Hal_wrMem(phost, phost->MediaFifoAddress + wp, &buffer[done], transfer);
			}
			wp += transfer;
			done += transfer;
			remaining -= transfer;
			if (wp >= phost->MediaFifoSize)
				wp -= phost->MediaFifoSize;
			eve_assert(wp < phost->MediaFifoSize);
			EVE_Hal_wr32(phost, REG_MEDIAFIFO_WRITE, wp);

			if (transfered)
			{
				*transfered = done;
				if (EVE_Cmd_rp(phost) == EVE_Cmd_wp(phost))
					return true; /* Early exit, finished processing. */
			}
		}

		return true;
	}
#endif
}

#if defined(_DEBUG)
void debugBackupRamG(EVE_HalContext *phost);
#endif

static bool checkWait(EVE_HalContext *phost, uint32_t rpOrSpace)
{
	if (EVE_CMD_FAULT(rpOrSpace))
	{
		char err[128];
		(void)err;
		/* Coprocessor fault */
		phost->CmdWaiting = false;
		eve_printf_debug("Coprocessor fault in processing media FIFO\n");
#if defined(_DEBUG)
		debugBackupRamG(phost);
		if (EVE_CHIPID >= EVE_BT815)
		{
			EVE_Hal_rdMem(phost, (uint8_t *)err, RAM_ERR_REPORT, 128);
			eve_printf_debug("%s\n", err);
			EVE_Hal_displayMessage(phost, err, 128);
		}
		else
		{
			EVE_Hal_displayMessage(phost, "Coprocessor fault in processing media FIFO ", sizeof("Coprocessor fault in processing media FIFO "));
		}
#endif
		/* eve_debug_break(); */
		return false;
	}

	if (phost->Status == EVE_STATUS_ERROR)
	{
		phost->CmdWaiting = false;
		eve_printf_debug("Host error\n");
		return false;
	}

	return true;
}

static bool handleWait(EVE_HalContext *phost, uint16_t rpOrSpace)
{
	/* Check for coprocessor fault */
	if (!checkWait(phost, rpOrSpace))
		return false;

	/* Process any idling */
	EVE_Hal_idle(phost);

	/* Process user idling */
	if (phost->CbCmdWait
	    && !phost->CbCmdWait(phost))
	{
		/* Wait aborted */
		phost->CmdWaiting = false;
		eve_printf_debug("Wait for media FIFO aborted\n");
		return false;
	}
	return true;
}

/* Wait for the media FIFO to fully empty.
Returns false in case a coprocessor fault occurred */
bool EVE_MediaFifo_waitFlush(EVE_HalContext *phost, bool orCmdFlush)
{
	return EVE_MediaFifo_waitSpace(phost, phost->MediaFifoSize - 4, orCmdFlush);
}

/* Wait for the media FIFO to have at least the requested amount of free space.
Returns 0 in case a coprocessor fault occurred */
uint32_t EVE_MediaFifo_waitSpace(EVE_HalContext *phost, uint32_t size, bool orCmdFlush)
{
	uint32_t space;

	if (!EVE_Hal_supportMediaFifo(phost))
	{
		eve_assert_ex(false, "EVE_MediaFifo_waitSpace is not available on the current graphics platform\n");
		return 0;
	}

	if (!phost->MediaFifoSize)
	{
		eve_printf_debug("EVE Media FIFO has not been set, cannot wait for space\n");
		return 0;
	}

	if (size > (phost->MediaFifoSize - 4))
	{
		eve_printf_debug("Requested free space exceeds media FIFO\n");
		return 0;
	}

	eve_assert(!phost->CmdWaiting);
	phost->CmdWaiting = true;

#if 1
	space = EVE_MediaFifo_space(phost);
	if (!checkWait(phost, space))
		return 0;
#endif

	do
	{
		space = EVE_MediaFifo_space(phost);
		if (!handleWait(phost, (uint16_t)space))
			return 0;
		phost->CmdWaiting = false;
		uint32_t cmdSpace = EVE_Cmd_waitSpace(phost, 0);
		if (!cmdSpace)
			return 0; /* Check for coprocessor error */
		if (orCmdFlush && cmdSpace == (EVE_CMD_FIFO_SIZE - 4))
			return 0; /* Processed */
		phost->CmdWaiting = true;
	} while (space < size);

	phost->CmdWaiting = false;
	return space;
}

#endif

/* end of file */
