#ifndef __BIND_BASE__H__
#define __BIND_BASE__H__

#include <vector>
#include <memory>
#include <string>
#include <typeinfo>

// 事前にクラスを登録することで名前からインスタンスを生成できるクラス

namespace FrameWork
{
#define REGISTER_COMPONENT(list, T) (list.RegisterComponent<T>(#T))

	template<class Base>
	class BindBase
	{
	public:
		std::string name;
		virtual Base * instantiate() { return new Base(); }
		virtual const type_info & GetType() { return typeid(Base); }
	};

	template<class Derived, class Base>
	class BindDerived : public BindBase<Base>
	{
	public:
		Derived * instantiate() override { return new Derived(); }
		const type_info & GetType() override { return typeid(Derived); }
	};

	template<class Base>
	class BindClass
	{
	public:
		std::vector<std::shared_ptr<BindBase<Base>>> list;

		template<class Derived>
		void RegisterComponent(std::string name)
		{
			list.emplace_back(std::make_shared<BindDerived<Derived, Base>>());
			list.back()->name = name;
		}

		template<class Derived>
		std::string GetBindClassName(const Derived & c)
		{
			for (auto & cl : list)
			{
				if (cl->GetType() == typeid(c))
					return cl->name;
			}
			return "";
		}
	};
}

#endif // !__BIND_BASE__H__

