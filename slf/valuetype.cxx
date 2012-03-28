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
#include <iostream>
#include "xyzzy/assert.hxx"
#include "slf/valuetype.hxx"

namespace slf {
    using namespace std;

    class ValueType::Impl {
    public:

        struct t_kidentBus {

            explicit t_kidentBus(const string &s, const TRcBus & bus)
            : m_string(s), m_bus(bus) {
            }
            const string m_string;
            TRcBus m_bus;
        };

        struct t_expr {

            explicit t_expr(const TRcExpr & expr) : m_expr(expr) {
            }
            TRcExpr m_expr;
        };

        struct t_number {

            explicit t_number(const TRcNumber &num, const string & unit)
            : m_num(num), m_unit(unit) {
            }
            TRcNumber m_num;
            const string m_unit;
        };

        explicit Impl(const TRcExpr &expr) : m_type(eExpr) {
            m_val.p_asExpr = new t_expr(expr);
        }

        explicit Impl(const string &kident, const TRcBus &bus) : m_type(eKident) {
            m_val.p_asIdentBus = new t_kidentBus(kident, bus);
        }

        explicit Impl(const string &str) : m_type(eString) {
            m_val.p_asString = new string(str);
        }

        explicit Impl(const TRcNumber &num, const string &unit) : m_type(eNumber) {
            m_val.p_asNumber = new t_number(num, unit);
        }

        explicit Impl(bool b) : m_type(eBool) {
            m_val.asBool = b;
        }

        union t_val {
            bool asBool;
            const t_expr *p_asExpr;
            const t_kidentBus *p_asIdentBus;
            const string *p_asString;
            const t_number *p_asNumber;
        };

        EType m_type;
        t_val m_val;

        ~Impl() {
            switch (m_type) {
                case (eExpr): delete m_val.p_asExpr;
                    break;
                case (eKident): delete m_val.p_asIdentBus;
                    break;
                case (eString): delete m_val.p_asString;
                    break;
                case (eNumber): delete m_val.p_asNumber;
                    break;
                default:
                    ; //bool
            }
        }
    };

    ValueType::ValueType(const TRcExpr &expr) {
        mp_impl = new Impl(expr);
    }

    ValueType::ValueType(const string &kident, const TRcBus &bus) {
        mp_impl = new Impl(kident, bus);
    }

    ValueType::ValueType(const string &str) {
        mp_impl = new Impl(str);
    }

    ValueType::ValueType(const TRcNumber &num, const string &unit) {
        mp_impl = new Impl(num, unit);
    }

    ValueType::ValueType(bool b) {
        mp_impl = new Impl(b);
    }

    ValueType::EType ValueType::getType() const {
        return mp_impl->m_type;
    }

    const string&
    ValueType::asIdent() const {
        EType type = getType();
        if (eKident == type) {
            return mp_impl->m_val.p_asIdentBus->m_string;
        } else if (eString == type) {
            return *(mp_impl->m_val.p_asString);
        } else {
            ASSERT_NEVER;
        }
    }

    bool
    ValueType::asBool() const {
        ASSERT_TRUE(eBool == getType());
        return mp_impl->m_val.asBool;
    }

    const TRcNumber&
    ValueType::asNumber() const {
        ASSERT_TRUE(eNumber == getType());
        return mp_impl->m_val.p_asNumber->m_num;
    }

    const TRcExpr&
    ValueType::asExpr() const {
        ASSERT_TRUE(eExpr == getType());
        return mp_impl->m_val.p_asExpr->m_expr;
    }

    ValueType::~ValueType() {
        delete mp_impl;
    }

#ifdef DEBUG
    DebugOstream&
    operator<<(DebugOstream &dos, const TRcValueType &vt) {
        dos << "ValueType {";
        switch (vt->getType()) {
            case ValueType::eBool:
                dos << (vt->asBool() ? "<true>" : "<false>");
                break;
            case ValueType::eKident: case ValueType::eString:
                dos << vt->asIdent(); break;
            case ValueType::eNumber:
                dos << vt->asNumber(); break;
            case ValueType::eExpr:
                dos << "<TODO: expr>"; break;
            default:
                ASSERT_NEVER;
        }
        dos << "}";
        return dos;
    }
#endif
}
