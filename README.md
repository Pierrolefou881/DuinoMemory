# Memory
Smart pointer library for Arduino. **Memory** offers two kinds of smart pointers:
- **U_ptr**, similar to std::unique_ptr.
- **S_ptr**, similar to std::shared_ptr.

Both implementations inherit from a common class **SmartPointer** that defines the following
behavior. **SmartPointer** is a generic type that manages memory deallocation acording to its 
concrete implementations rules. Like any pointer, a **SmartPointer** is polymorphic, i.e.
it can reference instance of derived types.

## **S_ptr**
Holds a reference count and destroys the wrapped object only when it drops down to zero.
Upon copy, the reference count is incremented and decremented when the destructor is called.

## **U_ptr**
Destroys its wrapped object when the destructor is called. Upon copy, ownership is passed
from the copied instance to the new one.

## get()
Returns the raw pointer to the wrapped object.

## operator ->
Much like a standard pointer, this operator is used for dereferencing and functions in
the exact same fashion.

## operator *
Much like a standard pointer, this operator is used for dereferencing and functions in
the exact same fashion.

## operator == and !=
Can be used to compare two smart pointers, a smart pointer with a common pointer, or a
smart pointer and a reference to an instance of the wrapped type. Accepts comparisons with
**nullptr**.

## **make_shared** and **make_unique**
Automates the instancing of smart pointers using either default or parametrized constructors.

## Limitations
**SmarPointer** instances are polymorphic with derived types of the wrapped type, but not
with each other. For instance, one cannot declare a smart pointer using the
**SmartPointer** type (which is not allowed because its default constructor is protected,
making the type behave like an abstract class) and assign a derived **S_ptr**
or **U_ptr** as its value.

For this reason, it is advised not to use **make_shared** or **make_unique** if the
declared type is a base type of what is being instanced. This is the reason why the use of
operator **new** may be required in some cases. 