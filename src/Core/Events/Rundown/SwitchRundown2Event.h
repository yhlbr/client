#pragma once

#include "../../Shared.h"

#include <QtCore/QString>

class CORE_EXPORT SwitchRundown2Event
{
    public:
        explicit SwitchRundown2Event(const QString& path = "");

        const QString& getPath() const;

    private:
        QString path;
};
