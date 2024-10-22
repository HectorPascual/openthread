/*
 *  Copyright (c) 2016, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

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

#define STCTRL      (*( ( volatile unsigned long *) 0xE000E010 ))
#define STRELOAD    (*( ( volatile unsigned long *) 0xE000E014 ))
#define STCURR      (*( ( volatile unsigned long *) 0xE000E018 ))  

/*******STCTRL bits*******/
#define SBIT_ENABLE     0
#define SBIT_TICKINT    1
#define SBIT_CLKSOURCE  2

#define RELOAD_VALUE  1600000


void CoapReqHandler(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo){
    (void)aContext;
    (void)aMessageInfo;
    char buf[100];
    otCliOutputFormat("ok");
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

    otCoapStart(instance,OT_DEFAULT_COAP_PORT);

    const char *res_name = "test-res";
    const char *buf = "Hola";
    otCoapResource resource;
    memset(&resource, 0, sizeof(resource));

    //otUdpSocket *socket;

    otMessageInfo messageInfo;
    otCoapCode   coapCode;
    otMessage *msg;
    otCoapType   coapType;
    otError error = OT_ERROR_NONE;

    resource.mUriPath = res_name;
    resource.mHandler = CoapReqHandler;
    resource.mContext = NULL;
    resource.mNext = NULL;

    otIp6Address ipAddress;
    memset(&ipAddress, 0, sizeof(ipAddress));
    otIp6AddressFromString("dead:dead:cafe:cafe:dead:dead:cafe:0001", &ipAddress);

    otCliOutputFormat(otThreadErrorToString(otCoapAddResource(instance, &resource)));
    while (!otSysPseudoResetWasRequested()) {
        otTaskletsProcess(instance);
        otSysProcessDrivers(instance);

        if (!read_button()){
//            socket = otUdpGetSockets(instance);
            
            coapCode = OT_COAP_CODE_PUT;
            coapType = OT_COAP_TYPE_NON_CONFIRMABLE;    
            
            memset(&messageInfo, 0, sizeof(messageInfo));
            messageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;
            messageInfo.mPeerAddr = ipAddress;


            msg = otCoapNewMessage(instance, NULL);
            otCoapMessageInit(msg, coapType, coapCode);
            otCoapMessageGenerateToken(msg, 2);
            otCliOutputFormat(otThreadErrorToString(otCoapMessageAppendUriPathOptions(msg, resource.mUriPath)));
            otCliOutputFormat(otThreadErrorToString(otCoapMessageSetPayloadMarker(msg)));
            otCliOutputFormat(otThreadErrorToString(otMessageAppend(msg, buf, (uint16_t)strlen(buf))));
            error = otCoapSendRequest(instance, msg, &messageInfo, NULL,NULL);
            if(error != OT_ERROR_NONE && msg != NULL)
            {
                otCliOutputFormat("send-error");
                otMessageFree(msg);
            }
            delay_ms(80);
        }

    }



    otInstanceFinalize(instance);

    goto pseudo_reset;

    return 0;
}

