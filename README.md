# Lazy Dynamic Object Pool Plugin

## Overview

The Lazy Dynamic Object Pool Plugin is a powerful and flexible solution for managing object pooling in Unreal Engine projects. It provides an efficient way to reuse actors, reducing the overhead of frequent spawning and destruction of objects in your game.

## Features

- Dynamic object pooling for actors
- Automatic pool growth and shrinking
- Blueprint support for spawning actors from the pool
- Editor integration for easy management
- Performance optimizations for large-scale object reuse

## Installation

1. Clone or download the plugin to your project's `Plugins` folder.
2. Enable the plugin in your project's plugin settings.
3. Rebuild your project.

## Usage

### In C++

To use the object pool in C++:

```cpp
#include "Subsystems/LazyDynamicObjectPoolSubsystem.h"

// Get the subsystem
ULazyDynamicObjectPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<ULazyDynamicObjectPoolSubsystem>();

// Spawn an actor from the pool
AActor* PooledActor = PoolSubsystem->InitializeActorFromPool(YourActorClass, NewOwner);
PooledActor = PoolSubsystem->FinishInitializeActorFromPool(PooledActor, DesiredTransform);

// Return an actor to the pool
PoolSubsystem->ReturnActorToPool(PooledActor);
```

### In Blueprints

The plugin provides a custom node "Spawn Actor From Pool" that can be used in Blueprint graphs to spawn actors from the pool.
![Pool Spawn Method](https://github.com/user-attachments/assets/894ddea2-202c-4d9a-8f74-3378adb04469)

### Implementing Poolable Actors

To make an actor poolable, implement the `IPoolableActorInterface`:

```cpp
UINTERFACE(MinimalAPI, Blueprintable)
class UPoolableActorInterface : public UInterface
{
    GENERATED_BODY()
};

class IPoolableActorInterface
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, Category = "Object Pool")
    void OnActivateFromPool();

    UFUNCTION(BlueprintNativeEvent, Category = "Object Pool")
    void OnDeactivateToPool();
};
```

## Configuration

The plugin provides a settings class `ULazyDynamicObjectPoolSettings` where you can configure:

- Default initial pool size
- Maximum pool size
- Pool growth factor
- Auto-shrink settings

## Editor Integration

The plugin includes an editor window for managing and monitoring object pools. Access it via:

![image](https://github.com/user-attachments/assets/7d01661e-3fdf-4a18-8a01-8166f70c6026)

- Window > Lazy Generic Pool Window
- The toolbar button with the plugin's icon

## Performance Considerations

- The plugin automatically grows and shrinks pools based on usage.
- Consider pre-warming pools for frequently used actors.
- Monitor pool sizes and adjust settings as needed for your specific use case.

## Support

For issues, feature requests, or contributions, please use the plugin's GitHub repository.

## License

This plugin is copyright (C) 2024 Job Omondiale - All Rights Reserved.
