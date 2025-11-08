#include <memory>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/FileSystem.h"

#include "ast.h"

using namespace std;

static unique_ptr<llvm::LLVMContext> g_ctx;
static unique_ptr<llvm::Module> g_module;
static unique_ptr<llvm::IRBuilder<>> g_builder;

void
translation_unit_n::llvm_codegen() const
{
  g_ctx = make_unique<llvm::LLVMContext>();
  g_module = make_unique<llvm::Module>(this->get_filename(), *g_ctx);
  g_builder = make_unique<llvm::IRBuilder<>>(*g_ctx);

  string output_filename = this->get_filename() + ".ll";
  error_code EC;
  llvm::raw_fd_ostream fout(output_filename, EC, llvm::sys::fs::OF_None);
  if (EC) {
    cout << "Error:\n" << EC.message() << "\n";
  }
  else {
    g_module->print(fout, nullptr);
  }
}

