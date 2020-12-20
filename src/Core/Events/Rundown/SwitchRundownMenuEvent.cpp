#include "SwitchRundownMenuEvent.h"

#include "Global.h"

SwitchRundownMenuEvent::SwitchRundownMenuEvent(bool enabled)
    : enabled(enabled)
{
}

bool SwitchRundownMenuEvent::getEnabled() const
{
    return this->enabled;
}
