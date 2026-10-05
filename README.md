# Core Framework - Item System

**Item System which use pattern similar to Item System from "Lyra" created by Epic Games**

Created by **Artur "Darkowy" Sekunda**

---

## Overview

Core Framework - Item System is a modular, data-driven item system for Unreal Engine 5.7+, designed to be flexible, extensible, and multiplayer-ready. It provides: 
- Lightweight UObject-based item pattern,
- Extensions for Items/Inventory/Equipment behavior,
- MVVM layer for driving inventory UI.


---

## Features

- **Built-in multiplayer support** - Items, Inventory, Equipment and their extensions are fully replicated and save-game compatible. (**WIP:** Multiplayer replication of items and their extensions is still not tested, but the system is designed with replication in mind.)
- **Lightweight UObject-based item pattern** - Items are `UObject`-based instances (`UItemInstance`), keeping the flexibility of polymorphism without the weight of full Actors.
- **Extensions** - Items, Inventory, and Equipment all use a modular Extension system, so you can add behavior (usable, equippable, stackable effects, etc.) by composition instead of deep inheritance chains.
- **MVVM support** - Includes a ready-to-use MVVM layer with 1:1 pairing to extensions, plus an `InventoryVMManagerComponent` for driving inventory UI cleanly from data. (**WIP:** MVVM layer are almost ready. I need to add support for InventoryExtensions and EquipmentExtensions)
- **GameplayTag-driven categorization** - Items uses `FGameplayTagContainer` for categorization, allowing for hierarchical matching and flexible filtering.
- **Data-driven items via Primary Data Assets** - Static item data lives in `UPDA_Item` (Primary Data Assets), separating designer-facing configuration from runtime logic. Architecturally similar in spirit to Epic's Lyra Inventory System (Definition/Fragment/Instance ≈ PDA/Extension/Instance).
- **Flexible creation API** - `ItemFactorySubsystem` exposes both a C++-friendly `CreateItemInstance(FPrimaryAssetId)` and a Blueprint-friendly `CreateItemInstanceFromPDA(TSoftObjectPtr<UPDA_Item>)`, avoiding common Blueprint pitfalls around asset ID resolution.
- **Saving feature** - System are compatible with UE's `USaveGame` system, allowing for easy saving and loading of Items/Inventory/Equipment and their extensions. (**WIP:** Saving feature will be soon tested and improved)

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
SOON
```
---

## Inventory & Equipment
Also I added a Inventory and Equipment system which is based on the same Extension system as Items. Equipment has only a `EquipmentExtensionSerializer`. Also both of them are child of Interface `IExtensionContainer` which allows them to have their own Extensions like Items. Please Check `Interface/ExtensionContainer.h`.
- `InventoryComponent` - FFastArraySerializer + `InventoryExtensionSerializer` for its own Extensions.
- `EquipmentComponent` - Has only a single `EquipmentExtensionSerializer` for its own Extensions.

---

## License

Licensed under the **MIT License**. See the [LICENSE](LICENSE) file for details.

---

## Author

**Artur "Darkowy" Sekunda** - Main Author and Maintainer.
