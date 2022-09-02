/*
author:yiliangwu880
you can get more refer from https://gitee.com/yiliangwu880/svr_util.git
brief:
	迷你日志。
	写库时需要打印日志，同时给使用库的用户提供改变日志实现的选择。
	日志宏在 log_def.h.  供开发库用户使用。(不放log_file,避免和用户项目日志宏冲突)
	使用库用户需要自己定义新的宏来定义新的实现。
	代码无依赖，直接复制就轻易使用到你的项目。



用户改写库日志实现例子：
void MyPrintf(LogLv lv, const char * file, int line, const char *fun, const char * pattern)
{
		...
}

int main(int argc, char* argv[])
{
	LogMgr::Ins().SetLogPrinter(&MyPrintf)
}

库用户使用：
	直接复制到你的库，改下命名空间， 参考log_def.h定义你的日志宏，就可以使用了。

*/
#pragma once
#include <string>
#include <sstream>
#include <map>
#include <vector>

namespace aoi
{
	enum LogLv
	{
		//优先级从高到底
		LL_FATAL,
		LL_ERROR,
		LL_WARN,
		LL_INFO,
		LL_DEBUG,
		LL_TRACE //追踪BUG用，频繁打印的时候用
	};
	
	using PrintfCB = void(*)(LogLv lv, const char *file, int line, const char *fun, const char * pattern);
	//缺省定义,打印到文件和标准输出
	class DefaultLog
	{
		int m_fd = -1;
		std::string m_file_name;
		std::string m_prefixName;
		std::string m_suffixName;
		int m_lastYear = 0;
		int m_lastYday = 0;
	public:
		//para:const char *fname, 文件路径名
		explicit DefaultLog(const char *fname = "svr_util_log.txt");
		~DefaultLog();
		void Printf(LogLv lv, const char *file, int line, const char *fun, const char *pattern);

	private:
		const char * GetLogLevelStr(LogLv lv) const;
		void OpenFile();
	};
	//日志管理单例
	class LogMgr
	{
		PrintfCB m_cb = &DefaultPrintf;//选用函数指针，不选用 function<void(LogLv lv...)>. 因为函数对象通常引用另一个对象，另一个对象什么时候会野是个多坑问题。
		bool m_isEnable = true;
		std::string m_defaultFileName = "svr_util_log.txt";
	public:
		static LogMgr &Ins();
		void SetLogPrinter(PrintfCB cb); //改变日志实现
		void DefaultFileName(std::string val) { m_defaultFileName = val; }
		void Printf(LogLv lv, const char * file, int line, const char *fun, const char * pattern, ...);
		void Printf(LogLv lv, const char * file, int line, const char *fun, const char * pattern, va_list vp);
		void PrintfCond(LogLv lv, const char * file, int line, const char *fun, const char * cond, const char * pattern = "", ...);
		void Enable(bool isEnable);//false == isEnable 表示不打日志

		template<class ... Args>
		void Write(LogLv lv, const char * file, int line, const char *fun, Args && ... args)
		{
			if (!m_isEnable)
			{
				return;
			}
			std::stringstream s;		
			std::initializer_list<int> { (s << std::forward<Args>(args) << " ", 0) ... }; //例如vector<int> v = { (3,1), (3,2) }; 和 {1,2} 返回结果一样; 里面的3只是执行，不返回给任何表达式使用

			std::string out_str = s.str();
			m_cb(lv, file, line, fun, out_str.c_str());
			if (out_str.length() >= 1000)
			{
				m_cb(lv, file, line, fun, "-----------[before str too long，was truncated,actual len=%d]-----------\n");
			}
		}
	private:
		LogMgr() {};
		static void DefaultPrintf(LogLv lv, const char *file, int line, const char *fun, const char * pattern);
	};
}

template<class T> 
std::stringstream& operator<<(std::stringstream& s, const std::vector<T> &vec)
{
	s << '[';
	bool isFirst = true;
	for (auto &v : vec)
	{
		if (!isFirst)
		{
			s << ", ";
		}
		s << v;
		isFirst = false;
	}
	s << ']';
	return s;
}

template<class K, class V>
std::stringstream& operator<<(std::stringstream& s, const std::map<K,V> &map)
{
	s << '{';
	bool isFirst = true;
	for (auto &v : map)
	{
		if (!isFirst)
		{
			s << ", ";
		}
		s << v.first << "=" << v.second;
		isFirst = false;
	}
	s << '}';
	return s;
}