# Frette - Gameplay Ability System Implementation

## Overview

This project implements Unreal Engine's Gameplay Ability System (GAS) with basic replicated player attributes for multiplayer gameplay.

## Features

### Implemented Attributes

1. **Health System**
   - `Health`: Current health value (0-100)
   - `MaxHealth`: Maximum health capacity (100)
   - Fully replicated across network
   - Fall damage implementation

2. **Stamina System**
   - `Stamina`: Current stamina value (0-100)
   - `MaxStamina`: Maximum stamina capacity (100)
   - Fully replicated across network
   - Sprint exhaustion mechanics

## Components

### FretteAttributeSet

The attribute set defines all player attributes with proper replication:

- **Health/MaxHealth**: Player's life points
- **Stamina/MaxStamina**: Player's energy for sprinting

All attributes use `FGameplayAttributeData` for GAS integration and are replicated with `COND_None` and `REPNOTIFY_Always` for reliable multiplayer synchronization.

### FrettePlayerCharacter

The player character has been updated to:

- Implement `IAbilitySystemInterface`
- Include `UAbilitySystemComponent` for GAS functionality
- Initialize attribute sets on both server and client
- Handle attribute-based gameplay mechanics

## Gameplay Mechanics

### Sprint System

- **Activation**: Call `DoPlayerSprint(true)` to start sprinting
- **Speed**: Movement speed increases by `SprintSpeedMultiplier` (default: 2.0x)
- **Stamina Drain**: Drains at `StaminaDrainRate` (default: 10/sec) while moving
- **Stamina Regen**: Regenerates at `StaminaRegenRate` (default: 20/sec) when not sprinting
- **Depletion**: Sprint automatically stops when stamina reaches 0

### Fall Damage System

- **Threshold**: `FallDamageThreshold` (default: 1000 units/sec)
- **Calculation**: Damage = (FallSpeed - Threshold) × `FallDamageMultiplier` (default: 0.01)
- **Server Authority**: Processed only on server to prevent cheating
- Triggers in `Landed()` override

## Blueprint Integration

The following functions are exposed to Blueprints:

### Attribute Getters
- `GetHealth()` - Returns current health
- `GetMaxHealth()` - Returns max health
- `GetStamina()` - Returns current stamina
- `GetMaxStamina()` - Returns max stamina
- `GetHealthPercent()` - Returns health as 0-1 ratio
- `GetStaminaPercent()` - Returns stamina as 0-1 ratio

### Gameplay Functions
- `DoPlayerMove(FVector2D)` - Handle movement input
- `DoPlayerLook(FVector2D)` - Handle camera rotation
- `DoPlayerJump()` - Handle jump input
- `DoPlayerSprint(bool)` - Toggle sprint on/off

## Health Feedback Widget

The character includes a `UWidgetComponent` for displaying health to other players:

- Attached to character mesh
- Positioned 120 units above character
- Screen-space widget (always faces camera)
- Size: 150×30 pixels

To use, create a widget blueprint and assign it to `HealthWidgetComponent` in the character blueprint.

## Replication

All attributes are fully replicated with the following configuration:

- **Replication Mode**: `EGameplayEffectReplicationMode::Mixed`
- **Condition**: `COND_None` (replicate to all clients)
- **RepNotify**: `REPNOTIFY_Always` (always notify on change)

This ensures reliable synchronization in multiplayer environments.

## Configuration

Adjust these properties in the character blueprint or C++:

| Property | Default | Description |
|----------|---------|-------------|
| `SprintSpeedMultiplier` | 2.0 | Speed multiplier during sprint |
| `StaminaDrainRate` | 10.0 | Stamina consumed per second while sprinting |
| `StaminaRegenRate` | 20.0 | Stamina regenerated per second |
| `FallDamageThreshold` | 1000.0 | Minimum fall speed for damage |
| `FallDamageMultiplier` | 0.01 | Damage scaling factor |

## Future Enhancements

Potential additions to this GAS foundation:

- **Gameplay Effects**: Replace direct attribute manipulation with proper gameplay effects for stamina and damage
- **Gameplay Abilities**: Create abilities for actions (jump, attack, etc.)
- **Gameplay Tags**: Add tags for ability states and conditions
- **Gameplay Cues**: Add visual/audio feedback for attribute changes
- Death/respawn system
- Damage type system
- Cooldown mechanics
- Buff/debuff system

### Technical Debt

The current implementation uses direct attribute manipulation for simplicity and to minimize initial changes. In production:
- Stamina drain/regen should use periodic gameplay effects
- Fall damage should use an instant damage gameplay effect
- This would enable proper event handling, damage mitigation, and buff/debuff interactions

## Technical Notes

### Dependencies

Added to `Frette.Build.cs`:
- `GameplayAbilities`
- `GameplayTags`
- `GameplayTasks`
- `UMG` (for widget component)

### Plugin Configuration

Enabled in `Frette.uproject`:
```json
{
  "Name": "GameplayAbilities",
  "Enabled": true
}
```

## Usage in Blueprints

1. **Sprint Input**: Wire input action to `DoPlayerSprint(true/false)`
2. **Health Display**: Bind widget text to `GetHealth()` or `GetHealthPercent()`
3. **Stamina Bar**: Bind progress bar to `GetStaminaPercent()`
4. **Custom Effects**: Use `AbilitySystemComponent` to apply gameplay effects

## Testing Multiplayer

To test replication:

1. Launch as listen server (Play as Listen Server in editor)
2. Connect multiple clients
3. Observe attribute synchronization across all clients
4. Test fall damage and stamina depletion
5. Verify health widget displays correctly for all players
