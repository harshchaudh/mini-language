# CITS2002 Project 1: Assumptions and Clarifications
All samples, with the exception of `sample05.ml`, work. However, slight changes to `sample05.ml` will allow the file to be executable.

Example:
```
# 18 is printed
#
function foo a b
	print a + b
#
foo(12, 6)
```

## Clarifications
- Identifier names cannot contain keywords such as, `print` and `function`.<br> As such `sample05.ml` does not work.
- The `ml` program does not accept any command line arguments. However, `runml` does, `./runml <pathToFile>`.