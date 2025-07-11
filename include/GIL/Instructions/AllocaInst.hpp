#ifndef GLU_GIL_INSTRUCTIONS_ALLOCATION_HPP
#define GLU_GIL_INSTRUCTIONS_ALLOCATION_HPP

#include "AST/ASTContext.hpp"
#include "AST/ASTNode.hpp"
#include "InstBase.hpp"
#include "Type.hpp"

namespace glu::gil {

///
///  @class AllocaInst
///  @brief Represents an allocation instruction.
///
///  This class is derived from InstBase and represents an allocation
///  instruction in the GLU GIL (Generic Intermediate Language).
///
class AllocaInst : public InstBase {
    using Context = glu::ast::ASTContext;
    Type _ptr; ///< The pointer type.
    Type _pointeeType; ///< The pointee type.
public:
    ///
    /// @brief Constructs an AllocaInst object.
    ///
    /// @param type The type of the allocation.
    /// @param context The AST Context.
    ///
    AllocaInst(Type pointeeType, Type pointerType)
        : InstBase(InstKind::AllocaInstKind)
        , _ptr(pointerType)
        , _pointeeType(pointeeType)
    {
    }

    Type getPointeeType() const { return _pointeeType; }

    virtual size_t getResultCount() const override { return 1; }

    virtual size_t getOperandCount() const override { return 1; }

    virtual Operand getOperand(size_t index) const override
    {
        assert(index == 0 && "Operand index out of range");
        return Operand(_pointeeType);
    }

    virtual Type getResultType(size_t index) const override
    {
        assert(index == 0 && "Result index out of range");
        return _ptr;
    }

    static bool classof(InstBase const *inst)
    {
        return inst->getKind() == InstKind::AllocaInstKind;
    }
};

} // end namespace glu::gil

#endif // GLU_GIL_INSTRUCTIONS_ALLOCATION_HPP
