/* The MIT License
 * 
 * vnl - verilog netlist
 * Copyright (c) 2006-2010  Karl W. Pfalzer
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
#include "vnl/pinref.hxx"
#include "vnl/wire.hxx"
#include "vnl/wirebitref.hxx"
#include "vnl/port.hxx"
#include "vnl/module.hxx"
#include "vnl/cell.hxx"

namespace vnl {
    static string stTypeName = "cell";
    const unsigned Cell::stTypeId = Object::getNextTypeId(stTypeName);

    Cell::Cell(string refNm, string instNm, TRcConnsByPortName &conns)
    : m_refNm(refNm), m_instNm(instNm), m_connsByPortName(conns) {
    }

    Cell::~Cell() {
    }

    TRcModule
    Cell::getRef() const {
        TRcModule mod;
        if (m_ref.isValid()) {
            mod = Module::downcast(m_ref);
        }
        return mod;
    }

    bool
    Cell::isHierarchical() const {
        bool hier = false;
        TRcModule mod = getRef();
        if (mod.isValid()) {
            hier = mod->isHierarchical();
        }
        return hier;
    }

    bool
    Cell::isLibCell() const {
        bool lib = false;
        TRcModule mod = getRef();
        if (mod.isValid()) {
            lib = mod->isLibCell();
        }
        return lib;
    }

    /**
     * Add PinRef entries to wires.
     */
    void
    Cell::updateWires(TRcCell &cell) {
        unsigned n;
        string portNm;
        TRcPinRef pinRef;
        TRcObject wireObj;
        TConnsByPortName::iterator iter = cell->m_connsByPortName->begin();
        for (; iter != cell->m_connsByPortName->end(); ++iter) {
            TBitConns &conns = iter->second;
            n = conns.length();
            if (0 == n) continue;
            portNm = iter->first;
            for (unsigned i = 0; i < n; i++) {
                wireObj = conns[i];
                if (wireObj.isNull()) continue;
                pinRef = new PinRef(cell, portNm, i);
                if (wireObj->isType(Wire::stTypeId)) {
                    TRcWire wire = Wire::downcast(wireObj);
                    wire->add(pinRef);
                } else if (wireObj->isType(Port::stTypeId)) {
                    TRcPort port = Port::downcast(wireObj);
                    port->add(pinRef);
                } else {
                    ASSERT_TRUE(wireObj->isType(WireBitRef::stTypeId));
                    TRcWireBitRef wireBitRef = WireBitRef::downcast(wireObj);
                    wireBitRef->add(pinRef);
                }
            }
        }
    }
}

