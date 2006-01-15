/*
 *  Copyright (c) 2006 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef KROSS_KRITACOREKRS_KERNEL_H
#define KROSS_KRITACOREKRS_KERNEL_H

#include <api/class.h>


class KisKernel;

namespace Kross {

namespace KritaCore {

class Kernel : public Kross::Api::Class<Kernel>
{
    public:
        Kernel();
       ~Kernel();
    public:
        KisKernel* kernel() { return m_kernel; }
    private:
        KisKernel* m_kernel;
};

}

}

#endif
