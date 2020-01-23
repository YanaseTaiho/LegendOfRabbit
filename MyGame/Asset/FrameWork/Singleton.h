#ifndef _SINGLETON__H__
#define _SINGLETON__H__

namespace FrameWork
{
	template<class T>
	class Singleton final
	{
	private:
		static T * instance;

	public:
		static void Create();
		static void Release();
		static T * Instance();
	};

	template<class T>
	inline void Singleton<T>::Create()
	{
		if (!instance) instance = new T();
	}
	template<class T>
	inline void Singleton<T>::Release()
	{
		if (instance)
		{
			delete instance;
			instance = nullptr;
		}
	}
	template<class T>
	inline T * Singleton<T>::Instance()
	{
		return instance;
	}

	template<class T> T * Singleton<T>::instance = nullptr;
}

#endif // !_SINGLETON__H__

