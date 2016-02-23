/**
@page vaam_manual Vaam Manual
csl::Vaam is a module for approximate matching in large dictionaries, considering
spelling variants (defined with a set of rewrite patterns) and also spelling errors
(using levenshtein distance).
 
This section is meant to be an application-oriented extension of the class-reference to
make the beginnings of work with Vaam easier.

@section previous_knowlwedge Previous Knowledge: Compiling dictionary automata
Vaam heavily relies on finite state technology. So before working with the tool, the 
dictionary at hand has to be compiled into a minimized deterministic FSA. In the csl library
the type csl::MinDic provides e functionality to create such automata and also to store them
on a hard disk in order to reload it into the memory for usage. Read the @link minDic_manual MinDic manual @endlink  to learn more about the
compilation and usage of dictionaries.

@section basic_usage Basic Usage
@subsection configuration Configuration
Vaam's most important configuration parameters are
- which dictionary to use as base. It is obligatory to pass a reference to a MinDic-object in the constructor (csl::Vaam::Vaam).
- which set of patterns to use. It is obligatory to specify a path to a set of patterns in the constructor (csl::Vaam::Vaam).

Other, optional configuration parameters are used to fine-tune the behaviour of csl::Vaam
- an upper bound for the number of patterns to be applied to one word (csl::Vaam::setMaxNrOfPatterns)
- an upper bound for the number of edit operations (deletion, substitution, insertion) to be applied to 
  the word on top of the pattern applications.  (csl::Vaam::setDistance)

A ready-to-use Vaam object might be created like this:

@subsection Queries
Querying a word is easy. However there's a bit more to say about understanding the answer.

The method csl::Vaam:query asks for two arguments: the query token and a pointer to an answer object. The latter must
be of the type std::vector< Interpretation >* . This is where Vaam will store the possible interpretations of the query
string. The example program csl/Vaam/example.cxx demonstrates how to interpret the structure of an Interpretation object.

*
*/
