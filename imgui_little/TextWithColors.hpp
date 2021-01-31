#include "imgui.h"
#include "stdio.h"

const char ColorMarkerStart = '{';
const char ColorMarkerEnd = '}';

static bool ProcessInlineHexColor( const char* start, const char* end, ImVec4& color )
{
	const int hexCount = ( int )( end - start );
	if( hexCount == 6 || hexCount == 8 )
	{
		char hex[9];
		strncpy( hex, start, hexCount );
		hex[hexCount] = 0;

		unsigned int hexColor = 0;
		if( sscanf( hex, "%x", &hexColor ) > 0 )
		{
			color.x = static_cast< float >( ( hexColor & 0x00FF0000 ) >> 16 ) / 255.0f;
			color.y = static_cast< float >( ( hexColor & 0x0000FF00 ) >> 8  ) / 255.0f;
			color.z = static_cast< float >( ( hexColor & 0x000000FF )       ) / 255.0f;
			color.w = 1.0f;

			if( hexCount == 8 )
			{
				color.w = static_cast< float >( ( hexColor & 0xFF000000 ) >> 24 ) / 255.0f;
			}

			return true;
		}
	}

	return false;
}

template<typename... Args>
void TextWithColors(const char* fmt, Args... args)
{
	char tempStr[4096];

	snprintf(tempStr, sizeof(tempStr), fmt, args...);
	tempStr[sizeof( tempStr ) - 1] = '\0';

	bool pushedColorStyle = false;
	const char* textStart = tempStr;
	const char* textCur = tempStr;
	while( textCur < ( tempStr + sizeof( tempStr ) ) && *textCur != '\0' )
	{
		if( *textCur == ColorMarkerStart )
		{
			// Print accumulated text
			if( textCur != textStart )
			{
				ImGui::TextUnformatted( textStart, textCur );
				ImGui::SameLine( 0.0f, 0.0f );
			}

			// Process color code
			const char* colorStart = textCur + 1;
			do
			{
				++textCur;
			}
			while( *textCur != '\0' && *textCur != ColorMarkerEnd );

			// Change color
			if( pushedColorStyle )
			{
				ImGui::PopStyleColor();
				pushedColorStyle = false;
			}

			ImVec4 textColor;
			if( ProcessInlineHexColor( colorStart, textCur, textColor ) )
			{
				ImGui::PushStyleColor( ImGuiCol_Text, textColor );
				pushedColorStyle = true;
			}

			textStart = textCur + 1;
		}
		else if( *textCur == '\n' )
		{
			// Print accumulated text an go to next line
			ImGui::TextUnformatted( textStart, textCur );
			textStart = textCur + 1;
		}

		++textCur;
	}

	if( textCur != textStart )
	{
		ImGui::TextUnformatted( textStart, textCur );
	}
	else
	{
		ImGui::NewLine();
	}

	if( pushedColorStyle )
	{
		ImGui::PopStyleColor();
	}
}