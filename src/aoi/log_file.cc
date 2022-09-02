
#include "log_file.h"
#include <sstream>
#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;
namespace aoi
{


	LogMgr & LogMgr::Ins()
	{
		static LogMgr d;
		return d;
	}

	void LogMgr::SetLogPrinter(PrintfCB cb)
	{
		m_cb = cb;
		if (nullptr == m_cb)
		{
			m_cb = &DefaultPrintf;
		}
	}

	void LogMgr::Printf(LogLv lv, const char * file, int line, const char *fun, const char * pattern, ...)
	{
		if (nullptr == pattern)
		{
			return;
		}
		if (!m_isEnable)
		{
			return;
		}
		va_list vp;
		va_start(vp, pattern);

		char out_str[1000];
		//用一次vfprintf，vp会被 vfprintf修改
		//最多写sizeof(out_str)-1个，自动末尾字符填'\0'
		//正确返回长度， 长度>=sizeof(out_str) 表示截断
		int r = vsnprintf(out_str, sizeof(out_str), pattern, vp);
		m_cb(lv, file, line, fun, out_str);
		if (r >= (int)sizeof(out_str))
		{
			snprintf(out_str, sizeof(out_str), "-----------[before str too long，was truncated,actual len=%d]-----------\n", r);
			m_cb(lv, file, line, fun, out_str);
		}


		va_end(vp);
	}

	void LogMgr::Printf(LogLv lv, const char * file, int line, const char *fun, const char * pattern, va_list vp)
	{
		if (nullptr == pattern)
		{
			return;
		}
		if (!m_isEnable)
		{
			return;
		}

		char out_str[1000];
		//用一次vfprintf，vp会被 vfprintf修改
		//最多写sizeof(out_str)-1个，自动末尾字符填'\0'
		//正确返回长度， 长度>=sizeof(out_str) 表示截断
		int r = vsnprintf(out_str, sizeof(out_str), pattern, vp);
		m_cb(lv, file, line, fun, out_str);
		if (r >= (int)sizeof(out_str))
		{
			snprintf(out_str, sizeof(out_str), "-----------[before str too long，was truncated,actual len=%d]-----------\n", r);
			m_cb(lv, file, line, fun, out_str);
		}
	}


	void LogMgr::PrintfCond(LogLv lv, const char * file, int line, const char *fun, const char * cond, const char * pattern/*=""*/, ...)
	{
		std::string fmt = cond;
		fmt.append(pattern);
		va_list vp;
		va_start(vp, pattern);
		Printf(lv, file, line, fun, fmt.c_str(), vp);
		va_end(vp);
	}



	void LogMgr::Enable(bool isEnable)
	{
		m_isEnable = isEnable;
	}

	void LogMgr::DefaultPrintf(LogLv lv, const char *file, int line, const char *fun, const char * pattern)
	{
		// 使用默认才创建s_log对象
		static DefaultLog s_log(LogMgr::Ins().m_defaultFileName.c_str());
		s_log.Printf(lv, file, line, fun, pattern);
	}




	DefaultLog::DefaultLog(const char *fname)
	{
		m_file_name = fname;
		auto it = m_file_name.find('.');
		if (it == std::string::npos)
		{
			m_prefixName = m_file_name;
			m_suffixName = ".txt";
		}
		else
		{
			m_prefixName.assign(m_file_name.begin(), m_file_name.begin()+it);
			m_suffixName.assign(m_file_name.begin()+it, m_file_name.end());
		}
	}





	DefaultLog::~DefaultLog()
	{
		if (-1 != m_fd)
		{
			close(m_fd);
		}
	}

	void DefaultLog::Printf(LogLv lv, const char * file, int line, const char *fun, const char * pattern)
	{
		char line_str[10];
		snprintf(line_str, sizeof(line_str), "%d", line);

		//add time infomation
		char time_str[1000];
		
		time_t long_time;
		time(&long_time);
		tm* pTm;
		pTm = localtime(&long_time);
		snprintf(time_str, sizeof(time_str), "[%04d-%02d-%02d %02d:%02d:%02d] ", pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday, pTm->tm_hour, pTm->tm_min, pTm->tm_sec);


		string s;
		s.append(time_str);
		s.append(" ");
		s.append(GetLogLevelStr(lv));
		s.append(pattern);
		//if (lv <= m_log_lv)
		{
			s.append("  --");
			s.append(file);
			s.append(":");
			s.append(line_str);
			s.append(" ");
			s.append(fun);
		}
		s.append("\n");


		if (m_lastYday != pTm->tm_year || m_lastYday != pTm->tm_yday)
		{//write new file
			m_lastYear = pTm->tm_year;
			m_lastYday = pTm->tm_yday;
			char log_file_name_tril[64];
			snprintf(log_file_name_tril, sizeof(log_file_name_tril), "_%d_%2.2d_%2.2d",
				pTm->tm_year +1900, pTm->tm_mon +1, pTm->tm_mday);
			m_file_name = m_prefixName;
			m_file_name += log_file_name_tril;
			m_file_name += m_suffixName;
			if (-1 == m_fd)
			{
				OpenFile();
			}
		}
		bool is_exit = (access(m_file_name.c_str(), F_OK | W_OK) == 0);
		if (!is_exit)
		{
			close(m_fd);
			m_fd = -1;
			OpenFile();
		}


		::write(m_fd, s.c_str(), s.length());
		::puts(s.c_str());
	}

	const char * DefaultLog::GetLogLevelStr(LogLv lv) const
	{
		switch (lv)
		{
		default:
			return "[unknow]";
			break;
		case LL_FATAL:
			return "[fatal] ";
			break;
		case LL_ERROR:
			return "[error] ";
			break;
		case LL_WARN:
			return "[warn]  ";
			break;
		case LL_DEBUG:
			return "[debug] ";
			break;
		case LL_INFO:
			return "[info]  ";
			break;
		case LL_TRACE:
			return "[trace]   ";
			break;
		}
		return "[unknow]";
	}



	void DefaultLog::OpenFile()
	{
		// 打开文件，不存在则创建
		m_fd = open(m_file_name.c_str(), O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if (m_fd < 0)
		{
			::printf("fail open file[%s]\n", m_file_name.c_str());
		}

	}

}