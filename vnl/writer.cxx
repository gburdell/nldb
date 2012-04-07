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
#include <deque>
#include "xyzzy/util.hxx"
#include "vnl/writer.hxx"
#include "vnl/message.hxx"
#include "vnl/module.hxx"
#include "vnl/wirebitref.hxx"
#include "vnl/asgnrhs.hxx"
//#include "xyzzy/array.hxx"
//#include "writer.hxx"
//#include "port.hxx"
//#include "cell.hxx"
//#include "pin.hxx"
//#include "message.hxx"

namespace vnl {
    using namespace std;
    using xyzzy::mapHasKey;
    using xyzzy::mapGetVal;

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
            print(s).flush(); //nl();
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
        typedef Module::t_wiresByName t_wiresByName;
        typedef Module::t_cellsByName t_cellsByName;

        ///Build up assign as lhs,rhs vector
        typedef vector<TRcObject> t_asgnEle;
        typedef pair<t_asgnEle, t_asgnEle> t_lhsRhs;
        typedef PTRcPtr<t_lhsRhs> trc_lhsRhs;
        typedef map<string, trc_lhsRhs> t_asgnByLhsName;

        static
        bool
        compare(const TRcModule &i, const TRcModule &j) {
            return (i->getName() < j->getName()); //(i<j); 
        }

        /**
         * Replace series of tail (1'b[01])+ to n'bxxx.
         * @param vals ...(1'b[01])+
         */
        static
        void
        replBitVal(deque<string> &vals) {
            //roll-em off into n'bxxx
            string peek;
            string nTicVal;
            while (!vals.empty()) {
                peek = vals.back(); //peek last
                if ((4 == peek.length()) && (peek.substr(0, 3) == "1'b")) {
                    nTicVal = peek.at(3) + nTicVal; //prepend
                    vals.pop_back();
                } else {
                    break; //while
                }
            }
            unsigned n = nTicVal.size();
            ASSERT_TRUE(0 < n);
            ostringstream os;
            os << n << "'b" << nTicVal;
            vals.push_back(os.str());
        }

        /**
         * Examine currEle (a WireBitRef).
         * If is next bit in subrange, return false.
         * Else, reduce existing elements and return true.
         * @param expr vector of elements, where expr[startI...currI]
         * contain WireBitRef.
         * @param rval push reduced element here, if necessary.
         * @param startI index into expr of 1st element of subrange.
         * @param currI index into expr of current element of subrange.
         * @pram force force reduction.
         * @return true if expr[currI] not in subrange, and reduce subrange and
         * push result onto rval.
         */
        static
        bool
        reduceBitVec(const t_asgnEle &expr, deque<string> &rval,
                const unsigned startI, const unsigned currI, bool force = false) {
            ASSERT_TRUE(startI <= currI);
            bool reduce = force || (startI == currI);
            unsigned endI = currI;
            if (!reduce) {
                //consecutive bits?
                const TRcWireBitRef &prev = WireBitRef::downcast(expr[currI - 1]);
                const TRcWireBitRef &curr = WireBitRef::downcast(expr[currI]);
                reduce = (prev->getBusName() != curr->getBusName());
                int delta = curr->getBitIx() - prev->getBitIx();
                reduce |= (delta != curr->getBusIncr());
                if (reduce) {
                    endI--; //dont include curr bit
                }
            }
            if (reduce) {
                const TRcWireBitRef &start = WireBitRef::downcast(expr[startI]);
                const TRcWireBitRef &end = WireBitRef::downcast(expr[endI]);
                const unsigned ixs[2] = {start->getBitIx(), end->getBitIx()};
                ostringstream os;
                os << start->getBusName();
                if ((ixs[0] != start->getBus()->getLb()) ||
                        (ixs[1] != start->getBus()->getRb())) {
                    os << "[" << ixs[0];
                    if (ixs[1] != ixs[0]) {
                        os << ":" << ixs[1];
                    }
                    os << "]";
                }
                string s = os.str();
                rval.push_back(s);
            }
            return reduce;
        }

        /*
         * Compress {...} expression.
         * 4 cases:
         * ========
         * 1) 1'b[01] ...
         * 2) foo,foo,... => {n{foo}}  (iff compression setting >1)
         * 3) bar[n],bar[n-1],bar[n-2],... => bar[n:n-m] (iff bit order ok)
         * 4) a,b,c,... => a,b,c,...
         */
        Impl&
        compress(const t_asgnEle &expr, vector<string> &rval) {
            if (expr.empty()) return *this;
            static const TRcWire nilw = new Wire("");
            static const TRcObject nilo = upcast(nilw);
            /* push the bogus entry on so we can iterate w/o worrying about
             * the end condition.
             */
            const_cast<t_asgnEle&> (expr).push_back(nilo);

            enum CompressState {
                eWait, eTic01, eBit
            };
            CompressState state = eWait;
            deque<string> tmpRval;
            string curr;
            unsigned spanStartI = 0;
            unsigned typeId;
            TRcWire wire;
            for (unsigned i = 0; i < expr.size(); ++i) {
                const TRcObject &ele = expr[i];
                curr = ele->getName();
                if (0 == m_config->m_compressBusConnExpr) {
                    //keep bit blasted
                    tmpRval.push_back(curr);
                    continue;
                }
                typeId = ele->getTypeId(); //Wire, Port, WireBitRef
                if (WireBitRef::stTypeId != typeId) {
                    wire = Wire::downcast(ele);
                }
                switch (state) {
                    case eWait:
                        if (WireBitRef::stTypeId == typeId) {
                            state = eBit;
                            spanStartI = i; //mark start in expr[]
                        } else {
                            tmpRval.push_back(curr);
                            if (wire->isConst()) {
                                state = eTic01;
                            }
                            //else state=eWait;
                        }
                        break;
                    case eTic01:
                        //at least 1 1'b[01] is on tail
                        if ((WireBitRef::stTypeId == typeId) || !wire->isConst()) {
                            //not a bit, so crunch existing n'bxxx
                            replBitVal(tmpRval);
                            if (WireBitRef::stTypeId == typeId) {
                                state = eBit;
                                spanStartI = i;
                            } else {
                                tmpRval.push_back(curr);
                                state = eWait;
                            }
                        } else {
                            ASSERT_TRUE(wire->isConst());
                            tmpRval.push_back(curr);
                            //state=eTic01;
                        }
                        break;
                    case eBit:
                        if (WireBitRef::stTypeId == typeId) {
                            if (reduceBitVec(expr, tmpRval, spanStartI, i)) {
                                spanStartI = i; //reset to this bit
                            }
                            // else state=eBit
                        } else {
                            //not a bit, so reduce [spanStarI,i-1]
                            ASSERT_TRUE(reduceBitVec(expr, tmpRval, spanStartI, i - 1, true));
                            tmpRval.push_back(curr);
                            state = (wire->isConst()) ? eTic01 : eWait;
                        }
                        break;
                    default:
                        ASSERT_NEVER;
                }
            }
            //Remove bogus entries: remove nilo
            ASSERT_TRUE(tmpRval.back().empty()); //nilo is ""
            tmpRval.pop_back();
            const_cast<t_asgnEle&> (expr).pop_back();
            //
            rval.clear();
            if (1 < m_config->m_compressBusConnExpr) {
                //push bogus to simplify end condition handling
                static const string nil = "";
                tmpRval.push_back(nil);
                //handle repeated
                unsigned n = 1;
                string last;
                for (unsigned i = 0; i < tmpRval.size(); ++i) {
                    string vi = tmpRval[i];
                    if (0 == i) {
                        last = vi;
                    } else {
                        const bool match = (last == vi);
                        if (match) {
                            n++;
                        } else {
                            ostringstream os;
                            if (1 < n) {
                                os << '{' << n << '{' << last << "}}";
                            } else {
                                os << last;
                            }
                            string s = os.str();
                            rval.push_back(s);
                            if (!match) {
                                last = vi;
                                n = 1;
                            }
                        }
                    }
                }
                ASSERT_TRUE(!rval.back().empty()); //make sure nil didnt slip by
            } else {
                FOREACH(deque<string>::const_iterator, tmpRval.begin(), tmpRval.end(), rval.push_back);
            }
            return *this;
        }

#ifdef NOPE

        static void writeBusPinConn(ostream &os, TRcBus bus, CollectionIter &iter) {
            //make sure dont ++iter on last, since caller does.
            map<int, TRcBit> eles;
            TRcNet net;
            TRcPin pin;
            TRcPort port;
            TRcBit portBit;
            const unsigned n = bus->size();
            unsigned validCnt = 0;
            for (unsigned i = n; 0 < i; i--) {
                pin = toPin(*iter);
                port = pin->getPort();
                { //check:
                    portBit = port->getBit();
                    ASSERT_TRUE(bus->inRange(portBit->m_bit));
                    ASSERT_TRUE(bus->m_name == portBit->m_name);
                }
                net = pin->getNet();
                TRcBit bit; //allow null entries
                if (net.isValid() && net->isAlive()) {
                    bit = net->asBit();
                    validCnt++;
                }
                eles[portBit->m_bit] = bit;
                if (1 < i) {
                    ++iter; //dont incr at last, since ++iter upon return
                }
            }
            if (1 > validCnt) return;
            if (0 < Writer::stConfig.m_compressBusConnExpr) {
                vector<string> conns;
                compress(eles, bus, conns);
                vector<string>::const_iterator iter = conns.begin();
                if (1 == conns.size()) {
                    os << *iter;
                } else {
                    os << '{';
                    nl(os);
                    for (; iter != conns.end(); ++iter) {
                        os << stInstDeclBusConnIndent << ((iter != conns.begin()) ? ',' : ' ')
                                << *iter;
                        nl(os);
                    }
                    os << stInstDeclPfx << '}';
                }

            } else {
                os << '{';
                nl(os);
                TRcBit bit;
                //left->right sweep
                for (unsigned cnt = 0, i = bus->m_range.first; n > cnt;
                        cnt++, i += bus->incr()) {
                    os << stInstDeclBusConnIndent << ((0 < cnt) ? ',' : ' ');
                    bit = eles[i];
                    if (bit.isValid()) {
                        os << bit;
                    }
                    nl(os);
                }
                os << stInstDeclPfx << '}';
            }
        }

        static void writeInst(ostream &os, TRcCell inst) {
            TRcDesign ref = inst->getRef();
            os << stInstDeclPfx << ref->getName() << " "
                    << inst->getName() << " (";
            nl(os);
            TRcCollection pins = sortByName(inst->getPins(inst));
            PortDone portsDone;
            TRcPin pin;
            TRcPort port;
            TRcNet net;
            TRcBit bit;
            bool isFirst = true;
            string nm;
            for (CollectionIter iter(*pins); iter.hasMore(); ++iter) {
                TRcBus asBus;
                pin = toPin(*iter);
                port = pin->getPort();
#ifdef VRFC_BUG_7012
                std::cout << "\nDBG: pin: " << inst->getName() << "." << port->getName() << std::endl;
#endif
                if (port->isBus()) {
                    asBus = port->getRange();
                    nm = asBus->m_name;
                } else {
                    nm = port->getName();
                }
                if (portsDone.find(nm) == portsDone.end()) {
                    os << stInstDeclPfx << (isFirst ? ' ' : ',')
                            << '.' << nm + "(";
                    if (asBus.isValid()) {
                        writeBusPinConn(os, asBus, iter);
                    } else {
                        net = pin->getNet();
                        //TODO: where do the null nets come from, requiring isAlive()
                        if (net.isValid() && net->isAlive()) {
                            bit = net->asBit();
                            os << bit;
                        }
                    }
                    os << ')';
                    nl(os);
                    isFirst = false;
                    portsDone[nm] = true;
                }
            }
            os << stInstDeclPfx << ");";
            nl(os);
        }
#endif

        Impl&
        writeInst(const TRcCell &cell) {
            print(cell->getRefName()).print(" ").print(cell->getInstName())
                    .print("(");
            //TODO
            println(");");
        }

        Impl&
        writeInsts(const TRcModule &mod) {
            const t_cellsByName &cells = mod->getCellsByName();
            if (cells.empty()) return *this;
            setPrefix(m_config->m_instDeclIndent);
            println("//BEGIN{{ instances (libcells, then rest)");
            for (int i = 0; i < 2; i++) { //0=leaf; 1=hier
                //iterate in instance name (sorted) order?
                for (t_cellsByName::const_iterator iter = cells.begin();
                        iter != cells.end(); ++iter) {
                    const TRcCell &inst = iter->second;
                    //libcells then not libcells
                    if ((0 == i && inst->isLibCell()) || (1 == i && !inst->isLibCell())) {
                        writeInst(inst);
                    }
                }
            }
            println("//instances }}END");
            return *this;
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
                    const TRcModule asMod = inst->getRef();
                    if (m_config->m_libCellStub || !asMod->isLibCell()) {
                        done[refnm] = asMod;
                        if (doHier && !asMod->isLibCell()) {
                            getUsedModules(asMod, done, doHier);
                        }
                    }
                }
            }
        }

        Impl &
        writePorts(const TRcModule &mod, bool defn = false) {
            if (defn && m_config->m_ansiPortDecl) return *this;
            typedef Module::t_ports t_ports;
            t_ports portsInDeclOrder;
            mod->getPortsInDeclOrder(portsInDeclOrder);
            if (portsInDeclOrder.empty()) return *this;
            if (defn) println("//BEGIN{{ port declarations");
            bool isFirst = true;
            ostringstream os;
            for (t_ports::const_iterator i = portsInDeclOrder.begin();
                    i != portsInDeclOrder.end(); ++i, isFirst = false) {
                const TRcPort &port = *i;
                if (!defn) os << (isFirst ? "" : ",");
                if (defn || m_config->m_ansiPortDecl) {
                    os << port; //<< " ";
                } else {
                    os << port->getName(); //<< " ";
                }
                if (defn) {
                    os << ';';
                    println(os);
                } else {
                    print(os);
                }
            }
            if (defn) println("//port declarations }}END");
            return *this;
        }

        static
        void
        updateAssigns(t_asgnByLhsName &lhsByNm, const string &lhsNm,
                const TRcObject &lhs, const TRcConnList & conns) {
            if (conns.isNull()) return;
            trc_lhsRhs entry;
            for (TConnList::const_iterator i = conns->begin(); i != conns->end(); ++i) {
                const TRcObject &rhs = *i;
                //we're only interested in rhs type AsgnRhs
                if (rhs->isType(AsgnRhs::stTypeId)) {
                    if (!mapGetVal(lhsByNm, lhsNm, entry)) {

                        entry = new t_lhsRhs();
                        lhsByNm[lhsNm] = entry;
                    }
                    const TRcAsgnRhs asgnRhs = AsgnRhs::downcast(rhs);
                    entry->first.push_back(lhs);
                    entry->second.push_back(asgnRhs->getEle());
                    /* 
                     * TODO: in theory, could have:
                     *  assign {w1,w1} = {w2,w3}
                     * which models both w2,w3 drivers to w1.
                     * Will need to handle this case (by having map of list<t_lhsRhs>)
                     */
                    ASSERT_TRUE(2 > conns->size());
                }
            }
        }

        static
        void
        updateAssigns(t_asgnByLhsName &lhsByNm, const TRcWire & lhs) { //lhs scalar
            const TRcObject lhso = upcast(lhs);
            updateAssigns(lhsByNm, lhs->getName(), lhso, lhs->getConns());
        }

        static
        void
        updateAssigns(t_asgnByLhsName &lhsByNm, const TRcWireBus & lhs) { //lhs bus
            //ok to cast away const, since we're not gonna modify anyway
            TRcConnList lhsBits = WireBitRef::bitBlast(const_cast<TRcWireBus&> (lhs));
            //iterate lhs[msb->lsb]
            for (TConnList::const_iterator iter(lhsBits->begin()); iter != lhsBits->end(); ++iter) {
                const TRcWireBitRef lhsBit = WireBitRef::downcast(*iter);
                const TRcObject lhso = upcast(lhsBit);
                updateAssigns(lhsByNm, lhs->getName(), lhso, lhsBit->getConns());
            }
        }

        /**
         * Write concatenation.
         * @param vals values.
         * @return this.
         */
        Impl&
        write(const vector<string> &vals) {
            if (1 < vals.size()) print("{");
            for (unsigned i = 0; i < vals.size(); i++) {
                string s = (0 < i) ? "," : "";
                s += vals[i];
                print(s);
            }
            if (1 < vals.size()) print("}");
            return *this;
        }

        Impl &
        writeAssigns(const t_asgnByLhsName::value_type & stmt) {
            const trc_lhsRhs &asgn = stmt.second;
            //compress lhs/rhs and write
            vector<string> toWrite;
            print("assign ");
            compress(asgn->first, toWrite).write(toWrite);
            print(" = ");
            compress(asgn->second, toWrite).write(toWrite);
            println(";");
            return *this;
        }

        Impl &
        writeAssigns(const t_wiresByName & wires) {
            /* Step 1: build up map of lhs=rhs by basename of lhs.
             * lhs,rhs can be a Wire | WireBitRef.
             * pair.first=lhs, pair.second=rhs.
             */
            t_asgnByLhsName mapOfLhsByName;
            for (t_wiresByName::const_iterator i = wires.begin();
                    i != wires.end(); ++i) {
                const TRcWire &wire = i->second;
                if (wire->isBus()) {
                    const TRcWireBus asBus = toWireBus(wire);
                    updateAssigns(mapOfLhsByName, asBus);
                } else if (!wire->isConst()) { //no const on lhs
                    updateAssigns(mapOfLhsByName, wire);
                }
            }
            FOREACH(t_asgnByLhsName::const_iterator,
                    mapOfLhsByName.begin(), mapOfLhsByName.end(), writeAssigns);
            return *this;
        }

        Impl &
        writeDecl(const TRcModule & mod) {
            ostringstream oss;
            oss << "module " << mod->getName() << " (";
            print(oss);
            setPrefix(m_config->m_portDeclIndent);
            writePorts(mod).setPrefix(0).println(");");
            setPrefix(m_config->m_wireDefnIndent);
            if (!m_config->m_ansiPortDecl) {
                writePorts(mod, true);
            }
            const t_wiresByName &wires = mod->getWiresByName();
            if (wires.empty()) return *this;
            println("//BEGIN{{ wire declarations");
            for (t_wiresByName::const_iterator i = wires.begin();
                    i != wires.end(); ++i) {
                const TRcWire &w = i->second;
                if (w->isPort() || w->isConst()) continue;
                oss << w << ";";
                println(oss);
            }
            println("//wire declarations }}END");
            return *this;
        }

        Impl &
        write(const TRcModule & mod) {
            writeDecl(mod);
            writeAssigns(mod->getWiresByName());
            writeInsts(mod);
            setPrefix(0).println("endmodule");
            return *this;
        }
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
        ASSERT_TRUE(top.isValid());
        Impl::trc_mods modOrder = mp_impl->getModuleDeclOrder(top, doHier);
        FOREACH(Impl::t_mods::const_iterator, modOrder->begin(), modOrder->end(),
                mp_impl->write);
        //TODO: print module
        mp_impl->flush();

        return mp_impl->m_lcnt;
    }

    Writer::~Writer() {
        delete mp_impl;
    }
}
