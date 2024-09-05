#pragma once

struct JumpControlComponent {
public:
    //88 bytes
    BUILD_ACCESS(bool, noJumpDelay, 0x10);


    char pad_0x0[0x58];
};
static_assert(sizeof(JumpControlComponent) == 88, "JumpControlComponent size is not 88 bytes!");