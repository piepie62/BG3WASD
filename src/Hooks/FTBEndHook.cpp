#include "FTBEndHook.hpp"
#include "../Settings.hpp"
#include "../State.hpp"

bool FTBEndHook::Prepare()
{
    std::array<uintptr_t, 1> address_array = { AsAddress(dku::Hook::Assembly::search_pattern<
        "E8 ?? ?? ?? ?? 4D ?? ?? 4C ?? ?? ?? 48 ?? ?? 48 ?? ?? E8 ?? ?? ?? ?? 49 ?? ?? E8">()) };

    if (!address_array[0])
    {
        address_array = { AsAddress(dku::Hook::Assembly::search_pattern<
            "E8 ?? ?? ?? ?? 4C ?? ?? 4C ?? ?? ?? ?? 48 ?? ?? 48 ?? "
            "?? E8 ?? ?? ?? ?? 48 ?? ?? E8 ?? ?? ?? ?? 41">()) };
    }
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("FTBEndHook #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    return all_found;
}

void FTBEndHook::Enable()
{
    if (not all_found)
    {
        return;
    }
    int i = 0;
    for (const auto& address : addresses)
    {
        OriginalFunc = dku::Hook::write_call<5>(address, OverrideFunc);
        DEBUG("Hooked FTBEndHook #{}: {:X}", i, AsAddress(address));
        ++i;
    }
}

int64_t FTBEndHook::OverrideFunc(int64_t a1, int64_t a2, char* a3)
{
    auto* state = State::GetSingleton();

    int64_t ret = OriginalFunc(a1, a2, a3);

    if (*Settings::GetSingleton()->enable_auto_toggling_movement_mode &&
        SDL_GetTicks() - state->last_time_combat_state_changed > 500)
    {
        state->SetCharacterMovementMode(true);
    }

    return ret;
}
