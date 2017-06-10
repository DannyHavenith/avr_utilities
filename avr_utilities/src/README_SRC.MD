About this directory
====================

This directory contains source (.cpp) files disguised as header files. Client projects are
expected to include the files that they need *only once* in a source file that is otherwise 
empty. This way, the sources will be built against all the project settings.