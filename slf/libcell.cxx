/* The MIT License
 * 
 * slf - verilog netlist
 * Copyright (c) 2012       George P. Burdell
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "vnl/port.hxx"
#include "vnl/portbus.hxx"
#include "slf/libcell.hxx"

namespace slf {
    using vnl::Port;
    using vnl::PortBus;
    using vnl::TRcPort;
    using vnl::TRcPortBus;
    
    static const string cRelatedPin = "related_pin";
    static const string cTimingSense = "timing_sense";

    static
    Port::EDirection
    getDirection(const string &dir) {
        Port::EDirection pdir = Port::eUnknown;
        if (dir == "input") {
            pdir = Port::eIn;
        } else if (dir == "output") {
            pdir = Port::eOut;
        } else {
            pdir = Port::eInout;
        }
        return pdir;
    }

    void
    LibCell::addPort(const string &nm, const TRcBus &bus, const string &direction) {
        Port::EDirection dir = getDirection(direction);
        vnl::TRcBus vbus = new vnl::Bus(bus->getMsb(), bus->getLsb());
        TRcPortBus pbus = new PortBus(nm, dir, vbus);
        appendPortDecl(pbus);
    }

    void
    LibCell::addPort(const string &nm, const string &direction) {
        Port::EDirection dir = getDirection(direction);
        TRcPort port = new Port(nm, dir);
        appendPortDecl(port);
    }

    void
    LibCell::addTiming(const string &opin, const trc_byOneKey &timinfo) {
        if (timinfo.isNull()) return;
        TRcKeyValue inKv, unateKv;
        for (unsigned i = 0; i < timinfo->size(); i++) {
            const TRcKeyValue &kv = timinfo->at(i);
            if (! kv->hasValSet()) continue;
#ifdef xDEBUG
            dbgOs << "DBG: addTiming {" << std::endl;
            dbgOs << kv->getValSet() << " }" << std::endl;
#endif
            //allowDups in asMap for
            //pin group statements (see sec 3.2 in Liberty Spec)
            ValueSet::trc_kvByKey rcvbk = kv->getValSet()->asMap(true);
            if (mapGetVal(rcvbk, cRelatedPin, inKv) && 
                    mapGetVal(rcvbk, cTimingSense, unateKv)) {
                string in = inKv->getVal()->asIdent();
                string unate = unateKv->getVal()->asIdent();
                m_unate.addRelation(in, opin, unate);
            }
        }
    }

    LibCell::~LibCell() {
    }
}
