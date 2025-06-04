A collection of lessons learned on this journey.


### 2025-06-04

_A couple of hours with the AI can save you from a couple of minutes reading the docs..._

https://github.com/WebAssembly/tool-conventions/blob/main/BasicCABI.md


### 2025-06-04

NVG doesn't crash, but it doesn't draw anything either.
Started by dumping this weird struct address stuff to see what's coming over in
these 16 bytes. No match.
So I ignore the AI's "advice" and just use the raw address that's "off by 16 bytes after crossing the WASM ABI".
Data is a match for the struct contents before WASM sends it over the fence.
There is NO weird "tail of" struct and sizeof() offset weirdness.
Structs as params are just sent as ptr's to the struct base address.
The address may (or may not, I guess) be shifted.
Weird.


### 2025-06-01

Setting up bindings for the nanovg proxy. The host functions need to accept a struct param.
The binding functions dont accept it and we need a custom binding wrapper. (See LinkHelper.h)
A struct param is unpacked and passed by WASM calling conventions as follows:


In WebAssembly’s C-ABI (as produced by Clang/LLVM targeting wasm32), simple scalars (i32, i64, floats) map directly to Wasm parameters. Any struct or aggregate that doesn’t fit entirely in a single 32- or 64-bit register is “lowered” to memory: the caller allocates a stack slot large enough for the entire struct and then passes a single i32 (the address of that slot) to the callee. In your case, a `struct { int a,b,c,d; }` is 16 bytes (> 8 bytes), so it doesn’t get split into four separate i32s. Instead:

1. **Caller-side:**

   * Compute `slot = sp – sizeof(struct)` (i.e. subtract 16 from the current stack pointer).
   * Copy each of the four ints into memory at `slot + offsetof(field)`.
   * Pass `slot` (an i32) as the one argument to the Wasm function.

2. **Callee-side:**

   * Receives that single i32 as a pointer to memory.
   * Inside the function, the compiler treats it like a normal by-value struct: it will read from `*(MyStruct *)arg_ptr`.

Because your WAT showed exactly one i32 whose value was “old SP minus 16,” that i32 is just the pointer to the caller’s stack-allocated copy of the struct. If the struct were small enough (≤ 8 bytes), LLVM would instead pack its fields into one i32 or i64 parameter. But for four 32-bit ints (16 bytes total), it falls back to “memory by reference,” hence only a single i32 appears in the WAT.



