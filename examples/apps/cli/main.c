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
#include <openthread/message.h>
#include <string.h>

#include "openthread-system.h"
#include "cc2538/leds.h"
#include "cc2538/gpio.h"
#include "cc2538/button.h"


#if OPENTHREAD_EXAMPLES_POSIX
#include <setjmp.h>
#include <unistd.h>

jmp_buf gResetJump;

void __gcov_flush();
#endif

#if OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
void *otPlatCAlloc(size_t aNum, size_t aSize)
{
    return calloc(aNum, aSize);
}

void otPlatFree(void *aPtr)
{
    free(aPtr);
}
#endif

void otTaskletsSignalPeotInstancending(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
}

void HandleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    (void)aContext;
    (void)aMessageInfo;
    char buf[100];
    otMessageRead(aMessage, otMessageGetOffset(aMessage), buf, otMessageGetLength(aMessage));
    if(cc2538GpioReadPin(GPIO_D_NUM, LED2_PIN)){
        LED2_OFF;
        otCliOutputFormat("led2 on");
    }
    else{
        LED2_ON;
        otCliOutputFormat("led2 off");
    }
    //otCliOutputFormat(buf);
}

int main(int argc, char *argv[])
{
    otInstance *instance;

#if OPENTHREAD_EXAMPLES_POSIX
    if (setjmp(gResetJump))
    {
        alarm(0);
#if OPENTHREAD_ENABLE_COVERAGE
        __gcov_flush();
#endif
        execvp(argv[0], argv);
    }
#endif

#if OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
    size_t   otInstanceBufferLength = 0;
    uint8_t *otInstanceBuffer       = NULL;
#endif

pseudo_reset:

    otSysInit(argc, argv);

#if OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
    // Call to query the buffer size
    (void)otInstanceInit(NULL, &otInstanceBufferLength);

    // Call to allocate the buffer
    otInstanceBuffer = (uint8_t *)malloc(otInstanceBufferLength);
    assert(otInstanceBuffer);

    // Initialize OpenThread with the buffer
    instance = otInstanceInit(otInstanceBuffer, &otInstanceBufferLength);
#else
    instance = otInstanceInitSingle();
#endif
    assert(instance);
    otUdpSocket *socket;
    otCliUartInit(instance);
    otPlatformLedsInit();
    init_button();
    while (!otSysPseudoResetWasRequested()) {
        otTaskletsProcess(instance);
        otSysProcessDrivers(instance);

        
        if(!read_button()){
            //otNetifAddress *st_iplist;
            //st_iplist = otIp6GetUnicastAddresses(instance);
            //otMessageInfo st_message = { st_iplist -> mAddress , 4, TRUE};
            //otUdpSendDatagram(instance, "hola", st_message);
            //otCliOutputFormat("LED ON!\r\n");
            socket = otUdpGetSockets(instance);
            socket->mHandler = HandleUdpReceive;

            char buffer_1[50];
            char buffer_2[50];
            otError error = OT_ERROR_NONE;
            int c;
            otCliOutputFormat("HOST IP\n");
            for (c = 0; c < 16; c++){
                otCliOutputFormat(itoa((int)otIp6GetUnicastAddresses(instance)->mAddress.mFields.m8[c], buffer_1, 16));
                if (c%2){
                    otCliOutputFormat(":");
                }            
            }
            otCliOutputFormat("\nPEER IP\n");
            for (c = 0; c < 16; c++){
                otCliOutputFormat(itoa((int)otUdpGetSockets(instance)->mPeerName.mAddress.mFields.m8[c], buffer_2, 16));
                if (c%2){
                    otCliOutputFormat(":");
                }            
            }
            otCliOutputFormat("\n");
            
            const char *buf = "Hola";
            otMessageInfo messageInfo;
    
            memset(&messageInfo, 0, sizeof(messageInfo));
            messageInfo.mSockPort = socket->mSockName.mPort;
            messageInfo.mPeerPort = socket->mPeerName.mPort;
            messageInfo.mSockAddr = socket->mSockName.mAddress;
            messageInfo.mPeerAddr = socket->mPeerName.mAddress;
            //messageInfo.mInterfaceId = OT_NETIF_INTERFACE_ID_THREAD;

            otMessage *msg = otUdpNewMessage(instance, NULL);
            otCliOutputFormat(otThreadErrorToString(otMessageAppend(msg, buf, (uint16_t)strlen(buf))));

            error = otUdpSend(socket, msg, &messageInfo);

            if(error != OT_ERROR_NONE && msg != NULL)
            {
                otMessageFree(msg);
            }
        }
        else{
            //LED3_OFF;
            //otCliOutputFormat("LED OFF!\r\n");
        }

    }



    otInstanceFinalize(instance);
#if OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
    free(otInstanceBuffer);
#endif

    goto pseudo_reset;

    return 0;
}

/*
 * Provide, if required an "otPlatLog()" function
 */
#if OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_APP
void otPlatLog(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, ...)
{
    OT_UNUSED_VARIABLE(aLogLevel);
    OT_UNUSED_VARIABLE(aLogRegion);
    OT_UNUSED_VARIABLE(aFormat);

    va_list ap;
    va_start(ap, aFormat);
    otCliPlatLogv(aLogLevel, aLogRegion, aFormat, ap);
    va_end(ap);
}
#endif
