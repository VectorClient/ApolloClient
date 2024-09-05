#pragma once

// This should be in Actor/Mob but using Player for a clean base since Plyaer is a child of Actor/Mob.
class Player : public Mob { // Player VTable
public:
	// GameMode Player::getGameMode(void)
	BUILD_ACCESS(GameMode*, Gamemode, 0xEC8); // 1.21.2
	BUILD_ACCESS(SerializedSkin*, Skin, 0x780); // 1.21.2
public:
	virtual void prepareRegion(ChunkSource&);
	virtual void destroyRegion(void);
	virtual void suspendRegion(void);
	virtual void _fireDimensionChanged(void);
	virtual void changeDimensionWithCredits(int);
	virtual void tickWorld(Tick const&);
	virtual void Function5();
	virtual void getTickingOffsets(void);
	virtual void moveView(void);
	virtual void moveSpawnView(Vector3<float> const&, int);
	virtual void checkMovementStats(Vector3<float> const&);
	virtual void getCurrentStructureFeature(void);
	virtual bool isAutoJumpEnabled(void);
	virtual void respawn(void);
	virtual void resetRot(void);
	virtual bool isInTrialMode(void);
	virtual void openPortfolio(void);
	virtual void openBook(int, bool, int, BlockActor*);
	virtual void openTrading(ActorUniqueID const&, bool);
	virtual void openChalkboard(ChalkboardBlockActor&, bool);
	virtual void openNpcInteractScreen(std::shared_ptr<INpcDialogueData>);
	virtual void openInventory(void);
	virtual void displayChatMessage(std::string const&, std::string const&);
	virtual void displayClientMessage(std::string const&);
	virtual void displayTextObjectMessage(TextObjectRoot const&, std::string const&, std::string const&);
	virtual void displayTextObjectWhisperMessage(ResolvedTextObject const&, std::string const&, std::string const&);
	virtual void displayTextObjectWhisperMessage(std::string const&, std::string const&, std::string const&);
	virtual void displayWhisperMessage(std::string const&, std::string const&, std::string const&, std::string const&);
	virtual void startSleepInBed(Vector3<int> const&);
	virtual void stopSleepInBed(bool, bool);
	virtual bool canStartSleepInBed(void);
	virtual void openSign(Vector3<int> const&, bool);
	virtual void playEmote(std::string const&, bool);
	virtual bool isLoading(void);
	virtual bool isPlayerInitialized(void);
	virtual void stopLoading(void);
	virtual void setPlayerGameType(GameType);
	virtual void initHUDContainerManager(void);
	virtual void _crit(Actor&);
	virtual void getEventing(void);
	virtual void getUserId(void);
	virtual void addExperience(int);
	virtual void addLevels(int);
	virtual void Function42();
	virtual void Function43();
	virtual void Function44();
	virtual void deleteContainerManager(void);
	virtual bool isActorRelevant(Actor const&);
	virtual void Function47();
	virtual void onSuspension(void);
	virtual void onLinkedSlotsChanged(void);
	virtual void Function50();
	virtual void Function51();
	virtual void sendNetworkPacket(Packet&);
	virtual void Function53();
	virtual bool isSimulated(void);
	virtual void getXuid(void);
	virtual void getMovementSettings(void);
	virtual void requestMissingSubChunk(SubChunkPos const&);
	virtual void getMaxChunkBuildRadius(void);
	virtual void onMovePlayerPacketNormal(Vector3<float> const&, Vector2<float> const&, float);
	virtual void _createChunkSource(ChunkSource&);
	virtual void setAbilities(LayeredAbilities const&);

	virtual void getEditorPlayer(void); // ServerPlayer
	virtual void destroyEditorPlayer(void); // ServerPlayer
	virtual void _getSpawnChunkLimit(void); // ServerPlayer
	virtual void _updateChunkPublisherView(Vector3<float> const&, float); // ServerPlayer
};