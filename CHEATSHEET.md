# DuinoMemory quick cheat sheet

```C++
// Create
auto up = DuinoMemory::make_unique<Foo>();
auto sp = DuinoMemory::make_shared<Foo>();

// Share (S_ptr only)
DuinoMemory::S_ptr<Foo> sp2 = sp;

// Transfer ownership (U_ptr)
DuinoMemory::U_ptr<Foo> up2 = up.release();

// Reset / destroy
up = nullptr;
sp = nullptr;

// Check before use
if (sp) { sp->method(); }

// Reference count
size_t n = sp.count();
```

## DO
- Use `make_unique` / `make_shared`.
- Copy `S_ptr` to share ownership.
- Use `.release()` to transfer `U_ptr`.
- Check pointer before dereferencing.

## DON'T
```C++
Foo* raw = new Foo();

S_ptr<Foo> a{ raw };
S_ptr<Foo> b{ raw };    // ❌ double delete

S_ptr<Foo> c{ a.get() }; // ❌ undefined behavior
```
### Embedded rules
- Avoid frequent creation/destruction of `S_ptr`
- Never use smart pointers inside ISRs
- Prefer static or long-lived objects on small MCUs
