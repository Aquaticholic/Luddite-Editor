#pragma once
#include "Editor/pch.hpp"

namespace Editor
{
struct ICommand
{
        virtual void Execute() = 0;
        virtual void Unexecute() = 0;
};

class History
{
        public:
        History() = default;

        template <typename Command_T, typename ... Args>
        std::shared_ptr<Command_T> ExecuteCommand(Args && ... args)
        {
                std::shared_ptr<Command_T> command = std::make_shared<Command_T>(std::forward<Args>(args)...);
                ExecuteCommand(command);
                return command;
        }

        void ExecuteCommand(std::shared_ptr<ICommand> command)
        {
                command->Execute();
                m_UndoStack.push(command);
                m_RedoStack = std::stack<std::shared_ptr<ICommand> >();
        }

        template <typename Command_T, typename ... Args>
        void ExecuteCommandDeffered(Args && ... args)
        {
                ExecuteCommandDeffered(std::make_shared<Command_T>(std::forward<Args>(args)...));
        }

        void ExecuteCommandDeffered(std::shared_ptr<ICommand> command)
        {
                m_DefferedStack.push_back(command);
        }

        void ExecuteDefferedCommands()
        {
                for (auto& command : m_DefferedStack)
                        ExecuteCommand(command);
                m_DefferedStack.clear();
        }

        void Undo()
        {
                if (m_UndoStack.empty())
                        return;
                m_UndoStack.top()->Unexecute();
                m_RedoStack.push(m_UndoStack.top());
                m_UndoStack.pop();
        }

        void Redo()
        {
                if (m_RedoStack.empty())
                        return;
                m_RedoStack.top()->Execute();
                m_UndoStack.push(m_RedoStack.top());
                m_RedoStack.pop();
        }

        private:
        std::stack<std::shared_ptr<ICommand> > m_UndoStack;
        std::stack<std::shared_ptr<ICommand> > m_RedoStack;
        std::vector<std::shared_ptr<ICommand> > m_DefferedStack;
};
}