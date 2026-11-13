///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Main.cpp
//
// This program recurively scans a directory of movie poster images in a filesystem
// The movies are organized by decade ("2000s"), year ("2001"), and Movie Title ("Monsters, Inc.")
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <set>
#include <map>
#include <vector>

#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/writer.h"

enum PosterType
{
	RELEASE,
	PREVIEW,
	CHARACTER,
	OTHER,
	IMAX,
	IMAX70MM,
	DOLBY,
	REAL3D,
	_4DX,
	FANDANGO,
	SCREENX,
	DBOX,
	UNDEFINED
};

extern int ScanJpeg(const char* filename, unsigned int* width, unsigned int* height);

///////////////////////////////////////////////////////////////////////////////////////////////////

typedef std::map<int, std::string> PosterList;

struct MovieInfo
{
	std::string title;
	std::string size;
	std::string decade;
	std::string year;
	std::string month;
	std::string day;
	std::string folder;
	std::string rootname;
	std::string format;
	PosterList release_posters;
	PosterList preview_posters;
	PosterList character_posters;
	PosterList other_posters;
	bool imax_poster;
	bool imax70mm_poster;
	bool real3d_poster;
	bool dolby_poster;
	bool fandango_poster;
	bool _4dx_poster;
	bool screenx_poster;
	bool dbox_poster;

	MovieInfo()
	{	
		imax_poster = false;
		imax70mm_poster = false;
		real3d_poster = false;
		dolby_poster = false;
		fandango_poster = false;
		_4dx_poster = false;
		screenx_poster = false;
		dbox_poster = false;
	}

	void Reset()
	{
		title = "";
		size = "27x40";
		folder = "";
		month = "";
		day = "";
		rootname = "";
		format = "";
		release_posters.clear();
		preview_posters.clear();
		character_posters.clear();
		other_posters.clear();
		imax_poster = false;
		imax70mm_poster = false;
		real3d_poster = false;
		dolby_poster = false;
		fandango_poster = false;
		_4dx_poster = false;
		screenx_poster = false;
		dbox_poster = false;
	}
};

std::set<std::string> g_all_movies;

int g_a_count = 0;
int g_b_count = 0;
int g_c_count = 0;
int g_d_count = 0;
int g_poster_count = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////

void FixTitle(std::string &title)
{
	// Some characters are not allowed in directory names
	// Fix the movie title in this function

	size_t i = title.find("()");  // Replace "()" with ": "
	if (i != std::string::npos)
	{
		title[i] = ':';
		title[i + 1] = ' ';
	}

	i = title.find("_");       // Replace "_" with "."
	if (i != std::string::npos)
	{
		title[i] = '.';
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void FixMovieTitle(std::string &title, MovieInfo *pMovieInfo)
{
	if (g_all_movies.find(title) != g_all_movies.end())
	{
		title += " (";
		title += pMovieInfo->year;
		title += ")";
	}
	g_all_movies.insert(title);
	FixTitle(title);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void  ScanInfoFile(const std::string &infoTxtNamne, MovieInfo *pMovieInfo)
{
	std::string line;
	std::ifstream infoTxtStream(infoTxtNamne.c_str());

	if (infoTxtStream.is_open())
	{
		//movie_info_ptr->size = "27x40";
		while (std::getline(infoTxtStream, line))
		{
			if (line.find("month=") != std::string::npos)
				pMovieInfo->month = line.substr(6, line.length() - 6);
			else if (line.find("day=") != std::string::npos)
				pMovieInfo->day = line.substr(4, line.length() - 4);
			else if (line.find("size=") != std::string::npos)
				pMovieInfo->size = line.substr(5, line.length() - 5);
			else if (line.find("format=") != std::string::npos)
				pMovieInfo->format = line.substr(7, line.length() - 7);
		}
		infoTxtStream.close();
	}
	else
	{
		std::cerr << "Unable to open file " << infoTxtNamne.c_str() << "\n";
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ScanFilePathname(std::string &filepath, std::string &filename, MovieInfo *pMovieInfo)
{
	// Ignore "save" folders
	if ( filepath.find( "save" ) != std::string::npos )
		return;
	
	if (filepath.find("info.txt") != std::string::npos)
	{
		ScanInfoFile(filepath, pMovieInfo);
		return;
	}

	PosterType type = PosterType::UNDEFINED;

	size_t n;
	int num = 0, exp = 1;
	char c;
	if ((n = filename.find("-imax")) != std::string::npos)
		type = PosterType::IMAX;
	else if ((n = filename.find("-imax70mm")) != std::string::npos)
		type = PosterType::IMAX70MM;
	else if ((n = filename.find("-dolby")) != std::string::npos)
			type = PosterType::DOLBY;
	else if ((n = filename.find("-real3d")) != std::string::npos)
		type = PosterType::REAL3D;
	else if ((n = filename.find("-4dx")) != std::string::npos)
		type = PosterType::_4DX;
	else if ((n = filename.find("-screenx")) != std::string::npos)
		type = PosterType::SCREENX;
	else if ((n = filename.find("-dbox")) != std::string::npos)
		type = PosterType::DBOX;
	else
	{
		if ((n = filename.find(".jpg")) != std::string::npos)
		{
			while ((c = filename[--n]) != '-')
			{
				if (isdigit(c))
					num += exp * (int)(c - '0');
				else if (c == 'a')
					type = PosterType::RELEASE;
				else if (c == 'b')
					type = PosterType::PREVIEW;
				else if (c == 'c')
					type = PosterType::CHARACTER;
				else if (c == 'd')
					type = PosterType::OTHER;
				else
					break;

				exp *= 10;
			}
		}
	}

	if (type != PosterType::UNDEFINED)
	{
		std::string rootname = filename.substr(0, n);
		if (pMovieInfo->rootname.empty())
			pMovieInfo->rootname = rootname;
		else if (pMovieInfo->rootname != rootname)
			std::cerr << "ERROR: Bad filename = " << filename.c_str() << "\n";

		unsigned int width = 0, height = 0;
		ScanJpeg(filepath.c_str(), &width, &height);
		float aspect = (float)width / (float)height;

		std::string size;
		if (abs(aspect - 0.658f) < 0.01)
			size = "27x41";
		else if (abs(aspect - 0.675f) < .01)
			size = "27x40";
		else
		{
			size = "error";
			std::cout << "ERROR: Bad aspect = " << filename.c_str() << "\n";
		}

		switch (type)
		{
		case PosterType::RELEASE:
			pMovieInfo->release_posters[num] = size;
			g_a_count++;
			g_poster_count++;
			break;

		case PosterType::PREVIEW:
			pMovieInfo->preview_posters[num] = size;
			g_b_count++;
			g_poster_count++;
			break;

		case PosterType::CHARACTER:
			pMovieInfo->character_posters[num] = size;
			g_c_count++;
			g_poster_count++;
			break;

		case PosterType::OTHER:
			pMovieInfo->other_posters[num] = size;
			g_d_count++;
			g_poster_count++;
			break;

		case PosterType::IMAX:
			pMovieInfo->imax_poster = true;
			break;

		case PosterType::IMAX70MM:
			pMovieInfo->imax70mm_poster = true;
			break;

		case PosterType::REAL3D:
			pMovieInfo->real3d_poster = true;
			break;

		case PosterType::DOLBY:
			pMovieInfo->dolby_poster = true;
			break;

		case PosterType::FANDANGO:
			pMovieInfo->fandango_poster = true;
			break;

		case PosterType::_4DX:
			pMovieInfo->_4dx_poster = true;
			break;

		case PosterType::SCREENX:
			pMovieInfo->screenx_poster = true;
			break;

		case PosterType::DBOX:
			pMovieInfo->dbox_poster = true;
			break;

		default:
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ScanMovies(std::string folder, std::string xml_file)
{
	std::ofstream fstream(xml_file);
	rapidjson::OStreamWrapper osw(fstream);
	rapidjson::Writer writer(osw);

	int title_count = 0;
	int image_count = 0;

	bool found_a_poster = false;
	bool has_info_text = false;
	int current_depth = 0;

	MovieInfo movieInfo;

	if (writer.StartArray())
	{
		std::cout << "Scanning " << folder <<  " for Movies...\n";

		const std::filesystem::path moviesPath{ folder.c_str() };
		for (auto decade = std::filesystem::directory_iterator(moviesPath);
			  decade != std::filesystem::directory_iterator();
			  decade++)
		{
			auto decadeStr = decade->path().filename().string();
			if (decadeStr[0] == '.')
				continue;

			movieInfo.decade = decadeStr;
			for (auto year = std::filesystem::directory_iterator(decade->path());
				  year != std::filesystem::directory_iterator();
				  year++)
			{
				auto yearStr = year->path().filename().string();
				if (yearStr[0] == '.')
					continue;

				movieInfo.year = yearStr;
				for (auto title = std::filesystem::directory_iterator(year->path());
					  title != std::filesystem::directory_iterator();
					  title++)
				{
					auto titleStr = title->path().filename().string();
					if (titleStr[0] == '.')
						continue;

					movieInfo.folder = movieInfo.title = titleStr;
					FixMovieTitle(movieInfo.title, &movieInfo);

					for (auto file = std::filesystem::directory_iterator(title->path());
						  file != std::filesystem::directory_iterator();
						  file++)
					{
							auto fileName = file->path().filename().string();
							if (fileName[0] == '.')
								continue;

							auto pathName = file->path().string();
							ScanFilePathname(pathName, fileName, &movieInfo);
					}

					if (movieInfo.release_posters.size() > 0)
					{
						// Write our poster info
						if (writer.StartObject())
						{
							writer.Key("title");
							writer.String(movieInfo.title.c_str());

							writer.Key("size");
							writer.String(movieInfo.size.c_str());

							writer.Key("decade");
							writer.String(movieInfo.decade.c_str());

							writer.Key("year");
							writer.String(movieInfo.year.c_str());

							if (!movieInfo.month.empty())
							{
								writer.Key("month");
								writer.String(movieInfo.month.c_str());
							}

							if (!movieInfo.day.empty())
							{
								writer.Key("day");
								writer.String(movieInfo.day.c_str());
							}

							writer.Key("folder");
							writer.String(movieInfo.folder.c_str());

							writer.Key("rootname");
							writer.String(movieInfo.rootname.c_str());

							writer.Key("posters");
							if (writer.StartArray())
							{
								for (auto &posterInfo : movieInfo.release_posters)
								{
									std::string code("a");
									code += std::to_string(posterInfo.first);
									writer.String(code.c_str());
								}
	
								for (auto& posterInfo : movieInfo.preview_posters)
								{
									std::string code("b");
									code += std::to_string(posterInfo.first);
									writer.String(code.c_str());
								}

								for (auto& posterInfo : movieInfo.character_posters)
								{
									std::string code("c");
									code += std::to_string(posterInfo.first);
									writer.String(code.c_str());
								}								
								
								for (auto& posterInfo : movieInfo.other_posters)
								{
									std::string code("d");
									code += std::to_string(posterInfo.first);
									writer.String(code.c_str());
								}

								if (movieInfo.imax_poster)
									writer.String("imax");

								if (movieInfo.imax70mm_poster)
									writer.String("imax70mm");

								if (movieInfo.real3d_poster)
									writer.String("real3d");

								if (movieInfo.dolby_poster)
									writer.String("dolby");

								if (movieInfo.fandango_poster)
									writer.String("fanango");

								if (movieInfo._4dx_poster)
									writer.String("4dx");

								if (movieInfo.screenx_poster)
									writer.String("screenx");

								if (movieInfo.dbox_poster)
									writer.String("dbox");

								writer.EndArray();
							}
							writer.EndObject();
						}
					}
					else
					{
						std::cout << movieInfo.title << " is missing a poster\n";
					}

					movieInfo.Reset();
				}
			}
		}

		writer.EndArray();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
	if (argc != 3)
		exit(-1);

	ScanMovies(argv[1], argv[2]);

	std::cout << "\n";
	std::cout << "Movies Found = " << g_all_movies.size() << "\n";
	std::cout << "\n";
	std::cout << "a movies = " << g_a_count << "\n";
	std::cout << "b movies = " << g_b_count << "\n";
	std::cout << "c movies = " << g_c_count << "\n";
	std::cout << "d movies = " << g_d_count << "\n";
	std::cout << "Total Posters = " << g_poster_count << "\n";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
