#pragma once

class RedstoneESP : public Module
{
public:
    RedstoneESP(int keybind = Keys::NONE, bool enabled = false) :
        Module("RedstoneESP", "Visual", "Esp for redstone ores", keybind, enabled)
    {
        addBool("Lit Redstone", "include lit redstone", &litredstone);
        addSlider("Range", "How far around you is esp rendered", &range, 1, 100);
    }

    bool litredstone = true;
    float range = 20;

    bool Render = false;

    vector<Vector3<int>> blocks;
    vector<Vector3<int>> blocksFuni;

    Vector3<float> blockPos;

    Vector3<int> getBlockPosition()
    {
        auto localPlayer = Address::getLocalPlayer();
        //if (localPlayer == nullptr) return;

        Vector3<float> pos = localPlayer->getAABBShape()->PosLower;

        return { (int)floor(pos.x), (int)floor(pos.y), (int)floor(pos.z) };
    }

    void onEvent(RenderContextEvent* event) override {
        auto player = Address::getLocalPlayer();
        if (player == nullptr) {
            return;
        }

        Vector3<float> pos = player->getStateVector()->Position;

        for (int x = -range; x <= range; x++) {
            for (int z = -range; z <= range; z++) {
                for (int y = -range; y <= range; y++) {
                    //blocks.push_back(Vector3<int>(x, y, z));

                    int id = (int)Address::getBlockSource()->getBlock(player->getStateVector()->Position.add(Vector3<int>(x, y, z).ToFloat()).ToInt())->GetBlockLegacy()->getBlockID();
                    // bool Render = false;

                    if (id == 73 || id == 74) {
                        if (blocks.empty())
                        blocks.push_back(Vector3<int>(x, y, z));
                    }
                    else {
                        continue;
                    }
                }
            }
        }

        /*for (const Vector3<int>& offset : blocks) {
            blockPos = player->getStateVector()->Position.add(Vector3<int>(offset).ToFloat());

            Block* block = Address::getBlockSource()->getBlock(blockPos.ToInt());

            if (block->GetBlockLegacy()->getBlockID() != 73 && block->GetBlockLegacy()->getBlockID() != 74) continue;

            Vector3<float> angle = blockPos;
            angle = angle.floor();
            angle.x += 1;
            angle.y += 1;
            angle.z += 1;

            //ImRenderUtil::drawBox(Vector3<float>(0, 0, 0), angle, UIColor(0, 199, 199), UIColor(255, 127, 127), 2.2, false, true);
        }*/
    }

    void onEvent(ImGuiRenderEvent* event) override {
        auto player = Address::getLocalPlayer();
        if (player == nullptr || !Address::canUseKeys()) {
            return;
        }

        blockPos = Vector3<float>(0, 50, 0);
        Vector3<float> blockPos1(0, 50, -1);
        Vector3<float> blockPos11(0, 50, 1);
        Vector3<float> blockPos111(0, 51, 0);
        Vector3<float> blockPos1111(0, 52, 0);
        Vector3<float> blockPos11111(0, 53, 0);

        //Block* block = Address::getBlockSource()->getBlock(blockPos.ToInt());

        //if (block->GetBlockLegacy()->getBlockID() != 73 && block->GetBlockLegacy()->getBlockID() != 74) continue;

        Vector3<float> angle = blockPos;
        Vector3<float> angle1 = blockPos1;
        Vector3<float> angle11 = blockPos11;
        Vector3<float> angle111 = blockPos111;
        Vector3<float> angle1111 = blockPos1111;
        Vector3<float> angle11111 = blockPos11111;
        angle = angle.floor();
        angle.x += 1;
        angle.y += 1;
        angle.z += 1;
        angle1 = angle1.floor();
        angle1.x += 1;
        angle1.y += 1;
        angle1.z += 1;
        angle11 = angle11.floor();
        angle11.x += 1;
        angle11.y += 1;
        angle11.z += 1;
        angle111 = angle111.floor();
        angle111.x += 1;
        angle111.y += 1;
        angle111.z += 1;
        angle1111 = angle1111.floor();
        angle1111.x += 1;
        angle1111.y += 1;
        angle1111.z += 1;
        angle11111 = angle11111.floor();
        angle11111.x += 1;
        angle11111.y += 1;
        angle11111.z += 1;

        ImRenderUtil::drawBox(blockPos, angle, UIColor(0, 199, 199), UIColor(255, 127, 127), 2.2, true, true);
        ImRenderUtil::drawBox(blockPos1, angle1, UIColor(0, 199, 199), UIColor(255, 127, 127), 2.2, false, true);
        ImRenderUtil::drawBox(blockPos11, angle11, UIColor(0, 199, 199), UIColor(255, 127, 127), 2.2, false, true);
        ImRenderUtil::drawBox(blockPos111, angle111, UIColor(0, 199, 199), UIColor(255, 127, 127), 2.2, false, true);
        ImRenderUtil::drawBox(blockPos1111, angle1111, UIColor(0, 199, 199), UIColor(255, 127, 127), 2.2, false, true);
        ImRenderUtil::drawBox(blockPos11111, angle11111, UIColor(0, 199, 199), UIColor(255, 127, 127), 2.2, false, true);
    }
};