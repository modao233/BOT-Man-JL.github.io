# Design Patterns Notes

> 2017/7 - 9
>
> Encapsulate the concept that varies. —— GOF

[heading-numbering]

## [no-toc] [no-number] TOC

[TOC]

## Golden Rules

> Program to an interface, not an implementation.
>
> Favor object composition over class inheritance.
>
> Consider what need to change over time.

## Creational Patterns

### Abstract Factory

> Abstract the creation of a **family** of product

#### [no-toc] Roles

- **Abstract Factory** construct **Abstract Product**
- **Concrete Factory** construct **Concrete Product** family

#### [no-toc] Process

- **Client** call methods of **Abstract Factory**
- **Concrete Factory** create **Concrete Product** family
  - by using **Factory Method** (subclassing, compile-time)
  - by using **Prototype** (composition, runtime)
- **Client** retrieve **Abstract Product** in the same family

#### [no-toc] Info Hidden

- **Abstract Factory** not know **Concrete Product**
- **Client** not know **Concrete Factory**
- **Client** not know **Concrete Product**

#### [no-toc] Uses

- Cross-platform solutions (different platform with different family of component)

### Factory Method

> Subclass to **defer instantiation** of concrete product

#### [no-toc] Roles

- **Abstract Creator** create **Abstract Product**
- **Concrete Creator** create **Concrete Product**

#### [no-toc] Process

- **Client** call factory method of **Abstract Creator**
- **Abstract Creator** defer instantiation to **Concrete Creator**
- **Concrete Creator** create subclass-specific **Concrete Product**
- **Client** retrieve **Abstract Product**

#### [no-toc] Info Hidden

- **Abstract Creator** not know **Concrete Product**
- **Client** not know **Concrete Creator**
- **Client** not know **Concrete Product**

#### [no-toc] Uses

- Creating event handler
  - (different component create component-specific handler)
  - (different handler know how to handle the same event on different component)

### Prototype

> Prototype to **clone** new product

#### [no-toc] Roles

- **Abstract Prototype** delegate clone to **Concrete Prototype**

#### [no-toc] Process

- **Client** call clone method of **Abstract Prototype**
- **Abstract Prototype** defer construction to **Concrete Prototype**
- **Concrete Prototype** clone itself and initialize the copy
- **Client** retrieve **Abstract Prototype**

#### [no-toc] Info Hidden

- **Abstract Prototype** not know **Concrete Prototype**
- **Client** not know **Concrete Prototype**

#### [no-toc] Uses

- Palette
  - (clone prototype on the palette to create component)
  - (clone composited or decorated component)
- Registry with key (dependency lookup, clone prototype in manager to create component)

### Builder

> Seperate **construction** from **representation**

#### [no-toc] Roles

- **Director** use the same construction process
- **Abstract Builder** handle the same build commands
- **Concrete Builder** build different products
- **Concrete Product** have different internal representations

#### [no-toc] Process

- **Client** create **Director** and **Concrete Builder**
- **Client** config **Director** with **Concrete Builder**
- **Director** call methods of **Abstract Builder**
- **Client** retrieve **Concrete Product** from **Concrete Builder**

#### [no-toc] Info Hidden

- **Director** not know **Concrete Builder**
- **Director** not know **Concrete Product**

#### [no-toc] Uses

- Format conversion (the same structure vs. different target)
- Parser (the same token flow vs. different abstract syntax tree)

### Singleton

> **Single** instance with global access

#### [no-toc] Roles

- **Singleton** define the interface to get instance
- **Singleton Subclass** represent various product

#### [no-toc] Process

- **Client** call method of **Singleton**
- **Singleton** construct instance of **Singleton** / **Singleton Subclass**
- **Client** retieve instance of **Singleton**

#### [no-toc] Info Hidden

- **Client** not know **Singleton Subclass**

#### [no-toc] Uses

- XXX manager (manage shared resources)
- Dependency lookup (store dependency objects)

## Structural Patterns

### Adapter

> **Convert** existing **interface** to expected

#### [no-toc] Roles

- **Target** define the expected new interface
- **Adaptee** expose the existing interface
- **Adapter** show **Target**'s interface, delegate to **Adaptee**
  - public inherit the interface of **Target**
  - private inherit the implementation of / compose an instance of **Adaptee**

#### [no-toc] Process

- **Client** call methods of **Target** on **Adapter**
- **Adapter** defer request to **Adaptee**

#### [no-toc] Info Hidden

- **Client** not know **Adaptee**
- **Target** and **Adaptee** not know **Adapter**
- **Target** and **Adaptee** not know each other

#### [no-toc] Uses

- Mix alternative library to existing toolkit (different interface)
- View model (convert models to what views expect)
- Focus on adapting **existing incompatible** interface (vs. [sec|Bridge] Bridge)

### Bridge

> Decouple **abstraction** from **implementation** to vary **independently**

#### [no-toc] Roles

- **Abstraction** define client expected interface
- **Refinded Abstraction** extend **Abstraction** by subclassing
- **Abstract Implementor** provide operation primitives for **Abstraction**
- **Concrete Implementor** define implementation of operations

#### [no-toc] Process

- **Client** call methods of **Abstraction** (or **Refinded Abstraction**)
- **Abstraction** defer request to **Abstract Implementor**
- **Abstract Implementor** let **Concrete Implementor** do specific operation

#### [no-toc] Info Hidden

- **Client** not know and not dependend on **Concrete Implementor**
- **Abstraction** not know **Concrete Implementor**
- **Abstract Implementor** not know **Concrete Implementor**
- **Abstract Implementor** and **Concrete Implementor** not know **Abstraction**

#### [no-toc] Uses

- Cross-platform solutions (the same abstraction component with different platform implementation)
- Dynamic select optimal implementation (determine according to environment status)
- Implementation with memory management (all instances share the same implemenation)
- Focus on providing **stable abstraction** interface and varying **evoluating implementation** (vs. [sec|Adapter] Adapter)

### Composite

> Define **part-whole interface** with **uniform** operation

#### [no-toc] Roles

- **Component** define interface for both composite and primitive
- **Primitive** define leaf component
- **Composite** define composition of components

#### [no-toc] Process

- **Client** call methods of **Component**
- **Primitive** handle request directly
- **Composite** defer request to **Component** (and perform additional operations)

#### [no-toc] Info Hidden

- **Client** not know **Primitive** and **Composite** (when requesting)
- **Component** not know **Primitive** and **Composite** (sometimes know **Abstract Composite** to refer to parent)
- **Primitive** and **Composite** not know each other

#### [no-toc] Uses

- GUI component (handle composite and primitive uniformly)
- Tasks (one task can contain others)
- Abstract syntax tree (with different gramma component)
- Focus on **object aggregation** (vs. [sec|Decorator] Decorator)

### Decorator

> **Attach responsiblity** from outside without changing interface and implementation

#### [no-toc] Roles

- **Abstract Component** define uniform interface
- **Concrete Component** define component entity
- **Abstract Decorator** define interface that conform to **Abstract Component** (optional)
- **Concrete Decorator** define additional responsibility

#### [no-toc] Process

- **Client** establish concrete decorated **Abstract Component**
- **Client** call methods of **Abstract Component**
- **Concrete Component** handle request as normal
- **Concrete Decorator** defer request to **Abstract Component** and perform additional operations

#### [no-toc] Info Hidden

- **Client** and **Abstract Component** not know **Concrete Component** and **Decorator**
- **Concrete Component** and **Decorator** not know each other

#### [no-toc] Uses

- GUI component (adding border and scrollbar to a view)
- Pipeline operation (encoding and encrypting before writing to file)
- Focus on **adding responsibility** (vs. [sec|Composite] Composite, **degenerate** composition)
- Focus on **dynamically** adding responsibility **recursively** (vs. [sec|Proxy] Proxy)
- Chain operations **outside** (vs. Strategy)

### Proxy

> **Intercept access** by proxy in the same interface

#### [no-toc] Roles

- **Abstract Subject** define uniform interface
- **Concrete Subject** define concrete object
- **Proxy** act like a **Abstract Subject** and intercept request to **Concrete Subject**

#### [no-toc] Process

- **Client** call methods of **Abstract Subject**
- **Concrete Subject** handle request as normal
- **Proxy** defer request to another **Abstract Subject** under its control

#### [no-toc] Info Hidden

- **Client** and **Abstract Subject** not know **Concrete Subject** and **Proxy**
- **Concrete Subject** not know **Proxy**
- **Proxy** may know **Concrete Subject** or not

#### [no-toc] Uses

- Remote calling (remote proxy, **not** keep reference to **Concrete Subject**)
- Optimize expensive action (virtual proxy, **may** keep reference to **Concrete Subject**)
  - Lazy loading
  - Copy on Write
- Access control (protection proxy, keep reference to **Concrete Subject**)
  - Restricted access
  - Read/Write Lock
- Smart pointer (resource management, keep reference to **Concrete Subject**)
- Focus on **intercepting** access in simple relation (vs. [sec|Decorator] Decorator)

### Facade

> **Encapsulate** subsystems into a **high-level interface**

#### [no-toc] Roles

- **Facade** define client-interested interface
- **Subsystem** define actual bussiness

#### [no-toc] Process

- **Client** call methods of **Facade** (or **Subsystem** directly)
- **Facade** defer actual works to **Subsystem**

#### [no-toc] Info Hidden

- **Subsystem** not know **Facade**

#### [no-toc] Uses

- Compiler's _Compile_ interface (hide lexer / parser details)
- Large systems (define layers by layered facades)

### Flyweight

> **Share** substantial **fine-grained objects**

#### [no-toc] Roles

- **Abstract Flyweight** define flyweight interface accepting extrinsic state
- **Concrete Flyweight** define sharable flyweight, storing intrinsic state
- **Flyweight Factory** create and manage flyweight object

#### [no-toc] Process

- **Client** retrieve **Abstract Flyweight** from **Flyweight Factory**
- **Client** store and compute extrinsic state and pass to **Abstract Flyweight**
- **Concrete Flyweight** combine intrinsic and extrinsic state to work

#### [no-toc] Info Hidden

- **Abstract Flyweight** not know **Concrete Flyweight**

#### [no-toc] Uses

- GUI component (sharing bitmap and style)
- State and Strategy object (retrieve by key)

## Behavioral Patterns

### Command

> **Encapsulate request** to decouple sender from handler

#### [no-toc] Roles

- **Abstract Command** define handling interface
- **Concrete Command** store receiver and its action
- **Invoker** issue request (async with **Client**)
- **Receiver** actually handle request

#### [no-toc] Process

- **Client** create **Concrete Command** with **Receiver**
- **Client** config **Invoker** with **Concrete Command**
- **Invoker** call methods of **Abstract Command** (async)
- **Concrete Command** delegate handling to **Receiver**

#### [no-toc] Info Hidden

- **Abstract Command** not know **Invoker** and **Receiver**
- **Invoker** not know **Concrete Command**
- **Receiver** not know **Abstract** and **Concrete Command**

#### [no-toc] Uses

- Support transaction
- Support logging operations (for recreating crashes)
- Support undoable operation (with _Unexecute_ interface)
- Enrich raw callback function (_lambda_ and _functor_ in C++)

### Chain of Responsibility

> **Chain handlers** to decouple sender from handler

#### [no-toc] Roles

- **Abstract Handler** define handling interface
- **Concrete Handler** handle request or forward to successor

#### [no-toc] Process

- **Client** call methods of **Abstract Handler**
- **Concrete Handler**
  - handle what it can handle
  - forward what it can't handle

#### [no-toc] Info Hidden

- **Client** not know **Concrete Handler**
- **Client** not know who actually handle the request
- **Concrete Handler** not know its **Concrete Handler** successor

#### [no-toc] Uses

- Handling user event (only handle what it can, and forward what it can't)
- Handling redraw event (only draw what is in its context)

### Interpreter

> Add **_Interpret_ interface** to abstract syntax tree ([sec|Composite] Composite)

#### [no-toc] Roles

- **Abstract Expression** define _Interpret_ interface
- **Nonterminal Expression** interpret nonterminal symbols in gramma
- **Terminal Expression** interpret terminal symbols in gramma
- **Context** provide input/output context for interpreter

#### [no-toc] Process

- **Client** build AST of **Nonterminal** and **Terminal Expression**
- **Client** initialize **Context** and start _Interpret_ on **Abstract Expression**
- **Nonterminal** and **Terminal Expression** store/access states in **Context**

#### [no-toc] Info Hidden

No info hidden...

#### [no-toc] Uses

- Match regular expressions
  - (context: pending sequence)
  - (slower than automaton solution)
- Evaluate expressions
  - (context: variables in expressions)
  - (evaluate constrants in compiler)
  - (evaluate varialbes in interpreter)

### Iterator

> Decouple **aggregate traversal** from **representation**

#### [no-toc] Roles

- **Aggregate** define interface to create iterator ([sec|Factory Method] Factory Method)
- **Iterator** define basic iteration operations
  - External iterator
    - _First_ / _Next_ / _IsDone_ / _GetItem_
  - Internal iterator
    - [sec|Strategy] Strategy (pass strategy into _Traverse_ interface)
    - [sec|Template Method] Template Method (override _ProcessItem_ method)

#### [no-toc] Process

- **Client** create **Iterator** of **Aggregate**
- **Client** call iteration methods of **Iterator**
- **Iterator** keep track of iteration and compute next position

#### [no-toc] Info Hidden

- **Client** not know representation of **Aggregate** / **Iterator**
- **Iterator** is _friend_ of **Aggregate** (tight coupling)
- **Aggregate** not know **Iterator**

#### [no-toc] Uses

- Sequential access of aggregate container (STL container)
- Stream operation (in/out stream opertions)

### Mediator

> **Encapsulate interaction** between components

#### [no-toc] Roles

- **Abstract Mediator** define interface
  - to receive **Colleague** notification
  - or act as [sec|Observer] Observer
- **Concrete Mediator** establish binding with **Colleague**
- **Colleague** interact with **Abstract Mediator** (async with **Client**)

#### [no-toc] Process

- **Client** create **Concrete Mediator** with **Colleague**
- **Colleage** send request to **Abstract Mediator** (async)
- **Concrete Mediator** send request to **Colleage**

#### [no-toc] Info Hidden

- **Colleague** not know **Concrete Mediator**

#### [no-toc] Uses

- Form/Pane/Window (create widgets -> show component -> handle interaction)
- Message center in publish-subscribe system

### Memento

> **Encapsulate representation** to restore/undo later

#### [no-toc] Roles

- **Memento** represent a snapshot of **Originator**
- **Originator** create and restore **Memento**

#### [no-toc] Process

- **Client** get **Memento** from **Originator**
- **Client** request restoration of **Originator** with **Memento** (async)
- **Originator** restore internal state in **Memento**

#### [no-toc] Info Hidden

- **Client** not know representation of **Memento** / **Originator**
- **Originator** is _friend_ of **Memento** (tight coupling)
- **Memento** not know **Originator**

#### [no-toc] Uses

- Undoable operation
  - Constraint solving system (state is solution)
  - Support state management in [sec|Command] Command
- Memento-based iteration ([sec|Iterator] Iterator)
  - Interface: `aggregate.next(iter_state)` vs. `iter.next()`
  - Reverse friendship between aggregate and iterator/state

### Observer

#### [no-toc] Roles
#### [no-toc] Process
#### [no-toc] Info Hidden
#### [no-toc] Uses

### State

#### [no-toc] Roles
#### [no-toc] Process
#### [no-toc] Info Hidden
#### [no-toc] Uses

### Strategy

#### [no-toc] Roles
#### [no-toc] Process
#### [no-toc] Info Hidden
#### [no-toc] Uses

### Template Method

#### [no-toc] Roles
#### [no-toc] Process
#### [no-toc] Info Hidden
#### [no-toc] Uses

### Visitor

#### [no-toc] Roles
#### [no-toc] Process
#### [no-toc] Info Hidden
#### [no-toc] Uses
