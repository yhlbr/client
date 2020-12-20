#pragma once

#include "../../Shared.h"

#include <QtCore/QString>

class CORE_EXPORT SwitchRundown1Event
{
    public:
        explicit SwitchRundown1Event(const QString& path = "");

        const QString& getPath() const;

    private:
        QString path;
};
