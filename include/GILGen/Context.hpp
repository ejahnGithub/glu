#ifndef GLU_GILGEN_CONTEXT_HPP
#define GLU_GILGEN_CONTEXT_HPP

#include "Decls.hpp"
#include "Stmts.hpp"

#include "BasicBlock.hpp"
#include "Instructions.hpp"

namespace glu::gilgen {

/// @brief The context/builder for the GIL code generation.
class Context {
    gil::Function *_function;
    gil::BasicBlock *_currentBB;
    gil::InstBase *_insertBefore = nullptr; // Insert at end of block by default
    ast::FunctionDecl *_functionDecl;
    llvm::BumpPtrAllocator &_arena;

public:
    Context(ast::FunctionDecl *decl, llvm::BumpPtrAllocator &arena);

    /// Returns the AST function being compiled.
    ast::FunctionDecl *getASTFunction() const { return _functionDecl; }

    /// Returns the GIL function being generated.
    gil::Function *getCurrentFunction() const { return _function; }

    /// Returns the current GIL basic block.
    gil::BasicBlock *getCurrentBasicBlock() const { return _currentBB; }

    /// Returns the current insertion point.
    gil::InstBase *getInsertionPoint() const { return _insertBefore; }

    /// Sets the current insertion point.
    void setInsertionPoint(gil::BasicBlock *bb, gil::InstBase *inst)
    {
        _currentBB = bb;
        _insertBefore = inst;
    }

    /// Positions the insertion point at the end of the given basic block.
    void positionAtEnd(gil::BasicBlock *bb)
    {
        _currentBB = bb;
        _insertBefore = nullptr;
    }

private:
    template <typename T> T *insertInstruction(T *inst)
    {
        static_assert(
            std::is_base_of_v<gil::InstBase, T>, "Invalid instruction type"
        );
        static_assert(
            !std::is_base_of_v<gil::TerminatorInst, T>,
            "Use insertTerminator for terminators"
        );
        assert(_currentBB && "Invalid context: no current basic block");
        _currentBB->addInstructionBefore(inst, _insertBefore);
        return inst;
    }

    template <typename T> T *insertTerminator(T *term)
    {
        static_assert(
            std::is_base_of_v<gil::TerminatorInst, T>, "Invalid terminator type"
        );
        assert(_currentBB && "Invalid context: no current basic block");
        assert(
            _currentBB->getTerminator() == nullptr
            && "Basic block already has a terminator"
        );
        assert(
            _insertBefore == nullptr
            && "Terminator must be inserted at the end of the block"
        );
        _currentBB->addInstructionAtEnd(term);
        _currentBB = nullptr;
        return term;
    }

public:
    /// Generate an unreachable basic block — no basic block branches to it.
    gil::BasicBlock *buildUnreachableBB()
    {
        auto *bb = new (_arena) gil::BasicBlock("unreachable");
        _function->addBasicBlockAtEnd(bb);
        return bb;
    }

    gil::BrInst *buildBr(gil::BasicBlock *dest)
    {
        return insertTerminator(new (_arena) gil::BrInst(dest));
    }

    gil::UnreachableInst *buildUnreachable()
    {
        return insertTerminator(new (_arena) gil::UnreachableInst());
    }

    gil::ReturnInst *buildRetVoid()
    {
        // TODO: design proper way to return void
        return insertTerminator(new (_arena)
                                    gil::ReturnInst(gil::Value::getEmptyKey()));
    }

    gil::ReturnInst *buildRet(gil::Value retValue)
    {
        return insertTerminator(new (_arena) gil::ReturnInst(retValue));
    }

    gil::StoreInst *buildStore(gil::Value value, gil::Value ptr)
    {
        return insertInstruction(new (_arena) gil::StoreInst(value, ptr));
    }
};

} // namespace glu::gilgen

#endif // GLU_GILGEN_CONTEXT_HPP
