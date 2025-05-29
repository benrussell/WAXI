# WAXI - WebAssembly X-Plane Interface

This is a small SDK layer that lets us create X-Plane plugins with WASM.

"Make it exist first. You can make it good later."

## What is WASM and why do I care?
WebAssembly (WASM) is a binary instruction format for a stack-based virtual machine, designed for safe and efficient execution.

- High performance due to near-native execution speeds.
- Sandboxed memory ensures that crashes are contained within the WebAssembly module, preventing them from affecting the host application.
- Sandboxed file I/O protects user data by restricting access to only explicitly allowed files and directories.
- Cross-platform compatibility ensures plugins work on multiple operating systems.


## Building this plugin
You will need wasmtime sources to build the WASM host plugin project.

Pre-built binaries will be made available.


## Compiling your code into WASM
You will need clang and the clang WASI SDK extensions to compile to WASM byte code.

See the [wasm_plugin](https://github.com/benrussell/wasm_plugin) project for more information.


