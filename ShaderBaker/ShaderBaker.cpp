#include "ShaderBaker.h"

int main(int argc, char *argv[])
{
    try
    {
        if (argc < 3)
        {
            throw TOO_SMALL;
        }

        if (argc > 3)
        {
            throw TOO_BIG;
        }

        std::cout << "BAKING SHADERS...\n";

        std::string dir_path = argv[1];
        std::string out_path = argv[2];
        std::vector<std::string> shader_paths;
        if (!GetFilesInDir(dir_path, shader_paths))
            throw NO_VALID_FILES;

        FileNode *head = new FileNode(); // head is a dud, probably shouldn't be but i can get back to it.

        for (size_t i = 0; i < shader_paths.size(); ++i)
        {
            std::string current_path = dir_path + "/" + shader_paths[i];
            std::string file_contents = GetFileContents(current_path);
            std::string name = GetNameFromFilename(shader_paths[i]);
            file_contents = FindAndReplace(file_contents, "\r", "");
            file_contents = FindAndReplace(file_contents, "\n", "");
            file_contents = FindAndReplace(file_contents, "	", "");

            switch (GetFileTypeFromFilename(shader_paths[i]))
            {
            case FRAG:
                head->InsertAtName(name, file_contents, "");
                break;
            case VERT:
                head->InsertAtName(name, "", file_contents);
                break;
            default:
                continue;
            }
        }
        std::string file_content = MakeFile(head);
        WriteFile(out_path, file_content);

        std::cout << "SHADERS BAKED SUCCESSFULLY\n";
    }
    catch (error_t error)
    {
        switch (error)
        {
        case TOO_SMALL:
            std::cerr << "[ERROR]: Not enough arguments provided.\n";
            return TOO_SMALL;
        case TOO_BIG:
            std::cerr << "[ERROR] Too many arguments provided.\n";
            throw TOO_BIG;
        case BAD_PATH:
            std::cerr << "[ERROR]: Directory path does not exist.\n";
            return BAD_PATH;
        case NO_VALID_FILES:
            std::cerr << "[ERROR]: No valid files.\n";
            return NO_VALID_FILES;
        default:
            std::cerr << "[ERROR] An error occurred.\n";
            return -1;
        }
    }
}