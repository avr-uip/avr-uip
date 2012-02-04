#include "webclient.h"
#include "uart-t.h"


void webclient_datahandler(char *data, uint16_t len)
{
	//printf("Webclient: got %d bytes of data.\n", len);
    *(data + len - 1) = '\0'; // NULL term the data
    sendString("webclient data from server: ");
    sendString(data);
    sendString("\r\n");
}
