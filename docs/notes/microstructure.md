# ITCH-001 · Market microstructure notes

## 1. What an exchange does; lit vs. dark venues

An exchange like Nasdaq does four jobs:

- Keeps an order book of bids and asks for each stock it trades.
- Matches buyers and sellers using the price-time priority rule.
- Publishes every execution.
- Sells its market data (for Nasdaq, through the ITCH feed).

**What ITCH covers:** Nasdaq's ITCH feed shows Nasdaq's book only. The same stock (e.g. AAPL) also trades on NYSE, Cboe and dark pools, each with its own book and its own feed, and none of that appears in ITCH. Within Nasdaq's book, ITCH is L3 data: every order, in price-time priority, which is enough to work out queue position. Orders are anonymous; to find my own order I match the order reference number Nasdaq returns through OUCH.

**Lit vs. dark:** A lit venue shows its book to everyone. A dark pool keeps orders hidden until they fill. A pension fund selling 2 million shares uses a dark pool so the order doesn't leak to the market and push the price down before it's filled (market impact). Dark pools have no visible book of their own, so they usually match at the midpoint of the lit market's best bid and ask (the NBBO).

## 2. Makers, takers, and how market makers earn (and lose) money

- **Maker:** the order already resting on the book. It provides liquidity and usually earns a rebate (whoever placed it, not only market makers).
- **Taker:** the incoming order that trades against a resting order. It removes liquidity and usually pays a fee.
- **One order can be both.** Book: ask 100.02 × 300, bid 100.00 × 600. A limit buy for 500 @ 100.02 takes the 300 shares at 100.02 immediately (taker), and the remaining 200 rest on the book (maker), becoming the new best bid at 100.02.

**How a market maker earns:** it quotes both sides and earns the spread. Quoting 100.00 bid / 100.02 ask and filling 100 shares on each side earns 100 × $0.02 = $2 before fees and rebates.

**How a market maker loses:**
- **Adverse selection:** trading against someone better informed. An informed seller sweeps the whole bid level, my order fills, and the price drops right after.
- **Inventory risk:** holding a position while the price moves. I buy 100 at 100.00, nobody buys it back, the price drifts to 99.90, and I've lost $10.

## 3. Order types and quote terms

| Term | Meaning |
|---|---|
| Limit order | Buy/sell at this price or better. Trades only if the other side has a price that reaches it; otherwise it rests on the book. |
| Market order | Trade now at the best available prices. Never rests; can walk through several levels. |
| Bid / ask | Highest price a buyer will pay / lowest price a seller will accept. |
| Spread | ask − bid, in dollars and in ticks. |
| Mid | (bid + ask) / 2. Always between the bid and the ask. |
| Tick size | Smallest price increment ($0.01 for US stocks ≥ $1). |

**Worked example.** Book: asks 50.18 × 400, 50.15 × 200, 50.14 × 300; bids 50.10 × 500, 50.09 × 100. Tick $0.01.

- Best bid 50.10, best ask 50.14, spread $0.04 = 4 ticks, mid 50.12.
- **Market buy 600** walks the book: 300 @ 50.14, 200 @ 50.15, 100 @ 50.18. New best ask: 50.18 (300 left).
- **Limit buy 200 @ 50.12** (original book) does not trade: no ask at 50.12 or lower. It rests and becomes the new best bid, so the spread narrows to $0.02 = 2 ticks. Being the best bid means first in line for the next seller, not a trade.

**Prices are integers.** ITCH sends prices with 4 implied decimals: 50.14 → `501400`, 50.15 → `501500` (divide by 10,000 for dollars). Binary floating point can't store most decimal prices exactly (`0.1 + 0.2 != 0.3`), which breaks price-level lookups, equality checks and running totals. My order book keeps prices as integers end to end and only converts to dollars for display.

## 4. The limit order book and price-time priority

The book is two sorted sides of resting limit orders: bids highest price first, asks lowest price first. **Price-time priority** means orders are ranked by price first, then by arrival time within each price level (FIFO).

**Example.** Bids arrive: A 100.00 × 300, B 100.01 × 100, C 100.00 × 200, D 100.01 × 400, E 100.00 × 100.

```
Bid 100.01:  B 100  →  D 400
Bid 100.00:  A 300  →  C 200  →  E 100
```

- **Market sell 700:** B 100 @ 100.01, D 400 @ 100.01, A 200 @ 100.00. Left: `100.00: A 100 → C 200 → E 100`. A is partly filled and keeps its place at the front.
- **A reprices to 100.01:** it goes to the back of 100.01, behind B and D. Changing price makes it a new order, so it loses time priority (in ITCH: a Replace message `U` with a new order reference number).

**Other rules exist.** CME uses pro-rata matching for some futures books: an incoming order is split by resting size, not arrival time. Level A 300, C 200, E 100 with an incoming sell 300 → FIFO gives A 300; pro-rata gives A 150, C 100, E 50. Under pro-rata, size matters more than speed.

## 5. Queue position and why it's valuable

Everyone at a price level gets the same price, so queue position doesn't buy a better price. It decides **whether** and **which** fills I get.

**Example.** I bid 100 @ 100.00 with 600 shares ahead of me.

- **Fill probability.** Before I fill, the 600 ahead must go: many small sells and cancels over the day (e.g. four 100-share sells + 200 cancelled), or one large sell. At the front, the very next 100-share sell fills me.
- **Adverse selection.** If I'm at the back and get filled all at once by a single 700-share sell, that seller probably knows something: the level was swept and the price is likely to drop, so my new position is worth less than I paid. Front-of-queue fills come mostly from small, ordinary orders, so they're safer.
- **Queue position is easy to lose.** Moving my bid from 100.00 to 100.01 costs one tick of edge (a smaller spread for me) and gives up my time priority at 100.00. The trade-off: if 100.01 is empty, I become first in line at a new best bid.

This is why firms pay for L3 data: only order-level data lets me track how many shares are ahead of me.

## 6. L1 / L2 / L3, and ITCH vs. OUCH

Same book three ways:

```
L1 (top of book)    Bid 100.01 × 500    Ask 100.03 × 400

L2 (by price, MBP)  Ask 100.03 × 400
                    Bid 100.01 × 500
                    Bid 100.00 × 600

L3 (by order, MBO)  Ask 100.03: F 250 → G 150
                    Bid 100.01: B 100 → D 400
                    Bid 100.00: A 300 → C 200 → E 100
```

- **L1:** best bid and best ask with their total sizes.
- **L2:** total size at every price level. No queues, no individual orders, no IDs.
- **L3:** every order with its reference number, in queue order, plus every add, cancel and execute.

From L2 I can get total size per level and the spread. I need L3 for the number of orders at a level, how many shares are ahead of a given order, and each order's history. L3 → L2 → L1 can be derived; the reverse can't.

**ITCH vs. OUCH:**
- **ITCH** flows **out** of Nasdaq to every subscriber: L3 market data for every order on Nasdaq's book (UDP multicast). Nasdaq sells it.
- **OUCH** flows **in** from my firm to Nasdaq: entering, replacing and cancelling orders, with confirmations back for **my orders only** (TCP).
- To find my own order in ITCH: OUCH's Order Accepted message gives the **order reference number**, and ITCH uses the same number.

## 7. How this maps to my code

Every change to Nasdaq's book arrives as one ITCH message. Every message starts with a **Stock Locate** code that says which stock (and so which book) it belongs to.

| Type | Name | Effect on the book |
|---|---|---|
| `A` | Add order | New order rests on the book (side, shares, price, order ref) |
| `F` | Add order with MPID | Same as `A`, plus the firm that placed it |
| `E` | Order executed | Resting order traded some/all shares at its own price |
| `C` | Executed with price | Traded at a price different from its resting price |
| `X` | Order cancel | Some shares cancelled; order keeps its queue place |
| `D` | Order delete | Whole order removed |
| `U` | Order replace | Old order removed; new ref, price and size, back of the queue |

**Worked example.** Start: `Bid 100.00: ref1 300 → ref3 200 → ref5 100`.

```
E ref1 300 exec        Bid 100.00: ref3 200 → ref5 100
X ref3 50 cancelled    Bid 100.00: ref3 150 → ref5 100
A ref7 Buy 100@100.00  Bid 100.00: ref3 150 → ref5 100 → ref7 100
U ref5 → ref8 100@100.01
                       Bid 100.01: ref8 100
                       Bid 100.00: ref3 150 → ref7 100
D ref3                 Bid 100.01: ref8 100
                       Bid 100.00: ref7 100
```

**Why `X` keeps priority but `U` doesn't:** cutting my own size doesn't hurt anyone behind me, so the order keeps its place. A replace can change the price (or raise the size); keeping priority would be unfair to orders already waiting there, so it's treated as a new order at the back.

**The core data structure.** `E`, `X`, `D` and `U` carry only the order ref number, not the price or side. The handler must remember those from the earlier `A`/`F`:

```
On A/F:  orders[ref] = { locate, side, price, shares }
On E:    o = orders[ref]; o.shares -= executed   // trade at o.price
         if o.shares == 0: remove ref from its level and the map
On X:    o = orders[ref]; o.shares -= cancelled
         if o.shares == 0: remove ref from its level and the map
On D:    remove orders[ref] from its level and the map
On U:    old = orders[old_ref]                    // U has no side field
         add new_ref = { old.locate, old.side, new price, new shares }
           at the back of the new price level
         remove old_ref from its level and the map
```

A fully executed or fully cancelled order gets no separate `D` message, so the handler must remove it itself when its shares reach 0.

One book per Stock Locate, and one map from order ref → order across the whole feed.
