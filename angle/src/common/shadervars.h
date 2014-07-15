//
// Copyright (c) 2013-2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// shadervars.h:
//  Types to represent GL variables (varyings, uniforms, etc)
//

#ifndef COMMON_SHADERVARIABLE_H_
#define COMMON_SHADERVARIABLE_H_

#include <string>
#include <vector>
#include <algorithm>
#include "GLSLANG/ShaderLang.h"

namespace sh
{

// Varying interpolation qualifier, see section 4.3.9 of the ESSL 3.00.4 spec
enum InterpolationType
{
    INTERPOLATION_SMOOTH,
    INTERPOLATION_CENTROID,
    INTERPOLATION_FLAT
};

// Uniform block layout qualifier, see section 4.3.8.3 of the ESSL 3.00.4 spec
enum BlockLayoutType
{
    BLOCKLAYOUT_STANDARD,
    BLOCKLAYOUT_PACKED,
    BLOCKLAYOUT_SHARED
};

// Base class for all variables defined in shaders, including Varyings, Uniforms, etc
struct ShaderVariable
{
    ShaderVariable()
        : type(0),
          precision(0),
          arraySize(0),
          staticUse(false)
    {}

    ShaderVariable(GLenum typeIn, GLenum precisionIn, const char *nameIn, unsigned int arraySizeIn)
        : type(typeIn),
          precision(precisionIn),
          name(nameIn),
          arraySize(arraySizeIn),
          staticUse(false)
    {}

    bool isArray() const { return arraySize > 0; }
    unsigned int elementCount() const { return std::max(1u, arraySize); }

    GLenum type;
    GLenum precision;
    std::string name;
    std::string mappedName;
    unsigned int arraySize;
    bool staticUse;
};

// Uniform registers (and element indices) are assigned when outputting shader code
struct Uniform : public ShaderVariable
{
    Uniform()
        : registerIndex(-1),
          elementIndex(-1)
    {}

    Uniform(GLenum typeIn, GLenum precisionIn, const char *nameIn, unsigned int arraySizeIn,
            unsigned int registerIndexIn, unsigned int elementIndexIn)
        : ShaderVariable(typeIn, precisionIn, nameIn, arraySizeIn),
          registerIndex(registerIndexIn),
          elementIndex(elementIndexIn)
    {}

    bool isStruct() const { return !fields.empty(); }

    std::vector<Uniform> fields;

    // HLSL-specific members
    unsigned int registerIndex;
    unsigned int elementIndex; // Offset within a register, for struct members
};

struct Attribute : public ShaderVariable
{
    Attribute()
        : location(-1)
    {}

    Attribute(GLenum typeIn, GLenum precisionIn, const char *nameIn, unsigned int arraySizeIn, int locationIn)
      : ShaderVariable(typeIn, precisionIn, nameIn, arraySizeIn),
        location(locationIn)
    {}

    int location;
};

struct InterfaceBlockField : public ShaderVariable
{
    InterfaceBlockField()
        : isRowMajorMatrix(false)
    {}

    InterfaceBlockField(GLenum typeIn, GLenum precisionIn, const char *nameIn, unsigned int arraySizeIn, bool isRowMajorMatrix)
        : ShaderVariable(typeIn, precisionIn, nameIn, arraySizeIn),
          isRowMajorMatrix(isRowMajorMatrix)
    {}

    bool isStruct() const { return !fields.empty(); }

    bool isRowMajorMatrix;
    std::vector<InterfaceBlockField> fields;
};

struct Varying : public ShaderVariable
{
    Varying()
        : interpolation(INTERPOLATION_SMOOTH)
    {}

    Varying(GLenum typeIn, GLenum precisionIn, const char *nameIn, unsigned int arraySizeIn, InterpolationType interpolationIn)
        : ShaderVariable(typeIn, precisionIn, nameIn, arraySizeIn),
          interpolation(interpolationIn)
    {}

    bool isStruct() const { return !fields.empty(); }

    InterpolationType interpolation;
    std::vector<Varying> fields;
    std::string structName;
};

struct BlockMemberInfo
{
    BlockMemberInfo(int offset, int arrayStride, int matrixStride, bool isRowMajorMatrix)
        : offset(offset),
          arrayStride(arrayStride),
          matrixStride(matrixStride),
          isRowMajorMatrix(isRowMajorMatrix)
    {}

    static BlockMemberInfo getDefaultBlockInfo()
    {
        return BlockMemberInfo(-1, -1, -1, false);
    }

    int offset;
    int arrayStride;
    int matrixStride;
    bool isRowMajorMatrix;
};

typedef std::vector<BlockMemberInfo> BlockMemberInfoArray;

struct InterfaceBlock
{
    InterfaceBlock()
        : arraySize(0),
          dataSize(0),
          layout(BLOCKLAYOUT_PACKED),
          isRowMajorLayout(false),
          staticUse(false),
          registerIndex(-1)
    {}

    InterfaceBlock(const char *name, unsigned int arraySize, unsigned int registerIndex)
        : name(name),
          arraySize(arraySize),
          dataSize(0),
          layout(BLOCKLAYOUT_SHARED),
          isRowMajorLayout(false),
          staticUse(false),
          registerIndex(registerIndex)
    {}

    std::string name;
    std::string mappedName;
    unsigned int arraySize;
    size_t dataSize;
    BlockLayoutType layout;
    bool isRowMajorLayout;
    bool staticUse;
    std::vector<InterfaceBlockField> fields;
    std::vector<BlockMemberInfo> blockInfo;

    // HLSL-specific members
    unsigned int registerIndex;
};

}

#endif // COMMON_SHADERVARIABLE_H_
