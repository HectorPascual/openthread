#include <assert.h>
#include <openthread-core-config.h>
#include <openthread/config.h>
#include <openthread/cli.h>
#include <openthread/diag.h>
#include <openthread/tasklet.h>
#include <openthread/error.h>
#include <openthread/platform/logging.h>
#include <stdlib.h>
#include <openthread/udp.h>
#include <openthread/ip6.h>
#include <openthread/coap.h>
#include <openthread/message.h>
#include <string.h>
#include <time.h>

#include "openthread-system.h"
#include "cc2538/delay.h"
#include "cc2538/leds.h"
#include "cc2538/gpio.h"
#include "cc2538/button.h"

void HandleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    (void)aContext;
    (void)aMessageInfo;
    char buf[100];
    otMessageRead(aMessage, otMessageGetOffset(aMessage), buf, otMessageGetLength(aMessage));
    otCliOutputFormat(buf);
}

int main(int argc, char *argv[])
{
    otInstance *instance;

pseudo_reset:

    otSysInit(argc, argv);

    instance = otInstanceInitSingle();
    assert(instance);

    otCliUartInit(instance);
    otPlatformLedsInit();
    init_button();

    const char *buf = "Hola";    

    otMessageInfo messageInfo;
    otMessage *msg;
    otError error = OT_ERROR_NONE;

    otIp6Address ipAddress;
    memset(&ipAddress, 0, sizeof(ipAddress));

    otIp6AddressFromString("dead:dead:cafe:cafe:dead:dead:cafe:0001", &ipAddress);

    while (!otSysPseudoResetWasRequested()) {
        otTaskletsProcess(instance);
        otSysProcessDrivers(instance);

        if (!read_button()){            
            
            memset(&messageInfo, 0, sizeof(messageInfo));
            messageInfo.mPeerPort = 1212;
            messageInfo.mPeerAddr = ipAddress;

            msg = otUdpNewMessage(instance, NULL);
            otCliOutputFormat(otThreadErrorToString(otMessageAppend(msg, buf, (uint16_t)strlen(buf))));

            error = otUdpSendDatagram(instance, msg, &messageInfo);

            if(error != OT_ERROR_NONE && msg != NULL)
            {
                otMessageFree(msg);
            }
            
            delay_ms(80);
        }

    }



    otInstanceFinalize(instance);

    goto pseudo_reset;

    return 0;
}

