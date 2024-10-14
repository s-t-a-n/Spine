# Spine units

This library is meant to be used to cross functional-spaces. It is not meant to be fast, just meant to be accurate.

For a function such as `foo(litre amount)` this library allows to transparently pass `litre_ml` and not have to think
twice.
Inside of this function one can call `amount.raw()` and be sure to get the exact (as far as floating points allow)
amount in litre.

A much more exhaustive alternative to this library is made by Nic Holthaus and can be
found [here on github](https://github.com/nholthaus/units).