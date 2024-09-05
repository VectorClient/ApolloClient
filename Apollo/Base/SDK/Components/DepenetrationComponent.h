#pragma once

struct DepenetrationComponent {
    int field_0x0;
    char pad_0x0000[0x34]; //0x0000

    [[nodiscard]] bool isPenetrating() const
    {
        return field_0x0 != 10;
    }
};
static_assert(sizeof(DepenetrationComponent) == 0x38, "DepenetrationComponent size is incorrect");