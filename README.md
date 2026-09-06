# mikro Client (yet another RouterOS interface)

A simple interface to a RouterOS instance. It exploits the socket-based API of the OS.
I've written this program mainly for educational purposes, as I wanted to show that C
can be used to write pretty neat CLI interfaces. As of the last commit, it is only possible
to look at the raw byte sequences that are exchanged between the participating nodes

## Build

create the build environment with

```bash
cmake -B .
```

within the project root and, continue with the build process itself

```bash
cmake --build .
```

> [!NOTE]
> the 3.22.1 version of CMake is the version that I use on my PC. It can be set to another one

> [!NOTE]
> I've forced the use of the C language, as this is a C project and, some hosts may not provide other compilers

## Use

the program binary will be mikroClient (built within the project root)

any CLI argument has the following format: --<option-name>\[=<option-value>\]

| option | value |
| --- | --- |
| help | concatenates the command synopsis to the current output stream and terminates execution, regardless of any other inserted option |
| authentication-data-dir | directory that will contain data necessary for the TLS handshake (currently unused) |
| key-file | file name of the public key used during the TLS handshake (currently unused) |
| crt-file | file name of the certificate used during the TLS handshake (currently unused) |
| host | the target RouterOS host (an IPv4 address) |
| port | can be either __8728__ or 8729 (for the last one, I've yet to support secure data exchange) |
| name | the user-name (mandatory for the authentication procedure) |
| password | the password (currently, it is also mandatory for the authentication procedure) |
| command | the RouterOS CLI version of the command that is to be executed. The first character has to be _/_ and any white space will be substituted with the slash character |
| attribute-name | a command attribute |
| attribute-value | the value corresponding to the attribute |

multiple attribute-name / attribute-value pairs can be specified. An attribute value cannot be specified without the corresponding name.
Moreover, an attribute may be value-less

### some random notes (to be expanded)

- the naming convention for the source code is a bit mixed-up, for a purpose. Generally, anything that starts with a mikro\_ prefix is related to the socket-based protocol or the CLI;
- phy\_ names are related to the socket interface (I've used the same module that I've implemented for my main AMR project but, I've adapted it a bit);
- generic macros and/or functions do not have any particular prefix
- the log prefix for decoded words is **mikro\_W:**
- the log prefix for encoded sentences is **encSent:**

### architecture

[Software architecture]{docs/mikrotik_api_client.md}
