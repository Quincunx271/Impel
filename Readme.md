# Impel

Unified static and dynamic nonintrusive polymorphism.
Provides "Rust Traits" as customization points for C++,
for both static polymorphism and dynamic polymorphism.

## Why Impel as opposed to ...?

### Regular inheritance

Regular inheritance is intrusive, meaning that you cannot implement traits (interfaces) for types which you do not own.

### Other dynamic polymorphism libraries

Impel works for any C++ interface.
You do not need to declare the interface using some special library-specific syntax.
This means that Impel can be used for interfaces that you do not own, and that code using Impel is less esoteric.
