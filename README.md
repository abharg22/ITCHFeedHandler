# ITCH Feed Handler

A C++20 feed handler that parses a full day of real NASDAQ TotalView-ITCH 5.0 market data and reconstructs the full order book (every individual order) for every symbol.

> **Status:** work in progress. Performance numbers will be added here only once they are measured. See [`docs/RESULTS.md`](docs/RESULTS.md).

## The problem

Every trading firm has to turn a high-volume stream of exchange messages into an accurate, low-latency view of the market. If the feed handler is slow or wrong, every strategy and risk system downstream is slow or wrong.

This project:

- decodes the binary ITCH 5.0 protocol (big-endian, fixed-point prices, nanosecond timestamps);
- rebuilds a per-symbol limit order book from adds, executions, cancels, deletes and replaces;
- prints the best bid/offer, the top price levels and the trade tape at any time of day;
- measures throughput and latency percentiles (p50 / p99 / p99.9);
- proves correctness with runtime invariants, differential testing against a reference book, fuzzing and sanitizers.

## Repository layout

```
include/itch/   public headers (message types, parser, order book, strong types)
src/            library implementation
apps/           itch_replay command-line tool
tests/          GoogleTest unit and differential tests
bench/          Google Benchmark micro-benchmarks
fuzz/           libFuzzer target for the parser
docs/           design notes, results, data instructions
data/           local ITCH files (git-ignored)
```

## Roadmap

| Phase | Focus |
|---|---|
| 0 | Build system, CI, market microstructure foundations |
| 1 | Binary parsing of every ITCH 5.0 message type |
| 2 | Message dispatch, symbol table, trading-state tracking |
| 3 | Reference order book, book snapshots, trade tape |
| 4 | Invariant checks, unit tests, sanitizers, fuzzing |
| 5 | Profiling and optimization, differential testing |
| 6 | Design write-up and final results |

## Data

ITCH sample files are large and are not stored in this repository. See [`docs/DATA.md`](docs/DATA.md) for where to download them and where to put them.

## Workflow

Work is planned as stories (`ITCH-0XX`). Each story gets its own branch (`feature/ITCH-0XX-short-name`) and pull request, and commit messages reference the story ID.

## License

[MIT](LICENSE)
