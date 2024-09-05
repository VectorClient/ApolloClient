#pragma once

class Sprint : public Module
{
public:
    Sprint(int keybind = Keys::NONE, bool enabled = false) :
        Module("Sprint", "Motion", "Automatically sprint without holding the key.", keybind, enabled)
    {
        addEnum("Mode", "Sprint mode", { "Normal", "Omni" }, &SprintMode);
    }

    int SprintMode = 0;

    void onEvent(RenderContextEvent* event) override
    {
        Player* player = Address::getLocalPlayer();
        ClientInstance* instance = Address::getClientInstance();

        // Check if player or valid key is null
        if (!player)
            return;

        bool isMoving = instance->getMinecraftGame()->getCanUseKeys();

        if (player->getStatusFlag(ActorFlags::Moving)) {
            if (SprintMode == 1) {
                player->setSprinting(true);
            }
            else if (SprintMode != 1 && player->getMoveInputHandler()->forward) {
                player->setSprinting(true);
            }
        }

        if (isMoving)
        {
            StateVectorComponent* stateVec = player->getStateVector();

            if (SprintMode == 1) {
                player->setSprinting(true);
            }
            else if (SprintMode != 1 && player->getMoveInputHandler()->forward) {
                player->setSprinting(true);
            }
        }
        else {
            player->setSprinting(false);
  
        }
    }
};