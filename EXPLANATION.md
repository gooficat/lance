`\f.f`
is equivalent to this javascript
`(f) => f`
or in pre-es6
`function(f) {return f}`

`(\f.fx)`
is equivalent to
`((f) => f)(x)`
ie a function call

variables are 1 letter long
`x`

the way lambda calculus is "interpreted" is by collapsing it into the most reduced form
so, this
`(\f.fx)`
turns into
`x`
because it is a function that returns the argument, called with `x`, which is an "unbound" variable,
that does not come from a function's arguments, so it has no assigned value

there is no other syntax in this language
there are no operators, no numbers
everything is syntax.
you represent numbers with a certain pattern of uncalled functions
