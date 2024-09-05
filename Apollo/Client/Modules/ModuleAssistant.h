#pragma once

#pragma region Module imports

// Combat
#include "Combat/AutoClicker.h"
#include "Combat/Killaura.h"
#include "Combat/Teams.h"

// Misc
#include "Misc/AntiRegen.h"
#include "Misc/AutoBoombox.h"
#include "Misc/AutoLootbox.h"
#include "Misc/AutoQueue.h"
#include "Misc/AutoTool.h"
#include "Misc/Desync.h"
#include "Misc/Disabler.h"
#include "Misc/FastBreak.h"
#include "Misc/Fatngga.h"
#include "Misc/Freecam.h"
#include "Misc/Goofy.h"
#include "Misc/IRC.h"
#include "Misc/KickDecryptor.h"
#include "Misc/NetSkip.h"
#include "Misc/NoPacket.h"
#include "Misc/OreMiner.h"
#include "Misc/PacketMine.h"
#include "Misc/PingHolder.h"
#include "Misc/Regen.h"
#include "Misc/RegenOld.h"
#include "Misc/Scaffold.h"
#include "Misc/Surround.h"
#include "Misc/TestModule.h"
#include "Misc/Timer.h"

// Motion
#include "Motion/AirJump.h"
#include "Motion/AntiVoid.h"
#include "Motion/FastStop.h"
#include "Motion/Fly.h"
#include "Motion/HiveFly.h"
#include "Motion/LongJump.h"
#include "Motion/NoSlowDown.h"
#include "Motion/Speed.h"
#include "Motion/Sprint.h"
#include "Motion/Step.h"
#include "Motion/Velocity.h"

// Player
#include "Player/AntiImmobile.h"
#include "Player/AutoSpellbook.h"
#include "Player/ChestStealer.h"
#include "Player/Derp.h"
#include "Player/ExperiencePoints.h"
#include "Player/InventoryManager.h"
#include "Player/InvMove.h"
#include "Player/Killsults.h"
#include "Player/MidClickPearl.h"
#include "Player/NoJumpDelay.h"
#include "Player/NoRotate.h"
#include "Player/NoSwing.h"
#include "Player/Phase.h"

// Visual
#include "Visual/Ambience.h"
#include "Visual/Animations.h"
#include "Visual/AntiInvis.h"
#include "Visual/ArrayList.h"
#include "Visual/ClickGUI/ClickGUI.h"
#include "Visual/BlockESP.h"
#include "Visual/DestroyProgress.h"
#include "Visual/ESP.h"
#include "Visual/FullBright.h"
#include "Visual/Glint.h"
#include "Visual/Hurtcolor.h"
#include "Visual/Interface.h"
#include "Visual/JumpCircle.h"
#include "Visual/LevelInfo.h"
#include "Visual/NameTags.h"
#include "Visual/NoFire.h"
#include "Visual/Notifications.h"
#include "Visual/RedstoneESP.h"
#include "Visual/ServerRotations.h"
#include "Visual/SuperHeroFX.h"
#include "Visual/TargetHUD.h"
#include "Visual/Trails.h"
#include "Visual/ViewModel.h"
#include "Visual/Watermark.h"

#pragma endregion

// Sort the categories in the modules
void SortCategories() {
	GUICategories.push_back("Search");
	for (Module* mod : modules) {
		if (std::find(categories.begin(), categories.end(), mod->getCategory()) == categories.end()) {
			categories.push_back(mod->getCategory());
			GUICategories.push_back(mod->getCategory());
		}
	}
	GUICategories.push_back("Script");
	GUICategories.push_back("Themes");
}

// Initialize and push all modules here.
void InitializeModules() {
	// Combat
	modules.push_back(new AutoClicker(Keys::NONE, false));
	modules.push_back(new Killaura(Keys::NONE, false));
	modules.push_back(new Teams(Keys::NONE, false));
	
	// Misc
	modules.push_back(new AntiRegen(Keys::NONE, false));
	modules.push_back(new AutoBoomBox(Keys::NONE, false));
	modules.push_back(new AutoLootbox(Keys::NONE, false));
	modules.push_back(new AutoQueue(Keys::NONE, false));
	modules.push_back(new AutoTool(Keys::NONE, false));
	modules.push_back(new Desync(Keys::NONE, false));
	modules.push_back(new Disabler(Keys::NONE, false));
	modules.push_back(new FastBreak(Keys::NONE, false));
	modules.push_back(new Fatngga(Keys::NONE, false));
	modules.push_back(new Freecam(Keys::NONE, false));
	modules.push_back(new Goofy(Keys::NONE, false));
	modules.push_back(new IRC(Keys::NONE, false));
	modules.push_back(new KickDecryptor(Keys::NONE, false));
	modules.push_back(new NetSkip(Keys::NONE, false));
	modules.push_back(new NoPacket(Keys::NONE, false));
	modules.push_back(new OreMiner(Keys::NONE, false));
	modules.push_back(new PacketMine(Keys::NONE, false));
	modules.push_back(new PingHolder(Keys::NONE, false));
	modules.push_back(new Regen(Keys::NONE, false));
	modules.push_back(new RegenOld(Keys::NONE, false));
	modules.push_back(new Scaffold(Keys::NONE, false));
	modules.push_back(new Surround(Keys::NONE, false));
	modules.push_back(new TestModule(Keys::NONE, false));
	modules.push_back(new Timer(Keys::NONE, false));
	
	// Movement
	modules.push_back(new AirJump(Keys::NONE, false));
	modules.push_back(new AntiVoid(Keys::NONE, false));
	modules.push_back(new FastStop(Keys::NONE, false));
	modules.push_back(new Fly(Keys::NONE, false));
	modules.push_back(new HiveFly(Keys::NONE, false));
	modules.push_back(new LongJump(Keys::NONE, false));
	modules.push_back(new NoSlowDown(Keys::NONE, false));
	modules.push_back(new Speed(Keys::NONE, false));
	modules.push_back(new Sprint(Keys::NONE, false));
	modules.push_back(new Step(Keys::NONE, false));
	modules.push_back(new Velocity(Keys::NONE, false));

	// Player
	modules.push_back(new AntiImmobile(Keys::NONE, false));
	modules.push_back(new AutoSpellbook(Keys::NONE, false));
	modules.push_back(new ChestStealer(Keys::NONE, false));
	modules.push_back(new Derp(Keys::NONE, false));
	modules.push_back(new ExperiencePoints(Keys::NONE, false));
	modules.push_back(new InventoryManager(Keys::NONE, false));
	modules.push_back(new InvMove(Keys::NONE, false));
	modules.push_back(new Killsults(Keys::NONE, false));
	modules.push_back(new MidClickPearl(Keys::NONE, false));
	modules.push_back(new NoJumpDelay(Keys::NONE, false));
	modules.push_back(new NoRotate(Keys::NONE, false));
	modules.push_back(new NoSwing(Keys::NONE, false));
	modules.push_back(new Phase(Keys::NONE, false));
	
	// Renders
	modules.push_back(new Ambience(Keys::NONE, false));
	modules.push_back(new Animations(Keys::NONE, false));
	modules.push_back(new AntiInvis(Keys::NONE, false));
	modules.push_back(new ArrayList(Keys::NONE, true));
	modules.push_back(new BlockESP(Keys::NONE, false));
	modules.push_back(new ClickGUI(Keys::INSERT, false));
	modules.push_back(new DestroyProgress(Keys::NONE, false));
	modules.push_back(new ESP(Keys::NONE, false));
	modules.push_back(new Fullbright(Keys::NONE, false));
	modules.push_back(new Glint(Keys::NONE, false));
	modules.push_back(new Hurtcolor(Keys::NONE, false));
	modules.push_back(new Interface(Keys::NONE, true));
	modules.push_back(new JumpCircle(Keys::NONE, false)); 
	modules.push_back(new LevelInfo(Keys::NONE, false));
	modules.push_back(new NameTags(Keys::NONE, false));
	modules.push_back(new NoFire(Keys::NONE, false));
	modules.push_back(new NotificationsModule(Keys::NONE, true));
	modules.push_back(new RedstoneESP(Keys::NONE, false));
	modules.push_back(new ServerRotations(Keys::NONE, true));
	modules.push_back(new SuperHeroFX(Keys::NONE, false));
	modules.push_back(new TargetHUD(Keys::NONE, false));
	modules.push_back(new Trails(Keys::NONE, false));
	modules.push_back(new ViewModel(Keys::NONE, false));
	modules.push_back(new Watermark(Keys::NONE, true));
}

void InitializeMods() {
	InitializeModules(); // Initialize modules.
	SortCategories(); // create categories
}

// Uninitialize modules.
void UninitializeMods() {
	for (auto mod : modules) {
		if (mod->isEnabled()) // If a module is enabled.
			mod->toggle(); // Disable the module.

		delete mod;
	}
}