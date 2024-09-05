#pragma once
class ItemStack;

namespace Containers
{
	static std::string Inventory = "inventory_items";
	static std::string Hotbar = "hotbar_items";
	static std::string Container = "container_items";
	static std::string Armor = "armor_items";
};

class ContainerScreenController { // indexes cuz im gonna make them methods that aren't virtual
private:
	virtual __int64 Destructor();
private:
	virtual __int64 tick(void);
	virtual __int64 handleEvent(__int64&);
	virtual __int64 getRoute(void);
	virtual __int64 setScreenState(__int64, std::string const&);
	virtual __int64 onOpen(void);
	virtual __int64 onTerminate(void);
	virtual __int64 onInit(void);
	virtual __int64 onDelete(void);
	virtual __int64 unknown1();

public:
	virtual bool canExit(void);
	virtual __int64 tryExit(void);

private:
	virtual __int64 areControllerTabsEnabled(void);
	virtual __int64 onCreation(void);
	virtual __int64 logCreationTime(std::string const&, double, double, unsigned char);
	virtual __int64 unknown2();
	virtual __int64 unknown3();
	virtual __int64 unknown4();
	virtual __int64 unknown5();
	virtual __int64 unknown6();

public:
	virtual __int64 onLeave(void);
	virtual __int64 leaveScreen(void);

private:
	virtual __int64 handleGameEventNotification(__int64);
	virtual __int64 bind(std::string const&, unsigned int, int, std::string const&, unsigned int, std::string const&, __int64&);
	virtual __int64 bind(std::string const&, unsigned int, std::string const&, __int64&);
	virtual __int64 handleLicenseChanged(void);
	virtual __int64 onDictationEvent(std::string const&);

public:
	virtual void setAssociatedBlockPos(Vector3<int> const&);
	virtual void setAssociatedEntityUniqueID(__int64);
	virtual void setSuspendInput(bool);

private:
	virtual __int64 getCallbackInterval(void);
	virtual __int64 onRender(void);
	virtual __int64 addStaticScreenVars(__int64&);
	virtual __int64 getAdditionalScreenInfo(void);
	virtual __int64 getTelemetryOverride(void);
	virtual __int64 addEventProperties(__int64) const;
	virtual __int64 getSceneType(void);
	virtual __int64 getScreenVersion(void);
	virtual __int64 screenHandlesGamepadMenuButton(void);
	virtual __int64 getProxy(void);
	virtual __int64 onEntered(void);
	virtual __int64 getNameId(std::string const&);
	virtual __int64 _doesScreenHaveExitBehavior(void);
	virtual __int64 _isStillValid(void);
	virtual __int64 _getGamepadHelperVisible(void);
	virtual __int64 _getMixedHelperVisible(void);
	virtual __int64 _getKeyboardHelperVisible(void);
	virtual __int64 _getButtonADescription(void);
	virtual __int64 _getButtonBDescription(void);
	virtual __int64 _getButtonXDescription(void);
	virtual __int64 _getButtonYDescription(void);
	virtual __int64 _getButtonKeyboardDescription(void);

public:
	virtual __int64 _handlePlaceAll(std::string const&, int);
	virtual __int64 _handlePlaceOne(std::string const&, int);
	virtual __int64 _handleSelectSlot(std::string const&, int);
	virtual __int64 _getSelectedSlotInfo(void);
	virtual __int64 unknown7();
	virtual __int64 unknown8();
	virtual __int64 unknown9();
	virtual __int64 unknown10();
	virtual __int64 unknown11();
	virtual __int64 unknown12();
	virtual ItemStack* _getItemStack(std::string, int) const;

private:
	virtual __int64 _getVisualItemStack(std::string const&, int);
	virtual __int64 _getTakeableItemStackBase(std::string const&, int);
	virtual __int64 _onContainerSlotHovered(std::string const&, int);
	virtual __int64 _onContainerSlotSelected(std::string const&, int);
	virtual __int64 _onContainerSlotPressed(std::string const&, int);
	virtual __int64 _shouldSwap(std::string const&, int, std::string const&, int);
	virtual __int64 _getCollectionName(__int64*);
	virtual __int64 _canSplit(std::string const&, int);
	virtual __int64 _sendFlyingItem(__int64 const&, std::string const&, int, std::string const&, int);

public:
	//void handleAutoPlace(uintptr_t a1, std::string name, int slot);
	//void handleTakeAll(std::string name, int slot);
	//void handlePlaceAll(std::string name, int slot);
	//void handleSwap(std::string name1, int slot1, std::string name2, int slot2);
	//void sendFlyingItem(ItemStack*, std::string, int, std::string, int);
	//ItemStack* getContainerItem(int slot);
	//void takeContainerItem(int slot);
public:
	// handleAutoPlace
	void shiftItems(std::string containerName, int slots) {
		static void* shiftClick = Memory::findSig(xorstr_("40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 45 8B E1 49 8B F0 44 8B FA 89 54 24 40 48 8B F9 48 8B 01 48 8B 80 ? ? ? ? FF 15 ? ? ? ? 84 C0 0F 84 ? ? ? ? 48 8B 8F ? ? ? ? 48 85 C9 74 6B E8 ? ? ? ? 84 C0 74 62 48 8D 8F ? ? ? ? 8B 91 ? ? ? ? 39 91 ? ? ? ? 74 13 44 89 64 24 ? 4C 8B CE"));

		Memory::CallFunc<void, ContainerScreenController*, uintptr_t, TextHolder, int>(
			shiftClick, this, 0x7FFFFFFF, containerName, slots);
	}

	// handleSwap
	void handleSwap(std::string name1, int slot1, std::string name2, int slot2) {
		this->shiftItems(name1, slot1);
		this->shiftItems(name2, slot2);
	}

	void closeContainer() {
		this->leaveScreen();
		this->canExit();
		this->tryExit();
	}
};

class ChestScreenController : public ContainerScreenController {
};
