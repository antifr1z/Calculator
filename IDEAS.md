# Coding Calculator — Project Plan & Ideas

## Assignment Summary

Build a **C++17 / Qt 6.x / QML** desktop app for viewing and editing automotive ECU "coding" — a fixed-length byte sequence where each bit or bit-group has a defined meaning (feature toggle, enum selection, nested sub-fields, etc.).

### Core Requirements
1. **Device selector** — pick from a list of ECUs; each loads its own config file.
2. **Config file** — describes the bit layout: field name, bit offset, bit width, possible values, **nested sub-fields** (e.g. *Region → Continent → Country*).
3. **Coding input** — enter hex string manually **or** load from file (BIN / JSON / XML).
4. **Visualisation** — display every field decoded from the coding; use a **tree view** for nested structures.
5. **Editing** — change any field via combo-box / checkbox / spin-box; the hex string updates in real time.
6. **Export** — save the modified coding back to file (same format as import).
7. **Deliverables** — source, config files, and a compiled executable.

---

## Architecture Ideas

### 1. Data Model (`CodingModel`)

```
CodingDefinition          (loaded from JSON config per device)
 ├─ totalBits: int
 └─ fields[]
      ├─ name: string
      ├─ bitOffset: int
      ├─ bitWidth: int
      ├─ type: enum { BOOL, ENUM, NESTED, RESERVED }
      ├─ options[]: { value: int, label: string }
      └─ children[]: CodingField        ← recursive for nested
```

- Coding **value** is stored as `QByteArray` (or `std::vector<uint8_t>`).
- A `CodingModel` class (QObject) exposes the tree to QML via `QAbstractItemModel` (tree model).
- Each node holds: field name, current decoded value, display text, editable flag.
- Editing a leaf recalculates the underlying byte array → hex string updates.

### 2. Config File Format (JSON)

```json
{
  "device": "BCM (Body Control Module)",
  "codingBytes": 2,
  "fields": [
    {
      "name": "Region",
      "bit": 9,
      "width": 7,
      "type": "nested",
      "children": [
        {
          "name": "Continent",
          "bit": 0,
          "width": 3,
          "type": "enum",
          "options": [
            { "value": 0, "label": "Europe" },
            { "value": 1, "label": "North America" },
            { "value": 2, "label": "South America" },
            { "value": 3, "label": "Australia" },
            { "value": 4, "label": "Africa" }
          ]
        },
        {
          "name": "Country",
          "bit": 3,
          "width": 4,
          "type": "enum",
          "dependsOn": "Continent",
          "options": {
            "Europe":        [{"value":0,"label":"Germany"},{"value":1,"label":"UK"},{"value":2,"label":"France"}],
            "North America": [{"value":0,"label":"USA"},{"value":1,"label":"Canada"}]
          }
        }
      ]
    },
    { "name": "Day Running Lights", "bit": 8, "width": 1, "type": "bool" },
    { "name": "Comfort Lock",       "bit": 7, "width": 1, "type": "bool" },
    { "name": "Tank Size",          "bit": 5, "width": 2, "type": "enum",
      "options": [
        { "value": 0, "label": "60L" },
        { "value": 1, "label": "45L" },
        { "value": 2, "label": "Reserved" },
        { "value": 3, "label": "Reserved" }
      ]
    },
    { "name": "Lock/Unlock Light",  "bit": 4, "width": 1, "type": "bool" },
    { "name": "Lock/Unlock Sound",  "bit": 3, "width": 1, "type": "bool" },
    { "name": "Reserved",           "bit": 0, "width": 3, "type": "reserved" }
  ]
}
```

- Relative bit offsets inside nested children (relative to parent's range).
- `dependsOn` allows context-sensitive enum options (e.g. countries depend on continent).
- Each device gets its own `.json` file in `configs/` folder.

### 3. UI Layout (QML)

```
┌─────────────────────────────────────────────────┐
│  [Device Selector ComboBox]                     │
├────────────────────┬────────────────────────────│
│  Coding Hex Input  │  [Load File] [Save File]   │
│  [ BA34          ] │                             │
├────────────────────┴────────────────────────────│
│  TreeView / ListView                             │
│  ├─ Region                                       │
│  │   ├─ Continent: [Europe ▾]                    │
│  │   └─ Country:   [Germany ▾]                   │
│  ├─ Day Running Lights: [✓]                      │
│  ├─ Comfort Lock: [✗]                            │
│  ├─ Tank Size: [60L ▾]                           │
│  ├─ Lock/Unlock Light: [✓]                       │
│  ├─ Lock/Unlock Sound: [✓]                       │
│  └─ Reserved (bits 0-2): 0b100                   │
├─────────────────────────────────────────────────│
│  Binary View: 1011 1010 0011 0100               │
│  Hex View:    BA 34                              │
└─────────────────────────────────────────────────┘
```

**QML Components:**
- `ComboBox` for device selection
- `TextField` for hex input (validated with regex)
- `TreeView` (Qt 6.4+ `TreeView` or custom `ListView` with delegates)
- Per-field delegate: `CheckBox` for bool, `ComboBox` for enum, label for reserved
- Bottom bar: binary + hex live preview
- `FileDialog` for load/save

### 4. Project Structure

```
CodingCalculator/
├── CMakeLists.txt
├── README.md
├── IDEAS.md                  ← this file
├── configs/
│   ├── bcm.json              (Body Control Module — example from PDF)
│   ├── instrument_cluster.json
│   └── comfort_module.json
├── src/
│   ├── main.cpp
│   ├── CodingEngine.h / .cpp       ← bit manipulation, encode/decode
│   ├── CodingTreeModel.h / .cpp    ← QAbstractItemModel for QML TreeView
│   ├── DeviceManager.h / .cpp      ← scans configs/, lists devices
│   └── FileIO.h / .cpp             ← load/save hex, BIN, JSON
├── qml/
│   ├── Main.qml
│   ├── DeviceSelector.qml
│   ├── CodingEditor.qml
│   ├── FieldDelegate.qml
│   └── HexInput.qml
└── resources.qrc
```

### 5. Key C++ Classes

| Class | Role |
|-------|------|
| `CodingEngine` | Parse JSON config → field tree; extract/insert bits from `QByteArray`; validate |
| `CodingTreeModel` | `QAbstractItemModel` exposing fields as tree; roles: name, value, type, options, editable |
| `DeviceManager` | Enumerate `configs/*.json`; expose device list to QML |
| `FileIO` | Read/write coding as hex string, raw BIN, or JSON `{"coding":"BA34"}` |

### 6. Bit Manipulation Strategy

```cpp
// Extract `width` bits starting at `bitOffset` from a byte array (MSB-first)
uint32_t extractBits(const QByteArray& data, int bitOffset, int width);

// Insert `width` bits at `bitOffset`
void insertBits(QByteArray& data, int bitOffset, int width, uint32_t value);
```

- Bit 0 = LSB of last byte (matching the PDF example).
- For nested fields: parent extracts its bits, then children work on that sub-range with relative offsets.

### 7. Build System (CMake)

```cmake
cmake_minimum_required(VERSION 3.20)
project(CodingCalculator LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)
qt_add_executable(CodingCalculator ...)
qt_add_qml_module(CodingCalculator ...)
target_link_libraries(CodingCalculator PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick)
```

---

## Implementation Phases

### Phase 1 — Skeleton
- CMake project compiles and shows a blank QML window
- Device selector populated from `configs/` folder

### Phase 2 — Config Loading & Decoding
- `CodingEngine` parses JSON config
- Hex input → decoded tree displayed in QML

### Phase 3 — Editing
- Changing a field in the tree recalculates the hex string
- Hex input changes re-decode the tree

### Phase 4 — File I/O
- Load/Save coding as hex text, BIN, or JSON

### Phase 5 — Polish
- Validation (wrong hex length, out-of-range values)
- Dark/light theme
- Error toasts
- Compile release build

---

## Sample Devices to Create

| Device | Coding Length | Features |
|--------|--------------|----------|
| **BCM** (Body Control Module) | 2 bytes | Lights, locks, tank, region (nested) |
| **Instrument Cluster** | 3 bytes | Units (km/mi), language, speed warning, max RPM display |
| **Comfort Module** | 2 bytes | Mirror fold, seat memory, auto-lock speed, window control |

---

## Prerequisites

- **Qt 6.x** (6.5+ recommended) — install via Qt Online Installer or `aqtinstall`
- **CMake 3.20+** — `winget install Kitware.CMake`
- **MSVC 2022** (C++17) or **MinGW 11+**
- **Ninja** (optional but faster) — `winget install Ninja-build.Ninja`

## Quick Start (after setup)

```bash
mkdir build && cd build
cmake .. -G "Ninja" -DCMAKE_PREFIX_PATH="C:/Qt/6.7.0/msvc2022_64"
cmake --build .
./CodingCalculator
```
