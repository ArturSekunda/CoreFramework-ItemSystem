# Core Framework - Item System

**A modular, multiplayer-ready Item & Inventory plugin for Unreal Engine 5.7+**

Created by **Artur "Darkowy" Sekunda**

---

## Overview

Core Framework Item System is a reusable UE5 plugin built to handle items, inventories, and equipment through a clean, decoupled architecture.
It's designed to be dropped into different projects with minimal friction - items are defined through data assets, extended through composition, and driven by GameplayTags rather than hardcoded enums, so new item categories and behaviors can be added without touching core code.


---

## Features

- **Built-in multiplayer support** - item state and extensions replicate out of the box via `FFastArraySerializer`, no manual replication boilerplate required.
- **Lightweight UObject-based item pattern** - items are `UObject`-based instances (`UItemInstance`), keeping the flexibility of polymorphism without the weight of full Actors.
- **Composable Extensions** - Items, Inventory, and Equipment all use a modular Extension system, so you add behavior (usable, equippable, stackable effects, etc.) by composition instead of deep inheritance chains.
- **MVVM support** - includes a ready-to-use MVVM layer (`VM_ItemBase`, `VM_ItemExtension`) with 1:1 pairing to extensions, plus an `InventoryVMManagerComponent` for driving inventory UI cleanly from data.
- **GameplayTag-driven categorization** - item categories, types, and cross-system communication use `FGameplayTagContainer` instead of enums, keeping the plugin decoupled from any single project's item taxonomy.
- **Data-driven items via Primary Data Assets** - static item data lives in `UPDA_Item` (Primary Data Assets), separating designer-facing configuration from runtime logic. Architecturally similar in spirit to Epic's Lyra Inventory System (Definition/Fragment/Instance ≈ PDA/Extension/Instance), with an added MVVM layer Lyra doesn't provide.
- **Flexible creation API** - `ItemFactorySubsystem` exposes both a C++-friendly `CreateItemInstance(FPrimaryAssetId)` and a Blueprint-friendly `CreateItemInstanceFromPDA(TSoftObjectPtr<UPDA_Item>)`, avoiding common Blueprint pitfalls around asset ID resolution.

---

## Requirements

- Unreal Engine **5.7+**
- C++ project (plugin is C++-based; Blueprint-exposed where relevant)

---

## Installation

1. Copy the `CoreFramework` plugin folder into your project's `Plugins/` directory.
2. Regenerate project files (or open the `.uproject` directly if using a launcher build).
3. Enable the plugin under **Edit → Plugins → Core Framework Item System**.
4. Restart the editor.

---

## Architecture at a Glance

```
UPDA_Item (Primary Data Asset)
   │   static, non-editable-at-runtime item definition
   ▼
UItemInstance (UObject)
   │   runtime instance, replicated via FFastArraySerializer
   ▼
UItemExtensionContainer
   │   composition point for modular behavior
   ▼
UItemExtension (e.g. Equipment, Usable, Effect extensions)
```

- **`FConstItemBaseData`** - non-editable-at-runtime base data (name, tags, stacking rules, asset references), set per data asset.
- **`FEditableItemBaseData`** - runtime-editable state carried per instance (e.g. quantity), replicated and save-game compatible.
- **`FGameplayTagContainer ItemTags`** - drives item categorization; supports hierarchical matching (`HasTag`).
---

## Inventory & Equipment

- `UC_Inventory` - FFastArraySerializer-based + `UInventoryExtensionContainer`.
- `UC_Equipment` - Has only a single `UEquipmentExtensionContainer` for its own Extensions.

---

## License

Licensed under the **[PolyForm Noncommercial License 1.0.0](https://polyformproject.org/licenses/noncommercial/1.0.0)** - see [LICENSE](LICENSE) for full terms.

In short: free to use, study, and modify for non-commercial purposes. Commercial use requires separate permission from the author.

> Required Notice: Artur "Darkowy" Sekunda (https://github.com/ArturSekunda/CoreFramework-ItemSystem)

---

## Author

**Artur "Darkowy" Sekunda**
Built as part of the Core Framework - a set of modular, decoupled UE5 systems designed for reuse across projects.