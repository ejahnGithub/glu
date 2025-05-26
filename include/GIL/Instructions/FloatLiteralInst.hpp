#ifndef GLU_GIL_INSTRUCTIONS_FLOAT_LITERAL_INST_HPP
#define GLU_GIL_INSTRUCTIONS_FLOAT_LITERAL_INST_HPP

#include "ConstantInst.hpp"
#include <llvm/Support/TrailingObjects.h>

namespace glu::gil {

class FloatLiteralInst final
    : public ConstantInst,
      private llvm::TrailingObjects<FloatLiteralInst, llvm::APFloat> {
    using TrailingArgs = llvm::TrailingObjects<FloatLiteralInst, llvm::APFloat>;
    friend TrailingArgs;

    Type type;

    // Method required by TrailingObjects to determine the number of trailing
    // objects
    size_t numTrailingObjects(
        typename TrailingArgs::OverloadToken<llvm::APFloat>
    ) const
    {
        return 1; // Always 1 APFloat
    }

    // Private constructor
    FloatLiteralInst(Type type, llvm::APFloat const &value)
        : ConstantInst(InstKind::FloatLiteralInstKind), type(type)
    {
        // Use the copy constructor of APFloat to initialize the trailing object
        new (getTrailingObjects<llvm::APFloat>()) llvm::APFloat(value);
    }

public:
    static FloatLiteralInst *create(
        llvm::BumpPtrAllocator &allocator, Type type, llvm::APFloat const &value
    )
    {
        void *mem = allocator.Allocate(
            totalSizeToAlloc<llvm::APFloat>(1), alignof(FloatLiteralInst)
        );
        return new (mem) FloatLiteralInst(type, value);
    }

    Type getType() const { return type; }
    void setType(Type newType) { this->type = newType; }

    llvm::APFloat const &getValue() const
    {
        return *getTrailingObjects<llvm::APFloat>();
    }

    // Direct modification of the value is not allowed after creation.
    // If necessary, create a new instance.

    Operand getOperand(size_t index) const override
    {
        switch (index) {
        case 0: return getType();
        case 1: return getValue();
        default: llvm_unreachable("Invalid operand index");
        }
    }

    Type getResultType([[maybe_unused]] size_t index) const override
    {
        return type;
    }

    static bool classof(InstBase const *inst)
    {
        return inst->getKind() == InstKind::FloatLiteralInstKind;
    }

    size_t getOperandCount() const override
    {
        return 2; // Type and APFloat
    }
};

} // end namespace glu::gil

#endif // GLU_GIL_INSTRUCTIONS_FLOAT_LITERAL_INST_HPP
