#pragma once
#include "../../../../Libs/entt/entt/entt.hpp"

template <>
class entt::entt_traits<EntityId> : public entt::basic_entt_traits<EntityIdTraits> {
public:
    static constexpr entity_type page_size = 2048;
};

class EntityId : public entt::entt_traits<EntityId> {
public:
    entity_type mRawId{};

    template <std::integral T>
        requires(!std::is_same_v<std::remove_cvref_t<T>, bool>)
    [[nodiscard]] constexpr EntityId(T rawId) : mRawId(static_cast<entity_type>(rawId))
    {
    } // NOLINT

    [[nodiscard]] constexpr bool isNull() const { return *this == entt::null; }

    template <std::integral T>
        requires(!std::is_same_v<std::remove_cvref_t<T>, bool>)
    [[nodiscard]] constexpr operator T() const
    {
        return static_cast<T>(mRawId);
    }
};

//static_assert(sizeof(entt::basic_registry<EntityId>) == 0x120);