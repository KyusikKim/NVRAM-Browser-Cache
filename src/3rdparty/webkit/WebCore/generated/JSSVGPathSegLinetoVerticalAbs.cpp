/*
    This file is part of the WebKit open source project.
    This file has been generated by generate-bindings.pl. DO NOT MODIFY!

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "config.h"

#if ENABLE(SVG)

#include "JSSVGPathSegLinetoVerticalAbs.h"

#include "SVGPathSegLinetoVertical.h"
#include <runtime/JSNumberCell.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

ASSERT_CLASS_FITS_IN_CELL(JSSVGPathSegLinetoVerticalAbs);

/* Hash table */

static const HashTableValue JSSVGPathSegLinetoVerticalAbsTableValues[3] =
{
    { "y", DontDelete, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGPathSegLinetoVerticalAbsY), (intptr_t)setJSSVGPathSegLinetoVerticalAbsY },
    { "constructor", DontEnum|ReadOnly, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGPathSegLinetoVerticalAbsConstructor), (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSSVGPathSegLinetoVerticalAbsTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 1, JSSVGPathSegLinetoVerticalAbsTableValues, 0 };
#else
    { 4, 3, JSSVGPathSegLinetoVerticalAbsTableValues, 0 };
#endif

/* Hash table for constructor */

static const HashTableValue JSSVGPathSegLinetoVerticalAbsConstructorTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSSVGPathSegLinetoVerticalAbsConstructorTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSSVGPathSegLinetoVerticalAbsConstructorTableValues, 0 };
#else
    { 1, 0, JSSVGPathSegLinetoVerticalAbsConstructorTableValues, 0 };
#endif

class JSSVGPathSegLinetoVerticalAbsConstructor : public DOMConstructorObject {
public:
    JSSVGPathSegLinetoVerticalAbsConstructor(ExecState* exec, JSDOMGlobalObject* globalObject)
        : DOMConstructorObject(JSSVGPathSegLinetoVerticalAbsConstructor::createStructure(globalObject->objectPrototype()), globalObject)
    {
        putDirect(exec->propertyNames().prototype, JSSVGPathSegLinetoVerticalAbsPrototype::self(exec, globalObject), None);
    }
    virtual bool getOwnPropertySlot(ExecState*, const Identifier&, PropertySlot&);
    virtual bool getOwnPropertyDescriptor(ExecState*, const Identifier&, PropertyDescriptor&);
    virtual const ClassInfo* classInfo() const { return &s_info; }
    static const ClassInfo s_info;

    static PassRefPtr<Structure> createStructure(JSValue proto) 
    { 
        return Structure::create(proto, TypeInfo(ObjectType, StructureFlags), AnonymousSlotCount); 
    }
    
protected:
    static const unsigned StructureFlags = OverridesGetOwnPropertySlot | ImplementsHasInstance | DOMConstructorObject::StructureFlags;
};

const ClassInfo JSSVGPathSegLinetoVerticalAbsConstructor::s_info = { "SVGPathSegLinetoVerticalAbsConstructor", 0, &JSSVGPathSegLinetoVerticalAbsConstructorTable, 0 };

bool JSSVGPathSegLinetoVerticalAbsConstructor::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSSVGPathSegLinetoVerticalAbsConstructor, DOMObject>(exec, &JSSVGPathSegLinetoVerticalAbsConstructorTable, this, propertyName, slot);
}

bool JSSVGPathSegLinetoVerticalAbsConstructor::getOwnPropertyDescriptor(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    return getStaticValueDescriptor<JSSVGPathSegLinetoVerticalAbsConstructor, DOMObject>(exec, &JSSVGPathSegLinetoVerticalAbsConstructorTable, this, propertyName, descriptor);
}

/* Hash table for prototype */

static const HashTableValue JSSVGPathSegLinetoVerticalAbsPrototypeTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSSVGPathSegLinetoVerticalAbsPrototypeTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSSVGPathSegLinetoVerticalAbsPrototypeTableValues, 0 };
#else
    { 1, 0, JSSVGPathSegLinetoVerticalAbsPrototypeTableValues, 0 };
#endif

const ClassInfo JSSVGPathSegLinetoVerticalAbsPrototype::s_info = { "SVGPathSegLinetoVerticalAbsPrototype", 0, &JSSVGPathSegLinetoVerticalAbsPrototypeTable, 0 };

JSObject* JSSVGPathSegLinetoVerticalAbsPrototype::self(ExecState* exec, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSSVGPathSegLinetoVerticalAbs>(exec, globalObject);
}

const ClassInfo JSSVGPathSegLinetoVerticalAbs::s_info = { "SVGPathSegLinetoVerticalAbs", &JSSVGPathSeg::s_info, &JSSVGPathSegLinetoVerticalAbsTable, 0 };

JSSVGPathSegLinetoVerticalAbs::JSSVGPathSegLinetoVerticalAbs(NonNullPassRefPtr<Structure> structure, JSDOMGlobalObject* globalObject, PassRefPtr<SVGPathSegLinetoVerticalAbs> impl)
    : JSSVGPathSeg(structure, globalObject, impl)
{
}

JSObject* JSSVGPathSegLinetoVerticalAbs::createPrototype(ExecState* exec, JSGlobalObject* globalObject)
{
    return new (exec) JSSVGPathSegLinetoVerticalAbsPrototype(JSSVGPathSegLinetoVerticalAbsPrototype::createStructure(JSSVGPathSegPrototype::self(exec, globalObject)));
}

bool JSSVGPathSegLinetoVerticalAbs::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSSVGPathSegLinetoVerticalAbs, Base>(exec, &JSSVGPathSegLinetoVerticalAbsTable, this, propertyName, slot);
}

bool JSSVGPathSegLinetoVerticalAbs::getOwnPropertyDescriptor(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    return getStaticValueDescriptor<JSSVGPathSegLinetoVerticalAbs, Base>(exec, &JSSVGPathSegLinetoVerticalAbsTable, this, propertyName, descriptor);
}

JSValue jsSVGPathSegLinetoVerticalAbsY(ExecState* exec, JSValue slotBase, const Identifier&)
{
    JSSVGPathSegLinetoVerticalAbs* castedThis = static_cast<JSSVGPathSegLinetoVerticalAbs*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGPathSegLinetoVerticalAbs* imp = static_cast<SVGPathSegLinetoVerticalAbs*>(castedThis->impl());
    JSValue result = jsNumber(exec, imp->y());
    return result;
}

JSValue jsSVGPathSegLinetoVerticalAbsConstructor(ExecState* exec, JSValue slotBase, const Identifier&)
{
    JSSVGPathSegLinetoVerticalAbs* domObject = static_cast<JSSVGPathSegLinetoVerticalAbs*>(asObject(slotBase));
    return JSSVGPathSegLinetoVerticalAbs::getConstructor(exec, domObject->globalObject());
}
void JSSVGPathSegLinetoVerticalAbs::put(ExecState* exec, const Identifier& propertyName, JSValue value, PutPropertySlot& slot)
{
    lookupPut<JSSVGPathSegLinetoVerticalAbs, Base>(exec, propertyName, value, &JSSVGPathSegLinetoVerticalAbsTable, this, slot);
}

void setJSSVGPathSegLinetoVerticalAbsY(ExecState* exec, JSObject* thisObject, JSValue value)
{
    JSSVGPathSegLinetoVerticalAbs* castedThisObj = static_cast<JSSVGPathSegLinetoVerticalAbs*>(thisObject);
    SVGPathSegLinetoVerticalAbs* imp = static_cast<SVGPathSegLinetoVerticalAbs*>(castedThisObj->impl());
    imp->setY(value.toFloat(exec));
    JSSVGContextCache::propagateSVGDOMChange(castedThisObj, imp->associatedAttributeName());
}

JSValue JSSVGPathSegLinetoVerticalAbs::getConstructor(ExecState* exec, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSSVGPathSegLinetoVerticalAbsConstructor>(exec, static_cast<JSDOMGlobalObject*>(globalObject));
}


}

#endif // ENABLE(SVG)
