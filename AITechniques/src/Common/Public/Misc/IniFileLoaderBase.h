#pragma once

#include <fstream>
#include <string>
#include <cassert>

const char* BAD_FILE_ERROR = "bad file";

class IniFileLoaderBase
{
private:

	// The file the parameters are stored in
	std::ifstream m_file;

	// The current line that is being readed
	std::string m_currentLine;

	// Default delimiters inside the file
	const std::string m_delims;

	// This is set to true if the file specified by the user is valid
	bool m_bGoodFile;

	// Removes any commenting from a line of text
	void RemoveCommentingFromLine(std::string& line);

	// This returns the next parameter as a string
	void GetParameterValueAsString(std::string& line);

	// This returs the next parameter inside our ini file
	std::string GetNextParameter();

	// This ignores any comments and finds the next delimited string
	std::string GetNextToken();

public:

	IniFileLoaderBase(char* filename)
		:m_currentLine(""),
		m_delims(" \;=,"),
		m_bGoodFile(true)
	{
		m_file.open(filename);

		if (!m_file)
		{
			m_bGoodFile = false;
		}
	}

	// Helper methods. They convert the next parameter value found into the relevant type
	double GetNextParameterDouble() { if (m_bGoodFile) return atof(GetNextParameter().c_str()); throw std::runtime_error(BAD_FILE_ERROR); }
	float GetNextParameterFloat() { if (m_bGoodFile) return (float)atof(GetNextParameter().c_str()); throw std::runtime_error(BAD_FILE_ERROR); }
	int GetNextParameterInt() { if (m_bGoodFile) return atoi(GetNextParameter().c_str()); throw std::runtime_error(BAD_FILE_ERROR); }
	bool GetNextParameterBool() { return (bool)(atoi(GetNextParameter().c_str())); throw std::runtime_error(BAD_FILE_ERROR); }

	double GetNextTokenAsDouble() { if (m_bGoodFile) return atof(GetNextToken().c_str()); throw std::runtime_error(BAD_FILE_ERROR); }
	float GetNextTokenAsFloat() { if (m_bGoodFile) return (float)atof(GetNextToken().c_str()); throw std::runtime_error(BAD_FILE_ERROR); }
	int GetNextTokenAsInt() { if (m_bGoodFile) return atoi(GetNextToken().c_str()); throw std::runtime_error(BAD_FILE_ERROR); }
	std::string GetNextTokenAsString() { if (m_bGoodFile) return GetNextToken(); throw std::runtime_error(BAD_FILE_ERROR); }

	bool eof() const { if (m_bGoodFile) return m_file.eof(); throw std::runtime_error(BAD_FILE_ERROR); }
	bool IsFileGood() const { return m_bGoodFile; }
};