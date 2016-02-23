/**
@page minDic_manual MinDic Manual

@section compileMD Use the program compileMD to create automata from text lists
In the directory INSTALL_PREFIX/bin you find a program @c compileMD. Use it like:

@code
compileMD foo.lex foo.mdic
@endcode
or
@code
./program_that_prints_lex_to_stdout | compileMD --stdin=1 foo.mdic
@endcode
In the following, whatever is said about "the .lex file" (especially about encoding and sorting), of course, also applies 
to data that is piped to compileMD via stdin. 

A .lex file is a text file of the form:
@code
anna#42
bill#50
charlie#54
@endcode
That is, an alphabetically sorted list of keys, followed by a '#' delimiter symbol and some integer value
which is stored with the key. If delimiter and integer are omitted, the value 0 will be annotated with each key.
The specified integer value has no pre-defined meaning - it simply will be returned when the respective string is 
looked up. So you can use the annotation to remember that Bill is 50 years old, or that Anna's favourite number is 42,
or that the string 'charlie' appears 54 times in your corpus.

It is important that the .lex file is utf8 encoded and sorted properly. You can sort your dictionary with
the tool @c sortLex included in this package. Much faster is the following unix command:
@code
LC_ALL=C sort -t'#' -k1,1 foo.unsorted > foo.lex
@endcode

The resulting .mdic file is a binary representation of the dictionary automaton and can now be used with other csl tools.


*/
