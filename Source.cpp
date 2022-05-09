/*------------------------------------------
Тут по суті шаблонна реалізація класу SharedPtr
В чому фішка цієї міні програми?

Замість того щоб створювати 5 окремих спрайтів на астероїд 
та 5 окремих спрайтів на автара, я використовуючи свій
SharedPtr вивожу один і той самий спрайт 5 разів.
Тобто економія ресурсів!

------------------------------------------*/

#include<Windows.h>
#include<iostream>
#include<vector>
#include<exception>
#include <iomanip>  
#include "Framework.h"

#define ASTEROID "big_asteroid.png"
#define PLAYER "avatar.jpg"

//Простенький клас який містить в собі спрайт

class UIEntity
{
public:
	explicit UIEntity(LPCSTR sprite_name)
	{
		m_sprite = createSprite(sprite_name);
		if (m_sprite == nullptr)
		{
			std::cout << "Sprite wasnt inititalized\n";
			throw "Somthing went wrong!";
		}

	}

	~UIEntity()
	{
		destroySprite(m_sprite);
		m_sprite = nullptr;
	}

	bool operator == (const UIEntity& other)
	{
		return this->m_sprite == other.m_sprite;
	}

	Sprite* GetSprite() const
	{
		return m_sprite;
	}

	/*UIEntity* operator->()
	{
		return this;
	}*/

private:
	
	Sprite* m_sprite;

};


//Шаблонна реалізація класу shared pointer
template <typename Type>
class SharedPtr
{
public:

	explicit SharedPtr(Type* entityName):m_entityName(entityName), m_countShareHolders(new uint32_t(1))
	{

	}

	bool operator== (const SharedPtr& other) const
	{
		return m_entityName == other.m_entityName;
	}

	void Share(const SharedPtr& other)
	{
		this->m_entityName = other.m_entityName;
		this->m_countShareHolders = other.m_countShareHolders;
		++(*other.m_countShareHolders);
	}

	void Clear()
	{
		if (this->m_entityName != nullptr)
		{
			int count = --(*this->m_countShareHolders);
			if (count == 0)
			{
				delete this->m_entityName;
				this->m_entityName = nullptr;
			}
		}
	}

	SharedPtr(const SharedPtr& other)
	{
		Share(other);
	}

	SharedPtr& operator = (const SharedPtr& other)
	{
		if (*this == other)
		{
			return *this;
		}

		Clear();
		
		Share(other);

	}

	Type* operator->()
	{
		return m_entityName;
	}

	~SharedPtr()
	{
		Clear();
	}

private:
	
	Type* m_entityName;
	uint32_t* m_countShareHolders;

};

class MyFramework : public Framework {

public:

	virtual void PreInit(int& width, int& height, bool& fullscreen)
	{
		width = 620;
		height = 400;
		fullscreen = false;
	}

	virtual bool Init()//тільки в цій функції можна робитит підтягування спрайтів
	{
		SharedPtr<UIEntity> asteroid(new UIEntity(ASTEROID));

		SharedPtr<UIEntity> player(new UIEntity(PLAYER));

		for (int i = 0; i < 10; i++)//Ми виходить економим прост і ресурс використовуючи всього дві картинки 
		{//тут виходить всі картинки посуті копії однієї якраз завдяки shared ptr
			if (i % 2 == 0)
				m_items.push_back(asteroid);
			else
				m_items.push_back(player);
		}

		
		showCursor(false);

		

		
		return true;
	}

	virtual void Close() {

	}

	virtual bool Tick()//це функція яка відображає головний ігровий цикл 
	{
		for (int i = 0; i < m_items.size(); i++)
		{//трохі цирк фішка втому шо всі спрайти по суті це одна і та сама картинка через share ptr; 
			drawSprite(m_items[i]->GetSprite(), 5*i * m_items.size(), 5* i * m_items.size());
		}
		
		return false;
	}

	virtual void onMouseMove(int x, int y, int xrelative, int yrelative)//намалювати ціль і саме по цбому буде вилітати пуля
	{//х і у це координати мишки в вікні, хрел і урел це координати відносно щойно зрушеної точки
		//std::cout << x << " " << y <<std::endl;
		
	}

	virtual void onMouseButtonClick(FRMouseButton button, bool isReleased)
	{

	}

	static int power;;

	virtual void onKeyPressed(FRKey k)
	{
		

	}

	virtual void onKeyReleased(FRKey k)
	{
		

	}

	virtual const char* GetTitle() override
	{
		return "Shared Pointer example";
	}

private:

	std::vector<SharedPtr<UIEntity>> m_items;

};


int main()
{
	run(new MyFramework);
	return EXIT_SUCCESS;
}