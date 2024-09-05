#pragma once

enum class ContainerEnumName : signed char {
    AnvilInputContainer = 0x0,
    AnvilMaterialContainer = 0x1,
    AnvilResultPreviewContainer = 0x2,
    SmithingTableInputContainer = 0x3,
    SmithingTableMaterialContainer = 0x4,
    SmithingTableResultPreviewContainer = 0x5,
    ArmorContainer = 0x6,
    LevelEntityContainer = 0x7,
    BeaconPaymentContainer = 0x8,
    BrewingStandInputContainer = 0x9,
    BrewingStandResultContainer = 0xA,
    BrewingStandFuelContainer = 0xB,
    CombinedHotbarAndInventoryContainer = 0xC,
    CraftingInputContainer = 0xD,
    CraftingOutputPreviewContainer = 0xE,
    RecipeConstructionContainer = 0xF,
    RecipeNatureContainer = 0x10,
    RecipeItemsContainer = 0x11,
    RecipeSearchContainer = 0x12,
    RecipeSearchBarContainer = 0x13,
    RecipeEquipmentContainer = 0x14,
    RecipeBookContainer = 0x15,
    EnchantingInputContainer = 0x16,
    EnchantingMaterialContainer = 0x17,
    FurnaceFuelContainer = 0x18,
    FurnaceIngredientContainer = 0x19,
    FurnaceResultContainer = 0x1A,
    HorseEquipContainer = 0x1B,
    HotbarContainer = 0x1C,
    InventoryContainer = 0x1D,
    ShulkerBoxContainer = 0x1E,
    TradeIngredient1Container = 0x1F,
    TradeIngredient2Container = 0x20,
    TradeResultPreviewContainer = 0x21,
    OffhandContainer = 0x22,
    CompoundCreatorInput = 0x23,
    CompoundCreatorOutputPreview = 0x24,
    ElementConstructorOutputPreview = 0x25,
    MaterialReducerInput = 0x26,
    MaterialReducerOutput = 0x27,
    LabTableInput = 0x28,
    LoomInputContainer = 0x29,
    LoomDyeContainer = 0x2A,
    LoomMaterialContainer = 0x2B,
    LoomResultPreviewContainer = 0x2C,
    BlastFurnaceIngredientContainer = 0x2D,
    SmokerIngredientContainer = 0x2E,
    Trade2Ingredient1Container = 0x2F,
    Trade2Ingredient2Container = 0x30,
    Trade2ResultPreviewContainer = 0x31,
    GrindstoneInputContainer = 0x32,
    GrindstoneAdditionalContainer = 0x33,
    GrindstoneResultPreviewContainer = 0x34,
    StonecutterInputContainer = 0x35,
    StonecutterResultPreviewContainer = 0x36,
    CartographyInputContainer = 0x37,
    CartographyAdditionalContainer = 0x38,
    CartographyResultPreviewContainer = 0x39,
    BarrelContainer = 0x3A,
    CursorContainer = 0x3B,
    CreatedOutputContainer = 0x3C,
    SmithingTableTemplateContainer = 0x3D,
    CrafterLevelEntityContainer = 0x3E,
};

enum class InventorySourceType : int {
    InvalidInventory          = -1,
    ContainerInventory        = 0,
    GlobalInventory           = 1,
    WorldInteraction          = 2,
    CreativeInventory         = 3,
    NonImplementedFeatureTODO = 99999,
};

class ItemDescriptor
{
public:
    class Item*  mItem;       // this+0x0 // InternalItemDescriptor
    short        mAuxValue;   // this+0x8
};
// Size: 0x010

class ItemDescriptorCount : public ItemDescriptor {
public:
    unsigned short mStackSize; // this+0x10
    char           pad[0x6];   // this+0x12
};
// Size: 0x018

struct ItemStackLegacyRequestIdTag {
public:

};

struct ItemStackNetIdTag {
public:

};

struct ItemStackRequestIdTag {
public:

};

using ItemStackLegacyRequestId = unsigned int;
using ItemStackNetId = unsigned int;
using ItemStackRequestId = unsigned int;

struct ItemStackNetIdVariant
{
public:
    //std::variant<ItemStackNetId, ItemStackRequestId, ItemStackLegacyRequestId> id;
    std::variant<ItemStackNetId, ItemStackRequestId, ItemStackLegacyRequestId> id;
};

class NetworkItemStackDescriptor : public ItemDescriptorCount
{
public:
    bool                  mIncludeNetIds;  // this+0x18
    ItemStackNetIdVariant mNetIdVariant;   // this+0x20
    int                   mBlockRuntimeId; // this+0x38
    std::string           mUserDataBuffer; // this+0x40

    NetworkItemStackDescriptor() = default;
};

class InventorySource {
public:
    enum class InventorySourceFlags : unsigned int {
        NoFlag = 0x0,
        WorldInteraction_Random = 0x1,
    };

    InventorySourceType  mType = InventorySourceType::InvalidInventory; // this+0x0
    char                 mContainerId = -1;                             // this+0x4
    InventorySourceFlags mFlags = InventorySourceFlags::NoFlag;         // this+0x8
};

class InventoryAction {
public:
    InventorySource            mSource;             // this+0x0
    int                        mSlot;               // this+0xC
    NetworkItemStackDescriptor mFromItemDescriptor; // this+0x10
    NetworkItemStackDescriptor mToItemDescriptor;   // this+0x70
    class ItemStack*           mFromItem;           // this+0xD0
    class ItemStack*           mToItem;             // this+0x170
};
// Size: 0x0210

class InventoryTransactionItemGroup {
public:
    InventoryTransactionItemGroup() = delete;

    int                                mItemId;   // this+0x0
    int                                mItemAux;  // this+0x4
    std::unique_ptr<class CompoundTag> mTag;      // this+0x8
    int                                mCount;    // this+0x10
    bool                               mOverflow; // this+0x14
};

class InventoryTransaction {
public:
    std::unordered_map<InventorySource, std::vector<InventoryAction>>     actions; // 0x0
    std::vector<InventoryTransactionItemGroup>                            items;   // 0x40
};

class ComplexInventoryTransaction {
public:
    enum class Type : int {
        NormalTransaction = 0x0,
        InventoryMismatch = 0x1,
        ItemUseTransaction = 0x2,
        ItemUseOnActorTransaction = 0x3,
        ItemReleaseTransaction = 0x4,
    };

    Type                 type;
    InventoryTransaction data;
}; 
// Size: 0x0104

class ItemUseOnActorInventoryTransaction : public ComplexInventoryTransaction {
public:
    enum class ActionType : int32_t {
        Interact = 0,
        Attack = 1,
        ItemInteract = 2,
    };

    uint64_t                     actorId;
    ActionType                   actionType;
    int32_t                      slot;
    NetworkItemStackDescriptor   itemInHand;
    Vector3<float>               playerPos{}, clickPos;
}; 
// Size: 0x0240

class ItemUseInventoryTransaction : public ComplexInventoryTransaction
{
public:
    enum class ActionType : int32_t {
        Place = 0,
        Use = 1,
        Destroy = 2,
    };

    ActionType actionType;
    Vector3<int> blockPos;
    uint32_t targetBlockRuntimeId;
    uint8_t face;
    int32_t slot;
    NetworkItemStackDescriptor itemInHand;
    Vector3<float> playerPos, clickPos;
};
// Size: 0x0256

class ItemReleaseInventoryTransaction : public ComplexInventoryTransaction
{
public:
    enum class ActionType : int32_t {
        Release = 0,
        Use = 1,
    };

    ActionType                 actionType;
    int32_t                    slot;
    NetworkItemStackDescriptor itemInHand;
    Vector3<float>             headPos;
};
// Size: 0x0224

class InventoryTransactionPacket : public Packet {
public:
    ItemStackLegacyRequestId                                              mLegacyRequestId;    // this+0x30
    std::vector<std::pair<ContainerEnumName, std::vector<unsigned char>>> mLegacySetItemSlots; // this+0x40
    std::unique_ptr<ComplexInventoryTransaction>                          mTransaction;        // this+0x58
    bool                                                                  mIsClientSide;       // this+0x60
};
// Size: 0x068