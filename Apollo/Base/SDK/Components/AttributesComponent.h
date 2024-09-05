#pragma once

struct AttributesComponent
{
    BaseAttributeMap baseAttributeMap;
};

static_assert(sizeof(AttributesComponent) == 0x58);