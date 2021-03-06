#include "Render.h"

void WriteValues(unsigned char* data, unsigned int length)
{
	unsigned int i;
	for(i = 0; i < length; i++)
	{
		UCB0TXBUF = data[i];
		__bis_SR_register(LPM3_bits | GIE);
	}
}

void BeginWrite()
{
	IE2 |= UCB0TXIE; // Enable TX ready interrupt
	__disable_interrupt();
	UCB0CTL1 |= UCTR + UCTXSTT; // I2C TX, start condition
	__bis_SR_register(LPM3_bits | GIE);
}


void EndWrite()
{
	UCB0CTL1 |= UCTXSTP;
	IE2 &= ~UCB0TXIE;
}



void WriteCommands(const unsigned char* data, unsigned int length)
{
	unsigned int i;
	for(i = 0; i < length; i++)
	{
		WriteValue(0x80);
		WriteValue(data[i]);
	}
}

void ResetDisplayRAM_Vertical()
{
	int x, y;
	BeginWrite();
	for (x = 0; x < DISPLAY_WIDTH; x++)
	{
		for (y = 0; y < DISPLAY_COLUMN_PAGES; y++)
		{
				WriteData(0x00);
		}
	}
	EndWrite();
}

void ResetDisplayRAM_Page()
{
	int x, y;
	BeginWrite();
	for (y = 0; y < DISPLAY_COLUMN_PAGES; y++)
	{
		WriteCommand(DISPLAY_SET_PAGE_START_ADDRESS | y);
		for (x = 0; x < DISPLAY_WIDTH; x++)
		{
			WriteData(0x00);
		}
	}
	EndWrite();
}


void DisplayRenderTarget(RenderTarget renderTarget)
{
	BeginWrite();

    WriteCommand(DISPLAY_SET_LOWER_COLUMN | (renderTarget.StartColumn & 0xF));
    WriteCommand(DISPLAY_SET_HIGHER_COLUMN | (renderTarget.StartColumn >> 4));

    int i;
	for (i = 0; i < renderTarget.ColumnCount * DISPLAY_COLUMN_PAGES; i++)
		WriteData(renderTarget.Buffer[i]);

	EndWrite();
}
