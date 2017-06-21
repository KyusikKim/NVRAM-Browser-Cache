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

#include "JSSVGStopElement.h"

#include "CSSMutableStyleDeclaration.h"
#include "CSSStyleDeclaration.h"
#include "CSSValue.h"
#include "JSCSSStyleDeclaration.h"
#include "JSCSSValue.h"
#include "JSSVGAnimatedNumber.h"
#include "JSSVGAnimatedString.h"
#include "SVGStopElement.h"
#include <runtime/Error.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

ASSERT_CLASS_FITS_IN_CELL(JSSVGStopElement);

/* Hash table */

static const HashTableValue JSSVGStopElementTableValues[5] =
{
    { "offset", DontDelete|ReadOnly, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGStopElementOffset), (intptr_t)0 },
    { "className", DontDelete|ReadOnly, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGStopElementClassName), (intptr_t)0 },
    { "style", DontDelete|ReadOnly, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGStopElementStyle), (intptr_t)0 },
    { "constructor", DontEnum|ReadOnly, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsSVGStopElementConstructor), (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSSVGStopElementTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 7, JSSVGStopElementTableValues, 0 };
#else
    { 8, 7, JSSVGStopElementTableValues, 0 };
#endif

/* Hash table for constructor */

static const HashTableValue JSSVGStopElementConstructorTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSSVGStopElementConstructorTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSSVGStopElementConstructorTableValues, 0 };
#else
    { 1, 0, JSSVGStopElementConstructorTableValues, 0 };
#endif

class JSSVGStopElementConstructor : public DOMConstructorObject {
public:
    JSSVGStopElementConstructor(ExecState* exec, JSDOMGlobalObject* globalObject)
        : DOMConstructorObject(JSSVGStopElementConstructor::createStructure(globalObject->objectPrototype()), globalObject)
    {
        putDirect(exec->propertyNames().prototype, JSSVGStopElementPrototype::self(exec, globalObject), None);
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

const ClassInfo JSSVGStopElementConstructor::s_info = { "SVGStopElementConstructor", 0, &JSSVGStopElementConstructorTable, 0 };

bool JSSVGStopElementConstructor::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSSVGStopElementConstructor, DOMObject>(exec, &JSSVGStopElementConstructorTable, this, propertyName, slot);
}

bool JSSVGStopElementConstructor::getOwnPropertyDescriptor(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    return getStaticValueDescriptor<JSSVGStopElementConstructor, DOMObject>(exec, &JSSVGStopElementConstructorTable, this, propertyName, descriptor);
}

/* Hash table for prototype */

static const HashTableValue JSSVGStopElementPrototypeTableValues[2] =
{
    { "getPresentationAttribute", DontDelete|Function, (intptr_t)static_cast<NativeFunction>(jsSVGStopElementPrototypeFunctionGetPresentationAttribute), (intptr_t)1 },
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSSVGStopElementPrototypeTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSSVGStopElementPrototypeTableValues, 0 };
#else
    { 2, 1, JSSVGStopElementPrototypeTableValues, 0 };
#endif

const ClassInfo JSSVGStopElementPrototype::s_info = { "SVGStopElementPrototype", 0, &JSSVGStopElementPrototypeTable, 0 };

JSObject* JSSVGStopElementPrototype::self(ExecState* exec, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSSVGStopElement>(exec, globalObject);
}

bool JSSVGStopElementPrototype::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticFunctionSlot<JSObject>(exec, &JSSVGStopElementPrototypeTable, this, propertyName, slot);
}

bool JSSVGStopElementPrototype::getOwnPropertyDescriptor(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    return getStaticFunctionDescriptor<JSObject>(exec, &JSSVGStopElementPrototypeTable, this, propertyName, descriptor);
}

const ClassInfo JSSVGStopElement::s_info = { "SVGStopElement", &JSSVGElement::s_info, &JSSVGStopElementTable, 0 };

JSSVGStopElement::JSSVGStopElement(NonNullPassRefPtr<Structure> structure, JSDOMGlobalObject* globalObject, PassRefPtr<SVGStopElement> impl)
    : JSSVGElement(structure, globalObject, impl)
{
}

JSObject* JSSVGStopElement::createPrototype(ExecState* exec, JSGlobalObject* globalObject)
{
    return new (exec) JSSVGStopElementPrototype(JSSVGStopElementPrototype::createStructure(JSSVGElementPrototype::self(exec, globalObject)));
}

bool JSSVGStopElement::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSSVGStopElement, Base>(exec, &JSSVGStopElementTable, this, propertyName, slot);
}

bool JSSVGStopElement::getOwnPropertyDescriptor(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    return getStaticValueDescriptor<JSSVGStopElement, Base>(exec, &JSSVGStopElementTable, this, propertyName, descriptor);
}

JSValue jsSVGStopElementOffset(ExecState* exec, JSValue slotBase, const Identifier&)
{
    JSSVGStopElement* castedThis = static_cast<JSSVGStopElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGStopElement* imp = static_cast<SVGStopElement*>(castedThis->impl());
    RefPtr<SVGAnimatedNumber> obj = imp->offsetAnimated();
    JSValue result =  toJS(exec, castedThis->globalObject(), obj.get(), imp);
    return result;
}

JSValue jsSVGStopElementClassName(ExecState* exec, JSValue slotBase, const Identifier&)
{
    JSSVGStopElement* castedThis = static_cast<JSSVGStopElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGStopElement* imp = static_cast<SVGStopElement*>(castedThis->impl());
    RefPtr<SVGAnimatedString> obj = imp->classNameAnimated();
    JSValue result =  toJS(exec, castedThis->globalObject(), obj.get(), imp);
    return result;
}

JSValue jsSVGStopElementStyle(ExecState* exec, JSValue slotBase, const Identifier&)
{
    JSSVGStopElement* castedThis = static_cast<JSSVGStopElement*>(asObject(slotBase));
    UNUSED_PARAM(exec);
    SVGStopElement* imp = static_cast<SVGStopElement*>(castedThis->impl());
    JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(imp->style()));
    return result;
}

JSValue jsSVGStopElementConstructor(ExecState* exec, JSValue slotBase, const Identifier&)
{
    JSSVGStopElement* domObject = static_cast<JSSVGStopElement*>(asObject(slotBase));
    return JSSVGStopElement::getConstructor(exec, domObject->globalObject());
}
JSValue JSSVGStopElement::getConstructor(ExecState* exec, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSSVGStopElementConstructor>(exec, static_cast<JSDOMGlobalObject*>(globalObject));
}

JSValue JSC_HOST_CALL jsSVGStopElementPrototypeFunctionGetPresentationAttribute(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSSVGStopElement::s_info))
        return throwError(exec, TypeError);
    JSSVGStopElement* castedThisObj = static_cast<JSSVGStopElement*>(asObject(thisValue));
    SVGStopElement* imp = static_cast<SVGStopElement*>(castedThisObj->impl());
    const UString& name = args.at(0).toString(exec);


    JSC::JSValue result = toJS(exec, castedThisObj->globalObject(), WTF::getPtr(imp->getPresentationAttribute(name)));
    return result;
}


}

#endif // ENABLE(SVG)
