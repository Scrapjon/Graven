#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <exception>
#include <vector>
#include <windows.h>

typedef enum
{
    TOO_SMALL = 1,
    TOO_BIG = 2,
    BAD_PATH = 3,
    NO_VALID_FILES = 4
} error_t;

typedef enum
{
    INVALID_TYPE,
    FRAG,
    VERT
} file_type_t;

static std::string file_schema_head =
    "#ifndef SHADER_SOURCES_H\n"
    "#define SHADER_SOURCES_H\n"
    "\n"
    "/**\n"
    " * AUTO-GENERATED FILE\n"
    " * COPYRIGHT MOLONEY\n"
    " */\n"
    "\n"
    "namespace Shaders\n"
    "{\n"
    "    \n";

static std::string file_schema_tail =
    "    \n"
    "} // namespace Shaders\n"
    "\n"
    "#endif /* SHADER_SOURCES_H */\n";

struct FileNode
{
    FileNode(

        std::string name = "",
        std::string frag = "",
        std::string vert = "",
        FileNode *head = NULL,
        FileNode *next = NULL) : next(next),
                                 name(name),
                                 frag(frag),
                                 vert(vert)

    {
        this->head = head ? head : this;
    }

    FileNode *next;
    FileNode *head;
    std::string name;
    std::string frag;
    std::string vert;

    FileNode *InsertAtName(const std::string &name, const std::string &frag = "", const std::string &vert = "")
    {
        if (name == "")
            return NULL; // Don't be a dumbass
        FileNode *current = head;

        // uninitialised head
        if (current->name == "")
        {
            current->name = name;
            goto insert_data;
        }

        while (current->name != name)
        {
            if (!current->next)
            {
                return current->Extend(name, frag, vert);
            }
            current = current->next;
        }

    insert_data:

        if (frag != "")
        {
            current->frag = frag;
        }
        if (vert != "")
        {
            current->vert = vert;
        }

        return current;
    }

    FileNode *Extend(const std::string &name,
                     const std::string &frag = "",
                     const std::string &vert = "")
    {
        FileNode *current = head;
        while (current->next)
        {
            current = current->next;
        }
        current->next = new FileNode(name, frag, vert, head);
        return current->next;
    }
};

inline bool FileExists(const std::string &path)
{
    std::ifstream file;
    file.open(path.c_str());
    return file.good();
}

// Source - https://stackoverflow.com/a/1494435
// Posted by yves Baumes, modified by community. See post 'Timeline' for change history
// Retrieved 2026-08-11, License - CC BY-SA 3.0

std::string FindAndReplace(std::string str,
                           const std::string &oldStr,
                           const std::string &newStr)
{
    std::string::size_type pos = 0u;
    while ((pos = str.find(oldStr, pos)) != std::string::npos)
    {
        str.replace(pos, oldStr.length(), newStr);
        pos += newStr.length();
    }
    return str;
}

std::string GetFileContents(const std::string &path)
{
    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file.is_open())
        return "";

    file.seekg(0, std::ios::end);
    std::streamsize length = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string content;
    content.resize(length);
    if (file.read(&content[0], length))
        return content;
    return "";
}

std::string GetNameFromFilename(const std::string &filename)
{
    std::string name = filename;
    size_t suffix_loc = name.find('.');
    name = name.substr(0, suffix_loc);
    name[0] = name[0] ^ 0x20;
    return name;
}

file_type_t GetFileTypeFromFilename(const std::string &filename)
{
    size_t suffix_loc = filename.find('.');
    std::string suffix = filename.substr(suffix_loc + 1);

    if (suffix == "fsh")
        return FRAG;
    else if (suffix == "vsh")
        return VERT;

    return INVALID_TYPE;
}

bool GetFilesInDir(std::string path, std::vector<std::string> &out_paths)
{

    std::string frag_path = path + "/*.fsh";
    std::string vert_path = path + "/*.vsh";

    std::string paths[2] = {frag_path, vert_path};

    for (size_t i = 0; i < sizeof(paths) / sizeof(paths[0]); i++)
    {
        std::string &current_path = paths[i];
        WIN32_FIND_DATA FindFileData;
        HANDLE hFind = FindFirstFile(current_path.c_str(), &FindFileData);
        if (hFind == INVALID_HANDLE_VALUE)
        {
            return false;
        }
        else
            do
            {
                out_paths.push_back(FindFileData.cFileName);
            } while (FindNextFile(hFind, &FindFileData));
        FindClose(hFind);
    }

    return true;
}

std::string MakeNamespace(const FileNode &node)
{
    std::string ns =
        "    namespace " + node.name + "\n" +
        "    {\n" +
        "       static const char *frag = " + "\"" + node.frag + "\";\n" +
        "       static const char *vert = " + "\"" + node.vert + "\";\n" +
        "    } // namespace " + node.name + "\n\n";
    return ns;
}

std::string MakeFile(FileNode *head)
{
    std::string file_content = "";
    std::string ns = "";
    FileNode *current = head;
    while (true)
    {
        if (!current) // just to be safe
            break;
        ns += MakeNamespace(*current);
        if (!current->next)
            break;
        current = current->next;
    }
    file_content =
        file_schema_head + ns + file_schema_tail;
    return file_content;
}

bool WriteFile(const std::string &path, const std::string &content)
{
    std::ofstream file;
    file.open(path.c_str());
    if (!file.good())
        return false;
    file.write(content.c_str(), content.size());
    return true;
}