# Structural Patterns Notes

> Reading in 2017/8
>
> Favor object composition over class inheritance. —— GOF

[heading-numbering]

## [no-toc] [no-number] Related Notes

- [Creational Patterns Notes](Design-Patterns-Notes-1.md)
- Structural Patterns Notes
- [Behavioral Patterns Notes](Design-Patterns-Notes-3.md)

## [no-toc] [no-number] Covered Patterns

[TOC]

## Adapter

> Convert **existing interface** to **conform new interface**

#### [no-toc] [no-number] &sect; Roles

- **Target** define the expected new interface
- **Adaptee** expose the existing interface
- **Adapter** show **Target**'s interface, delegate to **Adaptee**
  - public inherit the interface of **Target**
  - private inherit the implementation of / compose an instance of **Adaptee**

#### [no-toc] [no-number] &sect; Process

- **Client** call methods of **Target** on **Adapter**
- **Adapter** defer request to **Adaptee**

#### [no-toc] [no-number] &sect; Info Hidden

- **Client** not know **Adaptee**
- **Target** and **Adaptee** not know **Adapter**
- **Target** and **Adaptee** not know each other

#### [no-toc] [no-number] &sect; Uses

- Mix alternative library to existing toolkit (different interface)
- View model (convert models to what views expect)

#### [no-toc] [no-number] &sect; Comparison

- vs. [sec|Bridge] Bridge
  - Focus on adapting **existing incompatible** interface

## Bridge

> Decouple **abstraction** from **implementation** to vary independently

#### [no-toc] [no-number] &sect; Roles

- **Abstraction** define client expected interface
- **Abstract Implementor** provide operation primitives for **Abstraction**
- **Concrete Implementor** define implementation of operations

#### [no-toc] [no-number] &sect; Process

- **Client** call methods of **Abstraction**
- **Abstraction** defer request to **Abstract Implementor**
- **Abstract Implementor** let **Concrete Implementor** do specific operation

#### [no-toc] [no-number] &sect; Info Hidden

- **Client** not know and not dependend on **Concrete Implementor**
- **Abstraction** not know **Concrete Implementor**
- **Abstract Implementor** not know **Concrete Implementor**
- **Abstract Implementor** and **Concrete Implementor** not know **Abstraction**

#### [no-toc] [no-number] &sect; Uses

- Cross-platform solutions (the same abstraction component with different platform implementation)
- Dynamic select optimal implementation (determine according to environment status)
- Implementation with memory management (all instances share the same implemenation)

#### [no-toc] [no-number] &sect; Comparison

- vs. [sec|Adapter] Adapter
  - Focus on providing **stable abstraction** interface
  - Vary independent **evoluating implementation**

## Composite

> Define **part-whole interface** with **uniform** operation

#### [no-toc] [no-number] &sect; Roles

- **Component** define interface for both composite and primitive
- **Primitive** define leaf component
- **Composite** define composition of components

#### [no-toc] [no-number] &sect; Process

- **Client** call methods of **Component**
- **Primitive** handle request directly
- **Composite** defer request to **Component** (and perform additional operations)

#### [no-toc] [no-number] &sect; Info Hidden

- **Client** not know **Primitive** and **Composite** (when requesting)
- **Component** not know **Primitive** and **Composite** (sometimes know **Abstract Composite** to refer to parent)
- **Primitive** and **Composite** not know each other

#### [no-toc] [no-number] &sect; Uses

- GUI component (handle composite and primitive uniformly)
- Tasks (one task can contain others)
- Abstract syntax tree (with different gramma component)

#### [no-toc] [no-number] &sect; Comparison

- vs. [sec|Decorator] Decorator
  - Focus on **object aggregation**

## Decorator

> **Attach responsiblity** from outside without changing interface

#### [no-toc] [no-number] &sect; Roles

- **Abstract Component** define uniform interface
- **Concrete Component** define component entity
- **Abstract Decorator** define interface conforming to **Abstract Component** (optional)
- **Concrete Decorator** define additional responsibility

#### [no-toc] [no-number] &sect; Process

- **Client** establish concrete decorated **Abstract Component**
- **Client** call methods of **Abstract Component**
- **Concrete Component** handle request as normal
- **Concrete Decorator** defer request to **Abstract Component** and perform additional operations

#### [no-toc] [no-number] &sect; Info Hidden

- **Client** and **Abstract Component** not know **Concrete Component** and **Decorator**
- **Concrete Component** and **Decorator** not know each other

#### [no-toc] [no-number] &sect; Uses

- GUI component (adding border and scrollbar to a view)
- Pipeline operation (encoding and encrypting before writing to file)

#### [no-toc] [no-number] &sect; Comparison

- vs. [sec|Composite] Composite
  - Focus on **adding responsibility**, **degenerate** composition
- vs. [sec|Proxy] Proxy
  - Focus on **dynamically** adding responsibility **recursively**

## Proxy

> **Intercept access** and provide the same interface

#### [no-toc] [no-number] &sect; Roles

- **Abstract Subject** define uniform interface
- **Concrete Subject** define concrete object
- **Proxy** act like a **Abstract Subject** and intercept request to **Concrete Subject**

#### [no-toc] [no-number] &sect; Process

- **Client** call methods of **Abstract Subject**
- **Concrete Subject** handle request as normal
- **Proxy** defer request to another **Abstract Subject** under its control

#### [no-toc] [no-number] &sect; Info Hidden

- **Client** and **Abstract Subject** not know **Concrete Subject** and **Proxy**
- **Concrete Subject** not know **Proxy**
- **Proxy** may know **Concrete Subject** or not

#### [no-toc] [no-number] &sect; Uses

- Remote calling (remote proxy, **not** keep reference to **Concrete Subject**)
- Optimize expensive action (virtual proxy, **may** keep reference to **Concrete Subject**)
  - Lazy loading
  - Copy on Write
- Access control (protection proxy, keep reference to **Concrete Subject**)
  - Restricted access
  - Read/Write Lock
- Smart pointer (resource management, keep reference to **Concrete Subject**)

#### [no-toc] [no-number] &sect; Comparison

- vs. [sec|Decorator] Decorator
  - Focus on **intercepting** access in simple relation

## Facade

> **Encapsulate subsystems** to provide **high-level interface**

#### [no-toc] [no-number] &sect; Roles

- **Facade** define client-interested interface
- **Subsystem** define actual bussiness

#### [no-toc] [no-number] &sect; Process

- **Client** call methods of **Facade** (or **Subsystem** directly)
- **Facade** defer actual works to **Subsystem**

#### [no-toc] [no-number] &sect; Info Hidden

- **Subsystem** not know **Facade**

#### [no-toc] [no-number] &sect; Uses

- Compiler's _Compile_ interface (hide lexer / parser details)
- Large systems (define layers by layered facades)

## Flyweight

> **Share** substantial **fine-grained objects**

#### [no-toc] [no-number] &sect; Roles

- **Abstract Flyweight** define flyweight interface accepting extrinsic state
- **Concrete Flyweight** define sharable flyweight, storing intrinsic state
- **Flyweight Factory** create and manage flyweight object

#### [no-toc] [no-number] &sect; Process

- **Client** retrieve **Abstract Flyweight** from **Flyweight Factory**
- **Client** store and compute extrinsic state and pass to **Abstract Flyweight**
- **Concrete Flyweight** combine intrinsic and extrinsic state to work

#### [no-toc] [no-number] &sect; Info Hidden

- **Abstract Flyweight** not know **Concrete Flyweight**

#### [no-toc] [no-number] &sect; Uses

- GUI component (sharing bitmap and style)
- State and Strategy object (retrieve by key)
