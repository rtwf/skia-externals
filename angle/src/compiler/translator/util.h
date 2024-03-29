//
// Copyright (c) 2002-2010 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef COMPILER_UTIL_H
#define COMPILER_UTIL_H

#include <stack>

#include "compiler/translator/Types.h"
#include "angle_gl.h"
#include "common/shadervars.h"

// atof_clamp is like atof but
//   1. it forces C locale, i.e. forcing '.' as decimal point.
//   2. it clamps the value to -FLT_MAX or FLT_MAX if overflow happens.
// Return false if overflow happens.
extern bool atof_clamp(const char *str, float *value);

// If overflow happens, clamp the value to INT_MIN or INT_MAX.
// Return false if overflow happens.
extern bool atoi_clamp(const char *str, int *value);

namespace sh
{

GLenum GLVariableType(const TType &type);
GLenum GLVariablePrecision(const TType &type);
bool IsVaryingIn(TQualifier qualifier);
bool IsVaryingOut(TQualifier qualifier);
bool IsVarying(TQualifier qualifier);
InterpolationType GetInterpolationType(TQualifier qualifier);
BlockLayoutType GetBlockLayoutType(TLayoutBlockStorage blockStorage);
TString ArrayString(const TType &type);

template <typename VarT>
class GetVariableTraverser
{
  public:
    void traverse(const TType &type, const TString &name);

  protected:
    GetVariableTraverser(std::vector<VarT> *output);

    // Must be overloaded
    virtual void visitVariable(VarT *newVar) = 0;

  private:
    std::stack<std::vector<VarT> *> mOutputStack;
};

struct GetInterfaceBlockFieldTraverser : public GetVariableTraverser<InterfaceBlockField>
{
  public:
    GetInterfaceBlockFieldTraverser(std::vector<InterfaceBlockField> *output, bool isRowMajorMatrix);

  private:
    virtual void visitVariable(InterfaceBlockField *newField);

    bool mIsRowMajorMatrix;
};

}

#endif // COMPILER_UTIL_H
