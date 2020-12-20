#include "SwitchRundownEvent.h"

#include "Global.h"

SwitchRundownEvent::SwitchRundownEvent(const QString& path)
    : path(path)
{
}

const QString& SwitchRundownEvent::getPath() const
{
    return this->path;
}
