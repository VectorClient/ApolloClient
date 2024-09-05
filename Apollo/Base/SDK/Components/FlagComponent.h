#pragma once

struct IEntityComponent {};

template<typename T>
class FlagComponent {};

struct VerticalCollisionFlag {};
struct HorizontalCollisionFlag {};
struct MovingFlag {};
struct WasOnGroundFlag {};
struct JumpFromGroundRequestFlag {};
struct LocalPlayerJumpRequestFlag {};
struct LocalPlayerComponentFlag {};
struct ActorFlag {};
struct PlayerComponentFlag {};
struct InWaterFlag {};
struct InLavaFlag {};
struct PigFlag {};
struct ActorIsImmobileFlag {};
struct DiscardFrictionFlag {};
struct CanStandOnSnowFlag {};

struct OnGroundFlagComponent {};

//struct OnGroundFlag {};