// Copyright © 2016 Canonical Ltd.
//
// This file is part of Quick+.
//
// Quick+ is free software: you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; version 3.
//
// Quick+ is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Quick+. If not, see <http://www.gnu.org/licenses/>.

#ifndef LTTNGLOGGINGDEVICE_H
#define LTTNGLOGGINGDEVICE_H

#include "quickplusglobal.h"
#include "loggingdevice.h"

//
class QUICK_PLUS_EXPORT QuickPlusLTTNGLoggingDevice : public QuickPlusLoggingDevice
{
public:
    QuickPlusLTTNGLoggingDevice();
    ~QuickPlusLTTNGLoggingDevice();

    void log(const Counters &counters) override;

private:
    void* m_lttngLib;
    bool m_lttngLibLoadTried;
};

#endif
