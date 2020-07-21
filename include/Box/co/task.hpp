#ifndef _BOX_CO_TASK_HPP_
#define _BOX_CO_TASK_HPP_
#include <cstdlib>
#include <exception>
#include <functional>
#include "context.hpp"
namespace Box{
    namespace CoImpl{
        //当前任务
        extern thread_local void *current_task;
    };
    namespace Co{
        class TaskError:public std::exception{
            public:
                TaskError(const char *msg);
                const char *what() const noexcept;
            private:

        };
        template<class T>
        class Task{
            public:
                using result_type = typename std::function<T>::result_type;
            private:
                ContextEntry context_entry = [](void *task) -> void{
                    //调用入口
                    Task<T> *self = static_cast<Task<T>*>(task);
                    self->ret = self->entry();
                };
            public:
                template<class Fn,class ...Args>
                Task(size_t stack_size,Fn &&fn,Args &&...args)
                    :entry(std::bind(fn,args...)){
                    //设置一下上下文
                    task_context = ThisContext();

                    stack_ptr = AllocateStack(stack_size);
                    task_context.uc_link = &main_context;
                    task_context.set_stack(stack_ptr,stack_size);
                    task_context.set_entry(context_entry,this);
                };
                //默认没堆栈的
                template<class Fn,class ...Args>
                Task(Fn &&fn,Args &&...args):
                    Task(static_cast<size_t>(1024 * 20),fn,args...){};
                
                Task(const Task<T> &) = delete;
                ~Task(){
                    free(stack_ptr);
                };
                result_type resume(){
                    //切换上下文
                    main_context = ThisContext();
                    CoImpl::current_task = this;
                    
                    libc::swapcontext(&main_context,&task_context);
                    
                    CoImpl::current_task = nullptr;
                    return ret;
                };
                //返回数值
                template<class RetT>
                void yield(RetT &&val){
                    ret = val;
                    //切换回主要的上下文
                    libc::swapcontext(&task_context,&main_context);
                };
                result_type operator()(){
                    return resume();
                };
            private:
                std::function<result_type()> entry;
                result_type ret;//返回数值
                Context task_context;
                Context main_context;
                //堆栈数据
                void *stack_ptr;
        };
        [[noreturn]] void throwTaskError(const char *msg);
        //当前任务
        template<class T>
        Task<T> &ThisTask(){
            if(CoImpl::current_task == nullptr){
                //throwTaskError("No task");
            }
            return *static_cast<Task<T>*>(CoImpl::current_task);
        };
        
    };
};

#endif