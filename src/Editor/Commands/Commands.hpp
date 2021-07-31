#pragma once
#include "Editor/pch.hpp"
#include "Editor/History.hpp"

namespace Editor
{
// struct TransformCommand : public ICommand
// {
//         C_Transform3D src;
//         C_Transform3D dst;
//         Luddite::Entity entity;

//         TransformCommand(const Luddite::Entity& entity_, const C_Transform3D& src_, const C_Transform3D& dst_)
//                 : entity{entity_}, dst{dst_}, src{src_} {}

//         TransformCommand(const Luddite::Entity& entity_, const C_Transform3D& dst_)
//                 : entity{entity_}, dst{dst_}
//         {
//                 src = entity.GetComponent<C_Transform3D>();
//         }

//         void Execute()
//         {
//                 entity.ReplaceComponent<C_Transform3D>(dst);
//         }

//         void Unexecute()
//         {
//                 entity.ReplaceComponent<C_Transform3D>(src);
//         }
// };

struct RenameCommand : public ICommand
{
        std::string src;
        std::string dst;
        Luddite::Entity entity;
        RenameCommand(const Luddite::Entity& entity_, const std::string& src_, const std::string& dst_)
                : entity{entity_}, src(src_), dst(dst_) {}

        void Execute()
        {
                entity.ReplaceComponent<C_Name>(dst);
        }

        void Unexecute()
        {
                entity.ReplaceComponent<C_Name>(src);
        }
};

template <typename Component>
struct ReplaceComponentCommand : public ICommand
{
        Component src;
        Component dst;
        Luddite::Entity entity;
        ReplaceComponentCommand(const Luddite::Entity& entity_, const Component& src_, const Component& dst_)
                : entity{entity_}, src{src_}, dst{dst_} {}

        void Execute()
        {
                entity.ReplaceComponent<Component>(dst);
        }

        void Unexecute()
        {
                entity.ReplaceComponent<Component>(src);
        }
};
}