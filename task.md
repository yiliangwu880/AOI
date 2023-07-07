 # 功能
	裁剪，优先级：比如好友，敌人。。
	
	Scene 动态格子：分类存放 player,monster,item

# 加入视野结构优化：
    把：
```cpp
	for ( const uint16_t &v : ninescreen)
	{
		for (Entity *otherEntity : m_idx2VecEntity[v])
		{
			L_ASSERT(otherEntity != &entity);
			otherEntity->AddObserver(entity);
		}
		entity.AddObserver(m_idx2VecEntity[v]);
	}
```

    优化为：
```cpp
	for ( const uint16_t &v : ninescreen)
	{
		for (Entity *otherEntity : m_idx2VecEntity[v])
		{
			L_ASSERT(otherEntity != &entity);
			OnSee(otherEntity, entity); // 具体实现 参考 2抽象宇宙飞船相撞。 用map<{obj1,obj2}, fun> 定位到具体函数
                                        // 或者考虑效率。调用各自Entity 的虚函数。
		}
	}
```