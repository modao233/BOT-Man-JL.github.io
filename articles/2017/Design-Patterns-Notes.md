# Design Patterns Notes

> 2017/7
>
> Encapsulate the concept that varies. ¡ª¡ª GOF

## Golden Rules

> Program to an interface, not an implementation.
>
> Favor object composition over class inheritance.
>
> Consider what need to change over time.

## Creational Patterns

### Abstract Factory

> Abstract creation of a family of products

#### Roles

- **Abstract Factory** construct **Abstract Product**
- **Concrete Factory** construct **Concrete Product** family

#### Process

- **Client 1** create **Concrete Factory**
- **Client 1** pass **Abstract Factory** to **Client 2**
- **Client 2** call methods of **Abstract Factory**
- **Client 2** retrieve **Abstract Product**

#### Info Hidden

- **Abstract Factory** not know **Concrete Product**
- **Client 2** not know **Concrete Factory**
- **Client 2** not know **Concrete Product**

#### Uses

- Cross-platform solutions

### Builder

> Seperate construction from representation

#### Roles

- **Director** use the same construction process
- **Abstract Builder** handle the same build commands
- **Concrete Builder** build different products
- **Concrete Product** have different internal representations

#### Process

- **Client** create **Director** and **Concrete Builder**
- **Client** config **Director** with **Concrete Builder**
- **Director** call methods of **Abstract Builder**
- **Client** retrieve **Concrete Product** from **Concrete Builder**

#### Info Hidden

- **Director** not know **Concrete Builder**
- **Director** not know **Concrete Product**

#### Uses

- Format conversion
- Parse syntax-tree

