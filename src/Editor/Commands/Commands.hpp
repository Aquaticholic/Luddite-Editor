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

struct MultiCommand : public ICommand
{
        std::vector<std::shared_ptr<ICommand> > m_Commands;

        template<typename Command_T, typename ... Args>
        std::shared_ptr<Command_T> AddCommand(Args && ... args)
        {
                std::shared_ptr<Command_T> command = std::make_shared<Command_T>(std::forward<Args>(args)...);
                AddCommand(command);
                return command;
        }

        void AddCommand(std::shared_ptr<ICommand> command)
        {
                m_Commands.push_back(command);
        }

        void Execute()
        {
                for (auto it = m_Commands.begin(); it != m_Commands.end(); it++)
                        (*it)->Execute();
        }

        void Unexecute()
        {
                for (auto it = m_Commands.rbegin(); it != m_Commands.rend(); it++)
                        (*it)->Unexecute();
        }
};

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
                : entity{entity_}, src(src_), dst(dst_) {}

        void Execute()
        {
                entity.ReplaceComponent<Component>(dst);
        }

        void Unexecute()
        {
                entity.ReplaceComponent<Component>(src);
        }
};

template <typename Component>
struct AddComponentCommand : public ICommand
{
        Component comp;
        Luddite::Entity entity;
        AddComponentCommand(const Luddite::Entity& entity_, const Component& comp_ = {})
                : entity{entity_}, comp(comp_)
        {
        }

        void Execute()
        {
                entity.AddComponent<Component>(comp);
        }

        void Unexecute()
        {
                entity.RemoveComponent<Component>();
        }
};

struct CreateEmptyEntityCommand : public ICommand
{
        Luddite::EntityID id;
        Luddite::World* pWorld;
        CreateEmptyEntityCommand(Luddite::World& world)
        {
                pWorld = &world;
        }
        void Execute()
        {
                auto e = pWorld->CreateEntity();
                e.AddComponent<C_Name>("Unnamed");
                e.AddComponent<C_Transform3D>();
                id = e.GetID();
        }
        void Unexecute()
        {
                pWorld->DestroyEntityFromID(id);
        }
};
}