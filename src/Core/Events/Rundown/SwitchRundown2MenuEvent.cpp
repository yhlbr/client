#include "SwitchRundown2MenuEvent.h"

#include "Global.h"

SwitchRundown2MenuEvent::SwitchRundown2MenuEvent(bool enabled)
    : enabled(enabled)
{
}

bool SwitchRundown2MenuEvent::getEnabled() const
{
    return this->enabled;
}
