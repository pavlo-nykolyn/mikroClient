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
