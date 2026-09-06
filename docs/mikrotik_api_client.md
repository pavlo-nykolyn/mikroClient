# Software Architecture

the main modules that do participate to the program logic are:

- *mikroClient*
- *mikro\_cli\_parser*
- *mikro\_word*
- *mikro\_string*
- *phy\_socket* (I use the source file name to identify the module)

I've summarized the inter-module interactions in the following picture:

![Architecture](docs/mikrotik_api_client.png)

*mikroClient* uses *mikro\_cli\_parser* in order to both parse and retrieve the CLI
options (indicated as _argName_ and _argValue_ within the graph, instances of the
internal string defined by *mikro\_string*). Then it uses the functionalities of
*mikro\_Word* to encode both the authentication sentence and the command sentence.
Finally, *phy\_socket* provides the socket interface (based on Berkley sockets though,
with few modifications for the Windows-based interface) through which packets
are first written as commands and then read as replies. Finally, *mikro\_word* also
provides a recursive decoding feature for the replies.

## Test suite

I've launched the program without a proper test suite for several features of the program. I've rectified this grievous mistake
by adding the mikro_validation module within the validation sub-directory. The list file found within the directory can be used
to build the test suite. The resulting mikroValidator executable can be used to run the suite. If an assertion fails, than something
must be wrong.
Currently, only the following functions are interested by the suite:

- _mikro\_Word\_decodeSz_ ;
- _mikro\_word\_encodeSz_ (I haven't exposed it within the module interface so, I commented out the suite code I wrote for it)