/* The MIT License
 * 
 * vnl - verilog netlist
 * Copyright (c) 2006-2010  Karl W. Pfalzer
 * Copyright (c) 2012-      George P. Burdell
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
#include <ctime>
#include <map>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include "vnl/writer.hxx"
#include "vnl/message.hxx"
#include "vnl/module.hxx"
//#include "xyzzy/array.hxx"
//#include "writer.hxx"
//#include "port.hxx"
//#include "cell.hxx"
//#include "pin.hxx"
//#include "message.hxx"

//Similar to algorithm::for_each
#define FOREACH(_t, _trc, _func)                                                      \
{       for (_t::const_iterator i = _trc->begin(); i != _trc->end(); ++i) \
                _func(*i);                                                        \
}

//Similar to algorithm::for_each
#define FOREACH_VAL_OF_MAP(_t, _trc, _func)                               \
{       for (_t::const_iterator i = _trc->begin(); i != _trc->end(); ++i) \
                _func(i->second);                                                        \
}

namespace vnl {
    using namespace std;

    static
    unsigned
    abs(int x) {
        return (0 <= x) ? x : -x;
    }

    static
    string
    prefix(unsigned n) {
        string r = "";
        for (unsigned i = 0; i < n; i++) {
            r += ' ';
        }
        return r;
    }

    static
    string
    currentTime() {
        time_t rawtime;
        struct tm * timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        string s = asctime(timeinfo);
        return s;
    }

    Writer::Config::Config(bool ansiPortDecl,
            bool topModuleLast,
            bool instTopological,
            unsigned portDeclIndent,
            unsigned instDeclIndent,
            unsigned instDeclBusConnIndent,
            unsigned wireDefnIndent,
            unsigned compressBusConnExpr,
            bool libCellStub,
            unsigned maxCharsPerLine,
            bool allowUnresolved
            ) :
    m_ansiPortDecl(ansiPortDecl),
    m_topModuleLast(topModuleLast),
    m_instTopological(instTopological),
    m_libCellStub(libCellStub),
    m_portDeclIndent(portDeclIndent),
    m_instDeclIndent(instDeclIndent),
    m_instDeclBusConnIndent(instDeclBusConnIndent),
    m_wireDefnIndent(wireDefnIndent),
    m_compressBusConnExpr(compressBusConnExpr),
    m_maxCharsPerLine(maxCharsPerLine),
    m_allowUnresolved(allowUnresolved) {
    }

    class Writer::Impl {
    public:

        explicit Impl(const string &ofn, const TRcConfig &cfg) throw (bool)
        : m_config(cfg), m_ofname(ofn), m_lcnt(0), m_pfx(cNull) {
            m_os.open(m_ofname.c_str());
            if (m_os.fail()) {
                error("VNL-FILE-2", m_ofname, "write");
                throw (true);
            }
        }

        Impl& print(const string &s) {
            unsigned curr = m_oss.str().size();
            if (0 < curr) curr += m_pfx.size();
            const unsigned newTl = curr + s.size();
            if ((0 < curr) && (newTl > m_config->m_maxCharsPerLine)) {
                flush();
                curr = 0;
            }
            if (0 == curr) {
                m_oss << m_pfx;
            }
            m_oss << s;
            return *this;
        }

        Impl& print(ostringstream &os) {
            print(os.str());
            os.str(cNull);
            return *this;
        }

        Impl& println(const string &s) {
            print(s).flush();//nl();
            return *this;
        }

        Impl& println(ostringstream &os) {
            println(os.str());
            os.str(cNull);
            return *this;
        }

        Impl& println() {
            return println(cNull);
        }

        Impl& setPrefix(unsigned n) {
            m_pfx = prefix(n);
            return *this;
        }

        ~Impl() {
            flush();
            m_os.close();
        }

        Impl& flush() {
            if (!m_oss.str().empty()) {
                m_os << m_oss.str();
                nl();
            }
            m_os.flush();
            return *this;
        }

        const TRcConfig m_config;
        const string m_ofname;
        t_lineCnt m_lcnt;

    private:
        friend class Writer;

        static const string cNull;

        ostringstream m_oss;
        ofstream m_os;
        string m_pfx;

        Impl& nl() {
            m_os << endl;
            m_lcnt++;
            m_oss.str(cNull);
            return *this;            
        }

        typedef map<string, TRcModule> t_modsByName;
        typedef vector<TRcModule> t_mods;
        typedef PTRcPtr<t_mods> trc_mods;

        static
        bool
        compare(const TRcModule &i, const TRcModule &j) {
            return (i->getName() < j->getName()); //(i<j); 
        }

        trc_mods
        getModuleDeclOrder(const TRcModule &top, bool doHier) throw (bool) {
            t_modsByName done;
            getUsedModules(top, done, doHier, true);
            trc_mods mods = new t_mods();
            for (t_modsByName::const_iterator i = done.begin(); i != done.end(); ++i) {
                mods->push_back(i->second);
            }
            sort(mods->begin(), mods->end(), compare);
            if (m_config->m_topModuleLast) {
                mods->push_back(top);
            } else {
                mods->insert(mods->begin(), top);
            }
            return mods;
        }

        void
        getUsedModules(const TRcModule &top, t_modsByName &done,
                bool doHier, bool skipTop = false) throw (bool) {
            const Module::t_cellsByName &cellsByName = top->getCellsByName();
            const string &topnm = top->getName();
            if (mapHasKey(done, topnm)) return;
            if (!skipTop) {
                done[topnm] = top;
            }
            Module::t_cellsByName::const_iterator i = cellsByName.begin();
            for (; i != cellsByName.end(); ++i) {
                const TRcCell &inst = i->second;
                const string &refnm = inst->getRefName();
                if (!m_config->m_allowUnresolved && !inst->isResolved()) {
                    error("VNL-REF-1", top->getName(), inst->getInstName(), refnm);
                    throw (true);
                }
                if (inst->isResolved() && !mapHasKey(done, refnm)) {
                    const TRcObject &ref = inst->getRef();
                    const TRcModule asMod = Module::downcast(ref);
                    if (m_config->m_libCellStub || !asMod->isLibCell()) {
                        done[refnm] = asMod;
                        if (doHier && !asMod->isLibCell()) {
                            getUsedModules(asMod, done, doHier);
                        }
                    }
                }
            }
        }

        Impl&
        writePorts(const TRcModule &mod, bool defn = false) {
            if (defn && m_config->m_ansiPortDecl) return *this;
            typedef Module::t_ports t_ports;
            t_ports portsInDeclOrder;
            mod->getPortsInDeclOrder(portsInDeclOrder);
            if (portsInDeclOrder.empty()) return *this;
            bool isFirst = true;
            ostringstream os;
            for (t_ports::const_iterator i = portsInDeclOrder.begin();
                    i != portsInDeclOrder.end(); ++i, isFirst = false) {
                const TRcPort &port = *i;
                if (!defn) os << (isFirst ? " " : ",");
                if (defn || m_config->m_ansiPortDecl) {
                    os << port << " ";
                } else {
                    os << port->getName() << " ";
                }
                if (defn) {
                    os << ';';
                    println(os);
                } else {
                    print(os);
                }
            }
            return *this;
        }

        Impl&
        writeDecl(const TRcModule &mod) {
            ostringstream oss;
            oss << "module " << mod->getName() << " (";
            print(oss);
            setPrefix(m_config->m_portDeclIndent);
            writePorts(mod).setPrefix(0).println(");");
            setPrefix(m_config->m_wireDefnIndent);
            if (! m_config->m_ansiPortDecl) {
                writePorts(mod, true);
            }
            typedef Module::t_wiresByName t_wiresByName;
            //TODO: assign stmts
            const t_wiresByName &wires = mod->getWiresByName();
            for (t_wiresByName::const_iterator i = wires.begin();
                    i != wires.end(); ++i) {
                const TRcWire &w = i->second;
                if (w->isPort() || w->isConst()) continue;
                oss << w << " ;";
                println(oss);
            }
            setPrefix(0).println("endmodule");
            return *this;
        }

        Impl&
        write(const TRcModule &mod) {
            writeDecl(mod);
            //TODO
            return *this;
        }

#ifdef NOPE

        static void writePorts(ostream &os, TRcCollection ports, bool defn = false) {
            if (defn && Writer::stConfig.m_ansiPortDecl) return;
            PortDone portsDone;
            TRcPort port;
            bool isFirst = true;
            string nm;
            for (CollectionIter iter(*ports); iter.hasMore(); ++iter) {
                TRcBus asBus;
                port = toPort(*iter);
                if (port->isBus()) {
                    asBus = port->getRange();
                    nm = asBus->m_name;
                } else {
                    nm = port->getName();
                }
                if (portsDone.find(nm) == portsDone.end()) {
                    if (defn) {
                        os << stWireDefnPfx;
                    } else {
                        os << stPortDeclPfx << (isFirst ? ' ' : ',');
                    }
                    write(os, nm, port->getDirection(), asBus, defn || Writer::stConfig.m_ansiPortDecl);
                    if (defn) {
                        os << " ;";
                    }
                    nl(os);
                    isFirst = false;
                    portsDone[nm] = true;
                }
            }
        }

        static void writeWires(ostream &os, TRcCollection wires) {
            typedef map<string, bool> WireDone;
            WireDone wiresDone;
            TRcNet net;
            string nm;
            for (CollectionIter iter(*wires); iter.hasMore(); ++iter) {
                TRcBus asBus;
                net = toNet(*iter);
                if (net->isBus()) {
                    asBus = net->getRange();
                    nm = asBus->m_name;
                } else {
                    nm = net->getName();
                }
                if (wiresDone.find(nm) == wiresDone.end()) {
                    os << stWireDefnPfx << "wire ";
                    write(os, nm, Port::eInvalid, asBus, true);
                    os << " ;";
                    nl(os);
                    wiresDone[nm] = true;
                }
            }
        }

        static void writeWires(ostream &os, TRcDesign mod) {
            //Collect wires which are not ports
            TRcCollection justWires = new Collection();
            TRcCollection allWires = mod->getNets();
            TRcNet net;
            for (CollectionIter iter(*allWires); iter.hasMore(); ++iter) {
                net = toNet(*iter);
                if (!net->isLogic01() && !mod->hasPort(net->getName())) {
                    justWires->add(upcast(net));
                }
            }
            writeWires(os, justWires);
        }

        static void writeAssigns(ostream &os, TRcDesign mod) {
            TRcCollection ports = mod->getPorts(); //in decl order
            TRcCollection portsOnNet;
            TRcPort port, port2;
            TRcNet asNet;
            for (CollectionIter iter(*ports); iter.hasMore(); ++iter) {
                port = toPort(*iter);
                portsOnNet = port->getNet()->getOnNet(ana::Net::ePorts, false, false);
                for (CollectionIter iter2(*portsOnNet); iter2.hasMore(); ++iter2) {
                    port2 = toPort(*iter2);
                    if ((port2->getDirection() != ana::Port::eIn)
                            && (port2->getName() != port->getName())) {
                        os << stInstDeclPfx << "assign " << port2->getName() << " = "
                                << port->getName() << " ;";
                        nl(os);
                    }
                }
            }
        }

        static void writeDecl(ostream &os, TRcDesign mod) {
            TRcCollection ports = mod->getPorts(); //in decl order
            os << "module " << mod->getName() << " (";
            nl(os);
            writePorts(os, ports);
            os << ");";
            nl(os);
            writePorts(os, ports, true); //port definitions
        }

        static void writeMod(ostream &os, TRcDesign mod) {
            writeDecl(os, mod);
            writeWires(os, mod);
            writeAssigns(os, mod);
            writeInsts(os, mod);
            os << "endmodule";
            nl(os, 2);
        }
#endif //NOPE
    };

    //static 
    const string Writer::Impl::cNull = "";

    //static
    Writer::TRcConfig Writer::stDefaultConfig = new Writer::Config();

    Writer::Writer(const string &fname, const TRcConfig &cfg) throw (bool)
    : mp_impl(new Impl(fname, cfg)) {
    }

    Writer::t_lineCnt
    Writer::write(const TRcModule &top, bool doHier) throw (bool) {
        mp_impl->println("// Created: " + currentTime());
        Impl::trc_mods modOrder = mp_impl->getModuleDeclOrder(top, doHier);
        FOREACH(Impl::t_mods, modOrder, mp_impl->write);
        //TODO: print module
        mp_impl->flush();
        return mp_impl->m_lcnt;
    }

    Writer::~Writer() {
        delete mp_impl;
    }
}
