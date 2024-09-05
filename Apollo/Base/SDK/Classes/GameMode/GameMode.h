#pragma once

#include <functional>

class GameMode
{
public:
    BUILD_ACCESS(Player*, player, 0x8);
    BUILD_ACCESS(float, lastBreakProgress, 0x20);
    BUILD_ACCESS(float, BreakProgress, 0x24);
private:
    virtual void Destructor(); // 0 // GameMode Destructor
public:
    virtual __int64 startDestroyBlock(Vector3<int> const& pos, unsigned char blockSide, bool& isDestroyedOut); // 1
    virtual __int64 destroyBlock(const Vector3<int>&, unsigned char blockSide); // 2
    virtual __int64 continueDestroyBlock(Vector3<int> const& a1, unsigned char a2, Vector3<float> const& a3, bool& a4); // 3
    virtual __int64 stopDestroyBlock(Vector3<int> const&); // 4
    virtual __int64 startBuildBlock(Vector3<int> const& a1, unsigned char a2, bool auth); // 5
    virtual __int64 buildBlock(Vector3<int> const& a1, unsigned char a2, bool auth); // 6
    virtual __int64 continueBuildBlock(Vector3<int> const& a1, unsigned char a2); // 7
    virtual __int64 stopBuildBlock(); // 8
    virtual __int64 tick(); // 9
    virtual __int64 getPickRange(InputMode const& a1, bool a2); // 10
    virtual __int64 useItem(class ItemStack& a1); // 11
    virtual __int64 useItemOn(ItemStack& a1, Vector3<int> const& a2, unsigned char a3, Vector3<float> const& a4, class Block const* a5); // 12
    virtual __int64 interact(Actor* a1, Vector3<float> const& a2); // 13
    virtual __int64 attack(Actor&); // 14
    virtual __int64 releaseUsingItem(); // 15
    virtual void setTrialMode(bool a1); // 16
    virtual void isInTrialMode(); // 17
    virtual __int64 registerUpsellScreenCallback(std::function<void> a3); // 18

    float getDestroyRate(Block const& block) {
        using getDestroyRate = float(__fastcall*)(GameMode*, Block const&);
        static getDestroyRate getDestroyRateFunc = reinterpret_cast<getDestroyRate>(Memory::findSig(xorstr_("48 89 ? ? ? 57 48 83 EC ? 48 8B ? 0F 29 ? ? ? 48 8B ? ? E8")));
        return getDestroyRateFunc(this, block);
    }

    void baseUseItem(ItemStack& a1) {
        using GM_baseUseItem_t = void (*)(GameMode*, ItemStack&);
        static GM_baseUseItem_t baseUseItem = reinterpret_cast<GM_baseUseItem_t>(Memory::findSig(xorstr_("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 ? 48 8B F2 48 8B F9 45 33 F6 4C 89 75 ? 48 8D 4D ? E8 ? ? ? ? 90 48 8B 5F")));  //"ItemUseInventoryTransaction::handle - destroy block failed, rending blocks around area to player"

        return baseUseItem(this, a1);
    }
};