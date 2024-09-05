#pragma once

enum AttributeId {
    ZombieSpawnReinforcementsChange = -1,
    PlayerHunger = 2,
    PlayerSaturation = 3,
    PlayerExhaustion = 4,
    PlayerLevel = 5,
    PlayerExperience = 6,
    Health = 7,
    FollowRange = 8,
    KnockbackResistance = 9,
    MovementSpeed = 10,
    UnderwaterMovementSpeed = 11,
    LavaMovementSpeed = 12,
    AttackDamage = 13,
    Absorption = 14,
    Luck = 15,
    JumpStrength = 16, // for horse?
};

class AttributeInstance {
private:
    char __padding[0x74];
public:
    float minimumValue;
    float maximumValue;
    float currentValue;

    ~AttributeInstance();
    virtual void tick(void);
};

static_assert(sizeof(AttributeInstance) == 0x88, "AttributeInstance size is not correct");

class Attribute {
public:
    __int64 hash;
    __int64 hashedStringHash;
    std::string attributeName;
private:
    char __padding[0x32];
public:

    Attribute() {
        memset(this, 0x0, sizeof(Attribute));
    }

    Attribute(__int64 hash) {
        memset(this, 0x0, sizeof(Attribute));
        this->hash = hash;
    }
};

enum AttributeHashes : unsigned __int64 {
    HEALTH = 30064771328 - 4294967296,
    HUNGER = 8589934848 - 4294967296,
    MOVEMENT = 42949673217 - 4294967296,
    ABSORPTION = 60129542401 - 4294967296,
    SATURATION = 12884902144 - 4294967296,
    FOLLOW_RANGE = 34359738369 - 4294967296,
    LEVEL = 21474836736 - 4294967296,
    EXPERIENCE = 25769804032 - 4294967296
};

class HealthAttribute : public Attribute
{
public:
    HealthAttribute() { this->hash = AttributeHashes::HEALTH; }
};

class PlayerHungerAttribute : public Attribute
{
public:
    PlayerHungerAttribute() { this->hash = AttributeHashes::HUNGER; }
};

class MovementAttribute : public Attribute
{
public:
    MovementAttribute() { this->hash = AttributeHashes::MOVEMENT; }
};

class AbsorptionAttribute : public Attribute
{
public:
    AbsorptionAttribute() { this->hash = AttributeHashes::ABSORPTION; }
};

class PlayerSaturationAttribute : public Attribute
{
public:
    PlayerSaturationAttribute() { this->hash = AttributeHashes::SATURATION; }
};

class FollowRangeAttribute : public Attribute
{
public:
    FollowRangeAttribute() { this->hash = AttributeHashes::FOLLOW_RANGE; }
};

class PlayerLevelAttribute : public Attribute {
public:
    PlayerLevelAttribute() { this->hash = AttributeHashes::LEVEL; }
};

class PlayerExperienceAttribute : public Attribute
{
public:
    PlayerExperienceAttribute() { this->hash = AttributeHashes::EXPERIENCE; }
};
