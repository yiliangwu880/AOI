
改为引用svr_util库


利用stl 的 weak_ptr shared_ptr 实现互相引用例子。:
{
    使用例子。场景互相能看到的 entity就合适。要么互相看不到，要么看得到。
    connect = sharepoint(A,B) --  连接 A,B
    A.weakpoint(connect)
    B.weakpoint(connect)
}

game_task.h 里面的任务类型2任务，不用map,改成数组提升效率

游戏任务： 
{
通用
post event 
send event
}

效率随机部分数组元素。
{
	问题：长数组种，随机少量元素，需要效率。
	方法：迭代少量次数，每次从数组前端随机索引。 被随机到的元素，和数组后端元素交换。后端部分做已随机过用。
	待思考，这个方法好像没伟易达那个随机好用
}