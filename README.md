# gprintf
This library provides a function which can be used to print things to a 
target other than a string or a FILE pointer.

My main objective in creating this library was to provide a readable
implementation so that improvements and bug fixes would be easier later on.
I think I have succeeded in that.

The library consists of a single file 'gp.c'. This file may be compiled
on its own or may be included in another file. All symbols defined are
prefixed with 'gprintf_' therefore there should be no name clashes.

The functions defined are as follows:
```
typedef int (*gprintf_func_t)(void * , const void *, size_t);
int vgprintf(gprintf_func_t func, void *arg, const char *fmt, va_list *ap);
int gprintf(gprintf_func_t func, void *arg, const char *fmt, ...);
```
The arguments to func are:
- the provided 'arg'
- a buffer to read bytes from
- number of bytes to output

The return value should be 0 if there was an error doing the output. Otherwise,
the number of bytes written should be returned. I know the return type isn't
size_t, I should fix that.

## Features

The library supports:
- printing wide characters and strings.
- using POSIX-style m$ arguments, precisions and widths.
- printing floating points in all formats, including hexadecimal.

It doesn't recognize an overflow condition due to the output being too large.
i.e. when the return value doesn't fit into an integer.

It doesn't do grouping either, using the ' flag.

## Compilation

There are three options you can set to enable glibc compatible behaviour:
- gprintf_OPTION_denormal_glib
- gprintf_OPTION_dollar_resets_flags_glib
- gprintf_OPTION_pointer_precision_glib

You can set all three by defining the macro gprintf_glibc_compat.

### Denormalized Numbers

These are numbers so small in magnitude that, the regular floating point
format isn't sufficient to encode them. So, a special encoding is used.

When printing such numbers, the glibc implementation stays true to the
machine encoding. The rounding afterwards is done based on that. The musl
implementation normalizes the number. This results in more precise numbers
in general. However, the exponent resulting from the latter is sometimes
outside of the limits for the underlying floating point type.

An example call and its output is shown for the two options:
```
printf("%.0a",  0x0.008fffp-1022);
[libc] <0x0p-1022>
[musl] <0x1p-1031>
```
It's worth noting that the glibc output contains a non-zero exponent
even though the mantissa is zero.

### Flags in Dollar Style Arguments

When using dollar style arguments, it's not clear where the flags should
be. My implementation allows for them both before and after the argument
specification. For instance, without glibc compatibility:
```
printf("[%-1$*2$s]", "Left Adjusted?", 30);
[self]  <[Left Adjusted?                ]>
[libc]  <[%-1$*2$s]>    // error
[musl]  <>              // error
```
In some instances, the glibc allows flags before the argument specification
but such flags are not taken into account. The provided option enables
that behaviour.

Compatibility in this case is still a work in progress.

### Pointer Precision

The musl implementation provides a default precision and the hexadecimal
prefix when printing pointers. The glibc implementation doesn't do any
of that. The option mentioned above enables the glibc implementation.

### Invalid Sequences

The C specification says that an invalid format sequence results in 
undefined behaviour. There are three options to choose from when such
an erronous input is encountered.

First is to ignore the erronous specification and print it as it is, without
using any arguments. glibc does this. This is potentially quite dangerous
because the arguments to the function may no longer match the format specifiers
after skipping the offending part. For instance:
```
printf("Calculating.. %O times %s is: %d\n", 300, "Hello", 0);
```
Here, glibc would skip the %O and then read 300 as a pointer due to the '%s'
format specifier. This would likely end up crashing the program.

The second approach is to print until the offending specifier and ignore
the rest of the format string. This is what musl does. The above call would
print the string "Calculating.. " in this case.

The third option is to print nothing and return an error. This is the safest
and correct way. The library implements this approach. 
