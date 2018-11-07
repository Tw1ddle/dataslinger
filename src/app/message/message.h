#pragma once

#include <cstdint>

namespace dataslinger
{
namespace app
{
namespace message
{

enum class SlingerSentMessageId : std::uint32_t
{
    RUN_INLINE_CHAISCRIPT_SCRIPT ///< Instruction to execute the UTF-8 encoded ChaiScript script that follows this id
};

enum class ReceiverSentMessageId : std::uint32_t
{
    RUN_INLINE_CHAISCRIPT_SCRIPT ///< Instruction to execute the UTF-8 encoded ChaiScript script that follows this id
};

}
}
}
