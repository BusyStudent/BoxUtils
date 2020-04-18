#ifndef _BOX_REFPTR_HPP_
#define _BOX_REFPTR_HPP_
//引用计数器指针
#include <cassert>
namespace Box{
	template <class T>
	class RefPtr{
		public:
			RefPtr(T *ptr){
				//通过指针赋值
				assert(ptr != nullptr);
				this->ptr = ptr;//给个值
				refcount = new int(1);//初始化int
			}
			RefPtr(const RefPtr <T> &refptr){
				//复制指针
				refcount = refptr.refcount;
				ptr = refptr.ptr;
				//增加计数器
				(*refcount) ++;
			}
			~RefPtr(){
				//被销毁的时候
				if(refcount != nullptr){
					//如果不为nullptr
					(*refcount) --;//指针减去11
					if(*refcount <= 0){
						delete ptr;//需要释放数据
						delete refcount;//释放引用计数器
					}
				}
			}
			inline T *get(){
				//得到指针
				return ptr;
			}
			inline T *operator ->(){
				//操作指针
				return ptr;
			}
			inline T &operator *(){
				//得到引用
				return *ptr;
			}
			//重载等号
			RefPtr <T> &operator =(const RefPtr <T> &refptr){
				if(this != &refptr){
					//避免自我赋值
					refcount = refptr.refcount;//拷贝计数器指针
					ptr = refptr.ptr;//拷贝数据指针
					(*refcount) ++;//拷贝计数器
				}
				return *this;
			}
			//重置指针(可能会导致悬空指针)
			void reset(){
				(*refcount) --;//引用计数器减去1
				if(*refcount <= 0){
					//需要释放老指针
					delete ptr;
					delete refcount;
				}
				//指针变空
				ptr = nullptr;
				refcount = nullptr;
			}
			//用一个新的指针替代
			void reset(T *ptr){
				(*refcount) --;//引用计数器减去1
				if(*refcount <= 0){
					//需要释放老指针
					delete ptr;
					delete refcount;
				}
				//新的指针
				assert(ptr != nullptr);
				this->ptr = ptr;
				this->refcount = new int(1);
			}
			//判断是否相等
			inline bool operator ==(const RefPtr <T> &refptr){
				return ptr == refptr.ptr;
			} 
			//判断不相等
			inline bool operator !=(const RefPtr <T> &refptr){
				return ptr != refptr.ptr;
			}
			inline int get_count(){
				//得到计数
				return *refcount;
			}
			inline bool unique(){
				//独一无二 计数器是否为1
				if(refcount != nullptr){
					return (*refcount) == 1;
				}
				return false;
			}
			inline bool null(){
				//是否为空指针
				return ptr == nullptr;
			}
		private:
			int *refcount;
			T *ptr;//数据指针
	};
};
#endif
