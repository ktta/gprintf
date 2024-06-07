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
- gprintf_OPTION_denormal_glibc
- gprintf_OPTION_dollar_resets_flags_glibc
- gprintf_OPTION_pointer_precision_glibc

You can set all three by defining the macro gprintf_glibc_compat.

## Denormal Numbers

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

## Flags For Dollar Style Arguments

When using dollar style arguments, it's not clear where the flags should
be. My implementation allows for them both before and after the argument
specification. For instance, without glibc compatibility:
```
printf("[%-1$*2$s]", "Left Adjusted?", 30);
[self]  <[Left Adjusted?                ]>
[libc]  <[%-1$*2$s]>    // error
[musl]  <>              // error
```
In some instances, glibc allows flags before the argument specification
but such flags are not taken into account. The provided option enables
that behaviour.

Compatibility in this case is still a work in progress.

## Pointer Precision

The musl implementation provides a default precision and the hexadecimal
prefix when printing pointers. The glibc implementation doesn't do any
of that. The option mentioned above enables the glibc implementation.

## Invalid Sequences

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
format specifier. This would likely crash the program.

The second approach is to print until the offending specifier and ignore
the rest of the format string. This is what musl does. The above call would
print the string "Calculating.. " in this case. 

In both of these choices, there is no way of detecting the error since the
function is still printing some bytes.

The third option is to print nothing and return an error. This is the safest
and correct way. The library implements this approach and sets errno to
EINVAL when it encounters invalid formats.

## Type Safety

Dollar style arguments make it possible to reuse an argument multiple times. 
This can be another source of bugs if care is not taken. 

Let's look at a potentially problematic call:
```
printf("%1$*1$s", "Hello");
```
The pointer to the string would also be interpreted as an integer, resulting
in a potentially huge width. Neither glibc nor musl detects such misuse. My
library treats this as an invalid specification and returns an error.

In general, if an argument is used multiple times, all uses must imply the
same type for the argument. For instance, an integer is assumed for width
and precision values. This is compatible with an argument which is implied
to be an integer due to a '%d' formatter. 
```
printf("%1$*1$d", 42);
```
Is a valid use. 

Pointer types are exceptions to this rule. I did this to enable calls such as
```
printf("%1$p %1$s", "A string printed both ways");
```

More analysis about compatibility is needed. For instance, integers implied
by '%hhi' and '%d' are in fact compatible. They are the same size when
received by the function due to integer promotion. However, the library
currently disallows such multi-typed uses.

## Wide Strings

The specifiers '%lc' and '%ls' read wide characters and write them in 
multi-byte format. The library uses wcrtomb() for this purpose. This 
function relies on the locale setting LC_CTYPE in order to determine
the encoding of wide strings. In order to set this value, your program
should run
```
setlocale(LC_CTYPE, ""); 
```
before calling vgprintf(). 

## Other Locale Specific Quirks

glibc uses locale information to determine:
- the grouping separator for the ' flag
- the decimal point character for floating point numbers

musl doesn't do this at all and simply uses the US conventions. 

The focus of my library is to output strings that will be parsed by
other programs. Therefore, I also follow the musl strategy and use 
the '.' character as a decimal point.

## Things to Do

The output object shall contain a field which records the number of bytes
written so far. This will be used by the _out() function. This function
will also detect the condition where the output length grows beyond the
integer limit.

More analysis should be done on how strictly type safety should be enforced.
For example, is "%1$s %1$ls" valid? Pointers to different types of objects
could be incompatible due to alignment constraints. 

The grouping flag ' shall be implemented. 

An option to provide UTF-8 decoding regardless of setlocale() can be useful.
All modern machines probably use some sort of UTF-8 locale anyway.

Wide characters which can not be converted to multi-byte sequences are
currently replaced by the 'X' character. The POSIX specification says
that such characters should result in the EILSEQ error.

## Future Work

The glibc/musl extension '%m' can be quite useful. An mnemonic counterpart
'%M' could also be implemented. 

A couple of formatters for printing dates and times would be extremely useful.

A 'transport float' format could be nice. It would consist of a sign,
a hexadecimal integer and an exponent (base 2). Such an output could
potentially be parsed by existing tools.

Generic printers could be registered and then used as ("%lpNAME!", ptr).

Precision for strings should in fact specify characters, not bytes. This
would apply to both multibyte strings and wide strings.

## References

The POSIX specification:
 
https://pubs.opengroup.org/onlinepubs/9699919799/functions/fprintf.html

C specification:

https://en.cppreference.com/w/c/io/fprintf

musl implementation:

https://git.musl-libc.org/cgit/musl/tree/src/stdio/vfprintf.c

glibc implementation:

https://github.com/lattera/glibc/blob/master/stdio-common/vfprintf.c

For comments and patches:

cinarus (at) yahoo (dott) com
