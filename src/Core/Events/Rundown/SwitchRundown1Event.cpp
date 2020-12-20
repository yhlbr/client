#include "SwitchRundown1Event.h"

#include "Global.h"

SwitchRundown1Event::SwitchRundown1Event(const QString& path)
    : path(path)
{
}

const QString& SwitchRundown1Event::getPath() const
{
    return this->path;
}
