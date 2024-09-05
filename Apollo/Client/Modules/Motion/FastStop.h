#pragma once

class FastStop : public Module
{
public:
    FastStop(int keybind = Keys::NONE, bool enabled = false) :
        Module("FastStop", "Motion", "Quickly stops the player's movement", keybind, enabled)
    {
        addSlider("Speed", "The speed of which to stop", &stopAmount, 2, 6);
    }

    float stopAmount = 6;

    void onEvent(ActorBaseTickEvent* event) override
    {
        Player* player = Address::getLocalPlayer();
        ClientInstance* instance = Address::getClientInstance();

        // Check if player or valid key is null
        if (!player || !player->getMoveInputHandler())
            return;

        bool isMoving = instance->getMinecraftGame()->getCanUseKeys() && player->getMoveInputHandler()->isPressed();

        if (!isMoving)
        {
            StateVectorComponent* stateVec = player->getStateVector();

            // If state vector component is missing then no further action needed
            if (!stateVec)
                return;

            if (stopAmount != 6)
            {
                // Adjust velocity based on stop amount
                stateVec->Velocity.x /= stopAmount;
                stateVec->Velocity.z /= stopAmount;
            }
            else
            {
                // Set velocity to zero
                stateVec->Velocity.x = 0;
                stateVec->Velocity.z = 0;
            }
        }
    }
};
