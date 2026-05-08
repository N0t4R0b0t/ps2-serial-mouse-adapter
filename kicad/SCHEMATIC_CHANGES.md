# PS/2 to Serial Mouse Adapter — Schematic Changes

This document describes the component changes made to the original design.

---

## U2 — RS232 Level Converter: MAX232 → MAX3232 Module

### What changed

The original design used a discrete **MAX232** IC in a DIP-16 socket with four external
charge-pump capacitors (C1, C3, C4, C5 — 1 µF each). This was replaced by a pre-built
**HiLetgo MAX3232 HW-027 module** (15 × 11 mm, ~$1 each in quantity) that integrates the
MAX3232 IC and all required capacitors on a small PCB. This eliminates the need for the
external capacitors and simplifies assembly significantly.

### Module description

The HW-027 is a castellated module — it sits embedded in a rectangular slot cut into the
PCB (Edge.Cuts outline in the footprint). The board outline of the module is
**15 mm × 11 mm**. A 1 mm margin is added on each side giving a courtyard of
**17 mm × 13 mm**.

The module exposes connections on all four edges:

- **Left short edge (RS232 side)** — two castellated pads at 2.54 mm pitch.  
  Each pad position exposes two layers (CH1 on F.Cu, CH2 on B.Cu at the same XY):
  - Upper pad: RXD1 input (F.Cu) / TXD2 output (B.Cu)
  - Lower pad: TXD1 output (F.Cu) / RXD2 input (B.Cu)

- **Right short edge (TTL side)** — same castellated layout, mirrored:
  - Upper pad: TTL_RXD1 (F.Cu) / TTL_TXD2 (B.Cu)
  - Lower pad: TTL_TXD1 (F.Cu) / TTL_RXD2 (B.Cu)

- **Corners (power)** — four through-hole pads (1.7 mm pad, 1.0 mm drill), shared both
  faces, at the four corners of the module:
  - Top-left: VCC (pad 1, square)
  - Bottom-left: GND (pad 2)
  - Top-right: VCC (pad 3)
  - Bottom-right: GND (pad 4)

### Pin assignments

| Pad | Name | Side | Function |
|-----|------|------|----------|
| 1 | VCC | TH corner | 3.3 V – 5 V supply |
| 2 | GND | TH corner | Ground |
| 3 | VCC | TH corner | 3.3 V – 5 V supply (duplicate) |
| 4 | GND | TH corner | Ground (duplicate) |
| 5 | RS232_RXD1 | Left edge F.Cu | RS232 input → CH1 receiver |
| 6 | RS232_TXD1 | Left edge F.Cu | RS232 output ← CH1 driver |
| 7 | RS232_RXD2 | Left edge B.Cu | RS232 input → CH2 receiver |
| 8 | RS232_TXD2 | Left edge B.Cu | RS232 output ← CH2 driver |
| 9 | TTL_TXD1 | Right edge F.Cu | TTL output from CH1 receiver |
| 10 | TTL_RXD1 | Right edge F.Cu | TTL input to CH1 driver |
| 11 | TTL_TXD2 | Right edge B.Cu | TTL output from CH2 receiver |
| 12 | TTL_RXD2 | Right edge B.Cu | TTL input to CH2 driver |

### Schematic connections still needed

The symbol for U2 is placed but the pins are not yet wired. The following connections
must be added in the schematic:

| U2 Pin | Net | Notes |
|--------|-----|-------|
| 1, 3 (VCC, VCC2) | VBUS | Power from USB-C connector |
| 2, 4 (GND, GND2) | GND | Ground |
| 5 (RS232_RXD1) | → DB9 pin 3 (RXD) | RS232 receive from computer |
| 6 (RS232_TXD1) | → DB9 pin 2 (TXD) | RS232 transmit to computer |
| 7 (RS232_RXD2) | → DB9 pin 7 (RTS) | RTS line from computer |
| 8 (RS232_TXD2) | no_connect | CH2 driver unused |
| 9 (TTL_TXD1) | TX | Serial TX to Arduino |
| 10 (TTL_RXD1) | RX | Serial RX from Arduino |
| 11 (TTL_TXD2) | no_connect | CH2 TTL side unused |
| 12 (TTL_RXD2) | RTS | RTS signal to Arduino |

---

## Components Removed: C1, C3, C4, C5

The original MAX232 required four external **1 µF** electrolytic capacitors forming the
charge pump that generates the ±12 V RS232 swing:

| Ref | Function |
|-----|----------|
| C1 | C1+ charge pump capacitor |
| C3 | C1− charge pump capacitor |
| C4 | C2+ charge pump capacitor |
| C5 | C2− charge pump capacitor |

These are eliminated entirely — the HW-027 module contains its own capacitors. **C2
(100 nF decoupling on VCC) is retained.**

---

## U1 — Microcontroller: Arduino Mini → Arduino Nano

### What changed

The original design used an **Arduino Mini** in a DIP-24 socket (15.24 mm / 600 mil row
spacing). It was replaced by an **Arduino Nano** on a custom 2×15 pin socket footprint
(17.78 mm / 700 mil row spacing, 2.54 mm pitch, 30 pins total).

The Nano is a direct drop-in for most of the same functions but adds additional I/O pins
and integrates a USB-to-serial converter (CH340 or FT232), removing the need for a
separate programming adapter.

### Socket footprint dimensions

| Parameter | Value |
|-----------|-------|
| Pin count | 2 × 15 = 30 |
| Row spacing | 17.78 mm |
| Pin pitch | 2.54 mm |
| Pad size | 1.7 × 1.7 mm |
| Drill | 1.0 mm |
| Courtyard | 20.32 × 40.64 mm |

### Pin assignment (schematic symbol → socket pad)

Left column (pads 1–15):

| Pad | Signal |
|-----|--------|
| 1 | D1/TX |
| 2 | D0/RX |
| 3 | RST |
| 4 | GND |
| 5 | D2 |
| 6 | D3 |
| 7 | D4 |
| 8 | D5 |
| 9 | D6 |
| 10 | D7 |
| 11 | D8 |
| 12 | D9 |
| 13 | D10 |
| 14 | D11 |
| 15 | D12 |

Right column (pads 16–30):

| Pad | Signal |
|-----|--------|
| 16 | D13 |
| 17 | 3V3 |
| 18 | AREF |
| 19 | A0 |
| 20 | A1 |
| 21 | A2 |
| 22 | A3 |
| 23 | A4 |
| 24 | A5 |
| 25 | A6 |
| 26 | A7 |
| 27 | VIN |
| 28 | GND |
| 29 | RST |
| 30 | 5V |

### Signal mapping vs original Arduino Mini

The signals used by this design (PS/2 data/clock, serial TX/RX/RTS) are all on D-pins that
exist on both boards. The key differences:

| Function | Arduino Mini pin | Arduino Nano pad |
|----------|-----------------|-----------------|
| TX (serial out) | TX | 1 (D1/TX) |
| RX (serial in) | RX | 2 (D0/RX) |
| PS/2 DATA | D4 | 7 (D4) |
| PS/2 CLK | D5 | 8 (D5) |
| RTS | D3 | 6 (D3) |
| GND | GND | 4, 28 |
| VCC (5V) | VCC | 30 (5V) |

Pins not used in the original design (D6–D13, A0–A7, 3V3, AREF, VIN) are left
unconnected on the PCB.

---

## PWR1 — USB Connector: USB Mini-B → USB-C Via Header

### What changed

The original **USB Mini-B** through-hole connector (for power and programming) was
replaced by a **4-via solder header** (`USB_C_4Via` footprint). This is four
through-hole vias (1.7 mm pad, 1.0 mm drill) at 2.54 mm pitch, labelled VBUS, D−, D+,
GND.

The intent is to wire a **panel-mounted USB-C connector** externally to these pads using
short leads, rather than mounting the USB-C connector directly on the PCB. This suits
enclosure builds where the USB port needs to be on the panel face rather than on the
board edge.

### Footprint layout

```
  [1]    [2]    [3]    [4]
 VBUS    D-     D+    GND
 2.54mm pitch, all *.Cu *.Mask
 Pad size: 1.7 × 1.7 mm
 Drill: 1.0 mm
```

### Wiring to panel USB-C connector

A standard panel-mount USB-C connector (e.g. GX16 or M12 USB-C type) exposes the same
four signals. Connect with short wires:

| Via pad | USB-C connector pin |
|---------|-------------------|
| 1 VBUS | VBUS (5 V) |
| 2 D− | D− |
| 3 D+ | D+ |
| 4 GND | GND |

The CC resistors (5.1 kΩ to GND on CC1 and CC2) required for USB-C power negotiation
should be fitted on the panel connector side or added as R2/R3 on the PCB adjacent to
the via header if the panel connector does not include them.
