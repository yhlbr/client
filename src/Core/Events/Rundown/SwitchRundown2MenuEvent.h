#pragma once

#include "../../Shared.h"

class CORE_EXPORT SwitchRundown2MenuEvent
{
    public:
        explicit SwitchRundown2MenuEvent(bool enabled);

        bool getEnabled() const;

    private:
        bool enabled;
};
