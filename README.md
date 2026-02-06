# DuinoMemory

<!-- Arduino dynamic badge (enable when available)
[![Arduino Library Manager](https://img.shields.io/arduino-library/v/DuinoMemory.svg)](https://www.arduino.cc/reference/en/libraries/duinomemory/)
-->

![Arduino](https://img.shields.io/badge/Arduino-Compatible-00979D) [![PlatformIO Registry](https://badges.registry.platformio.org/packages/pierrolefou881/library/DuinoMemory.svg)](https://registry.platformio.org/libraries/pierrolefou881/DuinoMemory) ![Header Only](https://img.shields.io/badge/header--only-yes-brightgreen) ![License](https://img.shields.io/github/license/Pierrolefou881/DuinoMemory) ![C++](https://img.shields.io/badge/C%2B%2B-11%2F14-blue) ![Embedded Friendly](https://img.shields.io/badge/embedded-friendly-blue)


Lightweight, header-only smart pointers and memory utilities for Arduino and 
embedded platforms.

Compatible with Arduino IDE and PlatformIO.

License: MIT.

## Design goals

- Provide simple, lightweight smart pointers for constrained embedded systems.
- Avoid undefined behavior.
- Stay close to `STL` semantics while remaining Arduino-friendly.

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
    https://github.com/Pierrolefou881/DuinoMemory.git#v1.1.0
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
goes out of scope. CAUTION: When using U_ptr<T> with polymorphic 
types, T must have a virtual destructor to ensure correct 
destruction of derived objects. `U_ptr` owns exactly one object at a time and 
enforces single ownership.
- `S_ptr`, similar to the C++ `STL` `std::shared_ptr`. This
pointer counts the number of references to the object. When this
count drops to zero, the object gets destroyed.

For ease of use the library only requires including the
`DuinoMemory.hpp` header.

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
    // Using new operator is not recommended.
    _foo = new Foo{ };  // Code smell; defeats the purpose of a smart pointer

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
    _bar = some_s_ptr.get() // DANGER: undefined behavior.                    
}
```

### Allocation failure

Although unusual, it may occur that memory allocation fails when instantiating a 
new `SmartPointer`. Unfortunately, such an event can lead to undefined behaviors, 
due to the variety of Arduino compatible hardware. Symptoms for an allocation
failure include crashes, hard resets of the MCU or infinite loops. Moreover, the 
Arduino environment does not provide reliable failsafes for all platforms:
- Exceptions are not supported on most MCUs.
- Assert requires Exceptions.
- `std::nothrow` tends to induce UBs.

As a result allocation failures cannot be managed efficiently on Arduino. This
is a limitation one has to be aware of when developing on embedded systems. Most
often, allocation failures are caused by:
- Intantiation of large objects.
- Recursive allocations / deallocations.
- Heap fragmentation.
- Stack overflows.
- Heap / Stack collisions.

#### Troubleshooting allocation failures

In the event of memory allocation failures, here are a few things you can try:
- Avoid deep recursion.
- Avoid dynamic allocation.
- Manage object lifecycle.
- Allocate once (at `setup()` for instance).
- Favor static storage.
- Monitor `RAM` usage.

You can use the `bool` operator on `SmartPointer` to ensure you are accessing non
null data.

```C++
// If allocation crashed, _fail == nullptr.
// On some platforms however, failure might reset the MCU before reaching such test.
DuinoMemory::U_ptr<Foo> _fail{ DuinoMemory::make_unique<Foo>(params) };

// Check nullptr
if (_fail)
{
    _fail->do_something();
}
// If nullptr, log error, crash or implement failsafe
else
{
    Serial.println("ERROR -> Allocation failed");
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
size_t count = _bar.count();  // 2          

// CAUTION: Do not initialize or assign a S_ptr with the raw pointer from 
// another S_ptr; undefined behavior.
DuinoMemory::S_ptr<Bar> _error{ _bar.get() };
_error = _bar.get();

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

`U_ptr` and `S_ptr` function like their `STL` counterparts.

```C++
/**
 * Warning:
 *   Dereferencing a null SmartPointer (* or ->) leads to 
 *   undefined behavior.
 *   Always check that the pointer is valid before dereferencing:
 *       if (ptr) { ptr->method(); }
 */
if (_foo)
{
    // Dereference operator *
    Foo copy = *_foo;
}

if (_bar)
{
    // Dereference operator ->
    _bar->method();
}

// Get the raw pointer (can be nullptr).
Bar* ptr = _bar.get();

// get reference count (S_ptr only). If either raw pointer or reference count is 
// nullptr, returns 0.
size_t ref_count = _bar.count();
```

### Operator bool

The `bool` operator checks whether the smart pointer's raw pointer is null.
```C++
if (_foo)               // Same as if (_foo != nullptr)
{
    do_something();
}

if (!_foo)              // Same as if (_foo == nullptr)
{
    crash(ErrorCode::NULLPTR);
}

// Works also with S_ptr
if (_bar)
{
    _bar->do_something();
}

if (!_bar)
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

## License

MIT - see [LICENSE](LICENSE)