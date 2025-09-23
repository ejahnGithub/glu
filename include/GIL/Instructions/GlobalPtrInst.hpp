#ifndef GLU_GIL_GLOBAL_PTR_INST_HPP
#define GLU_GIL_GLOBAL_PTR_INST_HPP

#include "ConstantInst.hpp"
#include "Global.hpp"

namespace glu::gil {

/// @class GlobalPtrInst
/// @brief Represents an instruction that creates and holds a pointer to a
///        global variable.
///
/// This class is derived from InstBase and represents an instruction
/// to hold a pointer to a global variable in the GLU GIL (Generic Intermediate
/// Language).
class GlobalPtrInst : public ConstantInst {
    Global *_global; ///< The global variable.
    Type _type; ///< The type of the global variable pointer.

public:
    /// @brief Constructs a GlobalPtrInst object.
    ///
    /// @param global The global variable.
    /// @param type The type of the global variable pointer.
    GlobalPtrInst(Global *global, Type type)
        : ConstantInst(InstKind::GlobalPtrInstKind)
        , _global(global)
        , _type(type)
    {
        assert(global && "GlobalPtrInst requires a valid global variable");
    }

    /// @brief Gets the operand at the specified index.
    ///
    /// @param index The index of the operand.
    /// @return The operand at the specified index.
    Operand getOperand(size_t index) const override
    {
        if (index == 0)
            return Operand(_global);
        if (index == 1)
            return Operand(_type);
        llvm_unreachable("Invalid operand index");
    }

    /// @brief Gets the result type at the specified index.
    ///
    /// @param index The index of the result type.
    /// @return The result type at the specified index.
    Type getResultType([[maybe_unused]] size_t index) const override
    {
        return _type;
    }

    /// @brief Gets the global variable.
    ///
    /// @return The global variable.
    Global *getGlobal() const { return _global; }

    /// @brief Checks if the given instruction is of type GlobalPtrInst.
    ///
    /// @param inst The instruction to check.
    /// @return True if the instruction is of type GlobalPtrInst, false
    /// otherwise.
    static bool classof(InstBase const *inst)
    {
        return inst->getKind() == InstKind::GlobalPtrInstKind;
    }
};

} // end namespace glu::gil

#endif // GLU_GIL_GLOBAL_PTR_INST_HPP
