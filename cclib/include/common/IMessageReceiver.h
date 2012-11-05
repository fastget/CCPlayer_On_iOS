#ifndef IMESSAGERECEIVER_H
#define IMESSAGERECEIVER_H

#include "Event.h"
#include "SmartPtr.h"

namespace CCPlayer
{

class IMessageReceiver
{
public:
    virtual void ReceiverMessage(const SmartPtr<Event>& rSmtEvent) = 0;
};

}
#endif
