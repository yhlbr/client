#include "SwitchRundown2Event.h"

#include "Global.h"

SwitchRundown2Event::SwitchRundown2Event(const QString& path)
    : path(path)
{
}

const QString& SwitchRundown2Event::getPath() const
{
    return this->path;
}
