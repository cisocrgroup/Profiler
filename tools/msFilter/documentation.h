/**
@page msFilter_manual msFilter Manual

msFilter is a command line tool that uses csl::MSMatch to perform approximate search on dictionaries. 
The underlying technology uses Universal Levenshtein Automata as described in the following publication:<br>
"S. Mihov and K. Schulz. Fast approximate search in large dictionaries. Computational Linguistics, 30, 2004."<br>
Please refer to the class documentation of csl::LevDEA and csl::MSMatch for details.

@section msFilter_manual_compilation Compilation of the dictionary into binary file
Before using msFilter to perform approximate search in a dictionary, this dictionary has to be compiled into a binary file
representing a finite state automaton.

Refer to the @link fbDic_manual FBDic Manual @endlink to get details about 
the specified format of your source dictionary and the simple procedure to compile it into a binary file.

@section msFilter_manual_usage Usage of the tool msFilter
msFilter should be called like this:

@code
msFilter [--machineReadable=1] <lev_distance> <foo.fbdic>
@endcode
<tt>&lt;lev_distance&gt;</tt> specifies the distance threshold. Accepted values are [0..3]<br>
<tt>&lt;foo.fbdic&gt;</tt> is a binary dictionary computed by compileFBDic. See above.

The program reads queries (1 per line) from STDIN and returns for each query the set of results.
Each result is a triple of the form <tt>&lt;word,distance,annotation&gt;</tt>. Here is a sample call and result for
a frequency-annotated German dictionary and the query <tt>kild</tt> passed on over the console:

@code
$ ./bin/msFilter 1 ./lex_de.frq.fbdic
kild
bild,1,780977
kid,1,7888
kill,1,10427
kiln,1,151
kilo,1,15658
kilt,1,378
kind,1,303991
mild,1,14935
wild,1,55648
@endcode
This output mode is the default and is designed to be readable for human eyes.

@subsection msFilter_manual_machine_readable Machine-readable output
Often the output of msFilter is supposed to be parsed by some script - in this case, choose the option <tt>--humanReadable=1</tt>
for msFilter to return all matches in one line, separated by '|'. Usually, this kind of output is much easier to handle and parse - 
note that in this mode you will receive exactly one line as an answer for each query. In case of an empty answer set an empty line ("\n")
is returned.

@code
$ ./bin/msFilter --machineReadable=1 1 ~/IMPACT/lexika/staticlex_de/staticlex_de.frq.fbdic
kild
bild,1,780977|kid,1,7888|kill,1,10427|kiln,1,151|kilo,1,15658|kilt,1,378|kind,1,303991|mild,1,14935|wild,1,55648
@endcode

*/
