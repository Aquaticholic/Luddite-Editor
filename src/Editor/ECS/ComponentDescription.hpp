#pragma once
#include "Editor/pch.hpp"



#define COMPONENT_DATA_TYPES_DECLARE \
        COMPONENT_DATA_TYPE_DECLARATION(F32, float) \
        COMPONENT_DATA_TYPE_DECLARATION(F64, double) \
        COMPONENT_DATA_TYPE_DECLARATION(I8, int8_t) \
        COMPONENT_DATA_TYPE_DECLARATION(I16, int16_t) \
        COMPONENT_DATA_TYPE_DECLARATION(I32, int32_t) \
        COMPONENT_DATA_TYPE_DECLARATION(I64, int64_t) \
        COMPONENT_DATA_TYPE_DECLARATION(U8, uint8_t) \
        COMPONENT_DATA_TYPE_DECLARATION(U16, uint16_t) \
        COMPONENT_DATA_TYPE_DECLARATION(U32, uint32_t) \
        COMPONENT_DATA_TYPE_DECLARATION(U64, uint64_t) \
        COMPONENT_DATA_TYPE_DECLARATION(Vec1, glm::vec1) \
        COMPONENT_DATA_TYPE_DECLARATION(Vec2, glm::vec2) \
        COMPONENT_DATA_TYPE_DECLARATION(Vec3, glm::vec3) \
        COMPONENT_DATA_TYPE_DECLARATION(Vec4, glm::vec4) \
        COMPONENT_DATA_TYPE_DECLARATION(IVec1, glm::ivec1) \
        COMPONENT_DATA_TYPE_DECLARATION(IVec2, glm::ivec2) \
        COMPONENT_DATA_TYPE_DECLARATION(IVec3, glm::ivec3) \
        COMPONENT_DATA_TYPE_DECLARATION(IVec4, glm::ivec4) \
        COMPONENT_DATA_TYPE_DECLARATION(UVec1, glm::uvec1) \
        COMPONENT_DATA_TYPE_DECLARATION(UVec2, glm::uvec2) \
        COMPONENT_DATA_TYPE_DECLARATION(UVec3, glm::uvec3) \
        COMPONENT_DATA_TYPE_DECLARATION(UVec4, glm::uvec4) \
        COMPONENT_DATA_TYPE_DECLARATION(Mat3, glm::mat3) \
        COMPONENT_DATA_TYPE_DECLARATION(Mat4, glm::mat4) \
        COMPONENT_DATA_TYPE_DECLARATION(Color, Luddite::ColorRGBA) \
        COMPONENT_DATA_TYPE_DECLARATION(ColorRGB, Luddite::ColorRGB)


enum class EComponentDataTypes : uint32_t
{
    #define COMPONENT_DATA_TYPE_DECLARATION(Name, Type) Name,
        COMPONENT_DATA_TYPES_DECLARE
    #undef COMPONENT_DATA_TYPE_DECLARATION
        SIZE,
        // NULL
};

const char* ComponentDataTypeNames[] =
{
    #define COMPONENT_DATA_TYPE_DECLARATION(Name, Type) #Name,
        COMPONENT_DATA_TYPES_DECLARE
    #undef COMPONENT_DATA_TYPE_DECLARATION
};

const char* ComponentDataTypeCPPNames[] =
{
    #define COMPONENT_DATA_TYPE_DECLARATION(Name, Type) #Type,
        COMPONENT_DATA_TYPES_DECLARE
    #undef COMPONENT_DATA_TYPE_DECLARATION
};

const std::size_t ComponentDataTypeSizes[] =
{
    #define COMPONENT_DATA_TYPE_DECLARATION(Name, Type) sizeof(Type),
        COMPONENT_DATA_TYPES_DECLARE
    #undef COMPONENT_DATA_TYPE_DECLARATION
};

struct ComponentDataType
{
        EComponentDataTypes type;
        uint32_t offset;
        std::string name;
};

struct ComponentDescription
{
        std::string name;
        std::vector<ComponentDataType> members{};
        void AddMember(const EComponentDataTypes& type, const std::string& name)
        {
                auto size = ComponentDataTypeSizes[static_cast<uint32_t>(type)];
                //do 4 byte alignment
                if (current_offset / 4 != (current_offset + size - 1) / 4)
                        current_offset = ((current_offset / 4) * 4) + 4;
                members.push_back({type, current_offset, name});
                current_offset += size;
        };
        uint32_t current_offset = 0;
};

std::string format_typename(const std::string& name)
{
        std::string new_name;
        new_name.reserve(name.size());
        for (char c : name)
        {
                if (c == ' ')
                        c = '_';
                new_name.push_back(c);
        }
        new_name.shrink_to_fit();
};

void GenerateComponentHeader(const ComponentDescription& desc, std::ostream& stream)
{
        stream << "struct C_" << format_typename(desc.name) << " {" << std::endl;
        for (auto& member : desc.members)
                stream << ComponentDataTypeCPPNames[static_cast<uint32_t>(member.type)] << " " << format_typename(member.name) << ";" << std::endl;
        stream << "};";
}