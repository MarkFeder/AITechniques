#include "Common/Public/Misc/IniFileLoaderBase.h"

const char* BAD_FILE_ERROR = "bad file";

//----------------------------- RemoveCommentingFromLine ---------------------------
// Removes any commenting from a line of text
// ---------------------------------------------------------------------------------

void IniFileLoaderBase::RemoveCommentingFromLine(std::string& line)
{
	if (line.empty())
	{
		return;
	}

	// Search for any comment and remove
	std::string::size_type idx = line.find('//');
	if (idx != std::string::npos)
	{
		// cut out the comment
		line = line.substr(0, idx);
	}
}

//----------------------------- GetNextParameter ---------------------------
// Searches the text file for the next valid parameter. Discards any 
// comments and returns the value as a string
//--------------------------------------------------------------------------

std::string IniFileLoaderBase::GetNextParameter()
{
	// This will be the string that holds the next parameter
	std::string line;

	std::getline(m_file, line);

	RemoveCommentingFromLine(line);

	// If the line is of zero length, get the next line from the file
	if (line.length() == 0)
	{
		return GetNextParameter();
	}

	GetParameterValueAsString(line);

	return line;
}

//----------------------------- GetParameterValueAsString  ---------------------------
// Given a line of text this function removes the parameter description and returns
// just the parameter as a std::string
//------------------------------------------------------------------------------------

void IniFileLoaderBase::GetParameterValueAsString(std::string& line)
{
	// Find beginning of parameter description
	std::string::size_type begIdx;
	std::string::size_type endIdx;

	begIdx = line.find_first_not_of(m_delims);

	// Find the end of the parameter description
	if (begIdx != std::string::npos)
	{
		endIdx = line.find_first_of(m_delims, begIdx);

		// End of word is the end of the line
		if (endIdx == std::string::npos)
		{
			endIdx = line.length();
		}
	}

	// Find the beginning of the parameter value
	begIdx = line.find_first_not_of(m_delims, endIdx);
	// Find the end of the parameter value
	if (begIdx != std::string::npos)
	{
		endIdx = line.find_first_of(m_delims, begIdx);

		// End of the word is the end of the line
		if (endIdx == std::string::npos)
		{
			endIdx = line.length();
		}
	}

	line = line.substr(begIdx, endIdx);
}

//----------------------------- GetNextToken  ---------------------------
// Ignores any commening lines and gets the next string
//-----------------------------------------------------------------------

std::string IniFileLoaderBase::GetNextToken()
{
	// Strip the line of any commenting
	while (m_currentLine.length() == 0)
	{
		std::getline(m_file, m_currentLine);

		RemoveCommentingFromLine(m_currentLine);
	}

	// Find beginning of parameter description
	std::string::size_type begIdx;
	std::string::size_type endIdx;

	begIdx = m_currentLine.find_first_not_of(m_delims);
	// Find the end of the parameter description
	if (begIdx != std::string::npos)
	{
		endIdx = m_currentLine.find_first_of(m_delims, begIdx);

		// End of the word is the end of the line
		if (endIdx == std::string::npos)
		{
			endIdx = m_currentLine.length();
		}
	}

	std::string s(m_currentLine.substr(begIdx, endIdx));
	if (endIdx != m_currentLine.length())
	{
		// Strip the token from the line
		m_currentLine = m_currentLine.substr(endIdx + 1, m_currentLine.length());
	}
	else
	{
		m_currentLine = "";
	}

	return s;
}