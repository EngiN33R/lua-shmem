# lua-shmem

Low-level Lua convenience wrapper for IPC shared memory functionality.
Wraps calls to mmap/munmap to create and free shared memory, and
memset/memcpy calls to write and clear the shared memory area.

## Example

```lua
local shmem = require("shmem")

local mem = shmem.new(1024) -- 1 KiB shared memory
mem:write("hello world")
print(mem:read()) -- hello world
```

## API

| Function | Returns | Description |
| -------- | ------- | ----------- |
| `shmem.new(size:number)` | `shmem` | Creates new shared memory area of `size` bytes. |
| `shmem:free()` | `boolean, number` | Frees shared memory area. Returns `true` if successful, `false` and error code if not. |
| `shmem:write(data:string)` | None | Copies `data` to start of shared memory. |
| `shmem:fill(data:string[, fill:number])` | None | Copies `data` to shared memory and pads the rest of the memory area with `fill` or 0. |
| `shmem:clear()` | None | Fills entire shared memory area with 0. |
| `shmem:read()` | `string` | Reads contents of shared memory. |

## TODO

- [ ] Cursor-based write to non-start positions in memory similar to Lua's `io`
