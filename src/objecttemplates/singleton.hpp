#pragma once
#include <defines.hpp>
#include <utils/asserts/assert.hpp>

BEGIN_NAMESPACE

	template<typename T>
	class Singleton
	{
	public:
		inline static T& GetInstance() 
		{ 
			static T instance;
			return instance;
		}

		virtual void SingletonInit() {}
		virtual void SingletonShutdown() {}
	protected:
		Singleton() = default;
		Singleton(const Singleton&) = delete;
		Singleton(Singleton&&) = delete;
		Singleton& operator = (const Singleton&) = delete;
		Singleton& operator = (Singleton&&) = delete;
		~Singleton() = default;
	};

END_NAMESPACE

#define SM_INVOKE_SINGLETON_INIT(T) T::GetInstance().SingletonInit();
#define SM_INVOKE_SINGLETON_SHUTDOWN(T) T::GetInstance().SingletonShutdown();

#define SINGLETON(T) final : public Singleton<T>
#define SINGLETON_CLASS(C) class C SINGLETON(C)
#define SINGLETON_STRUCT(S) struct S SINGLETON(S)
#define SINGLETON_FRIEND(T) friend class Singleton<T>

// Must exists in private section of singleton class
#define SINGLETON_CONSTRUCTOR(T) SINGLETON_FRIEND(T); T(){}