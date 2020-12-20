#pragma once

#include "../../Shared.h"

#include <QtCore/QString>

class CORE_EXPORT SwitchRundownEvent
{
    public:
        explicit SwitchRundownEvent(const QString& path = "");

        const QString& getPath() const;

    private:
        QString path;
};
