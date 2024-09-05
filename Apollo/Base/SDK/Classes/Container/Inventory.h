#pragma once

class ContainerContentChangeListener;
class Player;
class Level;

class Inventory
{
private:
	virtual void Destructor(); // ~Inventory() // Inventory Destructor
public:
	
	void dropSlot(int slot, bool clearContainer, bool dropAll, bool randomize)
	{             
		// 85 D2 0F 88 ? ? ? ? 48 89 5C 24 ? 55 56 57 41 56 41 57 48
		// FillingContainer::dropSlot
		using drop_t = void(__fastcall*)(Inventory*, int, bool, bool, bool);
		static drop_t func = reinterpret_cast<drop_t>(Memory::findSig(xorstr_("85 D2 0F 88 ? ? ? ? 48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24")));
		if (func != 0)
			func(this, slot, clearContainer, dropAll, randomize);
	}

	virtual void init(void);
	virtual void serverInitItemStackIds();
	virtual void addContentChangeListener(ContainerContentChangeListener*);
	virtual void removeContentChangeListener(ContainerContentChangeListener*);
	virtual void addRemovedListener();
	virtual void removeRemovedListener();
	virtual ItemStack* getItem(int);
	virtual void hasRoomForItem(ItemStack const&);
	virtual void addItem(ItemStack&);
	virtual void addItemWithForceBalance(ItemStack&);
	virtual void addItemToFirstEmptySlot(ItemStack const&);
	virtual void setItem(int, ItemStack const&);
	virtual void setItemWithForceBalance(int, ItemStack const&, bool);
	virtual void removeItem(int, int);
	virtual void removeAllItems(void);
	virtual void removeAllItemsWithForceBalance(void);
	virtual void dropContents(BlockSource&, struct Vector3<float> const&, bool);
	virtual int getContainerSize(void);
	virtual int getMaxStackSize(void);
	virtual void startOpen(Player&);
	virtual void stopOpen(Player&);
	virtual int getSlotCopies(void);
	virtual int getSlots(void);
	virtual int getEmptySlotsCount(void);
	virtual int getItemCount(ItemStack const&);
	virtual void findFirstSlotForItem(ItemStack const&);
	virtual void canPushInItem(int, int, ItemStack const&);
	virtual void canPullOutItem(int, int, ItemStack const&);
	virtual void setContainerChanged(int);
	virtual void setContainerMoved(void);
	virtual void setCustomName(std::string const&);
	virtual void hasCustomName(void);
	virtual void readAdditionalSaveData(class CompoundTag const&);
	virtual void addAdditionalSaveData(CompoundTag&);
	virtual void createTransactionContext();
	virtual void initializeContainerContents(BlockSource&);
	virtual bool isEmpty(void);
	virtual void add(ItemStack&);
	virtual void canAdd(ItemStack const&);
	virtual void clearSlot(int);
	virtual void clearInventory(int);
	virtual void load(class ListTag const&, class SemVersion const&, Level&);
	virtual int getFirstEmptySlot(void);
	virtual void setContainerSize(int);
};