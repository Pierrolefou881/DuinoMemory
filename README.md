# DuinoMemory

<!-- Arduino dynamic badge (enable when available)
[![Arduino Library Manager](https://img.shields.io/arduino-library/v/DuinoMemory.svg)](https://www.arduino.cc/reference/en/libraries/duinomemory/)
-->

![Arduino](https://img.shields.io/badge/Arduino-Compatible-00979D) [![PlatformIO Registry](https://badges.registry.platformio.org/packages/pierrolefou881/library/DuinoMemory.svg)](https://registry.platformio.org/libraries/pierrolefou881/DuinoMemory) ![Header Only](https://img.shields.io/badge/header--only-yes-brightgreen) ![License](https://img.shields.io/github/license/Pierrolefou881/DuinoMemory) ![C++](https://img.shields.io/badge/C%2B%2B-11%2F14-blue) ![Embedded Friendly](https://img.shields.io/badge/embedded-friendly-blue)


Lightweight smart pointers for Arduino and embedded systems.
Header-only, STL-inspired, designed for memory-constrained MCUs.

Dynamic memory is one of the main causes of crashes on Arduino.
DuinoMemory helps you manage heap allocation safely, preventing memory leaks, double deletes and ownership bugs — without the complexity or overhead of the full STL.

Compatible with Arduino IDE and PlatformIO.

License: MIT.

## Quick example
```C++
#include <DuinoMemory.hpp>
using namespace DuinoMemory;

S_ptr<Foo> ptr = make_shared<Foo>();

if (ptr)
{
    ptr->do_something();
}

// Share ownership safely
S_ptr<Foo> other = ptr;
Serial.println(ptr.count()); // 2
```

## Why DuinoMemory?

- Safer dynamic memory: prevent memory leaks, double deletes and ownership bugs.
- Modern C++ and STL-like API (`make_unique`, `make_shared`).
- Embedded-first design: header-only, lightweight, no exceptions, predictable 
behavior.
- Works with Arduino IDE and PlatformIO.
- Designed for low-RAM boards and MCUs.
- Interrupt-aware reference counting (`S_ptr`).
- No STL required, no exceptions, no RTTI.
- Works on AVR, SAMD, ESP32…

Feel free to take a look at the [cheat-sheet](CHEATSHEET.md).

## When should I use DuinoMemory?
Use DuinoMemory when:
- You need dynamic allocation with clear ownership
- Objects are created/destroyed at runtime
- Code complexity makes manual delete error-prone

Avoid it when:
- Static allocation is sufficient
- Objects live for the entire program lifetime
- Running in hard real-time or ISR contexts


## Design goals
- Provide simple, lightweight smart pointers for constrained embedded systems.
- Avoid undefined behavior when used properly.
- Stay close to `STL` semantics while remaining Arduino-friendly.
- Predictable behavior on memory-constrained systems.

## Non-goals

- Full `STL` compliance.
- Thread safety.
- Custom allocators.


## Installation

### Arduino IDE
Install directly from the IDE via the library manager.

OR

Clone the following [git repo](https://github.com/Pierrolefou881/DuinoMemory.git) 
directly into your Arduino/libraries directory. 

### PlatformIO

Add the library to your project's `platformio.ini` file:

#### Stable
```ini
lib_deps =
    https://github.com/Pierrolefou881/DuinoMemory.git#v1.1.1
```

#### Development
```ini
lib_deps =
    https://github.com/Pierrolefou881/DuinoMemory.git#main
```

## Structure

The DuinoMemory library revolves around two concrete smart pointer types:
- `U_ptr`, similar to the C++ `STL` `std::unique_ptr`. This
pointer does not allow copying. A new assignment means destruction of previously
held data and change of ownership between two instances of `U_ptr` via the
`release()` method. It also destroys the object when it 
goes out of scope. `U_ptr` owns exactly one object at a time and 
enforces single ownership.
- `S_ptr`, similar to the C++ `STL` `std::shared_ptr`. This
pointer counts the number of references to the object. When this
count drops to zero, the object gets destroyed.

For ease of use the library only requires including the
`DuinoMemory.hpp` header.

## Basic usage

### Import library and declare pointers

``` C++
// example.ino
#include <DuinoMemory.hpp>         // Import library.

// Declare and initialize a null (nullptr) U_ptr.
DuinoMemory::U_ptr<Foo> foo{ };   

// Declare and initialize a null (nullptr) S_ptr.
DuinoMemory::S_ptr<Bar> bar{ };   
```

### Allocate and assign resources

```C++
void some_func(Foo* some_foo, Bar* some_bar) {
    //-------------------------------------------------------------------------
    // Common use cases
    //-------------------------------------------------------------------------

    // Use allocation function (recommended).
    foo = DuinoMemory::make_unique<Foo>();       

    // Also exists with parameters. The number and types of arguments must match 
    // an existing constructor of the template type.
    bar = DuinoMemory::make_shared<Bar>(param);  

    // You can also make polymorphic instantiations with either S_ptr or U_ptr, 
    // with or without parameters. Make sure Bar has a virtual destructor.
    bar = DuinoMemory::make_shared<Bar, BarDerived>(param);

    // foo being a U_ptr, some_ptr will be destroyed when exiting the scope of
    // this function. For that reason, some_ptr must not belong to any
    // SmartPointer. Here foo takes ownership of some_ptr.
    foo = some_foo;

    // Initializes reference count at 1. Reference count decreases whenever one
    // S_ptr referencing it is assigned a new value or goes out of scope. When
    // count reaches 0, some_ptr will be destroyed. For that reason, some_bar 
    // must not belong to any SmartPointer, as it may lead to undefined behavior.
    bar = some_bar;

    // Perfectly fine if the function allocates and returns a raw pointer or
    // preferably in this case, a U_ptr<Foo>.
    foo = a_function_returning_raw_or_unique();

    // bar and bar2 point to the same object (reference count incremented).
    DuinoMemory::S_ptr<Bar> bar2 = bar;
    DuinoMemory::S_ptr<Bar> bar3{ bar };    // Copy ctor
    size_t count = bar.count();  // 3

    // Proper way of transferring ownership with U_ptr.
    DuinoMemory::U_ptr<Foo> new_owner = foo.release();  // now foo == nullptr

    // If you need to copy data (same method for S_ptr). Foo must have a
    // copy constructor.
    DuinoMemory::U_ptr<Foo> copy = DuinoMemory::make_unique<Foo>{ *foo };

    //-------------------------------------------------------------------------
    // Not recommended
    //-------------------------------------------------------------------------
    
    // Code smell; defeats the purpose of a smart pointer
    foo = new Foo{ };

    //-------------------------------------------------------------------------
    // DON'T DO THAT
    //-------------------------------------------------------------------------

    // Already belongs to foo. Undefined behavior.
    DuinoMemory::U_ptr<Foo> foo_undefined = some_ptr;
    DuinoMemory::S_ptr<Foo> foo_undefined_shared = some_ptr;

    // Undefined behavior: double reference count. Whichever reaches zero first
    // deletes the raw pointer.
    DuinoMemory::S_ptr<Bar> bar_undefined = bar.get(); 

    // Likewise, with U_ptr: double delete. Expect a crash.
    DuinoMemory::U_ptr<Foo> other_undefined = foo.get(); 

    //-------------------------------------------------------------------------
    // COMPILATION ERROR
    //-------------------------------------------------------------------------
    
    // U_ptr does not support copy.
    DuinoMemory::U_ptr<Foo> foo_1{ foo };
    foo_1 = foo;
}
```

### Arrays
It is possible to create arrays or any kind of collection containing smart
pointers, however bear in mind that managing dynamically allocated arrays can be 
error-prone on embedded systems. 
```C++
//-------------------------------------------------------------------------
// Common use cases
//-------------------------------------------------------------------------
DuinoMemory::U_ptr<Foo> foo_array[capacity]; // OK
DuinoMemory::S_ptr<Bar> bar_array[capacity]; // OK

//-------------------------------------------------------------------------
// Not recommended
//-------------------------------------------------------------------------

// Resizing will cause heap fragmentation that can lead to crashes and resets.
DuinoMemory::U_ptr<Foo> foo_array = new DuinoMemory::U_ptr<Foo>[capacity];
DuinoMemory::S_ptr<Bar> bar_array = new DuinoMemory::S_ptr<Bar>[capacity];
```

### Deallocation
```C++
void cleanup() {
    // Object will be destroyed when exiting the function.
    DuinoMemory::U_ptr<Foo> _tmp = foo.release();          

    // Decrements the reference count. If it reaches 0, object gets destroyed.
    bar2 = nullptr;
}
```

### Data access

`U_ptr` and `S_ptr` function like their `STL` counterparts.

```C++
/**
 * Note:
 *   Dereferencing a null SmartPointer (* or ->) leads to 
 *   undefined behavior.
 *   Always check that the pointer is valid before dereferencing:
 *       if (ptr) { ptr->method(); }
 */
if (foo)
{
    // Dereference operator *
    Foo copy = *foo;
}

if (bar)
{
    // Dereference operator ->
    bar->method();
}

// Get the raw pointer (can be nullptr).
Bar* ptr = bar.get();

// get reference count (S_ptr only). If either raw pointer or reference count is 
// nullptr, returns 0.
size_t ref_count = bar.count();
```

### Operator bool

The `bool` operator checks whether the smart pointer's raw pointer is null.
```C++
if (foo)               // Same as if (foo != nullptr)
{
    do_something();
}

if (!foo)              // Same as if (foo == nullptr)
{
    crash(ErrorCode::NULLPTR);
}

// Works also with S_ptr
if (bar)
{
    bar->do_something();
}

if (!bar)
{
    log("ERROR -> nullptr detected");
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
#### Note on polymorphism
When using smart pointers with polymorphic types, the base type must have a 
virtual destructor to ensure correct destruction of derived objects.
Otherwise, deleting the object through a base pointer results in undefined 
behavior and may cause memory leaks or crashes.

```C++
struct Base {
    virtual ~Base() {}   // Required
};

struct Derived : Base {
    ~Derived() {
        // resources freed correctly
    }
};

DuinoMemory::S_ptr<Base> ptr = DuinoMemory::make_shared<Base, Derived>(); // Safe
DuinoMemory::U_ptr<Base> ptr2 = DuinoMemory::make_unique<Base, Derived>();// Safe

```
Rule of thumb:
If you use inheritance with smart pointers, always make the base destructor 
virtual.

## Safety considerations and limitations
DuinoMemory is designed for constrained embedded systems, but misusing smart 
pointers may still lead to undefined behavior. Please be aware of the 
following rules and  limitations:

### Ownership
- Never create multiple smart pointers from the same raw pointer.
- Never give a raw pointer obtained via .get() to another smart pointer.
- A pointer returned by .get() is non-owning.
```cpp
Foo* raw = new Foo();
S_ptr<Foo> a{ raw };
S_ptr<Foo> b{ raw };    // DANGER: double delete
U_ptr<Foo> c = b.get(); // DANGER: double delete
```
- For `U_ptr`, always transfer ownership through `release()`.
```C++
U_ptr<Foo> a = make_unique<Foo>();
U_ptr<Foo> b = a.release(); // SAFE, a == nullptr.
```
- For `S_ptr`, always share ownership through copy or assignment with another 
`S_ptr`
```C++
S_ptr<Foo> a = make_shared<Foo>();
S_ptr<Foo> b = a;      // SAFE
S_ptr<Foo> c{ b };     // SAFE
```

Violating these rules may result in double delete, heap corruption, crashes or 
MCU resets.

### Concurrency / interrupts
- S_ptr is not thread-safe.
- Do not share smart pointers across tasks or threads.
- Do not create or destroy S_ptr inside an ISR.
- Object construction/destruction may call new/delete, which is unsafe in 
interrupt context.
- Reference counting uses interrupt protection, but this does not make S_ptr 
interrupt-safe.


### Memory behavior
- S_ptr performs two allocations (object + reference counter).
- Frequent creation/destruction may cause heap fragmentation, especially on small 
AVR boards.
- Prefer long-lived objects or static allocation when possible.

### Array types
- Array types (T[]) are not supported.
- Arrays require delete[], which is intentionally avoided to prevent undefined 
behavior.
```C++
// Undefined behavior.
DuinoMemory::U_ptr<Foo[]> foo_array{ };    
DuinoMemory::S_ptr<Bar[]> bar_array{ };    
```

### Allocation failures
On many Arduino platforms, allocation failure may lead to:
- MCU reset
- freeze
- undefined behavior

Exceptions and reliable failure handling are generally unavailable.
Always monitor RAM usage and minimize dynamic allocation.

## License

MIT - see [LICENSE](LICENSE)