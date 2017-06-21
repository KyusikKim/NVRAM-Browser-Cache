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
#include "JSHTMLHRElement.h"

#include "HTMLHRElement.h"
#include "KURL.h"
#include <runtime/JSString.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

ASSERT_CLASS_FITS_IN_CELL(JSHTMLHRElement);

/* Hash table */

static const HashTableValue JSHTMLHRElementTableValues[6] =
{
    { "align", DontDelete, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLHRElementAlign), (intptr_t)setJSHTMLHRElementAlign },
    { "noShade", DontDelete, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLHRElementNoShade), (intptr_t)setJSHTMLHRElementNoShade },
    { "size", DontDelete, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLHRElementSize), (intptr_t)setJSHTMLHRElementSize },
    { "width", DontDelete, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLHRElementWidth), (intptr_t)setJSHTMLHRElementWidth },
    { "constructor", DontEnum|ReadOnly, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsHTMLHRElementConstructor), (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSHTMLHRElementTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 31, JSHTMLHRElementTableValues, 0 };
#else
    { 17, 15, JSHTMLHRElementTableValues, 0 };
#endif

/* Hash table for constructor */

static const HashTableValue JSHTMLHRElementConstructorTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSHTMLHRElementConstructorTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSHTMLHRElementConstructorTableValues, 0 };
#else
    { 1, 0, JSHTMLHRElementConstructorTableValues, 0 };
#endif

class JSHTMLHRElementConstructor : public DOMConstructorObject {
public:
    JSHTMLHRElementConstructor(ExecState* exec, JSDOMGlobalObject* globalObject)
        : DOMConstructorObject(JSHTMLHRElementConstructor::createStructure(globalObject->objectPrototype()), globalObject)
    {
        putDirect(exec->propertyNames().prototype, JSHTMLHRElementPrototype::self(exec, globalObject), None);
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

const ClassInfo JSHTMLHRElementConstructor::s_info = { "HTMLHRElementConstructor", 0, &JSHTMLHRElementConstructorTable, 0 };

bool JSHTMLHRElementConstructor::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSHTMLHRElementConstructor, DOMObject>(exec, &JSHTMLHRElementConstructorTable, this, propertyName, slot);
}

bool JSHTMLHRElementConstructor::getOwnPropertyDescriptor(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    return getStaticValueDescriptor<JSHTMLHRElementConstructor, DOMObject>(exec, &JSHTMLHRElementConstructorTable, this, propertyName, descriptor);
}

/* Hash table for prototype */

static const HashTableValue JSHTMLHRElementPrototypeTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSHTMLHRElementPrototypeTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSHTMLHRElementPrototypeTableValues, 0 };
#else
    { 1, 0, JSHTMLHRElementPrototypeTableValues, 0 };
#endif

const ClassInfo JSHTMLHRElementPrototype::s_info = { "HTMLHRElementPrototype", 0, &JSHTMLHRElementPrototypeTable, 0 };

JSObject* JSHTMLHRElementPrototype::self(ExecState* exec, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSHTMLHRElement>(exec, globalObject);
}

const ClassInfo JSHTMLHRElement::s_info = { "HTMLHRElement", &JSHTMLElement::s_info, &JSHTMLHRElementTable, 0 };

JSHTMLHRElement::JSHTMLHRElement(NonNullPassRefPtr<Structure> structure, JSDOMGlobalObject* globalObject, PassRefPtr<HTMLHRElement> impl)
    : JSHTMLElement(structure, globalObject, impl)
{
}

JSObject* JSHTMLHRElement::createPrototype(ExecState* exec, JSGlobalObject* globalObject)
{
    return new (exec) JSHTMLHRElementPrototype(JSHTMLHRElementPrototype::createStructure(JSHTMLElementPrototype::self(exec, globalObject)));
}

bool JSHTMLHRElement::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSHTMLHRElement, Base>(exec, &JSHTMLHRElementTable, this, propertyName, slot);
}

bool JSHTMLHRElement::getOwnPropertyDescriptor(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    return getStaticValueDescriptor<JSHTMLHRElement, Base>(exec, &JSHTMLHRElementTable, this, propertyName, descriptor);
}

JSValue jsHTMLHRElementAlign(ExecState* exec, JSValue slotBase, const Identifier&)
{
    JSHTMLHRElement* castedThis = static_cast<JSHTMLHRElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLHRElement* imp = static_cast<HTMLHRElement*>(castedThis->impl());
    JSValue result = jsString(exec, imp->align());
    return result;
}

JSValue jsHTMLHRElementNoShade(ExecState* exec, JSValue slotBase, const Identifier&)
{
    JSHTMLHRElement* castedThis = static_cast<JSHTMLHRElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLHRElement* imp = static_cast<HTMLHRElement*>(castedThis->impl());
    JSValue result = jsBoolean(imp->noShade());
    return result;
}

JSValue jsHTMLHRElementSize(ExecState* exec, JSValue slotBase, const Identifier&)
{
    JSHTMLHRElement* castedThis = static_cast<JSHTMLHRElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLHRElement* imp = static_cast<HTMLHRElement*>(castedThis->impl());
    JSValue result = jsString(exec, imp->size());
    return result;
}

JSValue jsHTMLHRElementWidth(ExecState* exec, JSValue slotBase, const Identifier&)
{
    JSHTMLHRElement* castedThis = static_cast<JSHTMLHRElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    HTMLHRElement* imp = static_cast<HTMLHRElement*>(castedThis->impl());
    JSValue result = jsString(exec, imp->width());
    return result;
}

JSValue jsHTMLHRElementConstructor(ExecState* exec, JSValue slotBase, const Identifier&)
{
    JSHTMLHRElement* domObject = static_cast<JSHTMLHRElement*>(asObject(slotBase));
    return JSHTMLHRElement::getConstructor(exec, domObject->globalObject());
}
void JSHTMLHRElement::put(ExecState* exec, const Identifier& propertyName, JSValue value, PutPropertySlot& slot)
{
    lookupPut<JSHTMLHRElement, Base>(exec, propertyName, value, &JSHTMLHRElementTable, this, slot);
}

void setJSHTMLHRElementAlign(ExecState* exec, JSObject* thisObject, JSValue value)
{
    JSHTMLHRElement* castedThisObj = static_cast<JSHTMLHRElement*>(thisObject);
    HTMLHRElement* imp = static_cast<HTMLHRElement*>(castedThisObj->impl());
    imp->setAlign(valueToStringWithNullCheck(exec, value));
}

void setJSHTMLHRElementNoShade(ExecState* exec, JSObject* thisObject, JSValue value)
{
    JSHTMLHRElement* castedThisObj = static_cast<JSHTMLHRElement*>(thisObject);
    HTMLHRElement* imp = static_cast<HTMLHRElement*>(castedThisObj->impl());
    imp->setNoShade(value.toBoolean(exec));
}

void setJSHTMLHRElementSize(ExecState* exec, JSObject* thisObject, JSValue value)
{
    JSHTMLHRElement* castedThisObj = static_cast<JSHTMLHRElement*>(thisObject);
    HTMLHRElement* imp = static_cast<HTMLHRElement*>(castedThisObj->impl());
    imp->setSize(valueToStringWithNullCheck(exec, value));
}

void setJSHTMLHRElementWidth(ExecState* exec, JSObject* thisObject, JSValue value)
{
    JSHTMLHRElement* castedThisObj = static_cast<JSHTMLHRElement*>(thisObject);
    HTMLHRElement* imp = static_cast<HTMLHRElement*>(castedThisObj->impl());
    imp->setWidth(valueToStringWithNullCheck(exec, value));
}

JSValue JSHTMLHRElement::getConstructor(ExecState* exec, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSHTMLHRElementConstructor>(exec, static_cast<JSDOMGlobalObject*>(globalObject));
}


}
