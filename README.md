# DuinoMemory

Lightweight smart pointers for the Arduino environment.

Supports Arduino / PlatformIO.

License: MIT.

## Installation

### Arduino IDE

Clone the following [git repo](https://github.com/Pierrolefou881/DuinoMemory.git) 
directly into your Arduino/libraries directory. 

### PlatformIO

Add the library to your project's **platformio.ini** file:

#### Stable
```ini
lib_deps =
    https://github.com/Pierrolefou881/DuinoMemory.git#v1.0.0
```

#### Development
```ini
lib_deps =
    https://github.com/Pierrolefou881/DuinoMemory.git#main
```

## Structure

The DuinoMemory library revolves around two concrete smart pointer types:
- **U_ptr**, similar to the C++ STL **std::unique_ptr**. This
pointer does not allow copying and changes ownership every time
a new assignment is made. It also destroys the object when it 
goes out of scope. CAUTION: When using U_ptr<T> with polymorphic 
types, T must have a virtual destructor to ensure correct 
destruction of derived objects.
- **S_ptr**, similar to the C++ STL **std::shared_ptr**. This
pointer counts the number of references to the object. When this
count drops to zero, the object gets destroyed.

For ease of use the library only requires including the
**DuinoMemory.hpp** header.

## Examples

### Import library and declare pointers

``` C++
// example.ino
#include <DuinoMemory.hpp>         // Import library.

// Declare and initialize a null (nullptr) U_ptr.
DuinoMemory::U_ptr<Foo> _foo{ };   

// Declare and initialize a null (nullptr) S_ptr.
DuinoMemory::S_ptr<Bar> _bar{ };   
```

### Allocate new resources

```C++
void some_func(Foo* some_ptr) {
    // Using new operator is not recommended (code smell).
    // _foo = new Foo{ };                    

    // Ownership is transferred; the pointed object will be destroyed when 
    // exiting the function.
    _foo = some_ptr;                         

    // Use allocation function (recommended).
    _foo = DuinoMemory::make_unique<Foo>();       

    // Also exists with parameters. The number and types of arguments must match 
    // an existing constructor of the template type.
    _bar = DuinoMemory::make_shared<Bar>(param);  

    // You can also make polymorphic instantiations with either S_ptr or U_ptr, 
    // with or without parameters.
    _bar = DuinoMemory::make_shared<Bar, BarDerived>(param); 

    // Very common use case. Sets reference count at 1. CAUTION: do not do it 
    // with another S_ptr's raw pointer.
    _bar = some_ptr;                         
}
```

### Assignment

```C++
// DuinoMemory::U_ptr<Foo> _foo_1{ };
// _foo_1 = _foo                      // ERROR ! Copy assignment deleted.

// _foo is now null (change of ownership).
DuinoMemory::U_ptr<Foo> _foo_2 = _foo.release();  

// _bar and _bar2 point to the same object (reference count incremented).
DuinoMemory::S_ptr<Bar> _bar2 = _bar;
uint16_t count = _bar.count();  // 2          

// CAUTION: Do not initialize or assign a S_ptr with the raw pointer from 
// another S_ptr; undefined behavior.
// DuinoMemory::S_ptr<Bar> _error{ _bar.get() };
// _error = _bar.get();

// This is the correct way.
DuinoMemory::S_ptr<Bar> _correct{ _bar };
_correct = _bar;
```

### Deallocation
```C++
void cleanup() {
    // Object will be destroyed when exiting the function.
    DuinoMemory::U_ptr<Foo> _tmp = _foo.release();          

    // Decrements the reference count. If it reaches 0, object gets destroyed.
    _bar2 = nullptr;
}
```

### Data access

**U_ptr** and **S_ptr** function like their STL counterparts.

```C++
/**
 * Warning:
 *   Dereferencing a null SmartPointer (* or ->) leads to 
 *   undefined behavior.
 *   Always check that the pointer is valid before dereferencing:
 *       if (ptr) { ptr->method(); }
 */

// Dereference operator *
Foo copy = *_foo;

// Dereference operator ->
_bar->method();

// Get the raw pointer.
Bar* ptr = _bar.get();

// get reference count (S_ptr only).
uint16_t ref_count = _bar.count();
```

### Operator bool

The ***bool*** opeator checks whether the smart pointer's raw pointer is null.
```C++
if (_foo)               // Same as if (_foo != nullptr)
{
    do_something();
}

if (!_foo)              // Same as if (_foo == nullptr)
{

}

// Works also with S_ptr
if (_bar)
{

}

if (!_bar)
{

}
```

### Polymorphic example

```C++
struct Base {
    virtual void say() { Serial.println("Base"); }
    virtual ~Base() = default;
};

struct Derived : public Base {
    void say() override { Serial.println("Derived"); }
};

// U_ptr with derived
DuinoMemory::U_ptr<Base> up = DuinoMemory::make_unique<Base, Derived>();
up->say();  // Prints "Derived"

// S_ptr with derived
DuinoMemory::S_ptr<Base> sp = DuinoMemory::make_shared<Base, Derived>();
sp->say();  // Prints "Derived"

// Reference counting
DuinoMemory::S_ptr<Base> sp2 = sp;
Serial.println(sp.count()); // 2
sp = nullptr;
Serial.println(sp2.count()); // 1
```

## License

MIT License

Copyright (c) 2026 Pierre DEBAS

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
