# Design Patterns Notes

> 2017/7 - 10
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

#### [no-toc] [no-number] &sect; Roles

- **Abstract Factory** construct **Abstract Product**
- **Concrete Factory** construct **Concrete Product** family

#### [no-toc] [no-number] &sect; Process

- **Client** call methods of **Abstract Factory**
- **Concrete Factory** create **Concrete Product** family
  - by using **Factory Method** (subclassing, compile-time)
  - by using **Prototype** (composition, runtime)
- **Client** retrieve **Abstract Product** in the same family

#### [no-toc] [no-number] &sect; Info Hidden

- **Abstract Factory** not know **Concrete Product**
- **Client** not know **Concrete Factory**
- **Client** not know **Concrete Product**

#### [no-toc] [no-number] &sect; Uses

- Cross-platform solutions (different platform with different family of component)

### Factory Method

> Subclass to **defer instantiation** of concrete product

#### [no-toc] [no-number] &sect; Roles

- **Abstract Creator** create **Abstract Product**
- **Concrete Creator** create **Concrete Product**

#### [no-toc] [no-number] &sect; Process

- **Client** call factory method of **Abstract Creator**
- **Abstract Creator** defer instantiation to **Concrete Creator**
- **Concrete Creator** create subclass-specific **Concrete Product**
- **Client** retrieve **Abstract Product**

#### [no-toc] [no-number] &sect; Info Hidden

- **Abstract Creator** not know **Concrete Product**
- **Client** not know **Concrete Creator**
- **Client** not know **Concrete Product**

#### [no-toc] [no-number] &sect; Uses

- Creating event handler
  - (different component create component-specific handler)
  - (different handler handle the same event on different component)

### Prototype

> Prototype to **clone** new product

#### [no-toc] [no-number] &sect; Roles

- **Abstract Prototype** delegate clone to **Concrete Prototype**

#### [no-toc] [no-number] &sect; Process

- **Client** call clone method of **Abstract Prototype**
- **Abstract Prototype** defer construction to **Concrete Prototype**
- **Concrete Prototype** clone itself and initialize the copy
- **Client** retrieve **Abstract Prototype**

#### [no-toc] [no-number] &sect; Info Hidden

- **Abstract Prototype** not know **Concrete Prototype**
- **Client** not know **Concrete Prototype**

#### [no-toc] [no-number] &sect; Uses

- Palette
  - (clone prototype on the palette to create component)
  - (clone composited or decorated component)
- Registry with key
  - (dependency lookup)
  - (clone prototype in manager to create component)

### Builder

> Seperate **construction** from **representation**

#### [no-toc] [no-number] &sect; Roles

- **Director** use the same construction process
- **Abstract Builder** handle the same build commands
- **Concrete Builder** build different products
- **Concrete Product** have different internal representations

#### [no-toc] [no-number] &sect; Process

- **Client** create **Director** and **Concrete Builder**
- **Client** config **Director** with **Concrete Builder**
- **Director** call methods of **Abstract Builder**
- **Client** retrieve **Concrete Product** from **Concrete Builder**

#### [no-toc] [no-number] &sect; Info Hidden

- **Director** not know **Concrete Builder**
- **Director** not know **Concrete Product**

#### [no-toc] [no-number] &sect; Uses

- Format conversion (the same structure vs. different target)
- Parser (the same token flow vs. different abstract syntax tree)

### Singleton

> Maintain **single instance** and config at **runtime**

#### [no-toc] [no-number] &sect; Roles

- **Singleton** define the interface to get instance
- **Singleton Subclass** represent various product

#### [no-toc] [no-number] &sect; Process

- **Client** call methods of **Singleton**
- **Singleton** construct instance of **Singleton** / **Singleton Subclass**
- **Client** retieve instance of **Singleton**

#### [no-toc] [no-number] &sect; Info Hidden

- **Client** not know **Singleton Subclass**

#### [no-toc] [no-number] &sect; Uses

- XXX manager (manage shared resources)
- Dependency lookup (store dependency objects)
- Solution to _static-virtual member function_

## Structural Patterns

### Adapter

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

- Focus on adapting **existing incompatible** interface (vs. [sec|Bridge] Bridge)

### Bridge

> Decouple **abstraction** from **implementation** to vary **independently**

#### [no-toc] [no-number] &sect; Roles

- **Abstraction** define client expected interface
- **Refinded Abstraction** extend **Abstraction** by subclassing
- **Abstract Implementor** provide operation primitives for **Abstraction**
- **Concrete Implementor** define implementation of operations

#### [no-toc] [no-number] &sect; Process

- **Client** call methods of **Abstraction** (or **Refinded Abstraction**)
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

- Focus on providing **stable abstraction** interface and varying **evoluating implementation** (vs. [sec|Adapter] Adapter)

### Composite

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

- Focus on **object aggregation** (vs. [sec|Decorator] Decorator)

### Decorator

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

- Focus on **adding responsibility**, **degenerate** composition (vs. [sec|Composite] Composite)
- Focus on **dynamically** adding responsibility **recursively** (vs. [sec|Proxy] Proxy)
- Chain responsibility **outside** (vs. [sec|Strategy] Strategy)

### Proxy

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

- Focus on **intercepting** access in simple relation (vs. [sec|Decorator] Decorator)

### Facade

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

### Flyweight

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

## Behavioral Patterns (Decouple Sender-Receiver)

### Command

> Keep **handler of receiver** from **request of sender**

#### [no-toc] [no-number] &sect; Roles

- **Abstract Command** define handling interface
- **Concrete Command** refer to receiver and track handling action
- **Invoker (sender)** issue request (async with **Client**)
- **Receiver** actually handle request

#### [no-toc] [no-number] &sect; Process

- **Client** create **Concrete Command** with **Receiver**
- **Client** config **Invoker** with **Concrete Command**
- **Invoker** call methods of **Abstract Command** (async)
- **Concrete Command** delegate handling to **Receiver**

#### [no-toc] [no-number] &sect; Info Hidden

- **Abstract Command** not know **Invoker** and **Receiver**
- **Invoker** not know **Concrete Command**
- **Receiver** not know **Abstract** and **Concrete Command**

#### [no-toc] [no-number] &sect; Uses

- Support transaction
- Support logging operations (for recreating crashes)
- Support undoable operation (with _Unexecute_ interface)
- Enrich raw callback function (_lambda_ and _functor_ in C++)

### Observer

> Decouple **one-to-many dependency** using **subscription-notification**

#### [no-toc] [no-number] &sect; Roles

- **Abstract Subject** provide interface to attach / detach **Observer**
- **Abstract Observer** provide interface to get notified from **Subject**
- **Concrete Subject** _publish to_ **Observer** (async with **Client**)
- **Concrete Observer** _subscribe to_ and react to **Subject**

#### [no-toc] [no-number] &sect; Process

- **Client** attach **Abstract Observer** to **Abstract Subject**
- **Concrete Subject** notify subscribed **Abstract Observer** (async)
  - called by **Subject** ifself (setter method)
  - called by **Client** (exteranl method call)
- **Concrete Observer** get and (handle or ignore) notification through
  - (Pull Model) querying state of its **Concrete Subject**
  - (Push Model) passing by **Concrete Subject** sender

#### [no-toc] [no-number] &sect; Info Hidden

- **Abstract** and **Concrete Subject** not know **Concrete Observer**
- **Abstract Observer** not know **Abstract** and **Concrete Subject**
- **Concrete Observer** not know **Abstract Subject**
- **Concrete Observer** not know **Concrete Subject** (Push Model only)

#### [no-toc] [no-number] &sect; Uses

- Model and view in MVC (view subscribing to model changes)

### Mediator

> **Centralize communication** between components

#### [no-toc] [no-number] &sect; Roles

- **Abstract Mediator**
  - define interface to receive **Colleague** notification
  - can be an [sec|Observer] _Observer_ (**Colleague** be _Subject_)
- **Concrete Mediator** establish binding with **Colleague**
- **Colleague** interact with **Abstract Mediator** (async with **Client**)

#### [no-toc] [no-number] &sect; Process

- **Client** create **Concrete Mediator** with **Colleague**
- **Colleage** send request to **Abstract Mediator** (async)
- **Concrete Mediator** send request to **Colleage**

#### [no-toc] [no-number] &sect; Info Hidden

- **Abstract Mediator** not know **Colleague**
- **Colleague** not know **Concrete Mediator**

#### [no-toc] [no-number] &sect; Uses

- Widget-Pane in GUI
  - (Widget be **Colleague** while Pane be **Mediator**)
  - (Pane: create widgets -> show component -> handle interaction)
- Message center in publish-subscribe system
  - (provide acyclic callback)
  - (clarify dependency between components)
- Decouple cyclic dependency for [sec|Observer] Observer
  - (introduce a **Mediator** to maintain dependency)
  - (dependent objects be _Subject_ while **Mediator** be _Observer_)

### Chain of Responsibility

> **Chain handlers** to hide sender from actual handler

#### [no-toc] [no-number] &sect; Roles

- **Abstract Handler** define handling interface
- **Concrete Handler** handle request or forward to successor

#### [no-toc] [no-number] &sect; Process

- **Client** call methods of **Abstract Handler**
- **Concrete Handler**
  - handle what it can handle
  - forward what it can't handle

#### [no-toc] [no-number] &sect; Info Hidden

- **Client** not know **Concrete Handler**
- **Client** not know who actually handle the request
- **Concrete Handler** not know its **Concrete Handler** successor

#### [no-toc] [no-number] &sect; Uses

- Handling user event (only handle what it can, and forward what it can't)
- Handling redraw event (only draw what is in its context)

## Behavioral Patterns (Encapsulate Behavior)

### Memento

> Encapsulate **object representation** to support undo

#### [no-toc] [no-number] &sect; Roles

- **Memento** represent a snapshot of **Originator**
- **Originator** create and restore **Memento**

#### [no-toc] [no-number] &sect; Process

- **Client** get **Memento** from **Originator**
- **Client** request restoration of **Originator** with **Memento** (async)
- **Originator** restore internal state in **Memento**

#### [no-toc] [no-number] &sect; Info Hidden

- **Client** not know representation of **Memento** / **Originator**
- **Originator** is _friend_ of **Memento** (tight coupling)
- **Memento** not know **Originator**

#### [no-toc] [no-number] &sect; Uses

- Undoable operation
  - Constraint solving system (state is solution)
  - Support state management in [sec|Command] Command
- Memento-based iteration ([sec|Iterator] Iterator)
  - Interface: `aggregate.next(iter_state)` vs. `iter.next()`
  - Reverse friendship between aggregate and iterator/state

### State

> Encapsulate **state-specific behavior** to **alter at runtime**

#### [no-toc] [no-number] &sect; Roles

- **Context** provide interface for **Client** and maintain **State** instance
- **Abstract State** provide interface for **Context**
- **Concrete State** implement state-specific behavior

#### [no-toc] [no-number] &sect; Process

- **Client** initialize **Context** with **Concrete State**
- **Client** call methods of **Context**
- **Context** defer request to **Abstract State**
- **Concrete State** handle request according to its state
- Transition to succeeding state
  - **Context** handle transition uniformly
  - **Concrete State** update **State** instance of **Context**

#### [no-toc] [no-number] &sect; Info Hidden

- **Context** is _friend_ of **Abstract State** (tight coupling)
- **Context** not know **Concrete State**

#### [no-toc] [no-number] &sect; Uses

- Simplify conditional statement (delegate behavior rather than check state)
- Automaton (behavior changes along with state changes)
- Polymorphic tool (different behavior in different context, like magic pen)

#### [no-toc] [no-number] &sect; Comparison

- Provide several methods, like [sec|Abstract Factory] Abstract Factory (vs. [sec|Strategy] Strategy)

### Strategy

> Encapsulate a **family of algorithm** with similar behavior

#### [no-toc] [no-number] &sect; Roles

- **Context** provide interface for **Client** and maintain **Strategy** instance
- **Abstract Strategy** provide common functionality interface of algorithm
- **Concrete Strategy** implement algorithm behavior

#### [no-toc] [no-number] &sect; Process

- **Client** config **Context** with **Concrete Strategy**
- **Client** call methods of **Context**
- **Context** delegate algorithm related behavior to **Abstract Strategy**

#### [no-toc] [no-number] &sect; Info Hidden

- **Context** not know **Concrete Strategy**
- **Abstract** and **Concrete Strategy** not know **Context** (only know information algorithm needed)

#### [no-toc] [no-number] &sect; Uses

- Simplify conditional statement (delegate behavior rather than check state)
- Strategy varies but doesn't take all responsibility for client (dependency injection)

#### [no-toc] [no-number] &sect; Comparison

- Provide single method, like [sec|Factory Method] Factory Method (vs. [sec|State] State)
- Use delegation to inverse control (IOC) (vs. [sec|Template Method] Template Method)
- Inject responsibility **inside** (vs. [sec|Decorator] Decorator)

### Template Method

> Abstract **algorithm's skeleton** and **subclass to override primitive**

#### [no-toc] [no-number] &sect; Roles

- **Abstract Class** define
  - _template method_ (public, non-virtual)
  - _primitive operation_ (protected, virtual, maybe pure virtual)
- **Concrete Class** override _primitive operation_

#### [no-toc] [no-number] &sect; Process

- **Client** call _template method_ of **Abstract Class**
- **Concrete Class** do specific work by overriding

#### [no-toc] [no-number] &sect; Info Hidden

- **Client** not know _primitive operation_

#### [no-toc] [no-number] &sect; Uses

- Framework (let user to override only some point, not template method)

#### [no-toc] [no-number] &sect; Comparison

- Use inheritance to inverse control (IOC) (vs. [sec|Strategy] Strategy)

## Behavioral Patterns (Access Aggregate)

- Aggregate be
  - collection (uniform-typed, list, array...)
  - composite (abstract-typed, [sec|Composite] Composite)
  - tuple (product-typed)

### Iterator

> Decouple **traversal on aggregate** from its **representation**

#### [no-toc] [no-number] &sect; Roles

- **Aggregate** define interface to create iterator ([sec|Factory Method] Factory Method)
- **Iterator** define basic iteration operations
  - External iterator
    - _First_ / _Next_ / _IsDone_ / _GetItem_
  - Internal iterator
    - [sec|Strategy] Strategy (pass strategy into _Traverse_ interface)
    - [sec|Template Method] Template Method (override _ProcessItem_ method)

#### [no-toc] [no-number] &sect; Process

- **Client** create **Iterator** of **Aggregate**
- **Client** call iteration methods of **Iterator**
- **Iterator** keep track of iteration and compute next position

#### [no-toc] [no-number] &sect; Info Hidden

- **Client** not know representation of **Aggregate** / **Iterator**
- **Iterator** is _friend_ of **Aggregate** (tight coupling)
- **Aggregate** not know **Iterator**

#### [no-toc] [no-number] &sect; Uses

- Sequential access of aggregate container (STL container)
- Stream operation (in/out stream opertions)

#### [no-toc] [no-number] &sect; Comparison

- Provide iteration over _collection_ (vs. [sec|Visitor] Visitor)

### Visitor

> Decouple **element of aggregate** from **operation** on them

#### [no-toc] [no-number] &sect; Roles

- **Abstract Visitor** provide interface to visit all **Concrete Element**
- **Concrete Visitor** implement operation on visiting **Concrete Element**
- **Abstract Element** provide interface to accept **Visitor**
- **Concrete Element** pass _this_ to **Visitor** on acception
- **Aggregate** provide interface to accept **Visitor** for **Client**

#### [no-toc] [no-number] &sect; Process

- **Client** create **Concrete Visitor**
- **Client** visit **Aggregate** using **Abstract Visitor**
- **Aggregate** pass **Abstract Visitor** to internal **Abstract Element**
- **Concrete Element** accept **Abstract Visitor** by passing _this_ to it
- **Concrete Visitor** access **Concrete Element** and accumulate result

#### [no-toc] [no-number] &sect; Info Hidden

- **Abstract** and **Concrete Visitor** _know all_ **Concrete Element**
- **Abstract** and **Concrete Element** not know **Concrete Visitor**

#### [no-toc] [no-number] &sect; Uses

- Solution to _double dispatch_ (type of **Element** data bind with type of **Visitor** action dynamically)

#### [no-toc] [no-number] &sect; Comparison

- Provide internal iteration over _composite_ and _tuple_ (vs. [sec|Iterator] Iterator)

### Interpreter

> Add **_Interpret_ interface** to abstract syntax tree (composite)

#### [no-toc] [no-number] &sect; Roles

- **Abstract Expression** define _Interpret_ interface
- **Nonterminal Expression** interpret nonterminal symbols in gramma
- **Terminal Expression** interpret terminal symbols in gramma
- **Context** provide input/output context for interpreter

#### [no-toc] [no-number] &sect; Process

- **Client** build **Abstract Expression** with **Nonterminal** and **Terminal Expression**
- **Client** initialize **Context** and start _Interpret_ on **Abstract Expression**
- **Nonterminal** and **Terminal Expression** store/access states in **Context**

#### [no-toc] [no-number] &sect; Info Hidden

No info hidden...

#### [no-toc] [no-number] &sect; Uses

- Match regular expressions
  - (context: pending sequence)
  - (slower than automaton solution)
- Evaluate expressions
  - (context: variables in expressions)
  - (evaluate constrants in compiler)
  - (evaluate varialbes in interpreter)

#### [no-toc] [no-number] &sect; Comparison

- Provide _Interpret_ (only) iteration over abstract syntax tree (only) (vs. [sec|Visitor] Visitor)
