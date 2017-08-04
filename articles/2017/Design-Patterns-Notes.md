# Design Patterns Notes

> 2017/7
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

- **Client 1** create **Concrete Factory**
- **Client 1** pass **Concrete Factory** as **Abstract Factory** to **Client 2**
- **Client 2** call methods of **Abstract Factory**
- **Concrete Factory** create **Concrete Product** family
  - by using **Factory Method** (subclassing, compile-time)
  - by using **Prototype** (composition, runtime)
- **Client 2** retrieve **Abstract Product** in the same family

#### [no-toc] Info Hidden

- **Abstract Factory** not know **Concrete Product**
- **Client 2** not know **Concrete Factory**
- **Client 2** not know **Concrete Product**

#### [no-toc] Uses

- Cross-platform solutions (different platform with different family of component)

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
- Parse syntax-tree (the same tranversal vs. different outcome)

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

### Singleton

> **Single** instance with global access

#### [no-toc] Roles

- **Singleton** define the interface of `Instance`
- **Singleton Subclass** represent various product

#### [no-toc] Process

- **Client** call `Instance` of **Singleton**
- **Singleton** construct instance of **Singleton** / **Singleton Subclass**
- **Client** retieve instance of **Singleton**

#### [no-toc] Info Hidden

- Client not know **Singleton Subclass**

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
- Implementation + memory management (all instances share the same implemenation)
