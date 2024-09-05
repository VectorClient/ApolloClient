#pragma once

// You might use this if you want to group all of your global variables,
// functions, etc. under a common name to make them easier to find and organize
namespace Global {
	static inline std::map<uint64_t, bool> Keymap = {}; // Keymap for key checks.

	// Rotations
	static inline float headYaw = 0.f;
	static inline float bodyYaw = 0.f;
	static inline float pitch = 0.f;

	static inline float easedHeadYaw = 0.f;
	static inline float easedBodyYaw = 0.f;
	static inline float easedPitch = 0.f;
	
	static inline float HeadYawSpeed = 2.07;
	static inline float BodyYawSpeed = 1.04;
	static inline float PitchSpeed = 2.07;

	static inline bool lockBodyYaw = true;

	static inline bool stopESP = false;

	// Interface
	struct HoverTextInfo {
		bool show = false;
		std::string text = "";
		Vector2<float> pos{ 0,0 };
	};

	static inline bool doChams = true;

	HoverTextInfo info;

	// Notifications
	static inline NotificationsManager Notification{};
	static inline bool RenderNotifications = true;

	// User and Auth
	static inline std::string Username = "Unknown";
	static inline bool renderUI = false;

	// Speed
	static inline Vector3<float> lastLerpVelocity = NULL;
	static inline bool effectSwiftness = false;

	// Regen
	static inline bool isMiningRedstone = false;
	static inline bool shouldAttack = true;
	static inline Vector3<int> miningPosition = NULL;
	static inline Vector3<int> stealingBlockPos = NULL;
	static inline Vector3<int> targettingBlockPos = NULL;
	static inline Vector3<int> blacklistedBlockPos = NULL;
	static inline bool DurabilityExploit = false;
	static inline bool StealOres = false;
	static inline bool StopStealing = false;

	// Ping Holder
	static inline float PingHolderValue = 70;
	static inline bool RandomizePing = true;
	static inline float LowPingValue = 30;
	static inline float HighPingValue = 40;

	static inline float testModuleFly = 1.5;

	static inline bool isOnFire = false;

	// Colours
	static inline int ArrayListColors = 0;
	static inline int ClientColor = 0;

	// Scaffold
	static inline int SwingType = false;

	// IsAtacking
	static inline bool isAttacking = false;

	// LayerName
	static inline std::string LayerName = "FuckMeUnknown";

	static inline float AmbienceTime = 15000;
	static inline float BreakSpeed = 10;
	static inline float scaffoldScale = 0;

	// Desync position
	static inline Vector3<float> freecamPosition(0, 0, 0);
	static inline Vector3<float> desyncPosition(0, 0, 0);
	static inline Vector3<float> testPosition(0, 0, 0);

	// ShouldBlock
	static inline bool ShouldBlock = false;
	static inline float swingSpeed = 13.f;

	// Killsults
	static inline bool shouldQueue = false;
	static inline bool shouldQueueOnDeath = false;
	static inline bool shouldQueueOnGameEnd = false;
	static inline std::string LastGame = "SKY"; // The target player name
	static inline bool Killed = false; // Weither you killed a player or not
	static inline std::string KilledTarget = "here"; // The target player name
	static inline int KilledTicks = 0;

	//HiveFly
	static inline int canFly = 0;
	static inline float fakeTimer = 3.f;
	static inline bool stopFly = false;
	static inline bool canChange = false;

	// NetSkip
	static inline bool shouldLagTicks = false;

	namespace RenderInfo {
		static inline HWND Window = nullptr;
		static inline ID3D11ShaderResourceView* HeadTexture = NULL;
		static inline ID3D11ShaderResourceView* CombatTexture = NULL;
		static inline ID3D11ShaderResourceView* MovementTexture = NULL;
		static inline ID3D11ShaderResourceView* PlayerTexture = NULL;
		static inline ID3D11ShaderResourceView* VisualTexture = NULL;
		static inline ID3D11ShaderResourceView* MiscTexture = NULL;
		static inline ID3D11ShaderResourceView* UserProfileTexture = NULL;
	}
}
