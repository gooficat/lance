### lance - a lambda calculus interpreter

--- note: this project is being actively worked!

This is a lambda calculus interpreter written in the c++ language.
The syntax is as follows:
- variables are single latin letters, i.e. `x` or `r`
- abstractions, a.k.a. anonymous functions, are defined as `\param.body`, ie `\x.r` or `\z.z`
- applications, a.k.a. function calls, are defined as `(abstractionargument)`, no space, i.e. `(\x.xy)`

The interpreter will reprint your expression with a slightly different syntax (which uses spaces between the abstraction and argument of an application, and the greek lambda symbol instead of a backslash), followed by each successive reduction step until no further reductions are possible. 

It is important to note that there is no brace elision, unlike many lambda calculus interpreters. This is a very deliberate design choice.

known issues:
- lack of error handling for malformed expressions
- no deletion-after-substitution
