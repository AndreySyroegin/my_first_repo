/* Коммерческая тайна общества с ограниченной ответственностью «МФИ Софт» (ООО «МФИ Софт»), место нахождения - Российская Федерация, 603126, г. Нижний Новгород, ул. Родионова, дом 192, корпус 1;
(с) ООО «МФИ Софт», 2010-2014.
* ПРИМЕЧАНИЕ: Информация, интеллектуальные и технические концепции, содержащиеся здесь, принадлежат ООО «МФИ Софт», защищены действующим законодательством РФ. 
* Их распространение без получения предварительного письменного согласия со стороны ООО «МФИ Софт» запрещено. */

#ifndef __STRINGUTILS_H_
#define __STRINGUTILS_H_

#include <string>
#include <vector>
#include <algorithm>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>

#include "Debug.h"
#include "TranslateRegEx.h"

#include "unicode/utypes.h"   /* Basic ICU data types */
#include "unicode/ucnv.h"     /* C   Converter API    */
#include "unicode/ustring.h"  /* some more string fcns*/
#include "unicode/uchar.h"    /* char names           */
#include "unicode/uloc.h"
#include "unicode/unistr.h"

#define	DEBUG_STRINGUTILS DEBUG


const char DIGIT[]			= "0123456789";

class StringUtils
{
public:

	static std::string & trim(std::string & str, const char* charSet = " \t")
	{
		return ltrim(rtrim(str,charSet),charSet);
	}
	static std::string & ltrim(std::string & str, const char* charSet = " \t")
	{
		std::string::size_type found = str.find_first_not_of(charSet);

		return (found != std::string::npos) ? (str.erase(0, found), str) : (str.clear(), str);
	}



	static std::string & rtrim(std::string & str, const char* charSet  = " \t")
	{
		std::string::size_type found = str.find_last_not_of(charSet);

		return (found != std::string::npos) ? (str.erase(found + 1), str) : (str.clear(), str);
	}

	static std::string GetParameter(const std::string& input, const char delimiter = ':')
	{
		std::vector<std::string> results; 
		split( input, results, delimiter, false );
		if( results.size() > 0 )
		{
			return results[ 0 ];
		}
		return "";
	}

	static std::string GetValue(const std::string& input, const char delimiter = ':')
	{
		if (input.find(delimiter) != std::string::npos)
		{
			static std::string sValue;
			sValue = input.substr(input.find(delimiter) + 1);
			return StringUtils::trim(sValue, " \t\r");
		}
		return "";
	}
	static bool GetParameter(std::string & retval, const std::string & input, const char delimiter = ':')
	{
		std::vector<std::string> results; 
		split(input, results, delimiter, false);

		return (results.empty()) ? (false) : (retval.assign(results[0]), true);
	}

	static bool GetValue(std::string & retval, const std::string& input, const char delimiter = ':')
	{
		retval = GetValue(input, delimiter);
		return !retval.empty();
	}

	template <typename E, typename C>
	static size_t split(std::basic_string<E> const& s,
             C &container,
             E const delimiter,
             bool keepBlankFields = true)
	{
		size_t n = 0;
		typename std::basic_string<E>::const_iterator it = s.begin(), end = s.end(), first;
		for (first = it; it != end; ++it)
		{
			// Examine each character and if it matches the delimiter
			if (delimiter == *it)
			{
				if (keepBlankFields || first != it)
				{
					std::basic_string<E> copy = std::basic_string<E>(first, it);
					container.push_back(trim(copy, " \t\r"));
					++n;
	                
					// skip the delimiter
					first = it + 1;
				}
				else
				{
					++first;
				}
			}
		}
		if (keepBlankFields || first != it)
		{
			std::basic_string<E> copy = std::basic_string<E>(first, it);
			container.push_back(trim(copy, " \t\r"));
			++n;
		}
		return n;
	}

	static std::string & LowerCase (std::string & sStr) 
	{ 
		transform(sStr.begin(), sStr.end(), sStr.begin(), static_cast<int(*)(int)>(tolower));
		return sStr;
	}

	static std::string & UpperCase (std::string & sStr) 
	{ 
		transform(sStr.begin(), sStr.end(), sStr.begin(), static_cast<int(*)(int)>(toupper));
		return sStr;
	}

	static bool StrCompareNoCase(const std::string & sStr1, const std::string & sStr2)
	{
		std::string sTempStr1(sStr1);
		std::string sTempStr2(sStr2);
		return (LowerCase(sTempStr1) == LowerCase(sTempStr2));
	}
	static unsigned int GetTimeToSecond(const std::string & sTime, const std::string & sFormat = "%d/%m/%Y %H:%M:%S")
	{
		struct tm tm;
		strptime(sTime.c_str(), sFormat.c_str(), &tm);
		return static_cast<unsigned int>(mktime(&tm));
	}

	template <typename T>
	static const std::string & ToString(std::string & retval, const T & n)
	{
		std::ostringstream stream;
		stream << n;
		return retval.assign(stream.str());
	}
	template <typename T>
	static std::string ToString(const T & n)
	{
		std::string value;
		ToString(value,n);
		return value;
	}
	static const std::string & ToString(std::string & retval, bool n)
	{
		std::ostringstream stream;
		stream << std::boolalpha << n;
		return retval.assign(stream.str());
	}
	template <class T>
	static bool FromStringTo(T & retval, const std::string& s, std::ios_base& (*f)(std::ios_base&) = std::dec)
	{
		std::istringstream iss(s);
		return !(iss >> f >> retval).fail() && iss.eof();
	}
	static bool FromStringTo(bool & retval, const std::string& s)
	{
		std::istringstream iss(s);
		return !(iss >> std::boolalpha >> retval).fail() && iss.eof();
	}
	static bool FromStringTo(unsigned char & retval, const std::string & s)
	{
		typedef unsigned char byte_t;
		uint32_t iUint32Value = uint32_t();
		if(FromStringTo<uint32_t>(iUint32Value, s) && iUint32Value <= std::numeric_limits<byte_t>::max()) 
		{
			retval = static_cast<byte_t>(iUint32Value);
			return true;
		}
		return false;
	}
	static bool FromStringTo(char & retval, const std::string & s)
	{
		typedef char byte_t;
		int32_t iInt32Value = int32_t();
		if(FromStringTo<int32_t>(iInt32Value, s) && iInt32Value <= std::numeric_limits<byte_t>::max()) 
		{
			retval = static_cast<byte_t>(iInt32Value);
			return true;
		}
		return false;
	}
	static std::string& ReplaceString( std::string& s, const char& OldVal, const char& NewVal)
	{
		std::replace( s.begin(), s.end(), OldVal, NewVal );
		return s;
	}

	static std::string& ReplaceString( std::string& s, const std::string& sOldStr, const std::string& sNewStr)
	{
		std::string::size_type iPos(0), iOldStrLen(sOldStr.size());

		while ((iPos = s.find(sOldStr, iPos)) != std::string::npos && !sOldStr.empty())
		{
			s.replace(iPos, iOldStrLen, sNewStr);
		}
		return s;
	}

	static bool ParseNumberInString(std::string & retval, const std::string & s, std::string::size_type & rStartNextSearch)
	{
		std::string::size_type start_pos = s.find_first_of(DIGIT, rStartNextSearch);
		std::string::size_type end_pos = s.find_first_not_of(DIGIT, start_pos);

		if( start_pos == std::string::npos )
	    {
			return false;
	    }
	    else if( end_pos == std::string::npos)
	    {
			rStartNextSearch = std::string::npos;
			return retval.assign(s.substr(start_pos)), true;
	    }
		rStartNextSearch = end_pos;
		return retval.assign(s.substr(start_pos, end_pos-start_pos)), true;
	}

	static bool ParseNumberInString(std::string & retval, const std::string & s)
	{
		std::string::size_type searchPos(0);
		return ParseNumberInString(retval, s, searchPos);
	}

	static bool IsContainingDigit( const std::string& s )
	{
		return s.find_first_of(DIGIT)!=std::string::npos;
	}

	template <typename CharT>
	static bool IsContainingOnlyDigits(const std::basic_string<CharT> &input)
	{
		return std::count_if(input.begin(), input.end(), isdigit) == input.size();
	}
	static bool EndsWith(const std::string& a, const std::string& b) 
	{
		if (b.size() > a.size()) return false;
		return std::equal(a.begin() + a.size() - b.size(), a.end(), b.begin());
	}
	static bool StartsWith(const std::string& a, const std::string& b) 
	{
		return b.size() > a.size() ? false : a.compare(0, b.length(), b) == 0;
	}

	static const std::string & ConvertIpAddr(std::string & retval, unsigned int ip)
	{
		std::ostringstream stream;
		stream  << (( ip >> 24 ) & 0xff) << "." 
			<< (( ip >> 16 ) & 0xff) << "."  
			<< (( ip >> 8 ) & 0xff)  << "."  
			<< (( ip) & 0xff);

		return retval.assign(stream.str());
	}

	static uint32_t ConvertIpAddr(const std::string & sIpAddrStr)
	{
		in_addr_t in = inet_addr( sIpAddrStr.c_str() );
		if( INADDR_NONE == in )
		{
			DEBUG_STRINGUTILS << "ConvertIpAddr(): Cannot convert ip address string = " << sIpAddrStr;
			return std::numeric_limits<uint32_t>::max();
		}

		return ntohl(in);
	}
	static bool Transcode(const std::string & SrcCodepage, const std::string & DstCodepage,
		const std::string & SrcData, std::string & ResultData)
	{
		UErrorCode status = U_ZERO_ERROR;
		std::vector<char> vTarget(SrcData.size());
		const size_t iTargetSize = ucnv_convert	(DstCodepage.c_str(), SrcCodepage.c_str(), 
			&vTarget[0], vTarget.size(), SrcData.c_str(), SrcData.length(), &status);

		if(U_BUFFER_OVERFLOW_ERROR == status) {
			status = U_ZERO_ERROR;
			vTarget.resize(iTargetSize);
			ucnv_convert(DstCodepage.c_str(), SrcCodepage.c_str(), 
				&vTarget[0], vTarget.size(), SrcData.c_str(), SrcData.length(), &status);
		}
		if(U_SUCCESS(status)) {
			ResultData.assign(vTarget.begin(), vTarget.begin() + iTargetSize);
			return true;
		}
		return false;
	}
	static bool Translate(std::string & phoneNumber, const std::string & rule)
	{
		enum { eExpIndex, eReplaceIndex, eRangeIndex };

		std::vector<std::string> tokens;
		split(rule, tokens, '/', true);
		if (tokens.size() > 1)
		{
			TranslateRegEx RegEx(tokens[eExpIndex], tokens[eReplaceIndex], (tokens.size() > 2 ? tokens[eRangeIndex] : std::string()));
			return RegEx.DoTranslation(phoneNumber, phoneNumber);
		}
		return false;
	}
};

#endif
