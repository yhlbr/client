#include "SwitchRundown1MenuEvent.h"

#include "Global.h"

SwitchRundown1MenuEvent::SwitchRundown1MenuEvent(bool enabled)
    : enabled(enabled)
{
}

bool SwitchRundown1MenuEvent::getEnabled() const
{
    return this->enabled;
}
