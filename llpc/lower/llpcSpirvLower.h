/*
 ***********************************************************************************************************************
 *
 *  Copyright (c) 2019-2021 Advanced Micro Devices, Inc. All Rights Reserved.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 **********************************************************************************************************************/
/**
 ***********************************************************************************************************************
 * @file  llpcSpirvLower.h
 * @brief LLPC header file: contains declaration of class Llpc::SpirvLower.
 ***********************************************************************************************************************
 */
#pragma once

#include "llpc.h"
#include "llpcUtil.h"
#include "llvm/Pass.h"

namespace llvm {

class Constant;
class GlobalVariable;
class Timer;

namespace legacy {

class PassManager;

} // namespace legacy

class PassRegistry;
void initializeSpirvLowerAccessChainPass(PassRegistry &);
void initializeSpirvLowerMathConstFoldingPass(PassRegistry &);
void initializeSpirvLowerMathFloatOpPass(PassRegistry &);
void initializeSpirvLowerConstImmediateStorePass(PassRegistry &);
void initializeSpirvLowerMemoryOpPass(PassRegistry &);
void initializeSpirvLowerGlobalPass(PassRegistry &);
void initializeSpirvLowerInstMetaRemovePass(PassRegistry &);
void initializeSpirvLowerResourceCollectPass(PassRegistry &);
void initializeSpirvLowerTerminatorPass(PassRegistry &);
void initializeLegacySpirvLowerTranslatorPass(PassRegistry &);
} // namespace llvm

namespace lgc {

class Builder;

} // namespace lgc

namespace Llpc {

// Initialize passes for SPIR-V lowering
//
// @param passRegistry : Pass registry
inline static void initializeLowerPasses(llvm::PassRegistry &passRegistry) {
  initializeSpirvLowerAccessChainPass(passRegistry);
  initializeSpirvLowerConstImmediateStorePass(passRegistry);
  initializeSpirvLowerMathConstFoldingPass(passRegistry);
  initializeSpirvLowerMathFloatOpPass(passRegistry);
  initializeSpirvLowerMemoryOpPass(passRegistry);
  initializeSpirvLowerGlobalPass(passRegistry);
  initializeSpirvLowerInstMetaRemovePass(passRegistry);
  initializeSpirvLowerResourceCollectPass(passRegistry);
  initializeSpirvLowerTerminatorPass(passRegistry);
  initializeLegacySpirvLowerTranslatorPass(passRegistry);
}

class Context;

llvm::ModulePass *createSpirvLowerAccessChain();
llvm::ModulePass *createSpirvLowerConstImmediateStore();
llvm::ModulePass *createSpirvLowerMathConstFolding();
llvm::ModulePass *createSpirvLowerMathFloatOp();
llvm::ModulePass *createSpirvLowerMemoryOp();
llvm::ModulePass *createSpirvLowerGlobal();
llvm::ModulePass *createSpirvLowerInstMetaRemove();
llvm::ModulePass *createSpirvLowerResourceCollect(bool collectDetailUsage);
llvm::ModulePass *createSpirvLowerTerminator();
llvm::ModulePass *createSpirvLowerTranslator(ShaderStage stage, const PipelineShaderInfo *shaderInfo);

// =====================================================================================================================
// Represents the pass of SPIR-V lowering operations, as the base class.
class SpirvLower {
public:
  explicit SpirvLower()
      : m_module(nullptr), m_context(nullptr), m_shaderStage(ShaderStageInvalid), m_entryPoint(nullptr) {}

  static void removeConstantExpr(Context *context, llvm::GlobalVariable *global);
  static void replaceConstWithInsts(Context *context, llvm::Constant *const constVal);

protected:
  void init(llvm::Module *module);

  llvm::Module *m_module;       // LLVM module to be run on
  Context *m_context;           // Associated LLPC context of the LLVM module that passes run on
  ShaderStage m_shaderStage;    // Shader stage
  llvm::Function *m_entryPoint; // Entry point of input module
  lgc::Builder *m_builder;      // LGC builder object
};

// =====================================================================================================================
// Legacy pass manager wrapper class. Used as the base class for the legacy
// lower passes.
class LegacySpirvLower : public llvm::ModulePass, public SpirvLower {
public:
  explicit LegacySpirvLower(char &pid) : llvm::ModulePass(pid) {}

  // Add per-shader lowering passes to pass manager
  static void addPasses(Context *context, ShaderStage stage, llvm::legacy::PassManager &passMgr,
                        llvm::Timer *lowerTimer
  );

private:
  LegacySpirvLower() = delete;
  LegacySpirvLower(const LegacySpirvLower &) = delete;
  LegacySpirvLower &operator=(const LegacySpirvLower &) = delete;
};

} // namespace Llpc
