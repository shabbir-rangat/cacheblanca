# cacheblanca

A Redis-protocol-compatible in-memory cache server, built from scratch in C++ — no networking or parsing libraries, just raw sockets, `epoll`, and a hand-written RESP parser.

## Why

I built this to actually understand what happens under the hood of a system like Redis: non-blocking I/O with an event loop, wire-protocol parsing byte by byte, and the design trade-offs that come with a single-threaded server handling many concurrent clients. It speaks real RESP (REdis Serialization Protocol), so you can point the standard `redis-cli` at it directly.

## Features

- Non-blocking TCP server using `epoll` (single-threaded event loop)
- Hand-written RESP parser for multi-bulk arrays (`decode_command`)
- Commands: `SET` (with optional `EX <seconds>` expiry), `GET`, `DEL`
- Lazy key expiration (checked on access, not via a background sweep)
- Compatible with `redis-cli` — no custom client required

## Architecture

```
client (redis-cli)
      │
      ▼
  epoll event loop  ──▶  accept new connections
      │
      ▼
  recv() into per-client read buffer
      │
      ▼
  decode_command()   — parses RESP bytes into a `command { args }`
      │
      ▼
  dispatch()          — routes by command name (SET/GET/DEL)
      │
      ▼
  eval*() functions    — apply the operation against the in-memory store
      │
      ▼
  write_buffer         — RESP-encoded reply
      │
      ▼
  send() back to client
```

Each client connection gets its own read/write buffers so partial reads (a command split across multiple TCP packets) and pipelined commands (several commands in one packet) are both handled correctly — the parser reports how many bytes it consumed, and the event loop keeps decoding until the buffer is exhausted or incomplete.

## Building

Requires a C++20 compiler and CMake ≥ 3.10.

```bash
cmake -S . -B build
cmake --build build
```

## Running

```bash
./build/my_program
```

The server listens on port `54000`. Connect with the standard Redis CLI:

```bash
redis-cli -h 127.0.0.1 -p 54000
```

```
127.0.0.1:54000> SET foo bar
OK
127.0.0.1:54000> GET foo
"bar"
127.0.0.1:54000> SET foo bar EX 60
OK
127.0.0.1:54000> DEL foo
(integer) 1
```

## Roadmap

-  Partial-write handling on the send path (`EPOLLOUT`-driven flush for large replies)
-  More commands (`EXISTS`, `EXPIRE`, `TTL`, `INCR`)
-  Persistence (snapshotting or an append-only log)
-  Benchmarks against real Redis (throughput, latency under load)


